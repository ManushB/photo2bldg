#include <utility>

#include <iostream>
#include "GL3D.h"
#include <GL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "GrammarParser.h"
#include "Rectangle.h"
#include "GLUtils.h"
#include <opencv2/calib3d.hpp>
#include <map>
#include "CVUtils.h"
#include "Utils.h"
#include "MassReconstruction.h"
#include "FacadeSegmentation.h"
#include "FacadeReconstruction.h"
#include "WindowRecognition.h"
#include "WindowPositioning.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QPainter>
//#include "OBJWriter.h"
//#include "GrammarWriter.h"

void Server::openImage(const QString& filename) {
    QImage temp;
    temp.load(filename);
    glWidget->imageOrig = QImage(std::max(temp.width(), temp.height()), std::max(temp.width(), temp.height()), QImage::Format_RGB32);
    glWidget->imageOrig.fill(QColor(255, 255, 255));
    QPainter temp_painter(&glWidget->imageOrig);
    temp_painter.drawImage((glWidget->imageOrig.width() - temp.width()) / 2, (glWidget->imageOrig.height() - temp.height()) / 2, temp);
    glWidget->image_loaded = true;

    float scale = std::min((float)glWidget->width() / glWidget->imageOrig.width(), (float)glWidget->height() / glWidget->imageOrig.height());
    QImage newImage = glWidget->imageOrig.scaled(glWidget->imageOrig.width() * scale, glWidget->imageOrig.height() * scale);

    glWidget->image = QImage(glWidget->width(), glWidget->height(), QImage::Format_RGB32);
    glWidget->image.fill(QColor(255, 255, 255));
    QPainter painter(&glWidget->image);
    painter.drawImage((glWidget->width() - newImage.width()) / 2, (glWidget->height() - newImage.height()) / 2, newImage);

    glWidget->geometryGenerated = false;
    glWidget->update();
}

void Server::openSilhouette(const QString& filename) {
    glWidget->silhouette.clear();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        while (true) {
            QString line = in.readLine();
            if (line.isNull()) break;
            QStringList list = line.split(QRegExp("(\t| )"));

            glm::dvec2 start(list[0].toFloat(), list[1].toFloat());
            glm::dvec2 end(list[2].toFloat(), list[3].toFloat());
            if (start != end) {
                glWidget->silhouette.push_back(vp::VanishingLine(start.x, start.y, end.x, end.y, list[4].toInt()));
            }
        }
        file.close();
    }
    glWidget->update();
}


void Server::saveImage(const QString& filename) {
    glWidget->output.save(filename);
}

void Server::onBuildingReconstruction(bool* bool_params, int* int_params) {
    glWidget->massReconstruction(bool_params[0], int_params[0] - 1,
                                 227, 25, -40, 0, -70, -20, -10, 10, 20, 90, -0.8, 0.8, -0.8, 0.8, -15, 15, -15, 15, 1, true, 3000, 0);

    glWidget->grammar_type = GL3D::GRAMMAR_TYPE_FACADE;
    glWidget->facadeReconstruction(bool_params[1], int_params[2], bool_params[2], bool_params[3]);

    glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
    glWidget->update();
    return;
}

GL3D::GL3D() {
//    mainWin = (Server*)parent;
    geometryGenerated = false;
    silhouetteWidth = 3;
    silhouetteColor = QColor(255, 255, 0);

    // 光源位置をセット
    // ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
    light_dir = glm::normalize(glm::vec3(-4, -5, -8));

    // シャドウマップ用のmodel/view/projection行列を作成
    glm::mat4 light_pMatrix = glm::ortho<float>(-50, 50, -50, 50, 0.1, 200);
    glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    light_mvpMatrix = light_pMatrix * light_mvMatrix;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Building mass

//	// load caffe model
	classifiers["mass"] = boost::shared_ptr<Classifier>(new Classifier("models/mass/deploy.prototxt", "models/mass/train_iter_80000.caffemodel", "models/mass/mean.binaryproto"));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_01.prototxt", "models/mass/train_01_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_02.prototxt", "models/mass/train_02_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_03.prototxt", "models/mass/train_03_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_04.prototxt", "models/mass/train_04_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_05.prototxt", "models/mass/train_05_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_06.prototxt", "models/mass/train_06_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_07.prototxt", "models/mass/train_07_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_08.prototxt", "models/mass/train_08_iter_120000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_09.prototxt", "models/mass/train_09_iter_160000.caffemodel")));
	regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_10.prototxt", "models/mass/train_10_iter_160000.caffemodel")));

    // load grammars
	grammars["mass"].resize(10);
	for (int i = 0; i < grammars["mass"].size(); ++i) {
		QString file_path = QString("cga/mass/contour_%1.xml").arg(i + 1, 2, 10, QChar('0'));
		cga::parseGrammar(file_path.toUtf8().constData(), grammars["mass"][i]);
	}
    /*
    cga::parseGrammar("cga/mass/contour_01.xml", grammars["mass"][0]);
    cga::parseGrammar("cga/mass/contour_02.xml", grammars["mass"][1]);
    cga::parseGrammar("cga/mass/contour_03.xml", grammars["mass"][2]);
    cga::parseGrammar("cga/mass/contour_04.xml", grammars["mass"][3]);
    cga::parseGrammar("cga/mass/contour_05.xml", grammars["mass"][4]);
    cga::parseGrammar("cga/mass/contour_06.xml", grammars["mass"][5]);
    cga::parseGrammar("cga/mass/contour_07.xml", grammars["mass"][6]);
    cga::parseGrammar("cga/mass/contour_08.xml", grammars["mass"][7]);
    cga::parseGrammar("cga/mass/contour_09.xml", grammars["mass"][8]);
    cga::parseGrammar("cga/mass/contour_10.xml", grammars["mass"][9]);
    */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Facade

    // load caffe model
	classifiers["facade"] = boost::shared_ptr<Classifier>(new Classifier("models/facade/deploy.prototxt", "models/facade/train_iter_40000.caffemodel", "models/facade/mean.binaryproto"));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_01.prototxt", "models/facade/train_01_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_02.prototxt", "models/facade/train_02_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_03.prototxt", "models/facade/train_03_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_04.prototxt", "models/facade/train_04_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_05.prototxt", "models/facade/train_05_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_06.prototxt", "models/facade/train_06_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_07.prototxt", "models/facade/train_07_iter_40000.caffemodel")));
	regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_08.prototxt", "models/facade/train_08_iter_40000.caffemodel")));

	// load grammars
	grammars["facade"].resize(8);
	for (int i = 0; i < grammars["facade"].size(); ++i) {
		QString file_path = QString("cga/facade/facade_%1.xml").arg(i + 1, 2, 10, QChar('0'));
		cga::parseGrammar(file_path.toUtf8().constData(), grammars["facade"][i]);
	}
    /*
    cga::parseGrammar("cga/facade/facade_01.xml", grammars["facade"][0]);
    cga::parseGrammar("cga/facade/facade_02.xml", grammars["facade"][1]);
    cga::parseGrammar("cga/facade/facade_03.xml", grammars["facade"][2]);
    cga::parseGrammar("cga/facade/facade_04.xml", grammars["facade"][3]);
    cga::parseGrammar("cga/facade/facade_05.xml", grammars["facade"][4]);
    cga::parseGrammar("cga/facade/facade_06.xml", grammars["facade"][5]);
    cga::parseGrammar("cga/facade/facade_07.xml", grammars["facade"][6]);
    cga::parseGrammar("cga/facade/facade_08.xml", grammars["facade"][7]);
    */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Facade auxiliary
	regressions["floors"].push_back(boost::shared_ptr<Regression>(new Regression("models/floors/deploy_01.prototxt", "models/floors/train_01_iter_40000.caffemodel")));
	classifiers["win_exist"] = boost::shared_ptr<Classifier>(new Classifier("models/window_existence/deploy.prototxt", "models/window_existence/train_iter_40000.caffemodel", "models/window_existence/mean.binaryproto"));
	regressions["win_pos"].push_back(boost::shared_ptr<Regression>(new Regression("models/window_position/deploy_01.prototxt", "models/window_position/train_01_iter_80000.caffemodel")));
//

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Window

    // load caffe model
	classifiers["window"] = boost::shared_ptr<Classifier>(new Classifier("models/window/deploy.prototxt", "models/window/train_iter_40000.caffemodel", "models/window/mean.binaryproto"));

    // load grammars
	grammars["window"].resize(31);
	for (int i = 0; i < grammars["window"].size(); ++i) {
		QString file_path = QString("cga/window/window_%1.xml").arg(i + 1, 2, 10, QChar('0'));
		cga::parseGrammar(file_path.toUtf8().constData(), grammars["window"][i]);
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Door

	grammars["door"].resize(1);
	cga::parseGrammar("cga/door/door_01.xml", grammars["door"][0]);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Material
	{
		grammars["material"].resize(1);
		cga::parseGrammar("cga/material/material.xml", grammars["material"][0]);
		/*
		cga::parseGrammar("cga/material/bldg2.xml", grammars["material"][0]);
		cga::parseGrammar("cga/material/bldg3.xml", grammars["material"][1]);
		cga::parseGrammar("cga/material/bldg5.xml", grammars["material"][2]);
		cga::parseGrammar("cga/material/bldg7.xml", grammars["material"][3]);
		cga::parseGrammar("cga/material/bldg8.xml", grammars["material"][4]);
		cga::parseGrammar("cga/material/bldg11.xml", grammars["material"][5]);
		cga::parseGrammar("cga/material/bldg14.xml", grammars["material"][6]);
		cga::parseGrammar("cga/material/bldg29.xml", grammars["material"][7]);
		cga::parseGrammar("cga/material/bldg35.xml", grammars["material"][8]);
		cga::parseGrammar("cga/material/bldg43.xml", grammars["material"][9]);
		cga::parseGrammar("cga/material/bldg45.xml", grammars["material"][10]);
		*/
	}

	// default grammar and pm values
	grammar_type = GRAMMAR_TYPE_MASS;
	grammar_ids["mass"] = 0;
	pm_params["mass"].resize(grammars["mass"][0].attrs.size(), 0.5);
	grammar_ids["facade"] = 0;
	pm_params["facade"].resize(grammars["facade"][0].attrs.size(), 0.5);
	grammar_ids["material"] = 0;

    canvas = QImage(width(), height(), QImage::Format_RGB32);
    output = QImage(width(), height(), QImage::Format_RGB32);
    initializeGL();
    resizeGL(width(), height());
}

int GL3D::height() {
    return 500;
}
int GL3D::width() {
    return 500;
}

int GL3D::update(){
    paintEvent();
}

QImage GL3D::grabFrameBuffer() {
    glBindTexture(GL_TEXTURE_2D, default_texture);
    QImage tmp = QImage(width(), height(), QImage::Format_RGBA8888);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp.bits());
    glBindTexture(GL_TEXTURE_2D, 0);
    return tmp;
}

///**
/// * Draw the scene.
/// */
void GL3D::drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    renderManager.renderAll();
}


void GL3D::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PASS 1: Render to texture
    glUseProgram(renderManager.programs["pass1"]);

    glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB);
    glClearColor(0.95, 0.95, 0.95, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragDataTex[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderManager.fragDataTex[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderManager.fragDataTex[2], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderManager.fragDataTex[3], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, DrawBuffers); // "4" is the size of DrawBuffers
    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("+ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
        exit(0);
    }

    glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
    glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "lightDir"), light_dir.x, light_dir.y, light_dir.z);
    glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "light_mvpMatrix"), 1, false, &light_mvpMatrix[0][0]);

    glUniform1i(glGetUniformLocation(renderManager.programs["pass1"], "shadowMap"), 6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, renderManager.shadow.textureDepth);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    drawScene();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PASS 2: Create AO
    if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
        glUseProgram(renderManager.programs["ssao"]);
        glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB_AO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragAOTex, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex_AO, 0);
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Always check that our framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("++ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
            exit(0);
        }

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);

        glUniform2f(glGetUniformLocation(renderManager.programs["ssao"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());

        glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

        glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex1"), 2);
        glActiveTexture(GL_TEXTURE2);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

        glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex2"), 3);
        glActiveTexture(GL_TEXTURE3);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

        glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "depthTex"), 8);
        glActiveTexture(GL_TEXTURE8);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

        glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "noiseTex"), 7);
        glActiveTexture(GL_TEXTURE7);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragNoiseTex);

        {
            glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
        }

        glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "uKernelSize"), renderManager.uKernelSize);
        glUniform3fv(glGetUniformLocation(renderManager.programs["ssao"], "uKernelOffsets"), renderManager.uKernelOffsets.size(), (const GLfloat*)renderManager.uKernelOffsets.data());

        glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uPower"), renderManager.uPower);
        glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uRadius"), renderManager.uRadius);

        glBindVertexArray(renderManager.secondPassVAO);

        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
        glDepthFunc(GL_LEQUAL);
    }
    else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE || renderManager.renderingMode == RenderManager::RENDERING_MODE_HATCHING) {
        glUseProgram(renderManager.programs["line"]);

        glBindFramebuffer(GL_FRAMEBUFFER, default_fb);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);

        glUniform2f(glGetUniformLocation(renderManager.programs["line"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());
        glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["line"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
        if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE) {
            glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 0);
        }
        else {
            glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 1);
        }

        glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex0"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

        glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex1"), 2);
        glActiveTexture(GL_TEXTURE2);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

        glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex2"), 3);
        glActiveTexture(GL_TEXTURE3);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

        glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex3"), 4);
        glActiveTexture(GL_TEXTURE4);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[3]);

        glUniform1i(glGetUniformLocation(renderManager.programs["line"], "depthTex"), 8);
        glActiveTexture(GL_TEXTURE8);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

        glUniform1i(glGetUniformLocation(renderManager.programs["line"], "hatchingTexture"), 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_3D, renderManager.hatchingTextures);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindVertexArray(renderManager.secondPassVAO);

        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
        glDepthFunc(GL_LEQUAL);
    }
    else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_CONTOUR) {
        glUseProgram(renderManager.programs["contour"]);

        glBindFramebuffer(GL_FRAMEBUFFER, default_fb);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);

        glUniform2f(glGetUniformLocation(renderManager.programs["contour"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());

        glUniform1i(glGetUniformLocation(renderManager.programs["contour"], "depthTex"), 8);
        glActiveTexture(GL_TEXTURE8);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

        glBindVertexArray(renderManager.secondPassVAO);

        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
        glDepthFunc(GL_LEQUAL);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Blur

    if (renderManager.renderingMode == RenderManager::RENDERING_MODE_BASIC || renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
        glBindFramebuffer(GL_FRAMEBUFFER, default_fb);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);

        glUseProgram(renderManager.programs["blur"]);
        glUniform2f(glGetUniformLocation(renderManager.programs["blur"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());
        //printf("pixelSize loc %d\n", glGetUniformLocation(vboRenderManager.programs["blur"], "pixelSize"));

        glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex0"), 1);//COLOR
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

        glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex1"), 2);//NORMAL
        glActiveTexture(GL_TEXTURE2);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

        /*glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex2"), 3);
        glActiveTexture(GL_TEXTURE3);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);*/

        glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "depthTex"), 8);
        glActiveTexture(GL_TEXTURE8);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

        glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex3"), 4);//AO
        glActiveTexture(GL_TEXTURE4);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, renderManager.fragAOTex);

        if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
            glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 1); // ssao used
        }
        else {
            glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 0); // no ssao
        }

        glBindVertexArray(renderManager.secondPassVAO);

        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
        glDepthFunc(GL_LEQUAL);

    }

    // REMOVE
    glActiveTexture(GL_TEXTURE0);
}

//void GL3D::clearGeometry() {
//    renderManager.removeObjects();
//    renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
//    geometryGenerated = false;
//    update();
//}


/**
 * Use the silhouette as an input to the pretrained network, and obtain the probabilities as output.
 * Then, display the options ordered by the probabilities.
 */
void GL3D::massReconstruction(bool automaticRecognition, int grammarId, int image_size, float cameraDistanceBase, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float zrotMin, float zrotMax, float fovMin, float fovMax, float oxMin, float oxMax, float oyMin, float oyMax, float xMin, float xMax, float yMin, float yMax, int silhouette_line_type, bool refinement, int maxIters, int refinement_method) {
	// adjust the original background image such that the ratio of width to height is equal to the ratio of the window
	float imageScale = std::min((float)width() / imageOrig.width(), (float)height() / imageOrig.height());
	resizeImageCanvasSize(imageOrig, width() / imageScale, height() / imageScale);

	// recognition
	time_t start = clock();

    if (automaticRecognition) {
		grammarId = massrec::recognition(classifiers["mass"], image_size, width(), height(), silhouette);
	}
	time_t end = clock();
	std::cout << "mass recognition by CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

    grammar_ids.clear();

	// grammar id
	grammar_ids["mass"] = grammarId;

	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "Mass grammar: #" << grammar_ids["mass"] + 1 << std::endl;

	start = clock();

	std::vector<float> params = massrec::parameterEstimation(this, regressions["mass"][grammar_ids["mass"]], &grammars["mass"][grammar_ids["mass"]], silhouette, image_size, cameraDistanceBase, xrotMin, xrotMax, yrotMin, yrotMax, zrotMin, zrotMax, fovMin, fovMax, oxMin, oxMax, oyMin, oyMax, xMin, xMax, yMin, yMax, silhouette_line_type, refinement, maxIters, refinement_method);
	end = clock();
	std::cout << "mass param estimation by CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

	// set the camera
	setupCamera(params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

	// set PM parameter values
	pm_params["mass"].clear();
	pm_params["mass"].insert(pm_params["mass"].begin(), params.begin() + 8, params.end());

	start = clock();
	faces = updateGeometry(GRAMMAR_TYPE_MASS, &grammars["mass"][grammar_ids["mass"]], &pm_params["mass"]);
	end = clock();
	std::cout << "update geometry: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

	update();
}

void GL3D::facadeReconstruction(bool automaticRecognition, int grammarId, bool adjustContrast, bool useMultileFacadeColors) {
	// convert image to cv::Mat object
	cv::Mat imageMat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
	cv::cvtColor(imageMat, imageMat, cv::COLOR_BGRA2BGR);

    cv::Mat max_facade;
	float max_facade_area = 0;
	glm::vec2 max_geometric_size;

	time_t start = clock();
	// rectify the facade image
	for (int fi = 0; fi < faces.size(); ++fi) {
		// non-quadratic face is not supported.
		if (faces[fi]->vertices.size() % 6 != 0) continue;

		// if the non-terminal name is not "Facade", skip it
		if (!boost::starts_with(faces[fi]->name, "Facade")) continue;

		std::vector<cv::Mat> rectified_images;
		std::vector<float> projected_areas;
		std::vector<bool> visibilities;
		std::vector<glm::vec2> face_sizes;
		bool visible = false;

		// we create a texture for each quad because the face might be curved
		for (int qi = 0; qi < faces[fi]->vertices.size() / 6; ++qi) {
			// obtain the 2d/3d coordinates of the quad
			std::vector<cv::Point2f> pts;
			std::vector<glm::vec3> pts3d;
			for (int k = 0; k < 6; ++k) {
				if (k == 3 || k == 4) continue;

				int vi = qi * 6 + k;

				pts3d.push_back(faces[fi]->vertices[vi].position);

				glm::vec2 pp = utils::projectPoint(width(), height(), faces[fi]->vertices[vi].position, camera.mvpMatrix);
				pts.push_back(cv::Point2f(pp.x, pp.y));
			}

            // check if the quad is visible
			glm::vec3 normal = glm::cross(pts3d[1] - pts3d[0], pts3d[2] - pts3d[1]);
			normal = glm::vec3(camera.mvMatrix * glm::vec4(normal, 0));
			normal /= glm::length(normal);
			glm::vec3 view_dir = glm::vec3(camera.mvMatrix * glm::vec4(pts3d[0], 1));
			view_dir /= glm::length(view_dir);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
			// DEBUG
			/*
			std::cout << "face " << fi << ": " << glm::dot(normal, view_dir);
			if (glm::dot(normal, view_dir) < -0.2) {
				std::cout << " *";
			}
			std::cout << std::endl;
			*/
			////////////////////////////////////////////////////////////////////////////////////////////////////
			if (glm::dot(normal, view_dir) < -0.2) {
				visible = true;
				visibilities.push_back(true);

				// rectify the facade image
				cv::Mat transformMat;
                cv::Mat rectifiedImage = cvutils::rectify_image(imageMat, pts, transformMat, glm::length(pts3d[1] - pts3d[0]) / glm::length(pts3d[2] - pts3d[1]));

                rectified_images.push_back(rectifiedImage);

				// compute the projected area
				projected_areas.push_back(utils::computeArea(pts));
			}
			else {
				visibilities.push_back(false);
				rectified_images.push_back(cv::Mat(glm::length(pts3d[2] - pts3d[1]) * 100, glm::length(pts3d[1] - pts3d[0]) * 100, CV_8UC3));
				projected_areas.push_back(0);
			}

            // set the size
			face_sizes.push_back(glm::vec2(glm::length(pts3d[1] - pts3d[0]), glm::length(pts3d[2] - pts3d[1])));
		}

		// save the texture image
		if (visible) {
			// reorder the rectified image
			std::vector<cv::Mat> reordered_rectified_images;
			std::vector<glm::vec2> reordered_face_sizes;
			bool skip = false;
			for (int i = 0; i < rectified_images.size(); ++i) {
				if (visibilities[i]) {
					reordered_rectified_images.push_back(rectified_images[i]);
					reordered_face_sizes.push_back(face_sizes[i]);
				}
				else {
					skip = true;
					break;
				}
			}

			if (skip) {
				for (int i = rectified_images.size() - 1; i >= 0; --i) {
					if (visibilities[i]) {
						reordered_rectified_images.insert(reordered_rectified_images.begin(), rectified_images[i]);
						reordered_face_sizes.insert(reordered_face_sizes.begin(), face_sizes[i]);
					}
					else {
						break;
					}
				}
			}

			// obtain the largest height of the images
			int height_max = 0;
			for (int i = 0; i < reordered_rectified_images.size(); ++i) {
				if (reordered_rectified_images[i].rows > height_max) {
					height_max = reordered_rectified_images[i].rows;
				}
			}

			// determine the size of the merged texture
			int width_total = 0;
			for (int i = 0; i < reordered_rectified_images.size(); ++i) {
				width_total += (float)reordered_rectified_images[i].cols * height_max / reordered_rectified_images[i].rows;
			}

			// initialize the texture
			cv::Mat textureImg(height_max, width_total, CV_8UC3, cv::Scalar(255, 255, 255));

			// merge the texture
			int offset = 0;
			for (int i = 0; i < reordered_rectified_images.size(); ++i) {
				int width = (float)reordered_rectified_images[i].cols * height_max / reordered_rectified_images[i].rows;

				cv::Mat roi(textureImg, cv::Rect(offset, 0, width, height_max));

				cv::resize(reordered_rectified_images[i], reordered_rectified_images[i], cv::Size(width, height_max));
				reordered_rectified_images[i].copyTo(roi);

				offset += width;
			}

			// compute the sum of the projected area
			float projected_area = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				projected_area = projected_areas[i];
			}


			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// DEBUG
			// save each visible facade
			/*
			char fn[256];
			sprintf(fn, "facade_%d.png", fi);
			cv::imwrite(fn, textureImg);
			*/
			///////////////////////////////////////////////////////////////////////////////////////////////////////////


			//if (textureImg.rows * textureImg.cols > max_facade_area) {
			if (projected_area > max_facade_area) {
				//max_facade_area = textureImg.rows * textureImg.cols;
				max_facade_area = projected_area;
				max_facade = textureImg;

				// compute the geometric size of the face
				max_geometric_size.x = 0;
				for (int i = 0; i < face_sizes.size(); ++i) {
					if (visibilities[i]) max_geometric_size.x += face_sizes[i].x;
				}
				max_geometric_size.y = face_sizes[0].y;
			}
		}
	}
	time_t end = clock();
	std::cout << "facade rectifaction: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

	// get the largest facade image
	cv::imwrite("facade.png", max_facade);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ここから、Facade CNNと同じはず
	cv::Mat facade_img = max_facade;

	// floor height / column width
	start = clock();
	cv::Mat resized_facade_img;
	cv::resize(facade_img, resized_facade_img, cv::Size(227, 227));
	std::vector<float> floor_params = regressions["floors"][0]->Predict(resized_facade_img);
	int num_floors = std::round(floor_params[0] + 0.3);
	int num_columns = std::round(floor_params[1] + 0.3);
	float average_floor_height = (float)facade_img.rows / num_floors;
	float average_column_width = (float)facade_img.cols / num_columns;
	end = clock();
	std::cout << "floors CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "#floors = " << num_floors << ", #columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////

	// subdivide the facade into tiles and windows
	start = clock();
	std::vector<float> x_splits;
	std::vector<float> y_splits;
	std::vector<std::vector<fs::WindowPos>> win_rects;
	fs::subdivideFacade(max_facade, (float)max_facade.rows / num_floors, (float)max_facade.cols / num_columns, y_splits, x_splits, win_rects);
	end = clock();
	std::cout << "facade subdivision: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

	// update #floors and #columns
	num_floors = y_splits.size() - 1;
	num_columns = x_splits.size() - 1;

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "updated after subdivision:" << std::endl;
	std::cout << "#floors = " << num_floors << ", #columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////

	// obtain the dominant facade color
	/*
	cv::Scalar col = fs::getDominantColor(facade_img, y_splits, x_splits, win_rects, 10);
	facade_color = QString("#%1%2%3").arg((int)col[0], 2, 16, QChar('0')).arg((int)col[1], 2, 16, QChar('0')).arg((int)col[2], 2, 16, QChar('0'));
	std::cout << "Facade color = " << facade_color.toUtf8().constData() << std::endl;
	*/

	// gray scale
	cv::Mat facade_gray_img;
	cv::cvtColor(facade_img, facade_gray_img, cv::COLOR_BGR2GRAY);
	cv::cvtColor(facade_gray_img, facade_gray_img, cv::COLOR_GRAY2BGR);

	// use window positioning CNN to locate windows
	start = clock();
	for (int i = 0; i < y_splits.size() - 1; ++i) {
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			cv::Mat tile_img(facade_gray_img, cv::Rect(x_splits[j], y_splits[i], x_splits[j + 1] - x_splits[j] + 1, y_splits[i + 1] - y_splits[i] + 1));

			cv::Mat resized_tile_img;
			cv::resize(tile_img, resized_tile_img, cv::Size(227, 227));

			// check the existence of window
			std::vector<Prediction> pred_exist = classifiers["win_exist"]->Classify(resized_tile_img, 2);
			if (pred_exist[0].first == 1) {
				win_rects[i][j].valid = fs::WindowPos::VALID;
			}
			else {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}

			if (fs::WindowPos::VALID) {
				// predict the window position
				//std::vector<float> pred_params = win_pos_regression.Predict(resized_tile_img);
				std::vector<float> pred_params = wp::parameterEstimation(regressions["win_pos"][0], resized_tile_img, true, 3000);
				//utils::output_vector(pred_params);
				win_rects[i][j].left = std::round(pred_params[0] * tile_img.cols);
				win_rects[i][j].top = std::round(pred_params[1] * tile_img.rows);
				win_rects[i][j].right = std::round(pred_params[2] * tile_img.cols);
				win_rects[i][j].bottom = std::round(pred_params[3] * tile_img.rows);
			}
		}
	}
	end = clock();
	std::cout << "window position CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

	//////////////////////////////////////////////////////////////////////////////////////////
	// HACK:
	// refine the #floors / #columns
	for (int i = 0; i < y_splits.size() - 1;) {
		int win_nums = 0;
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			if (win_rects[i][j].valid) win_nums++;
		}

		// if there are too small number of windows detected on this floor,
		// assume that they are false detection, and remove them.
		if (win_nums < (float)(x_splits.size() - 1) * 0.3) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}
			num_floors--;
			if (i < y_splits.size() - 2) {
				y_splits.erase(y_splits.begin() + i + 1);
			}
			else {
				y_splits.erase(y_splits.begin() + i);
			}
			win_rects.erase(win_rects.begin() + i);
		}
		else {
			i++;
		}
	}
	for (int j = 0; j < x_splits.size() - 1; ++j) {
		int win_nums = 0;
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			if (win_rects[i][j].valid) win_nums++;
		}

		// if there are too small number of windows detected on this column,
		// assume that they are false detection, and remove them.
		if (win_nums < (float)(y_splits.size() - 1) * 0.3) {
			for (int i = 0; i < y_splits.size() - 1; ++i) {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}
			num_columns--;
		}
	}

	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "updated after removing non-window floors/columns:" << std::endl;
	std::cout << "#updated floors = " << num_floors << ", #updated columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////////////


	// DEBUG: generate window image of the original size
	/*
	cv::Mat window_img;
	fs::generateWindowImage(y_splits, x_splits, win_rects, max_facade.size(), window_img);
	cv::imwrite("window.png", window_img);
	*/

	cv::Mat initial_facade_parsing = cv::Mat(facade_img.rows, facade_img.cols, CV_8UC3, cv::Scalar(255, 255, 255));// cv::Scalar(0, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, -1, cv::Scalar(0, 0, 0), initial_facade_parsing);

	// generate input image for facade CNN
	cv::Mat input_img(227, 227, CV_8UC3, cv::Scalar(255, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, 1, cv::Scalar(0, 0, 0), input_img);
	//cv::imwrite("window227.png", input_img);

	// recognize the facade grammar id
	if (automaticRecognition) {
		start = clock();
		grammar_ids["facade"] = facarec::recognition(classifiers["facade"], input_img, grammar_ids["mass"], num_floors);
		end = clock();
		std::cout << "facade recognition by CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
		std::cout << "Facade grammar: #" << grammar_ids["facade"] + 1 << std::endl;
	}
	else {
		grammar_ids["facade"] = grammarId;
	}

	// recognize window styles
	start = clock();
	std::vector<int> selected_win_types = winrec::recognition(max_facade, grammar_ids["facade"], y_splits, x_splits, win_rects, classifiers["window"]);
	end = clock();
	std::cout << "win recognition by CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

    // parameter estimation
	start = clock();
	std::vector<float> params = facarec::parameterEstimation(grammar_ids["facade"], regressions["facade"][grammar_ids["facade"]], input_img, max_geometric_size.x, max_geometric_size.y, num_floors, num_columns, initial_facade_parsing, selected_win_types);
	end = clock();
	std::cout << "facade param estimation by CNN: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;
	utils::output_vector(params);

	// generate final facade parsing image
	cv::Mat facade_parsing;
	facarec::generateFacadeImage(grammar_ids["facade"], facade_img.cols, facade_img.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(0, 255, 255), cv::Scalar(0, 0, 255), facade_parsing);
	cv::imwrite("estimated_facade.png", facade_parsing);

	// decode the parameter values
	params = facarec::decodeParameters(grammar_ids["facade"], max_geometric_size.x, max_geometric_size.y, num_floors, num_columns, params, selected_win_types, grammar_ids["mass"]);
	std::cout << "width: " << max_geometric_size.x << ", height: " << max_geometric_size.y << std::endl;
	utils::output_vector(params);
	for (int i = 0; i < params.size(); ++i) {
		params[i] = params[i] / 100;
	}

	// set PM parameter values
	pm_params["facade"] = params;

	////////////////////////////////////////////////////////////////////////////////
	// 2017/8/23
	// get the facade image excluding windows
	cv::Mat facade_wall_img = facade_img;
	if (adjustContrast) {
		facade_wall_img = cvutils::adjust_contrast(facade_wall_img);
	}
	fs::getWallImage(facade_wall_img, y_splits, x_splits, win_rects, facade_wall_img);
	cv::imwrite("facade_wall.png", facade_wall_img);

	// extract a dominant color for the entire region of facade
	cv::Scalar col = fs::getDominantColor(facade_wall_img, 10);
	facade_color = QString("#%1%2%3").arg((int)col[0], 2, 16, QChar('0')).arg((int)col[1], 2, 16, QChar('0')).arg((int)col[2], 2, 16, QChar('0'));
	std::cout << "--------------------------------------------" << std::endl;
	std::cout << "Facade colors:" << std::endl;
	std::cout << "Facade color " << " = " << facade_color.toUtf8().constData() << std::endl;

	// extract color for each region of the facade
	std::vector<cv::Scalar> cols = facarec::getFacadeColors(grammar_ids["facade"], params, facade_wall_img, max_geometric_size.x, max_geometric_size.y, 10);
	facade_colors.resize(cols.size());
	for (int i = 0; i < cols.size(); i++) {
		if (useMultileFacadeColors) {
			facade_colors[i] = QString("#%1%2%3").arg((int)cols[i][0], 2, 16, QChar('0')).arg((int)cols[i][1], 2, 16, QChar('0')).arg((int)cols[i][2], 2, 16, QChar('0'));
		}
		else {
			facade_colors[i] = facade_color;
		}
		std::cout << "Facade color " << i << " = " << facade_colors[i].toUtf8().constData() << std::endl;
	}
	////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < selected_win_types.size(); ++i) {
		char window_nt_name[256];
		sprintf(window_nt_name, "Window%d", i);

		grammar_ids[window_nt_name] = selected_win_types[i];
	}
    updateGeometry();
    update();
}

/**
 * Generate an image using the selected grammar and the PM parameter values.
 *
 * @param grammar			grammar
 * @param pm_params			PM parameter values
 * @param rendered_image	rendered image [OUT]
 */
bool GL3D::renderImage(cga::Grammar* grammar, std::vector<float>* pm_params, cv::Mat& rendered_image, bool discardIfTopFaceIsVisible, bool discardIfBottomFaceIsVisible) {
	std::vector<boost::shared_ptr<glutils::Face>> faces = updateGeometry(GRAMMAR_TYPE_MASS, grammar, pm_params);

    // if the top face is visible, discard this
    // Hack: assuming that faces[0] is the top face.
    if (discardIfTopFaceIsVisible) {
        glm::vec3 top_view_dir = glm::vec3(camera.mvMatrix * glm::vec4(faces[0]->vertices[0].position, 1));
        glm::vec3 top_normal = glm::vec3(camera.mvMatrix * glm::vec4(faces[0]->vertices[0].normal, 0));
        if (glm::dot(top_normal, top_view_dir) < 0) return false;
    }

    // if the bottom face is visible, discard this
    // Hack: assuming that faces[1] is the bottom face.
    if (discardIfBottomFaceIsVisible) {
        glm::vec3 bottom_view_dir = glm::vec3(camera.mvMatrix * glm::vec4(faces[1]->vertices[0].position, 1));
        glm::vec3 bottom_normal = glm::vec3(camera.mvMatrix * glm::vec4(faces[1]->vertices[0].normal, 0));
        if (glm::dot(bottom_normal, bottom_view_dir) < 0) return false;
    }

    render();
    QImage img = grabFrameBuffer();
    rendered_image = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();
    cv::cvtColor(rendered_image, rendered_image, cv::COLOR_BGRA2BGR);

    return true;
}

/**
 * Compute the distance between the rendered image and the target.
 *
 * @param rendered_image		rendered image
 * @param reference_dist_map	distance map of the target
 * @return						distance
 */
double GL3D::distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map) {
    cv::cvtColor(rendered_image, rendered_image, CV_BGR2GRAY);

    // compute the distance map
    cv::Mat rendered_dist_map;
    cv::threshold(rendered_image, rendered_image, 254, 255, CV_THRESH_BINARY);
    cv::distanceTransform(rendered_image, rendered_dist_map, CV_DIST_L2, 3);
    rendered_dist_map.convertTo(rendered_dist_map, CV_64F);

    /*
    // compute the squared difference
    cv::Mat diff_mat;
    cv::reduce((reference_dist_map - rendered_dist_map).mul(reference_dist_map - rendered_dist_map), diff_mat, 0, CV_REDUCE_SUM);
    cv::reduce(diff_mat, diff_mat, 1, CV_REDUCE_SUM);

    return diff_mat.at<double>(0, 0);
    */

    double diff = 0.0f;
    int count = 0;
    for (int r = 0; r < rendered_dist_map.rows; ++r) {
        for (int c = 0; c < rendered_dist_map.cols; ++c) {
            if (rendered_dist_map.at<double>(r, c) == 0) {
                diff += reference_dist_map.at<double>(r, c) * reference_dist_map.at<double>(r, c);
                count++;
            }
            if (reference_dist_map.at<double>(r, c) == 0) {
                diff += rendered_dist_map.at<double>(r, c) * rendered_dist_map.at<double>(r, c);
                count++;
            }
        }
    }

    return diff / count * 0.5;
}

/**
 * Update the geometry using the specified grammars and the PM parameter values.
 * Note: this function updates the geometry on GPU, so you need to call update() to actually render it on the screen.
 *
 * @param grammar_type		grammar type
 * @param mass_grammar		grammar for building mass
 * @param mass_params		PM parameter values for building mass
 * @param facade_grammar	gramamr for facade
 * @param facade_params		PM parameter values for facade
 * @param window_grammar	gramamr for window
 * @param window_params		PM parameter values for window
 */
std::vector<boost::shared_ptr<glutils::Face>> GL3D::updateGeometry(int grammar_type, cga::Grammar* mass_grammar, std::vector<float>* mass_params, cga::Grammar* facade_grammar, std::vector<float>* facade_params, cga::Grammar* window_grammar, std::vector<float>* window_params) {
	// set param values
	cga::setParamValues(*mass_grammar, *mass_params);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		cga::setParamValues(*facade_grammar, *facade_params);
		cga::setParamValues(*window_grammar, *window_params);
	}

	// setup CGA
	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0));

	// set axiom
	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
	cga.stack.push_back(start);

	std::vector<cga::Grammar> grammar_list;
	grammar_list.push_back(*mass_grammar);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		grammar_list.push_back(*facade_grammar);
		grammar_list.push_back(*window_grammar);
	}

	// generate 3d model
	std::vector<boost::shared_ptr<glutils::Face>> faces;
	cga.derive(grammar_list, true);
	cga.generateGeometry(faces, true);
	renderManager.removeObjects();
	renderManager.addFaces(faces, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

	geometryGenerated = true;

	return faces;
}

/**
 * Update the geometry using the currently selected grammars and the PM parameter values.
 * Note: this function updates the geometry on GPU, so you need to call update() to actually render it on the screen.
 *
 * @param grammar		grammar
 * @param pm_params		PM parameter values
 */
void GL3D::updateGeometry() {
    // set param values
    cga::setParamValues(grammars["mass"][grammar_ids["mass"]], pm_params["mass"]);
    if (grammar_type == GRAMMAR_TYPE_FACADE && grammar_ids.contains("facade")) {
        cga::setParamValues(grammars["facade"][grammar_ids["facade"]], pm_params["facade"]);
    }

    // setup CGA
    cga::CGA cga;
    cga.modelMat = glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0));

    // set axiom
    boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
    cga.stack.push_back(start);

    // combine all the grammar snippets
    std::vector<cga::Grammar> grammar_list;
    grammar_list.push_back(grammars["mass"][grammar_ids["mass"]]);
    if (grammar_type == GRAMMAR_TYPE_FACADE && grammar_ids.contains("facade")) {
        // add facade grammar snippet
        grammar_list.push_back(grammars["facade"][grammar_ids["facade"]]);

        // add window grammar snippets
        for (int k = 0; k < 99; ++k) {
            char window_nt_name[256];
            sprintf(window_nt_name, "Window%d", k);

            if (!grammar_ids.contains(window_nt_name)) break;

            grammar_list.push_back(grammars["window"][grammar_ids[window_nt_name]]);

            // rename the axiom of the window grammar snippet
            for (auto it = grammar_list.back().rules.begin(); it != grammar_list.back().rules.end(); ++it) {
                if (it->first == "Window") {
                    cga::Rule rule = it->second;
                    grammar_list.back().rules.erase(it);
                    grammar_list.back().rules[window_nt_name] = rule;
                    break;
                }
            }
        }

        // add door grammar snippet
        //grammar_list.push_back(grammars["door"][0]);

        // add material grammar snippet
        grammars["material"][grammar_ids["material"]].attrs["facade_color"].value = facade_color.toUtf8().constData();
        for (int i = 0; i < facade_colors.size(); i++) {
            QString s = QString("facade_color%1").arg(i);
            grammars["material"][grammar_ids["material"]].attrs[s.toUtf8().constData()].value = facade_colors[i].toUtf8().constData();
        }
        grammar_list.push_back(grammars["material"][grammar_ids["material"]]);
    }

    // generate 3d model
    //std::vector<boost::shared_ptr<glutils::Face>> faces;
    faces.clear();
    cga.derive(grammar_list, true);
    cga.generateGeometry(faces, false);
    glutils::BoundingBox bbox = cga::computeBoundingBox(faces);
    for (int i = 0; i < faces.size(); ++i) {
        for (int j = 0; j < faces[i]->vertices.size(); ++j) {
            faces[i]->vertices[j].position -= bbox.center();
        }
    }
    renderManager.removeObjects();
    renderManager.addFaces(faces, true);

    // add ground plane
    //if (showGroundPlane) {
    std::vector<Vertex> vertices;
    glutils::drawQuad(200, 200, glm::vec4(1, 1, 1, 1), glm::translate(glm::rotate(glm::mat4(), (float)(-3.14159265 * 0.5), glm::vec3(1, 0, 0)), glm::vec3(0, 0, -bbox.center().y)), vertices);
    renderManager.addObject("ground_plane", "", vertices, true);
    //}

    // update shadow map
    renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

    geometryGenerated = true;
}

/**
 * Set up the camera based on the specified camera parameters.
 *
 * @param params			Camera parameter values
 * @param xrotMax			Maximum value of the X rotation angle
 * @param xrotMin			Minimum value of the X rotation angle
 * @param yrotMax			Maximum value of the Y rotation angle
 * @param yrotMin			Minimum value of the Y rotation angle
 * ...
 */
void GL3D::setupCamera(const std::vector<float>& params, float xrotMax, float xrotMin, float yrotMax, float yrotMin, float zrotMax, float zrotMin, float fovMax, float fovMin, float oxMax, float oxMin, float oyMax, float oyMin, float xMax, float xMin, float yMax, float yMin) {
	camera.xrot = params[0] * (xrotMax - xrotMin) + xrotMin;
	camera.yrot = params[1] * (yrotMax - yrotMin) + yrotMin;
	camera.zrot = params[2] * (zrotMax - zrotMin) + zrotMin;
	camera.fovy = params[3] * (fovMax - fovMin) + fovMin;
	camera.center.x = params[4] * (oxMax - oxMin) + oxMin;
	camera.center.y = params[5] * (oyMax - oyMin) + oyMin;
	camera.pos.x = params[6] * (xMax - xMin) + xMin;
	camera.pos.y = params[7] * (xMax - xMin) + xMin;
	camera.pos.z = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));

	camera.updatePMatrix(width(), height());
}

//void GL3D::textureMapping() {
//	// create texture folder
////	if (!QDir("textures").exists()) QDir().mkdir("textures");
//
//	// convert image to cv::Mat object
////	cv::Mat imageMat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
////	cv::cvtColor(imageMat, imageMat, cv::COLOR_BGRA2BGR);
//
//	// rectify the facade image
//	for (int fi = 0; fi < faces.size(); ++fi) {
//		// non-quadratic face is not supported.
//		if (faces[fi]->vertices.size() % 6 != 0) continue;
//
//		std::vector<cv::Mat> rectified_images;
//		std::vector<bool> visibilities;
//		bool visible = false;
//
//		// we create a texture for each quad because the face might be curved
//		for (int qi = 0; qi < faces[fi]->vertices.size() / 6; ++qi) {
//			// obtain the 2d/3d coordinates of the quad
//			std::vector<cv::Point2f> pts;
//			std::vector<glm::vec3> pts3d;
//			for (int k = 0; k < 6; ++k) {
//				if (k == 3 || k == 4) continue;
//
//				int vi = qi * 6 + k;
//
//				pts3d.push_back(faces[fi]->vertices[vi].position);
//
////				glm::vec2 pp = utils::projectPoint(width(), height(), faces[fi]->vertices[vi].position, camera.mvpMatrix);
////				pts.push_back(cv::Point2f(pp.x, pp.y));
//			}
//
//			// check if the quad is visible
//			glm::vec3 normal = glm::cross(pts3d[1] - pts3d[0], pts3d[2] - pts3d[1]);
//			normal = glm::vec3(camera.mvMatrix * glm::vec4(normal, 0));
//			glm::vec3 view_dir = glm::vec3(camera.mvMatrix * glm::vec4(pts3d[0], 1));
//			if (glm::dot(normal, view_dir) < 0) {
//				visible = true;
//				visibilities.push_back(true);
//
//				// rectify the facade image
//				cv::Mat transformMat;
////				cv::Mat rectifiedImage = cvutils::rectify_image(imageMat, pts, transformMat, glm::length(pts3d[1] - pts3d[0]) / glm::length(pts3d[2] - pts3d[1]));
//				//rectifiedImage = cvutils::adjust_contrast(rectifiedImage);
//
////				rectified_images.push_back(rectifiedImage);
//			}
//			else {
//				visibilities.push_back(false);
//				rectified_images.push_back(cv::Mat(glm::length(pts3d[2] - pts3d[1]) * 100, glm::length(pts3d[1] - pts3d[0]) * 100, CV_8UC3));
//			}
//		}
//
//		// save the texture image
//		if (visible) {
//			// obtain the largest height of the images
//			int height_max = 0;
//			for (int i = 0; i < rectified_images.size(); ++i) {
//				if (visibilities[i]) {
//					if (rectified_images[i].rows > height_max) {
//						height_max = rectified_images[i].rows;
//					}
//				}
//			}
//
//			// determine the size of the merged texture
//			int width_total = 0;
//			for (int i = 0; i < rectified_images.size(); ++i) {
//				width_total += (float)rectified_images[i].cols * height_max / rectified_images[i].rows;
//			}
//
//			// initialize the texture
//			cv::Mat textureImg(height_max, width_total, CV_8UC3, cv::Scalar(255, 255, 255));
//
//			// merge the texture
//			int offset = 0;
//			for (int i = 0; i < rectified_images.size(); ++i) {
//				int width = (float)rectified_images[i].cols * height_max / rectified_images[i].rows;
//
//				if (visibilities[i]) {
//					cv::Mat roi(textureImg, cv::Rect(offset, 0, width, height_max));
//
//					cv::resize(rectified_images[i], rectified_images[i], cv::Size(width, height_max));
//					rectified_images[i].copyTo(roi);
//				}
//
//				offset += width;
//			}
//
//			time_t now = clock();
//			QString name = QString("textures/%1_%2_%3.png").arg(faces[fi]->name.c_str()).arg(fi).arg(now);
//			cv::imwrite(name.toUtf8().constData(), textureImg);
//
//			faces[fi]->texture = name.toUtf8().constData();
//		}
//	}
//
////	renderManager.removeObjects();
////	renderManager.addFaces(faces, true);
////	renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
////	update();
//}


/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GL3D::initializeGL() {
    // init glew
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
    }

    if (glewIsSupported("GL_VERSION_4_2"))
        printf("Ready for OpenGL 4.2\n");
    else {
        printf("OpenGL 4.2 not supported\n");
        exit(1);
    }
    const GLubyte* text = glGetString(GL_VERSION);
    printf("VERSION: %s\n", text);

    glGenRenderbuffers(1, &default_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, default_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width(), height());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glGenTextures(1, &default_texture);
    glBindTexture(GL_TEXTURE_2D, default_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glGenFramebuffers(1, &default_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, default_fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, default_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, default_rbo);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_3D);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glDisable(GL_TEXTURE_3D);

    glEnable(GL_TEXTURE_2D_ARRAY);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glDisable(GL_TEXTURE_2D_ARRAY);

    ////////////////////////////////
    renderManager.init(default_fb, "", "", "", true, 8192);
    renderManager.resize(this->width(), this->height());
    renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;

    glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 0);//tex0: 0

    //fixCamera();

    std::vector<Vertex> vertices;
    glutils::drawQuad(0, 0, glm::vec4(1, 1, 1, 1), glm::mat4(), vertices);
    renderManager.addObject("dummy", "", vertices, true);
}

/**
 * This function is called whenever the widget has been resized.
 */
void GL3D::resizeGL(int width, int height) {
    height = height ? height : 1;
    glViewport(0, 0, width, height);
    camera.updatePMatrix(width, height);

    renderManager.resize(width, height);

    // resize image
    float scale = std::min((float)width / imageOrig.width(), (float)height / imageOrig.height());
    QImage newImage = imageOrig.scaled(imageOrig.width() * scale, imageOrig.height() * scale);

    image = QImage(width, height, QImage::Format_RGB32);
    image.fill(QColor(255, 255, 255));
    QPainter painter(&image);
    painter.drawImage((width - newImage.width()) / 2, (height - newImage.height()) / 2, newImage);
}

void GL3D::paintEvent() {
    // OpenGLで描画
//    makeCurrent();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    render();
    //REMOVE
    glActiveTexture(GL_TEXTURE0);

    //OpenGLの設定を元に戻す
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    QPainter painter(&canvas);

    // draw background image
    if (image_loaded) {
        if (geometryGenerated) {
            painter.setOpacity(0.5f);
        }
        else {
            painter.setOpacity(1.0f);
        }
        painter.drawImage(0, 0, image);
    }

    // draw silhouette
    painter.setPen(QPen(silhouetteColor, silhouetteWidth));
    float scale = std::min((float)width() / imageOrig.width(), (float)height() / imageOrig.height());
    float image_size = std::max(imageOrig.width(), imageOrig.height()) * scale;
    for (auto line : silhouette) {
        int x1 = std::round(line.start.x * image_size + width() * 0.5);
        int y1 = std::round(line.start.y * image_size + height() * 0.5);
        int x2 = std::round(line.end.x * image_size + width() * 0.5);
        int y2 = std::round(line.end.y * image_size + height() * 0.5);
        painter.drawLine(x1, y1, x2, y2);
    }

    painter.end();

    glEnable(GL_DEPTH_TEST);
}

/**
 * Resize the canvas size while keeping the image size.
 */
void GL3D::resizeImageCanvasSize(QImage& image, int width, int height) {
    QImage tmp = image;
    image = QImage(width, height, tmp.format());
    QPainter painter(&image);
    painter.drawImage((image.width() - tmp.width()) / 2, (image.height() - tmp.height()) / 2, tmp);
}
