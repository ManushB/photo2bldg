#include <iostream>
#include "GLWidget3D.h"
#include <QDir>
#include <QFileInfoList>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "GrammarParser.h"
//#include "GLUtils.h"
#include <opencv2/calib3d.hpp>
#include <map>
//#include "CVUtils.h"
//#include "Utils.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QMovie>
#include <QLabel>
#include <sys/stat.h>
//#include "WindowRecognition.h"
//#include "WindowPositioning.h"
//#include <boost/algorithm/string/predicate.hpp>
//#include <boost/lexical_cast.hpp>
//#include "OBJWriter.h"
//#include "GrammarWriter.h"
#include "Client.h"


GLWidget3D::GLWidget3D(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
    mainWin = (Client *) parent;

    ctrlPressed = false;
    shiftPressed = false;
    altPressed = false;

    opacityOfBackground = 1.0f;
    horizontalLeftColor = QColor(0, 0, 192);
    horizontalRightColor = QColor(64, 64, 255);
    verticalColor = QColor(140, 140, 255);
    silhouetteWidth = 3;
    silhouetteColor = QColor(255, 255, 0);
    geometryGenerated = false;

    // This is necessary to prevent the screen overdrawn by OpenGL
    setAutoFillBackground(false);

    char *home = getenv("HOME");
    wrkDir.sprintf("%s/photo2bldg_data/", home);
    inputPrm.sprintf("%s/photo2bldg_data/parameters.xml", home);
    if (mkdir(wrkDir.toUtf8().constData(), 0777) == -1) {
        if (errno != EEXIST) {
            std::cout << "cannot create working directory " << strerror(errno) << std::endl;
            exit(1);
        }
    }

    // 光源位置をセット
    // ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
    light_dir = glm::normalize(glm::vec3(-4, -5, -8));

    // シャドウマップ用のmodel/view/projection行列を作成
    glm::mat4 light_pMatrix = glm::ortho<float>(-50, 50, -50, 50, 0.1, 200);
    glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    light_mvpMatrix = light_pMatrix * light_mvMatrix;

//	// default grammar and pm values
    grammar_type = GRAMMAR_TYPE_MASS;
//	grammar_ids["mass"] = 0;
//	pm_params["mass"].resize(grammars["mass"][0].attrs.size(), 0.5);
//	grammar_ids["facade"] = 0;
//	pm_params["facade"].resize(grammars["facade"][0].attrs.size(), 0.5);
//	grammar_ids["material"] = 0;
}

/**
 * Draw the scene.
 */
//void GLWidget3D::drawScene() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glDepthMask(true);

//	renderManager.renderAll();
//}


//void GLWidget3D::render() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glMatrixMode(GL_MODELVIEW);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PASS 1: Render to texture
//	glUseProgram(renderManager.programs["pass1"]);

//	glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB);
//	glClearColor(0.95, 0.95, 0.95, 1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragDataTex[0], 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderManager.fragDataTex[1], 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderManager.fragDataTex[2], 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderManager.fragDataTex[3], 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex, 0);

// Set the list of draw buffers.
//	GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
//	glDrawBuffers(4, DrawBuffers); // "4" is the size of DrawBuffers
//	// Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//		printf("+ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
//		exit(0);
//	}

//	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
//	glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "lightDir"), light_dir.x, light_dir.y, light_dir.z);
//	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "light_mvpMatrix"), 1, false, &light_mvpMatrix[0][0]);
//
//	glUniform1i(glGetUniformLocation(renderManager.programs["pass1"], "shadowMap"), 6);
//	glActiveTexture(GL_TEXTURE6);
//	glBindTexture(GL_TEXTURE_2D, renderManager.shadow.textureDepth);

//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	drawScene();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PASS 2: Create AO
//	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
//		glUseProgram(renderManager.programs["ssao"]);
//		glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB_AO);
//
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragAOTex, 0);
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex_AO, 0);
//		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//		glClearColor(1, 1, 1, 1);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// Always check that our framebuffer is ok
//		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//			printf("++ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
//			exit(0);
//		}
//
//		glDisable(GL_DEPTH_TEST);
//		glDepthFunc(GL_ALWAYS);
//
//		glUniform2f(glGetUniformLocation(renderManager.programs["ssao"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 1);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex1"), 2);
//		glActiveTexture(GL_TEXTURE2);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex2"), 3);
//		glActiveTexture(GL_TEXTURE3);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "depthTex"), 8);
//		glActiveTexture(GL_TEXTURE8);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "noiseTex"), 7);
//		glActiveTexture(GL_TEXTURE7);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragNoiseTex);
//
//		{
//			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
//			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
//		}
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "uKernelSize"), renderManager.uKernelSize);
//		glUniform3fv(glGetUniformLocation(renderManager.programs["ssao"], "uKernelOffsets"), renderManager.uKernelOffsets.size(), (const GLfloat*)renderManager.uKernelOffsets.data());
//
//		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uPower"), renderManager.uPower);
//		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uRadius"), renderManager.uRadius);
//
//		glBindVertexArray(renderManager.secondPassVAO);
//
//		glDrawArrays(GL_QUADS, 0, 4);
//		glBindVertexArray(0);
//		glDepthFunc(GL_LEQUAL);
//	}
//	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE || renderManager.renderingMode == RenderManager::RENDERING_MODE_HATCHING) {
//		glUseProgram(renderManager.programs["line"]);
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glClearColor(1, 1, 1, 1);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glDisable(GL_DEPTH_TEST);
//		glDepthFunc(GL_ALWAYS);
//
//		glUniform2f(glGetUniformLocation(renderManager.programs["line"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());
//		glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["line"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
//		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE) {
//			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 0);
//		}
//		else {
//			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 1);
//		}
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex0"), 1);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex1"), 2);
//		glActiveTexture(GL_TEXTURE2);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex2"), 3);
//		glActiveTexture(GL_TEXTURE3);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex3"), 4);
//		glActiveTexture(GL_TEXTURE4);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[3]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "depthTex"), 8);
//		glActiveTexture(GL_TEXTURE8);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "hatchingTexture"), 5);
//		glActiveTexture(GL_TEXTURE5);
//		glBindTexture(GL_TEXTURE_3D, renderManager.hatchingTextures);
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//		glBindVertexArray(renderManager.secondPassVAO);
//
//		glDrawArrays(GL_QUADS, 0, 4);
//		glBindVertexArray(0);
//		glDepthFunc(GL_LEQUAL);
//	}
//	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_CONTOUR) {
//		glUseProgram(renderManager.programs["contour"]);
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glClearColor(1, 1, 1, 1);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glDisable(GL_DEPTH_TEST);
//		glDepthFunc(GL_ALWAYS);
//
//		glUniform2f(glGetUniformLocation(renderManager.programs["contour"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["contour"], "depthTex"), 8);
//		glActiveTexture(GL_TEXTURE8);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);
//
//		glBindVertexArray(renderManager.secondPassVAO);
//
//		glDrawArrays(GL_QUADS, 0, 4);
//		glBindVertexArray(0);
//		glDepthFunc(GL_LEQUAL);
//	}
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	// Blur
//
//	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_BASIC || renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		qglClearColor(QColor(0xFF, 0xFF, 0xFF));
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glDisable(GL_DEPTH_TEST);
//		glDepthFunc(GL_ALWAYS);
//
//		glUseProgram(renderManager.programs["blur"]);
//		glUniform2f(glGetUniformLocation(renderManager.programs["blur"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());
//		//printf("pixelSize loc %d\n", glGetUniformLocation(vboRenderManager.programs["blur"], "pixelSize"));
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex0"), 1);//COLOR
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex1"), 2);//NORMAL
//		glActiveTexture(GL_TEXTURE2);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);
//
//		/*glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex2"), 3);
//		glActiveTexture(GL_TEXTURE3);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);*/
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "depthTex"), 8);
//		glActiveTexture(GL_TEXTURE8);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);
//
//		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex3"), 4);//AO
//		glActiveTexture(GL_TEXTURE4);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, renderManager.fragAOTex);
//
//		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
//			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 1); // ssao used
//		}
//		else {
//			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 0); // no ssao
//		}
//
//		glBindVertexArray(renderManager.secondPassVAO);
//
//		glDrawArrays(GL_QUADS, 0, 4);
//		glBindVertexArray(0);
//		glDepthFunc(GL_LEQUAL);
//
//	}

// REMOVE
//	glActiveTexture(GL_TEXTURE0);
//}

void GLWidget3D::paintGL() {
    objReader.render(camera, this->width(), this->height());
}

void GLWidget3D::clearBackground() {
    image_loaded = false;
    image = QImage(width(), height(), QImage::Format_RGB32);
    imageOrig = image;
    mainWin->setWindowTitle("Photo to 3D");
    update();
}

void GLWidget3D::loadImage(const QString &filename) {
    // make the image square size
    QImage temp;
    temp.load(filename);
    imageOrig = QImage(std::max(temp.width(), temp.height()), std::max(temp.width(), temp.height()),
                       QImage::Format_RGB32);
    imageOrig.fill(QColor(255, 255, 255));
    QPainter temp_painter(&imageOrig);
    temp_painter.drawImage((imageOrig.width() - temp.width()) / 2, (imageOrig.height() - temp.height()) / 2, temp);
    image_loaded = true;

    float scale = std::min((float) width() / imageOrig.width(), (float) height() / imageOrig.height());
    QImage newImage = imageOrig.scaled(imageOrig.width() * scale, imageOrig.height() * scale);

    image = QImage(width(), height(), QImage::Format_RGB32);
    image.fill(QColor(255, 255, 255));
    QPainter painter(&image);
    painter.drawImage((width() - newImage.width()) / 2, (height() - newImage.height()) / 2, newImage);

    opacityOfBackground = 1.0f;
    geometryGenerated = false;

    //mainWin->setWindowTitle(QString("Photo to 3D - ") + filename);

    update();
}

void GLWidget3D::clearSilhouette() {
    silhouette.clear();
    update();
}

void GLWidget3D::loadSilhouette(const QString &filename) {
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

void GLWidget3D::saveSilhouette(const QString &filename) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);

        for (auto line : silhouette) {
            out << line.start.x << " " << line.start.y << " " << line.end.x << " " << line.end.y << " " << line.type
                << "\n";
        }
    }
    file.close();
}

void GLWidget3D::execProcess() {
    QProcess p;

    std::cout << inputImg.toUtf8().constData() << std::endl;
    char cmd[200];
    sprintf(cmd, "submit photo2bldg %s %s %s", inputImg.toUtf8().constData(), inputCtr.toUtf8().constData(),
            inputPrm.toUtf8().constData());

    std::cout << cmd << std::endl;
    //p.execute("submit photo2bldg /home/mygeohub/manush/photo2bldg/client/data/02.jpg /home/mygeohub/manush/photo2bldg/client/data/02.ctr");
    p.setWorkingDirectory(wrkDir);
    this->setCursor(Qt::WaitCursor);
    p.execute(cmd);
    this->setCursor(Qt::ArrowCursor);

    if (p.exitStatus() == QProcess::NormalExit) {
        std::cout << p.readAllStandardOutput().toStdString() << std::endl;

        this->loadImage("output.jpg");

    } else {
        std::cout << "Exit on error" << std::endl;
    }
}

void GLWidget3D::clearGeometry() {
//	renderManager.removeObjects();
//	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
    geometryGenerated = false;
    update();
}

/**
 * Load a grammar from a file and generate a 3d geometry.
 * This is only for test usage.
 */
void GLWidget3D::loadCGA(const QString &filename) {
//	std::vector<cga::Grammar> grammar_list;
//	cga::parseGrammar(filename.toUtf8().constData(), grammar_list);

//	cga::CGA cga;
//	cga.modelMat = glm::rotate(glm::mat4(), -3.1415926f * 0.5f, glm::vec3(1, 0, 0));

//	renderManager.removeObjects();

    // set axiom
//	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
//	cga.stack.push_back(start);

    // generate 3d model
//	cga.derive(grammar_list, true);
//	std::vector<boost::shared_ptr<glutils::Face> > faces;
//	cga.generateGeometry(faces, true);
//	renderManager.addFaces(faces, true);

//	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

    // render 2d image
//	render();
}

void GLWidget3D::saveCGA(const QString &filename) {
//	std::vector<cga::Grammar> grammar_list;
//	grammar_list.push_back(grammars["mass"][grammar_ids["mass"]]);
    if (grammar_type == GRAMMAR_TYPE_FACADE && grammar_ids.contains("facade")) {
        // add facade grammar snippet
//		grammar_list.push_back(grammars["facade"][grammar_ids["facade"]]);

        // add window grammar snippets
        for (int k = 0; k < 99; ++k) {
            char window_nt_name[256];
            sprintf(window_nt_name, "Window%d", k);

            if (!grammar_ids.contains(window_nt_name)) break;

//			grammar_list.push_back(grammars["window"][grammar_ids[window_nt_name]]);

            // rename the axiom of the window grammar snippet
//			for (auto it = grammar_list.back().rules.begin(); it != grammar_list.back().rules.end(); ++it) {
//				if (it->first == "Window") {
//					cga::Rule rule = it->second;
//					grammar_list.back().rules.erase(it);
//					grammar_list.back().rules[window_nt_name] = rule;
//					break;
//				}
//			}
        }

        // add door grammar snippet
//		grammar_list.push_back(grammars["door"][0]);

        // add material grammar snippet
//		grammar_list.push_back(grammars["material"][grammar_ids["material"]]);
    }

    //cga::save(filename.toUtf8().constData(), grammars["mass"][grammar_ids["mass"]]);
//	cga::save(filename.toUtf8().constData(), grammar_list);
}

void GLWidget3D::loadOBJ(const QString &filename) {
    if (!objReader.LoadObjAndConvert(filename.toUtf8().data())) {
        return;
    }
    updateGL();
}

void GLWidget3D::saveOBJ(const QString &filename) {
//	OBJWriter::write(faces, filename.toUtf8().constData());
}

void GLWidget3D::undo() {
    if (silhouette.size() > 0) {
        silhouette.pop_back();
        update();
    }
}

/**
 * Generate an image using the selected grammar and the PM parameter values.
 *
 * @param grammar			grammar
 * @param pm_params			PM parameter values
 * @param rendered_image	rendered image [OUT]
 */
//bool GLWidget3D::renderImage(cga::Grammar* grammar, std::vector<float>* pm_params, cv::Mat& rendered_image, bool discardIfTopFaceIsVisible, bool discardIfBottomFaceIsVisible) {
////	std::vector<boost::shared_ptr<glutils::Face>> faces = updateGeometry(GRAMMAR_TYPE_MASS, grammar, pm_params);
//
//	// if the top face is visible, discard this
//	// Hack: assuming that faces[0] is the top face.
//	if (discardIfTopFaceIsVisible) {
//		glm::vec3 top_view_dir = glm::vec3(camera.mvMatrix * glm::vec4(faces[0]->vertices[0].position, 1));
//		glm::vec3 top_normal = glm::vec3(camera.mvMatrix * glm::vec4(faces[0]->vertices[0].normal, 0));
//		if (glm::dot(top_normal, top_view_dir) < 0) return false;
//	}
//
//	// if the bottom face is visible, discard this
//	// Hack: assuming that faces[1] is the bottom face.
//	if (discardIfBottomFaceIsVisible) {
//		glm::vec3 bottom_view_dir = glm::vec3(camera.mvMatrix * glm::vec4(faces[1]->vertices[0].position, 1));
//		glm::vec3 bottom_normal = glm::vec3(camera.mvMatrix * glm::vec4(faces[1]->vertices[0].normal, 0));
//		if (glm::dot(bottom_normal, bottom_view_dir) < 0) return false;
//	}
//
//	render();
//	QImage img = grabFrameBuffer();
//	rendered_image = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();
//	cv::cvtColor(rendered_image, rendered_image, cv::COLOR_BGRA2BGR);
//
//	return true;
//}

/**
 * Compute the distance between the rendered image and the target.
 *
 * @param rendered_image		rendered image
 * @param reference_dist_map	distance map of the target
 * @return						distance
 */
//double GLWidget3D::distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map) {
////	cv::cvtColor(rendered_image, rendered_image, CV_BGR2GRAY);
//
//	// compute the distance map
//	cv::Mat rendered_dist_map;
////	cv::threshold(rendered_image, rendered_image, 254, 255, CV_THRESH_BINARY);
////	cv::distanceTransform(rendered_image, rendered_dist_map, CV_DIST_L2, 3);
//	rendered_dist_map.convertTo(rendered_dist_map, CV_64F);
//
//	/*
//	// compute the squared difference
//	cv::Mat diff_mat;
//	cv::reduce((reference_dist_map - rendered_dist_map).mul(reference_dist_map - rendered_dist_map), diff_mat, 0, CV_REDUCE_SUM);
//	cv::reduce(diff_mat, diff_mat, 1, CV_REDUCE_SUM);
//
//	return diff_mat.at<double>(0, 0);
//	*/
//
//	double diff = 0.0f;
//	int count = 0;
//	for (int r = 0; r < rendered_dist_map.rows; ++r) {
//		for (int c = 0; c < rendered_dist_map.cols; ++c) {
//			if (rendered_dist_map.at<double>(r, c) == 0) {
//				diff += reference_dist_map.at<double>(r, c) * reference_dist_map.at<double>(r, c);
//				count++;
//			}
//			if (reference_dist_map.at<double>(r, c) == 0) {
//				diff += rendered_dist_map.at<double>(r, c) * rendered_dist_map.at<double>(r, c);
//				count++;
//			}
//		}
//	}
//
//	return diff / count * 0.5;
//}

/**
 * Update the geometry using the currently selected grammars and the PM parameter values.
 * Note: this function updates the geometry on GPU, so you need to call update() to actually render it on the screen.
 *
 * @param grammar		grammar
 * @param pm_params		PM parameter values
 */
void GLWidget3D::updateGeometry() {
    // set param values
//	cga::setParamValues(grammars["mass"][grammar_ids["mass"]], pm_params["mass"]);
    if (grammar_type == GRAMMAR_TYPE_FACADE && grammar_ids.contains("facade")) {
//		cga::setParamValues(grammars["facade"][grammar_ids["facade"]], pm_params["facade"]);
    }

    // setup CGA
//	cga::CGA cga;
//	cga.modelMat = glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0));

    // set axiom
//	boost::shared_ptr<cga::Shape> start = boost::shared_ptr<cga::Shape>(new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -(float)vp::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1)));
//	cga.stack.push_back(start);

    // combine all the grammar snippets
//	std::vector<cga::Grammar> grammar_list;
//	grammar_list.push_back(grammars["mass"][grammar_ids["mass"]]);
    if (grammar_type == GRAMMAR_TYPE_FACADE && grammar_ids.contains("facade")) {
        // add facade grammar snippet
//		grammar_list.push_back(grammars["facade"][grammar_ids["facade"]]);

        // add window grammar snippets
        for (int k = 0; k < 99; ++k) {
            char window_nt_name[256];
            sprintf(window_nt_name, "Window%d", k);

            if (!grammar_ids.contains(window_nt_name)) break;

//			grammar_list.push_back(grammars["window"][grammar_ids[window_nt_name]]);

            // rename the axiom of the window grammar snippet
//			for (auto it = grammar_list.back().rules.begin(); it != grammar_list.back().rules.end(); ++it) {
//				if (it->first == "Window") {
//					cga::Rule rule = it->second;
//					grammar_list.back().rules.erase(it);
//					grammar_list.back().rules[window_nt_name] = rule;
//					break;
//				}
//			}
        }

        // add door grammar snippet
        //grammar_list.push_back(grammars["door"][0]);

        // add material grammar snippet
//		grammars["material"][grammar_ids["material"]].attrs["facade_color"].value = facade_color.toUtf8().constData();
        for (int i = 0; i < facade_colors.size(); i++) {
            QString s = QString("facade_color%1").arg(i);
//			grammars["material"][grammar_ids["material"]].attrs[s.toUtf8().constData()].value = facade_colors[i].toUtf8().constData();
        }
//		grammar_list.push_back(grammars["material"][grammar_ids["material"]]);
    }

    // generate 3d model
    //std::vector<boost::shared_ptr<glutils::Face>> faces;
//	faces.clear();
//	cga.derive(grammar_list, true);
//	cga.generateGeometry(faces, false);
//	glutils::BoundingBox bbox = cga::computeBoundingBox(faces);
//	for (int i = 0; i < faces.size(); ++i) {
//		for (int j = 0; j < faces[i]->vertices.size(); ++j) {
//			faces[i]->vertices[j].position -= bbox.center();
//		}
//	}
//	renderManager.removeObjects();
//	renderManager.addFaces(faces, true);

    // add ground plane
    //if (showGroundPlane) {
//	std::vector<Vertex> vertices;
//	glutils::drawQuad(200, 200, glm::vec4(1, 1, 1, 1), glm::translate(glm::rotate(glm::mat4(), (float)(-3.14159265 * 0.5), glm::vec3(1, 0, 0)), glm::vec3(0, 0, -bbox.center().y)), vertices);
//	renderManager.addObject("ground_plane", "", vertices, true);
    //}

    // update shadow map
//	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

//	geometryGenerated = true;
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
//void GLWidget3D::setupCamera(const std::vector<float>& params, float xrotMax, float xrotMin, float yrotMax, float yrotMin, float zrotMax, float zrotMin, float fovMax, float fovMin, float oxMax, float oxMin, float oyMax, float oyMin, float xMax, float xMin, float yMax, float yMin) {
//	camera.xrot = params[0] * (xrotMax - xrotMin) + xrotMin;
//	camera.yrot = params[1] * (yrotMax - yrotMin) + yrotMin;
//	camera.zrot = params[2] * (zrotMax - zrotMin) + zrotMin;
//	camera.fovy = params[3] * (fovMax - fovMin) + fovMin;
//	camera.center.x = params[4] * (oxMax - oxMin) + oxMin;
//	camera.center.y = params[5] * (oyMax - oyMin) + oyMin;
//	camera.pos.x = params[6] * (xMax - xMin) + xMin;
//	camera.pos.y = params[7] * (xMax - xMin) + xMin;
//	camera.pos.z = camera.distanceBase / tan(vp::deg2rad(camera.fovy * 0.5));
//
//	camera.updatePMatrix(width(), height());
//}
//
//void GLWidget3D::textureMapping() {
//	// create texture folder
//	if (!QDir("textures").exists()) QDir().mkdir("textures");
//
//	// convert image to cv::Mat object
//	cv::Mat imageMat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
//	cv::cvtColor(imageMat, imageMat, cv::COLOR_BGRA2BGR);
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
//				glm::vec2 pp = utils::projectPoint(width(), height(), faces[fi]->vertices[vi].position, camera.mvpMatrix);
//				pts.push_back(cv::Point2f(pp.x, pp.y));
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
//				cv::Mat rectifiedImage = cvutils::rectify_image(imageMat, pts, transformMat, glm::length(pts3d[1] - pts3d[0]) / glm::length(pts3d[2] - pts3d[1]));
//				//rectifiedImage = cvutils::adjust_contrast(rectifiedImage);
//
//				rectified_images.push_back(rectifiedImage);
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
//	renderManager.removeObjects();
//	renderManager.addFaces(faces, true);
//	renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
//	update();
//}

void GLWidget3D::updateStatusBar() {
    char buff[256];

    QString format("rot=(%.3f, %.3f, %.3f), fov=%.3f, O=(%.6f, %.6f), pos=(%.3f, %.3f, %.3f), PM=(");
    sprintf(buff, format.toUtf8().constData(), camera.xrot, camera.yrot, camera.zrot, camera.fovy, camera.center.x,
            camera.center.y, camera.pos.x, camera.pos.y, camera.pos.z);

    QString msg(buff);

    int k = 0;
    if (grammar_type == GRAMMAR_TYPE_MASS) {
//		for (auto it = grammars["mass"][grammar_ids["mass"]].attrs.begin(); it != grammars["mass"][grammar_ids["mass"]].attrs.end(); ++it, ++k) {
//			if (k > 0) msg += ", ";
//			sprintf(buff, "%.3f", pm_params["mass"][k] * (it->second.range_end - it->second.range_start) + it->second.range_start);
//			msg += buff;
//		}
    } else if (grammar_type == GRAMMAR_TYPE_FACADE) {
//		for (auto it = grammars["facade"][grammar_ids["facade"]].attrs.begin(); it != grammars["facade"][grammar_ids["facade"]].attrs.end(); ++it, ++k) {
//			if (k > 0) msg += ", ";
//			sprintf(buff, "%.3f", pm_params["facade"][k] * (it->second.range_end - it->second.range_start) + it->second.range_start);
//			msg += buff;
//		}
    }
    msg += ")";

    mainWin->statusBar()->showMessage(msg);
}

/**
 * Resize the canvas size while keeping the image size.
 */
void GLWidget3D::resizeImageCanvasSize(QImage &image, int width, int height) {
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
            } else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 0) {
                pm_params["facade"][0] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            }
            updateGeometry();
            updateStatusBar();
            update();
            break;
        case Qt::Key_2:
            if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 1) {
                pm_params["mass"][1] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            } else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 1) {
                pm_params["facade"][1] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            }
            updateGeometry();
            updateStatusBar();
            update();
            break;
        case Qt::Key_3:
            if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 2) {
                pm_params["mass"][2] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            } else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 2) {
                pm_params["facade"][2] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            }
            updateGeometry();
            updateStatusBar();
            update();
            break;
        case Qt::Key_4:
            if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 3) {
                pm_params["mass"][3] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            } else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 3) {
                pm_params["facade"][3] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            }
            updateGeometry();
            updateStatusBar();
            update();
            break;
        case Qt::Key_5:
            if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 4) {
                pm_params["mass"][4] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            } else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 4) {
                pm_params["facade"][4] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            }
            updateGeometry();
            updateStatusBar();
            update();
            break;
        case Qt::Key_6:
            if (grammar_type == GRAMMAR_TYPE_MASS && pm_params["mass"].size() > 5) {
                pm_params["mass"][5] += 0.01 * (ctrlPressed ? 0.1 : 1) * (altPressed ? -1 : 1);
            } else if (grammar_type == GRAMMAR_TYPE_FACADE && pm_params["facade"].size() > 5) {
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

void GLWidget3D::keyReleaseEvent(QKeyEvent *e) {
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

    eye[0] = 0.0f;
    eye[1] = 0.0f;
    eye[2] = -3.0f;

    lookat[0] = 0.0f;
    lookat[1] = 0.0f;
    lookat[2] = 0.0f;

    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;

    glewExperimental = true;
    // init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    objReader.initShader();

//    qDebug() << "gDrawObjects has " << gDrawObjects.size() << " objects";
//    qDebug() << "the first object has " << gDrawObjects[0].numTriangles << " triangles to draw";
//
//    qDebug() << "materials has " << materials.size() << " numbers.";
//
//    qDebug() << "the index of materials are " << shapes[0].mesh.material_ids.size() << " numbers.";
//    qDebug() << "the index of indices in mesh are " << shapes[0].mesh.indices.size()<< " numbers.";

}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
    height = height ? height : 1;
    glViewport(0, 0, width, height);
//    camera.updatePMatrix(width, height);

//	renderManager.resize(width, height);

    // resize image
    float scale = std::min((float) width / imageOrig.width(), (float) height / imageOrig.height());
    QImage newImage = imageOrig.scaled(imageOrig.width() * scale, imageOrig.height() * scale);

    image = QImage(width, height, QImage::Format_RGB32);
    image.fill(QColor(255, 255, 255));
    QPainter painter(&image);
    painter.drawImage((width - newImage.width()) / 2, (height - newImage.height()) / 2, newImage);
}

void GLWidget3D::paintEvent(QPaintEvent *event) {
    // OpenGLで描画
    makeCurrent();

    // Paint
    QPainter painter(this);

    // draw background image
    if (image_loaded) {
        if (geometryGenerated) {
            painter.setOpacity(0.5f);
        } else {
            painter.setOpacity(1.0f);
        }
        painter.drawImage(0, 0, image);
    }

    // draw silhouette
    painter.setPen(QPen(silhouetteColor, silhouetteWidth));
    float scale = std::min((float) width() / imageOrig.width(), (float) height() / imageOrig.height());
    float image_size = std::max(imageOrig.width(), imageOrig.height()) * scale;
    for (auto line : silhouette) {
        int x1 = std::round(line.start.x * image_size + width() * 0.5);
        int y1 = std::round(line.start.y * image_size + height() * 0.5);
        int x2 = std::round(line.end.x * image_size + width() * 0.5);
        int y2 = std::round(line.end.y * image_size + height() * 0.5);
        painter.drawLine(x1, y1, x2, y2);
    }

    painter.end();

//	glEnable(GL_DEPTH_TEST);
}

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::RightButton) {
        camera.mouseDown(e->x(), e->y());
//        camera.mousePress(e->x(), e->y());
    } else if (e->button() & Qt::LeftButton) {
//        float scale = std::min((float) width() / imageOrig.width(), (float) height() / imageOrig.height());
//        float screen_size = std::max(imageOrig.width(), imageOrig.height()) * scale;
//        float x1 = (float) (e->x() - width() * 0.5) / screen_size;
//        float y1 = (float) (e->y() - height() * 0.5) / screen_size;
//
        if (shiftPressed) {
//            silhouette.push_back(vp::VanishingLine(x1, y1, x1, y1, vp::VanishingLine::TYPE_HORIZONTAL_RIGHT));
        } else if (altPressed) {
//            silhouette.push_back(vp::VanishingLine(x1, y1, x1, y1, vp::VanishingLine::TYPE_VERTICAL));
        } else {
//            silhouette.push_back(vp::VanishingLine(x1, y1, x1, y1, vp::VanishingLine::TYPE_HORIZONTAL_LEFT));
        }
    }
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::RightButton) { // Rotate
        camera.zoom(e->x(), e->y());
//        if (shiftPressed) { // Move
//            camera.move(e->x(), e->y());
//        } else {
//            camera.rotate(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
//        }
    } else if (e->buttons() & Qt::MidButton) { // Rotate around Z axis
        camera.rotateAroundZ(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
    } else if (e->buttons() & Qt::LeftButton) {
        if (silhouette.size() > 0) {
            float scale = std::min((float) width() / imageOrig.width(), (float) height() / imageOrig.height());
            float screen_size = std::max(imageOrig.width(), imageOrig.height()) * scale;
            float x = (float) (e->x() - width() * 0.5) / screen_size;
            float y = (float) (e->y() - height() * 0.5) / screen_size;

            silhouette.back().end = glm::vec2(x, y);
        } else {
            camera.rotate(e->x(), e->y());
        }
    }
//
    updateStatusBar();
    updateGL();
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
//    if (e->button() == Qt::LeftButton) {
//        if (silhouette.size() > 0 && silhouette.back().start == silhouette.back().end) {
//            silhouette.pop_back();
//        }
//    }
    camera.mouseUp();
    updateGL();
}


void GLWidget3D::wheelEvent(QWheelEvent *e) {
    //camera.changeFov(e->delta() * 0.05, (ctrlPressed ? 0.1 : 1), width(), height());
    updateStatusBar();
    //update();
    updateGL();
}
