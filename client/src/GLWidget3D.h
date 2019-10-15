#pragma once

#include <GL/glew.h>
#include <GL/glu.h>
//#include "src/Shader.h"
//#include "src/Vertex.h"
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <QImage>
#include "Camera.h"
//#include "src/ShadowMapping.h"
//#include "src/RenderManager.h"
//#include "src/CGA.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include "src/Classifier.h"
//#include "src/Regression.h"
#include "VanishingPoint.h"
#include "OBJReader.h"

class  Client;

class GLWidget3D : public QGLWidget {
public:
	//static enum { PEN_TYPE_SILHOUETTE = 0 };
	enum { GRAMMAR_TYPE_MASS = 0, GRAMMAR_TYPE_FACADE };

	Client* mainWin;
	OBJReader objReader;

    double prevMouseX, prevMouseY;
    bool mouseLeftPressed;
    bool mouseMiddlePressed;
    bool mouseRightPressed;
    float curr_quat[4];
    float prev_quat[4];
    float eye[3], lookat[3], up[3];

    // camera
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;

	// rendering engine
//	RenderManager renderManager;

	// key status
	bool ctrlPressed;
	bool shiftPressed;
	bool altPressed;

	// CNN and grammars
//	std::map<std::string, boost::shared_ptr<Classifier>> classifiers;
//	std::map<std::string, std::vector<boost::shared_ptr<Regression>>> regressions;
//	std::map<std::string, std::vector<cga::Grammar>> grammars;
	QMap<std::string, int> grammar_ids;
	std::map<std::string, std::vector<float>> pm_params;
	QString facade_color;
	std::vector<QString> facade_colors;
//	std::vector<boost::shared_ptr<glutils::Face>> faces;
	
	// GUI options
	float opacityOfBackground;
	bool image_loaded;
	QImage image;
	QImage imageOrig;
	int grammar_type;
	QColor horizontalLeftColor;
	QColor horizontalRightColor;
	QColor verticalColor;
	std::vector<vp::VanishingLine> silhouette;
	int silhouetteWidth;
	QColor silhouetteColor;
	bool geometryGenerated;

	// Inputs to QProcess
	QString inputImg;
	QString inputCtr;
	QString inputObj;
    QString wrkDir;
    QString inputPrm;


public:
	GLWidget3D(QWidget *parent);

//	void drawScene();
//	void render();
	void clearBackground();
	void loadImage(const QString& filename);
	void clearSilhouette();
	void loadSilhouette(const QString& filename);
	void saveSilhouette(const QString& filename);
	void clearGeometry();
	void loadCGA(const QString& filename);
	void saveCGA(const QString& filename);
    void loadOBJ(const QString& filename);
    void saveOBJ(const QString& filename);
	void undo();
//  bool renderImage(cga::Grammar* grammar, std::vector<float>* pm_params, cv::Mat& rendered_image, bool discardIfTopFaceIsVisible = false, bool discardIfBottomFaceIsVisible = false);
//	double distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map);
	void updateGeometry();
	void updateStatusBar();
	void resizeImageCanvasSize(QImage& image, int width, int height);

    void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void execProcess();


protected:
	void initializeGL();
	void resizeGL(int width, int height);
    void paintGL();
    void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent* e);
};

