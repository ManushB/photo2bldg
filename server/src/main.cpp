#include "GL3D.h"
#include "XmlParser.h"
#include <QtWidgets/QApplication>
#include <EGL/egl.h>
#include <GL/glew.h>
#include <iostream>
#include <QFile>
#include <hiredis/hiredis.h>
#include <cjson/cJSON.h>

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

    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    }
    printf("Connected to redis\n");
    QApplication a(argc, argv);

    auto* s = new Server();
    s->glWidget = new GL3D();
    printf("Loaded Network Successfully\n");

    while(1)
   {
       const cJSON *sourceImg = NULL;
       const cJSON *sourceSilhouette = NULL;
       const cJSON *parametersFile = NULL;
       const cJSON *result = NULL;
       const cJSON *result_obj = NULL;
       const cJSON *job_id = NULL;
       redisReply *reply;
       reply = (redisReply *)redisCommand(c,"RPOP p2brequest");
       printf("RPOP: %s\n", reply->str);
       if (reply->str == NULL){
         printf("Reply was null. Sleeping \n");
         //freeReplyObject(reply);
         sleep(1);
	 continue;
       }
       cJSON *req_json = cJSON_Parse(reply->str);
       freeReplyObject(reply);
       if (req_json == NULL)
       {
           const char *error_ptr = cJSON_GetErrorPtr();
           if (error_ptr != NULL)
           {
               fprintf(stderr, "Error before: %s\n", error_ptr);
           }
       }
       sourceImg = cJSON_GetObjectItemCaseSensitive(req_json, "bldgImg");
       if (cJSON_IsString(sourceImg) && (sourceImg->valuestring != NULL))
       {
           printf("Source Image is \"%s\"\n", sourceImg->valuestring);
       }
       sourceSilhouette = cJSON_GetObjectItemCaseSensitive(req_json, "bldgCtr");
       if (cJSON_IsString(sourceSilhouette) && (sourceSilhouette->valuestring != NULL))
       {
           printf("Source Contour is \"%s\"\n", sourceSilhouette->valuestring);
       }
       parametersFile = cJSON_GetObjectItemCaseSensitive(req_json, "paramFile");
       if (cJSON_IsString(parametersFile) && (parametersFile->valuestring != NULL))
       {
           printf("Parameters File is \"%s\"\n", parametersFile->valuestring);
       }
       result = cJSON_GetObjectItemCaseSensitive(req_json, "outImg");
       if (cJSON_IsString(result) && (result->valuestring != NULL))
       {
           printf("Result is \"%s\"\n", result->valuestring);
       }
       result_obj = cJSON_GetObjectItemCaseSensitive(req_json, "outObj");
       if (cJSON_IsString(result_obj) && (result_obj->valuestring != NULL))
       {
           printf("Result obj is \"%s\"\n", result_obj->valuestring);
       }
       job_id = cJSON_GetObjectItemCaseSensitive(req_json, "jobID");
       if (cJSON_IsString(job_id) && (job_id->valuestring != NULL))
       {
           printf("Job ID is \"%s\"\n", job_id->valuestring);
       }

       XmlParser xmlpr;
       xmlpr.readFile(parametersFile->valuestring);

       s->openImage(sourceImg->valuestring);
       s->openSilhouette(sourceSilhouette->valuestring);
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
       s->saveImage(result->valuestring);
       s->saveObj(result_obj->valuestring);
       
       char* cmd; 
       sprintf(cmd, "SET %s 1", job_id->valuestring);
       redisCommand(c, cmd);
       cJSON_Delete(req_json);
   }

    redisFree(c);

    eglTerminate(eglDpy);

    // 6. Terminate EGL when finished
    return 0;
}
