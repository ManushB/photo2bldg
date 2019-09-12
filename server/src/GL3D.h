#ifndef GL3D_H
#define GL3D_H

#include <GL/glew.h>
#include "Shader.h"
#include "Vertex.h"
#include <vector>
#include "Camera.h"
#include "ShadowMapping.h"
#include "RenderManager.h"
#include "CGA.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Classifier.h"
#include "Regression.h"
#include "VanishingPoint.h"
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtGui/QImage>
#include <QObject>
class GL3D;

class Server {
public:
    void openImage(const QString& filename);
    void openSilhouette(const QString& filename);
    void saveImage(const QString& filename);
    void saveObj(const QString& filename);
    void onBuildingReconstruction(bool* bool_params, int* int_params);

    GL3D* glWidget;
};

class GL3D : public QObject {
public:

    QImage image;
    QImage imageOrig;
    QImage canvas;
    QImage output;

    GLuint default_fb;
    GLuint default_texture;
    GLuint default_rbo;

    bool image_loaded;
    bool geometryGenerated;
    int silhouetteWidth;
    QColor silhouetteColor;

    enum { GRAMMAR_TYPE_MASS = 0, GRAMMAR_TYPE_FACADE };

//    Server *mainWin;

    // camera
    Camera camera;
    glm::vec3 light_dir;
    glm::mat4 light_mvpMatrix;

    // rendering engine
    RenderManager renderManager;

    // CNN and grammars
	std::map<std::string, boost::shared_ptr<Classifier>> classifiers;
	std::map<std::string, std::vector<boost::shared_ptr<Regression>>> regressions;
    std::map<std::string, std::vector<cga::Grammar>> grammars;
    QMap<std::string, int> grammar_ids;
    std::map<std::string, std::vector<float>> pm_params;
    QString facade_color;
    std::vector<QString> facade_colors;
    std::vector<boost::shared_ptr<glutils::Face>> faces;
    int grammar_type;
    std::vector<vp::VanishingLine> silhouette;

public:
    GL3D();
    void render();
//    void clearGeometry();
	void massReconstruction(bool automaticRecognition, int grammarId, int image_size, float cameraDistanceBase, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float zrotMin, float zrotMax, float fovMin, float fovMax, float oxMin, float oxMax, float oyMin, float oyMax, float xMin, float xMax, float yMin, float yMax, int silhouette_line_type, bool refinement, int maxIters, int refinement_method);
	void facadeReconstruction(bool automaticRecognition, int grammarId, bool adjustContrast, bool useMultileFacadeColors);
    bool renderImage(cga::Grammar* grammar, std::vector<float>* pm_params, cv::Mat& rendered_image, bool discardIfTopFaceIsVisible = false, bool discardIfBottomFaceIsVisible = false);
    double distanceMap(cv::Mat rendered_image, const cv::Mat& reference_dist_map);
	std::vector<boost::shared_ptr<glutils::Face>> updateGeometry(int grammar_type, cga::Grammar* mass_grammar, std::vector<float>* mass_params, cga::Grammar* facade_grammar = NULL, std::vector<float>* facade_params = NULL, cga::Grammar* window_grammar = NULL, std::vector<float>* window_params = NULL);
    void updateGeometry();
	void setupCamera(const std::vector<float>& params, float xrotMax, float xrotMin, float yrotMax, float yrotMin, float zrotMax, float zrotMin, float fovMax, float fovMin, float oxMax, float oxMin, float oyMax, float oyMin, float xMax, float xMin, float yMax, float yMin);
//	void textureMapping();
	void resizeImageCanvasSize(QImage& image, int width, int height);
    void drawScene();

    int width();
    int height();
    int update();
    QImage grabFrameBuffer();
    void convertFromGLImage(QImage &img, int w, int h, bool alpha_format, bool include_alpha);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintEvent();
};

#endif //GL3D_H
