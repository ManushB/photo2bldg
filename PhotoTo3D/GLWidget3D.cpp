#include <iostream>
#include "GLWidget3D.h"
#include "MainWindow.h"
#include <GL/GLU.h>
#include <QDir>
#include <QFileInfoList>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "GrammarParser.h"
#include "Rectangle.h"
#include "GLUtils.h"
#include <opencv2/calib3d.hpp>
#include <map>
#include "CVUtils.h"
#include "Utils.h"
#include <QFile>
#include <QTextStream>
#include "MassReconstruction.h"
#include "FacadeSegmentation.h"
#include "FacadeReconstruction.h"

GLWidget3D::GLWidget3D(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
	mainWin = (MainWindow*)parent;
	ctrlPressed = false;
	shiftPressed = false;
	altPressed = false;

	opacityOfBackground = 0.5f;
	horizontalLeftColor = QColor(0, 0, 192);
	horizontalRightColor = QColor(64, 64, 255);
	verticalColor = QColor(140, 140, 255);
	silhouetteWidth = 3;
	silhouetteColor = QColor(255, 0, 0);

	// This is necessary to prevent the screen overdrawn by OpenGL
	setAutoFillBackground(false);

	// 光源位置をセット
	// ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// シャドウマップ用のmodel/view/projection行列を作成
	glm::mat4 light_pMatrix = glm::ortho<float>(-50, 50, -50, 50, 0.1, 200);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Building mass

	//classifier = boost::shared_ptr<Classifier>(new Classifier("models/deploy.prototxt", "models/train_iter_20000.caffemodel", "models/contour_mean.binaryproto"));

	// caffe modelを読み込む
	{
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_01.prototxt", "models/mass/train_01_iter_240000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_02.prototxt", "models/mass/train_02_iter_160000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_03.prototxt", "models/mass/train_03_iter_120000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_04.prototxt", "models/mass/train_04_iter_120000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_05.prototxt", "models/mass/train_05_iter_120000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_06.prototxt", "models/mass/train_06_iter_160000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_07.prototxt", "models/mass/train_07_iter_240000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_08.prototxt", "models/mass/train_08_iter_120000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_09.prototxt", "models/mass/train_09_iter_120000.caffemodel")));
		regressions["mass"].push_back(boost::shared_ptr<Regression>(new Regression("models/mass/deploy_10.prototxt", "models/mass/train_10_iter_80000.caffemodel")));
	}

	// Grammarを読み込む
	{
		grammars["mass"].resize(10);
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
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Facade

	classifiers["facade"] = boost::shared_ptr<Classifier>(new Classifier("models/facade/deploy.prototxt", "models/facade/train_iter_40000.caffemodel", "models/facade/mean.binaryproto"));

	{
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_01.prototxt", "models/facade/train_01_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_02.prototxt", "models/facade/train_02_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_03.prototxt", "models/facade/train_03_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_04.prototxt", "models/facade/train_04_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_05.prototxt", "models/facade/train_05_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_06.prototxt", "models/facade/train_06_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_07.prototxt", "models/facade/train_07_iter_40000.caffemodel")));
		regressions["facade"].push_back(boost::shared_ptr<Regression>(new Regression("models/facade/deploy_08.prototxt", "models/facade/train_08_iter_40000.caffemodel")));
	}

	{
		grammars["facade"].resize(4);
		cga::parseGrammar("cga/facade/facade_01.xml", grammars["facade"][0]);
		cga::parseGrammar("cga/facade/facade_02.xml", grammars["facade"][1]);
		cga::parseGrammar("cga/facade/facade_03.xml", grammars["facade"][2]);
		cga::parseGrammar("cga/facade/facade_04.xml", grammars["facade"][3]);

		grammars["window"].resize(9);
		cga::parseGrammar("cga/window/window_01.xml", grammars["window"][0]);
		cga::parseGrammar("cga/window/window_02.xml", grammars["window"][1]);
		cga::parseGrammar("cga/window/window_03.xml", grammars["window"][2]);
		cga::parseGrammar("cga/window/window_04.xml", grammars["window"][3]);
		cga::parseGrammar("cga/window/window_05.xml", grammars["window"][4]);
		cga::parseGrammar("cga/window/window_06.xml", grammars["window"][5]);
		cga::parseGrammar("cga/window/window_07.xml", grammars["window"][6]);
		cga::parseGrammar("cga/window/window_08.xml", grammars["window"][7]);
		cga::parseGrammar("cga/window/window_09.xml", grammars["window"][8]);
	}

	// default grammar and pm values
	grammar_type = GRAMMAR_TYPE_MASS;
	grammar_ids["mass"] = 0;
	pm_params["mass"].resize(grammars["mass"][0].attrs.size(), 0.5);
	grammar_ids["facade"] = 0;
	pm_params["facade"].resize(grammars["facade"][0].attrs.size(), 0.5);
	grammar_ids["window"] = 0;
	pm_params["window"].resize(grammars["window"][0].attrs.size(), 0.5);
}

/**
 * Draw the scene.
 */
void GLWidget3D::drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);

	renderManager.renderAll();
}

void GLWidget3D::render() {
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		qglClearColor(QColor(0xFF, 0xFF, 0xFF));
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

void GLWidget3D::clearBackground() {
	image_loaded = false;
	image = QImage(width(), height(), QImage::Format_RGB32);
	imageOrig = image;
	mainWin->setWindowTitle("Photo to 3D");

	update();
}

void GLWidget3D::loadImage(const QString& filename) {
	imageOrig.load(filename);
	image_loaded = true;

	float scale = std::min((float)width() / imageOrig.width(), (float)height() / imageOrig.height());
	QImage newImage = imageOrig.scaled(imageOrig.width() * scale, imageOrig.height() * scale);

	image = QImage(width(), height(), QImage::Format_RGB32);
	QPainter painter(&image);
	painter.drawImage((width() - newImage.width()) / 2, (height() - newImage.height()) / 2, newImage);

	opacityOfBackground = 0.5f;

	mainWin->setWindowTitle(QString("Photo to 3D - ") + filename);

	update();
}

void GLWidget3D::clearSilhouette() {
	silhouette.clear();
	update();
}

void GLWidget3D::loadSilhouette(const QString& filename) {
	silhouette.clear();

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
				silhouette.push_back(vp::VanishingLine(start.x, start.y, end.x, end.y, list[4].toInt()));
			}
		}

		file.close();
	}
	update();
}

void GLWidget3D::saveSilhouette(const QString& filename) {
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);

		for (auto line : silhouette) {
			out << line.start.x << " " << line.start.y << " " << line.end.x << " " << line.end.y << " " << line.type << "\n";
		}
	}
	file.close();
}

void GLWidget3D::clearGeometry() {
	renderManager.removeObjects();
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
	update();
}

/**
 * Load a grammar from a file and generate a 3d geometry.
 * This is only for test usage.
 */
void GLWidget3D::loadCGA(const std::string& filename) {
	cga::Grammar grammar;
	cga::parseGrammar(filename.c_str(), grammar);

	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -3.1415926f * 0.5f, glm::vec3(1, 0, 0));

	renderManager.removeObjects();

	// set axiom
	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
	cga.stack.push_back(start);

	// generate 3d model
	cga.derive(grammar, true);
	std::vector<boost::shared_ptr<glutils::Face> > faces;
	cga.generateGeometry(faces, true);
	renderManager.addFaces(faces, true);

	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

	// render 2d image
	render();
}

void GLWidget3D::undo() {
	if (silhouette.size() > 0) {
		silhouette.pop_back();
		update();
	}
}

/**
 * Use the silhouette as an input to the pretrained network, and obtain the probabilities as output.
 * Then, display the options ordered by the probabilities.
 */
void GLWidget3D::massReconstruction(bool automaticRecognition, int grammarSnippetId, int image_size, float cameraDistanceBase, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float zrotMin, float zrotMax, float fovMin, float fovMax, float oxMin, float oxMax, float oyMin, float oyMax, float xMin, float xMax, float yMin, float yMax, int silhouette_line_type, bool imageBlur, int imageBlurSize, bool refinement, int maxIters, int refinement_method) {
	// adjust the original background image such that the ratio of width to height is equal to the ratio of the window
	float imageScale = std::min((float)width() / imageOrig.width(), (float)height() / imageOrig.height());
	resizeImageCanvasSize(imageOrig, width() / imageScale, height() / imageScale);

	// grammar id
	grammar_ids["mass"] = grammarSnippetId;

	std::vector<float> params = massrec::parameterEstimation(this, regressions["mass"][grammar_ids["mass"]], &grammars["mass"][grammar_ids["mass"]], silhouette, image_size, cameraDistanceBase, xrotMin, xrotMax, yrotMin, yrotMax, zrotMin, zrotMax, fovMin, fovMax, oxMin, oxMax, oyMin, oyMax, xMin, xMax, yMin, yMax, silhouette_line_type, imageBlur, imageBlurSize, refinement, maxIters, refinement_method);

	// set the camera
	setupCamera(params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

	// set PM parameter values
	pm_params["mass"].clear();
	pm_params["mass"].insert(pm_params["mass"].begin(), params.begin() + 8, params.end());

	faces = updateGeometry(GRAMMAR_TYPE_MASS, &grammars["mass"][grammar_ids["mass"]], &pm_params["mass"]);

	updateStatusBar();
	update();
}

void GLWidget3D::autoTest(int grammar_id, int image_size, const QString& param_filename, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float zrotMin, float zrotMax, float fovMin, float fovMax, float oxMin, float oxMax, float oyMin, float oyMax, float xMin, float xMax, float yMin, float yMax, int silhouette_line_type, bool imageBlur, int imageBlurSize, bool refinement) {
	QFile param_file(param_filename);
	param_file.open(QIODevice::ReadOnly);
	QTextStream param_in(&param_file);
	
	while (!param_in.atEnd()) {
		QString line = param_in.readLine();
		QStringList list = line.split(",");
		int id = list[0].toInt();

		std::vector<float> param_values;
		for (int k = 1; k < list.size(); ++k) {
			param_values.push_back(list[k].toFloat());
		}

		// normalize the camera parameter values
		param_values[0] = (param_values[0] - xrotMin) / (xrotMax - xrotMin);
		param_values[1] = (param_values[1] - yrotMin) / (yrotMax - yrotMin);
		param_values[2] = (param_values[2] - zrotMin) / (zrotMax - zrotMin);
		param_values[3] = (param_values[3] - fovMin) / (fovMax - fovMin);
		param_values[4] = (param_values[4] - oxMin) / (oxMax - oxMin);
		param_values[5] = (param_values[5] - oyMin) / (oyMax - oyMin);
		param_values[6] = (param_values[6] - xMin) / (xMax - xMin);
		param_values[7] = (param_values[7] - yMin) / (yMax - yMin);

		// normalize the PM parameter values
		int k = 8;
		for (auto it = grammars["mass"][grammar_id].attrs.begin(); it != grammars["mass"][grammar_id].attrs.end(); ++it, ++k) {
			param_values[k] = (param_values[k] - it->second.range_start) / (it->second.range_end - it->second.range_start);
		}

		// read silhouette
		QString filename = QString("data/%1.ctr").arg(id, 2, 10, QChar('0'));
		loadSilhouette(filename);

		// create image of silhouette
		cv::Mat silhouette_image(height(), width(), CV_8UC1, cv::Scalar(255));
		for (auto line : silhouette) {
			cv::line(silhouette_image, cv::Point(line.start.x, line.start.y), cv::Point(line.end.x, line.end.y), cv::Scalar(0), 1);
		}

		// create distance map of silhouette
		cv::Mat silhouette_dist_map;
		cv::threshold(silhouette_image, silhouette_image, 254, 255, CV_THRESH_BINARY);
		cv::distanceTransform(silhouette_image, silhouette_dist_map, CV_DIST_L2, 3);
		silhouette_dist_map.convertTo(silhouette_dist_map, CV_64F);

		// create input image to CNN
		glm::vec2 scale((float)image_size / width(), (float)image_size / height());
		cv::Mat input = cv::Mat(image_size, image_size, CV_8UC3, cv::Scalar(255, 255, 255));
		for (auto stroke : silhouette) {
			cv::Point p1(stroke.start.x * scale.x, stroke.start.y * scale.y);
			cv::Point p2(stroke.end.x * scale.x, stroke.end.y * scale.y);

			if (silhouette_line_type == 0) {
				cv::line(input, p1, p2, cv::Scalar(0, 0, 0), 1, cv::LINE_8);
			}
			else {
				cv::line(input, p1, p2, cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
			}
		}

		if (imageBlur) {
			cv::blur(input, input, cv::Size(imageBlurSize, imageBlurSize));
		}

		// estimate paramter values by CNN
		std::vector<float> cur_params = regressions["mass"][grammar_id]->Predict(input);

		if (refinement) {
			renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;

			double diff_min = std::numeric_limits<double>::max();

			// setup the camera
			setupCamera(cur_params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

			cv::Mat rendered_image;
			renderImage(&grammars["mass"][grammar_id], &std::vector<float>(cur_params.begin() + 8, cur_params.end()), rendered_image);

			// compute the difference
			double diff = distanceMap(rendered_image, silhouette_dist_map);

			// coordinate descent
			float delta = 0.002;
			for (int iter2 = 0; iter2 < 10; ++iter2) {
				for (int k = 0; k < cur_params.size(); ++k) {
					// option 1
					std::vector<float> next_params1 = cur_params;
					next_params1[k] -= delta;
					setupCamera(next_params1, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);
					cv::Mat rendered_image1;
					double diff1 = std::numeric_limits<double>::max();
					if (renderImage(&grammars["mass"][grammar_id], &std::vector<float>(next_params1.begin() + 8, next_params1.end()), rendered_image1, true, true)) {
						diff1 = distanceMap(rendered_image1, silhouette_dist_map);
					}

					// option 2
					std::vector<float> next_params2 = cur_params;
					next_params2[k] += delta;
					setupCamera(next_params2, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);
					cv::Mat rendered_image2;
					double diff2 = std::numeric_limits<double>::max();
					if (renderImage(&grammars["mass"][grammar_id], &std::vector<float>(next_params2.begin() + 8, next_params2.end()), rendered_image2, true, true)) {
						diff2 = distanceMap(rendered_image2, silhouette_dist_map);
					}

					if (diff1 < diff2 && diff1 < diff) {
						diff = diff1;
						cur_params = next_params1;
					}
					else if (diff2 < diff1 && diff2 < diff) {
						diff = diff2;
						cur_params = next_params2;
					}
				}

				delta *= 0.8;
			}

			renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
		}

		// show the absolute error
		printf("------------------------------------------------\n");
		printf("Blg: %d\n", id);
		printf("Ground truth:\n");
		for (int k = 0; k < param_values.size(); ++k) {
			if (k > 0) printf(", ");
			printf("%.4f", param_values[k]);
		}
		printf("\n");
		printf("Estimated values:\n");
		for (int k = 0; k < cur_params.size(); ++k) {
			if (k > 0) printf(", ");
			printf("%.4f", cur_params[k]);
		}
		printf("\n");
		printf("Error:\n");
		float total_error = 0.0f;
		for (int k = 0; k < param_values.size(); ++k) {
			if (k > 0) printf(", ");
			float error = fabs(param_values[k] - cur_params[k]);
			printf("%.3f", error);
			total_error += error;
		}
		printf("\n");
		printf("Avg abs error: %.3f\n", total_error / param_values.size());
	}
}

void GLWidget3D::facadeReconstruction() {
	// convert image to cv::Mat object
	cv::Mat imageMat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
	cv::cvtColor(imageMat, imageMat, cv::COLOR_BGRA2BGR);

	cv::Mat max_facade;
	float max_facade_area = 0;
	glm::vec2 max_geometric_size;

	// rectify the facade image
	for (int fi = 0; fi < faces.size(); ++fi) {
		// non-quadratic face is not supported.
		if (faces[fi]->vertices.size() % 6 != 0) continue;

		std::vector<cv::Mat> rectified_images;
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
			glm::vec3 view_dir = glm::vec3(camera.mvMatrix * glm::vec4(pts3d[0], 1));
			if (glm::dot(normal, view_dir) < -0.01) {
				visible = true;
				visibilities.push_back(true);

				// rectify the facade image
				cv::Mat transformMat;
				cv::Mat rectifiedImage = cvutils::rectify_image(imageMat, pts, transformMat, glm::length(pts3d[1] - pts3d[0]) / glm::length(pts3d[2] - pts3d[1]));
				rectifiedImage = cvutils::adjust_contrast(rectifiedImage);

				rectified_images.push_back(rectifiedImage);
			}
			else {
				visibilities.push_back(false);
				rectified_images.push_back(cv::Mat(glm::length(pts3d[2] - pts3d[1]) * 100, glm::length(pts3d[1] - pts3d[0]) * 100, CV_8UC3));
			}

			// set the size
			face_sizes.push_back(glm::vec2(glm::length(pts3d[1] - pts3d[0]), glm::length(pts3d[2] - pts3d[1])));
		}

		// save the texture image
		if (visible) {
			// obtain the largest height of the images
			int height_max = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				if (visibilities[i]) {
					if (rectified_images[i].rows > height_max) {
						height_max = rectified_images[i].rows;
					}
				}
			}

			// determine the size of the merged texture
			int width_total = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				width_total += (float)rectified_images[i].cols * height_max / rectified_images[i].rows;
			}

			// initialize the texture
			cv::Mat textureImg(height_max, width_total, CV_8UC3, cv::Scalar(255, 255, 255));

			// merge the texture
			int offset = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				int width = (float)rectified_images[i].cols * height_max / rectified_images[i].rows;

				if (visibilities[i]) {
					cv::Mat roi(textureImg, cv::Rect(offset, 0, width, height_max));

					cv::resize(rectified_images[i], rectified_images[i], cv::Size(width, height_max));
					rectified_images[i].copyTo(roi);
				}

				offset += width;
			}

			if (textureImg.rows * textureImg.cols > max_facade_area) {
				max_facade_area = textureImg.rows * textureImg.cols;
				max_facade = textureImg;

				// compute the geometric size of the face
				max_geometric_size.x = 0;
				for (int i = 0; i < face_sizes.size(); ++i) {
					max_geometric_size.x += face_sizes[i].x;
				}
				max_geometric_size.y = face_sizes[0].y;
			}
		}
	}

	// get the largest facade image
	cv::imwrite("facade.png", max_facade);

	// extract windows
	std::vector<float> x_split;
	std::vector<float> y_split;
	std::vector<std::vector<fs::WindowPos>> win_rects;
	fs::subdivideFacade(max_facade, false, y_split, x_split, win_rects);

	// resize the window coordinates
	for (int i = 0; i < win_rects.size(); ++i) {
		for (int j = 0; j < win_rects[i].size(); ++j) {
			win_rects[i][j].left = win_rects[i][j].left * 227.0f / max_facade.cols;
			win_rects[i][j].right = win_rects[i][j].right * 227.0f / max_facade.cols;
			win_rects[i][j].top = win_rects[i][j].top * 227.0f / max_facade.rows;
			win_rects[i][j].bottom = win_rects[i][j].bottom * 227.0f / max_facade.rows;
		}
	}
	for (int i = 0; i < x_split.size(); ++i) {
		x_split[i] = x_split[i] * 227.0f / max_facade.cols;
	}
	for (int i = 0; i < y_split.size(); ++i) {
		y_split[i] = y_split[i] * 227.0f / max_facade.rows;
	}

	// generate window image
	cv::Mat input_img(227, 227, CV_8UC3, cv::Scalar(255, 255, 255));
	for (int i = 0; i < y_split.size() - 1; ++i) {
		for (int j = 0; j < x_split.size() - 1; ++j) {
			if (win_rects[i][j].valid == fs::WindowPos::VALID) {
				int x1 = std::round(x_split[j] + win_rects[i][j].left);
				int y1 = std::round(y_split[i] + win_rects[i][j].top);
				int x2 = std::round(x_split[j + 1] - 1 - win_rects[i][j].right);
				int y2 = std::round(y_split[i + 1] - 1 - win_rects[i][j].bottom);
				cv::rectangle(input_img, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1), cv::Scalar(0, 0, 0), 1);
			}
		}
	}
	cv::imwrite("window.png", input_img);

	// recognize the grammar id
	grammar_ids["facade"] = facarec::recognition(classifiers["facade"], input_img);
	std::cout << grammar_ids["facade"] << std::endl;

	// parameter estimation
	std::vector<float> params = facarec::parameterEstimation(grammar_ids["facade"], regressions["facade"][grammar_ids["facade"]], input_img, max_geometric_size.x, max_geometric_size.y);
	utils::output_vector(params);

	// set PM parameter values
	pm_params["facade"] = params;

	updateGeometry();

	updateStatusBar();
	update();

}

/**
 * Generate an image using the selected grammar and the PM parameter values.
 *
 * @param grammar			grammar
 * @param pm_params			PM parameter values
 * @param rendered_image	rendered image [OUT]
 */
bool GLWidget3D::renderImage(cga::Grammar* grammar, std::vector<float>* pm_params, cv::Mat& rendered_image, bool discardIfTopFaceIsVisible, bool discardIfBottomFaceIsVisible) {
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
double GLWidget3D::distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map) {
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
std::vector<boost::shared_ptr<glutils::Face>> GLWidget3D::updateGeometry(int grammar_type, cga::Grammar* mass_grammar, std::vector<float>* mass_params, cga::Grammar* facade_grammar, std::vector<float>* facade_params, cga::Grammar* window_grammar, std::vector<float>* window_params) {
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

	std::vector<cga::Grammar*> grammar_list;
	grammar_list.push_back(mass_grammar);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		grammar_list.push_back(facade_grammar);
		grammar_list.push_back(window_grammar);
	}

	// generate 3d model
	std::vector<boost::shared_ptr<glutils::Face>> faces;
	cga.derive(grammar_list, true);
	cga.generateGeometry(faces, true);
	renderManager.removeObjects();
	renderManager.addFaces(faces, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

	return faces;
}

/**
 * Update the geometry using the currently selected grammars and the PM parameter values.
 * Note: this function updates the geometry on GPU, so you need to call update() to actually render it on the screen.
 *
 * @param grammar		grammar
 * @param pm_params		PM parameter values
 */
void GLWidget3D::updateGeometry() {
	// set param values
	cga::setParamValues(grammars["mass"][grammar_ids["mass"]], pm_params["mass"]);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		cga::setParamValues(grammars["facade"][grammar_ids["facade"]], pm_params["facade"]);
		cga::setParamValues(grammars["window"][0], pm_params["window"]);
	}

	// setup CGA
	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0));

	// set axiom
	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
	cga.stack.push_back(start);

	std::vector<cga::Grammar*> grammar_list;
	grammar_list.push_back(&grammars["mass"][grammar_ids["mass"]]);
	if (grammar_type == GRAMMAR_TYPE_FACADE) {
		grammar_list.push_back(&grammars["facade"][grammar_ids["facade"]]);
		grammar_list.push_back(&grammars["window"][0]);
	}

	// generate 3d model
	//std::vector<boost::shared_ptr<glutils::Face>> faces;
	faces.clear();
	cga.derive(grammar_list, true);
	cga.generateGeometry(faces, true);
	renderManager.removeObjects();
	renderManager.addFaces(faces, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
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
void GLWidget3D::setupCamera(const std::vector<float>& params, float xrotMax, float xrotMin, float yrotMax, float yrotMin, float zrotMax, float zrotMin, float fovMax, float fovMin, float oxMax, float oxMin, float oyMax, float oyMin, float xMax, float xMin, float yMax, float yMin) {
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

void GLWidget3D::textureMapping() {
	// create texture folder
	if (!QDir("textures").exists()) QDir().mkdir("textures");

	// convert image to cv::Mat object
	cv::Mat imageMat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
	cv::cvtColor(imageMat, imageMat, cv::COLOR_BGRA2BGR);

	// rectify the facade image
	for (int fi = 0; fi < faces.size(); ++fi) {
		// non-quadratic face is not supported.
		if (faces[fi]->vertices.size() % 6 != 0) continue;

		std::vector<cv::Mat> rectified_images;
		std::vector<bool> visibilities;
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
			glm::vec3 view_dir = glm::vec3(camera.mvMatrix * glm::vec4(pts3d[0], 1));
			if (glm::dot(normal, view_dir) < 0) {
				visible = true;
				visibilities.push_back(true);

				// rectify the facade image
				cv::Mat transformMat;
				cv::Mat rectifiedImage = cvutils::rectify_image(imageMat, pts, transformMat, glm::length(pts3d[1] - pts3d[0]) / glm::length(pts3d[2] - pts3d[1]));
				rectifiedImage = cvutils::adjust_contrast(rectifiedImage);

				rectified_images.push_back(rectifiedImage);
			}
			else {
				visibilities.push_back(false);
				rectified_images.push_back(cv::Mat(glm::length(pts3d[2] - pts3d[1]) * 100, glm::length(pts3d[1] - pts3d[0]) * 100, CV_8UC3));
			}
		}

		// save the texture image
		if (visible) {
			// obtain the largest height of the images
			int height_max = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				if (visibilities[i]) {
					if (rectified_images[i].rows > height_max) {
						height_max = rectified_images[i].rows;
					}
				}
			}

			// determine the size of the merged texture
			int width_total = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				width_total += (float)rectified_images[i].cols * height_max / rectified_images[i].rows;
			}

			// initialize the texture
			cv::Mat textureImg(height_max, width_total, CV_8UC3, cv::Scalar(255, 255, 255));

			// merge the texture
			int offset = 0;
			for (int i = 0; i < rectified_images.size(); ++i) {
				int width = (float)rectified_images[i].cols * height_max / rectified_images[i].rows;

				if (visibilities[i]) {
					cv::Mat roi(textureImg, cv::Rect(offset, 0, width, height_max));

					cv::resize(rectified_images[i], rectified_images[i], cv::Size(width, height_max));
					rectified_images[i].copyTo(roi);
				}

				offset += width;
			}

			time_t now = clock();
			QString name = QString("textures/%1_%2_%3.png").arg(faces[fi]->name.c_str()).arg(fi).arg(now);
			cv::imwrite(name.toUtf8().constData(), textureImg);

			faces[fi]->texture = name.toUtf8().constData();
		}
	}

	renderManager.removeObjects();
	renderManager.addFaces(faces, true);
	renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
	update();
}

void GLWidget3D::updateStatusBar() {
	char buff[256];

	QString format("rot=(%.3f, %.3f, %.3f), fov=%.3f, O=(%.6f, %.6f), pos=(%.3f, %.3f, %.3f), PM=(");
	sprintf(buff, format.toUtf8().constData(), camera.xrot, camera.yrot, camera.zrot, camera.fovy, camera.center.x, camera.center.y, camera.pos.x, camera.pos.y, camera.pos.z);

	QString msg(buff);

	int k = 0;
	if (grammar_type == GRAMMAR_TYPE_MASS) {
		for (auto it = grammars["mass"][grammar_ids["mass"]].attrs.begin(); it != grammars["mass"][grammar_ids["mass"]].attrs.end(); ++it, ++k) {
			if (k > 0) msg += ", ";
			sprintf(buff, "%.3f", pm_params["mass"][k] * (it->second.range_end - it->second.range_start) + it->second.range_start);
			msg += buff;
		}
	}
	else if (grammar_type == GRAMMAR_TYPE_FACADE) {
		for (auto it = grammars["facade"][grammar_ids["facade"]].attrs.begin(); it != grammars["facade"][grammar_ids["facade"]].attrs.end(); ++it, ++k) {
			if (k > 0) msg += ", ";
			sprintf(buff, "%.3f", pm_params["facade"][k] * (it->second.range_end - it->second.range_start) + it->second.range_start);
			msg += buff;
		}
	}
	msg += ")";

	mainWin->statusBar()->showMessage(msg);
}

/**
 * Resize the canvas size while keeping the image size.
 */
void GLWidget3D::resizeImageCanvasSize(QImage& image, int width, int height) {
	QImage tmp = image;
	image = QImage(width, height, tmp.format());
	QPainter painter(&image);
	painter.drawImage((image.width() - tmp.width()) / 2, (image.height() - tmp.height()) / 2, tmp);
}

void GLWidget3D::keyPressEvent(QKeyEvent *e) {
	ctrlPressed = false;

	if (e->modifiers() == Qt::ControlModifier) {
		ctrlPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = true;
		break;
	case Qt::Key_Alt:
		altPressed = true;
		break;
	case Qt::Key_Delete:
		clearGeometry();
		break;
	case Qt::Key_1:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 0) {
			pm_params["mass"][0] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 0) {
			pm_params["facade"][0] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_2:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 1) {
			pm_params["mass"][1] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 1) {
			pm_params["facade"][1] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_3:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 2) {
			pm_params["mass"][2] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 2) {
			pm_params["facade"][2] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_4:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 3) {
			pm_params["mass"][3] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 3) {
			pm_params["facade"][3] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_5:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 4) {
			pm_params["mass"][4] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 4) {
			pm_params["facade"][4] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_6:
		if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 5) {
			pm_params["mass"][5] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 5) {
			pm_params["facade"][5] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
		}
		updateGeometry();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Left:
		camera.pos.x += 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Right:
		camera.pos.x -= 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Up:
		camera.pos.y -= 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	case Qt::Key_Down:
		camera.pos.y += 0.5 * (ctrlPressed ? 0.1 : 1);
		camera.updateMVPMatrix();
		updateStatusBar();
		update();
		break;
	default:
		break;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Alt:
		altPressed = false;
		break;
	default:
		break;
	}
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL() {
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
	renderManager.init("", "", "", true, 8192);
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
void GLWidget3D::resizeGL(int width, int height) {
	height = height ? height : 1;
	glViewport(0, 0, width, height);
	camera.updatePMatrix(width, height);

	renderManager.resize(width, height);
}

void GLWidget3D::paintEvent(QPaintEvent *event) {
	// OpenGLで描画
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	render();

	// REMOVE
	glActiveTexture(GL_TEXTURE0);

	// OpenGLの設定を元に戻す
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	QPainter painter(this);

	// draw background image
	if (image_loaded) {
		painter.setOpacity(opacityOfBackground);
		painter.drawImage(0, 0, image);
	}

	// draw silhouette
	painter.setPen(QPen(silhouetteColor, silhouetteWidth));
	for (auto line : silhouette) {
		painter.drawLine(line.start.x, line.start.y, line.end.x, line.end.y);
	}

	// draw the center of the building
	/*
	glm::vec2 pp = vp::projectPoint(camera.mvpMatrix, glm::dvec3(0, 0, 0));
	painter.setPen(QPen(QColor(255, 0, 0), 1, Qt::SolidLine));
	painter.setBrush(QBrush(QColor(255, 0, 0)));
	painter.drawEllipse((pp.x + 1) * 0.5 * width() - 3, (1 - pp.y) * 0.5 * height() - 3, 7, 7);
	*/

	painter.end();

	glEnable(GL_DEPTH_TEST);
}

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) {
		camera.mousePress(e->x(), e->y());
	}
	else if (e->button() & Qt::LeftButton) {
		if (shiftPressed) {
			silhouette.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
		}
		else if (altPressed) {
			silhouette.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_VERTICAL));
		}
		else {
			silhouette.push_back(vp::VanishingLine(e->x(), e->y(), e->x(), e->y(), vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
		}
	}
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) { // Rotate
		if (shiftPressed) { // Move
			camera.move(e->x(), e->y());
		}
		else {
			camera.rotate(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
		}
	}
	else if (e->buttons() & Qt::MidButton) { // Rotate around Z axis
		camera.rotateAroundZ(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
	}
	else if (e->buttons() & Qt::LeftButton) {
		if (silhouette.size() > 0) {
			silhouette.back().end = glm::vec2(e->x(), e->y());
		}
	}
	
	updateStatusBar();
	update();
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		if (silhouette.size() > 0 && silhouette.back().start == silhouette.back().end) {
			silhouette.pop_back();
		}
	}
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.changeFov(e->delta() * 0.05, (ctrlPressed ? 0.1 : 1), width(), height());
	updateStatusBar();
	update();
}

