#pragma once

#include <GL/glew.h>
#include <GL/glu.h>
#include <QVector3D>
#include <vector>
#include <memory>
#include "Camera.h"
#include "util.h"


class OBJReader {

public:
    OBJReader(){
        shader = 0;
        uniXform = 0;
        tex_unit_location = 0;
        texture = 1;
    }
    typedef struct {
        GLuint vb_id;  // vertex buffer id
        int numTriangles;
        size_t material_id;
    } DrawObject;

    bool LoadObjAndConvert(const char* filename);
    void render(Camera camera, int width, int height);
    void initShader();

    std::vector<DrawObject> drawObjects;

    // 3D object render

    float maxExtent;
    float bmin[3], bmax[3];
    std::vector<GLuint> indices_roof;
    std::vector<GLuint> indices_window;
    std::vector<GLuint> indices_wall;

    GLuint shader;          // Shader program
    GLuint uniXform;        // Location of transformation matrix in shader
    GLuint tex_unit_location;

    GLuint vao;   // Vertex array object
    GLuint vao1;   // Vertex array object
    GLuint vbuf;  // Vertex buffer
    GLuint IBO;
    GLuint IBO1;

    GLuint vao2;   // Vertex array object
    GLuint vbuf1;  // Vertex buffer
    GLuint vbuf2;
    GLuint IBO2;

    GLuint texture;

    std::map<std::string, GLuint> textures;
};
