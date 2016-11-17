#include "ShadowMapping.h"
#include "GLWidget3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#ifndef M_PI
#define M_PI	3.1415926535
#endif

ShadowMapping::ShadowMapping() {
}

/**
 * シャドウマッピングの初期化。
 * 本関数は、GLWidget3D::initializeGL()内で呼び出すこと。
 *
 * @param programId		シェイダーのprogram id
 * @param width			シャドウマッピングの幅
 * @param height		シャドウマッピングの高さ
 */
void ShadowMapping::init(int programId, int width, int height) {
	this->programId = programId;
	this->width = width;
	this->height = height;

	glUseProgram(programId);
			
	// FBO作成
	glGenFramebuffers(1, &fboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDepth);
		
	// 影のデプスバッファを保存するための2Dテクスチャを作成
	glGenTextures(1, &textureDepth);

	// GL_TEXTURE6に、このデプスバッファをbindすることで、
	// シェーダからは6番でアクセスできる
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, textureDepth);

	// テクスチャパラメータの設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// テクスチャの外側、つまり、光源の外側は、影ってことにする(?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ただ、そもそも光源の外にならないよう、projection行列を設定すべき！
		
    // テクスチャ領域の確保(GL_DEPTH_COMPONENTを用いる)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// 生成した2Dテクスチャを、デプスバッファとしてfboに括り付ける。
	// 以後、このfboに対するレンダリングを実施すると、デプスバッファのデータは
	// この2Dテクスチャに自動的に保存される。
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth, 0);

	glActiveTexture(GL_TEXTURE0);
		
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

/**
 * シャドウマップを作成し、GL_TEXTURE6にテクスチャとして保存する。
 *
 * @param glWidget3D		GLWidget3Dクラス。このクラスのdrawScene(1)を呼び出してシーンを描画し、シャドウマップを生成する。
 * @param light_dir			光の進行方向
 */
void ShadowMapping::update(GLWidget3D* glWidget3D, const glm::vec3& light_dir, const glm::mat4& light_mvpMatrix) {
	int origWidth = glWidget3D->width();
	int origHeigh = glWidget3D->height();
				
	glUseProgram(programId);

	// レンダリング結果をFBOに保存するようにする
	// この結果、デプスバッファはtextureDepthに保存される。
    glBindFramebuffer(GL_FRAMEBUFFER, fboDepth);
    glEnable(GL_TEXTURE_2D);

	// ビューポートをシャドウマップの大きさに変更
	glViewport(0, 0, width, height);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.1f, 4.0f);

	// シャドウマップ用のmodel/view/projection行列を設定
	glUniformMatrix4fv(glGetUniformLocation(programId, "light_mvpMatrix"), 1, GL_FALSE, &light_mvpMatrix[0][0]);

	// 光の方向を設定
	//glUniform3f(glGetUniformLocation(programId, "lightDir"), light_dir.x, light_dir.y, light_dir.z);

	// 色バッファには描画しない
	glDrawBuffer(GL_NONE);

	// デプスバッファをクリア
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//RENDER
	glWidget3D->drawScene();
	
	// この時点で、textureDepthにデプス情報が格納されている
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDrawBuffer(GL_BACK);

	// ビューポートを戻す
	glViewport(0, 0, origWidth, origHeigh);
}
