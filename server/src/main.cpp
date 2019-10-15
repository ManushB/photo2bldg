#include "GL3D.h"
#include "XmlParser.h"
#include <QtWidgets/QApplication>
#include <EGL/egl.h>
#include <GL/glew.h>
#include <iostream>
#include <QFile>

static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
};

static const int pbufferWidth = 9;
static const int pbufferHeight = 9;

static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
};



int main(int argc, char *argv[])
{
    // 1. Initialize EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;

    eglInitialize(eglDpy, &major, &minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
                                                 pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                         NULL);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

    if (argc <= 5 || argc > 8)
    {
        std::cerr << "Usage: Photo2Bldg [sourceImage] [sourceSilhouette] [paramtersFile] [outputImg] [outputObj]"
                  << std::endl;
        return 0;
    }
    QApplication a(argc, argv);

    const QString sourceImg = argv[1];
    const QString sourceSilhouette = argv[2];
    const QString result = argv[4];
    const QString result_obj = argv[5];
    QString parametersFile = argv[3];

    XmlParser xmlpr;
    xmlpr.readFile(parametersFile);

    auto* s = new Server();
    s->glWidget = new GL3D();
    s->openImage(sourceImg);
    s->openSilhouette(sourceSilhouette.toUtf8().constData());
    if (xmlpr.function == xmlpr.BUILDING_RECONSTRUCTION){
        s->onBuildingReconstruction(&xmlpr.buildRec);
    } else if (xmlpr.function == xmlpr.MASS_RECONSTRUCTION){
        s->onMassReconstruction(&xmlpr.massRec);
    } else if (xmlpr.function == xmlpr.FACADE_RECONSTRUCTION) {
        s->onMassReconstruction(&xmlpr.massRec);
        s->onFacadeReconstruction();
    } else {
        std::cout << "Function: " << xmlpr.function.toUtf8().constData() << " is not available" << std::endl;
    }
    s->saveImage(result);
    s->saveObj(result_obj);
    eglTerminate(eglDpy);

    // 6. Terminate EGL when finished
    return 0;
}
