#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera {
public:
	Camera();

	void mousePress(int mouse_x, int mouse_y);
	void rotate(int mouse_x, int mouse_y, float senstivity);
	void rotateAroundZ(int mouse_x, int mouse_y, float senstivity);
	void zoom(float delta);
	void changeFov(float delta, float senstivity, int width, int height);
	void move(int mouse_x, int mouse_y);
	void updatePMatrix(int width,int height);
	void updateMVPMatrix();
	float f();

    // 3D object camera
    void setWindowSize(int w, int h);
    void mouseDown(int cx, int cy);
    void rotate(int x, int y);
    void zoom(int x, int y);
    void mouseUp();
    double rt[16];
    double z;

public:
	glm::vec2 center;
	float fovy;
	glm::vec3 pos;
	float xrot;
	float yrot;
	float zrot;

	float distanceBase;

	glm::vec2 mouse_pos;

public:
	glm::mat4 mvMatrix;
	glm::mat4 pMatrix;
	glm::mat4 mvpMatrix;


private:
    // 3D object camera
    void qmul(double r[], const double p[], const double q[]);
    void qrot(double r[], double q[]);
    int w;
    int h;
    int cx;
    int cy;
    double cq[4];
    double tq[4];

};

