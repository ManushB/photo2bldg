#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI	3.141592653589
#endif

Camera::Camera() {
	center = glm::vec2(0, 0);
	xrot = 50.0f;
	yrot = -45.0;
	zrot = 0.0f;
	pos = glm::vec3(0, 0, 93.30f);
	fovy = 45.0f;

	distanceBase = 25.0f;

    w = 1;
    h = 1;
    rt[ 0] = 1;
    rt[ 1] = 0;
    rt[ 2] = 0;
    rt[ 3] = 0;
    rt[ 4] = 0;
    rt[ 5] = 1;
    rt[ 6] = 0;
    rt[ 7] = 0;
    rt[ 8] = 0;
    rt[ 9] = 0;
    rt[10] = 1;
    rt[11] = 0;
    rt[12] = 0;
    rt[13] = 0;
    rt[14] = 0;
    rt[15] = 1;
    cq[0] = 1;
    cq[1] = 0;
    cq[2] = 0;
    cq[3] = 0;
    tq[0] = 1;
    tq[1] = 0;
    tq[2] = 0;
    tq[3] = 0;

    z = 1.0;
}

/**
 * Call this function when the mouse button is pressed.
 */
void Camera::mousePress(int mouse_x, int mouse_y) {
	mouse_pos = glm::vec2(mouse_x, mouse_y);
}

/**
 * Call this function whenever the mouse moves while rotating the model.
 */
void Camera::rotate(int mouse_x, int mouse_y, float senstivity) {
	xrot += (mouse_y - mouse_pos.y) * senstivity;
	yrot += (mouse_x - mouse_pos.x) * senstivity;
	updateMVPMatrix();

	mouse_pos = glm::vec2(mouse_x, mouse_y);
}

void Camera::rotateAroundZ(int mouse_x, int mouse_y, float senstivity) {
	zrot += (mouse_x - mouse_pos.x) * senstivity;
	updateMVPMatrix();

	mouse_pos = glm::vec2(mouse_x, mouse_y);
}

/**
 * Call this function whenever the mouse moves while zooming.
 */
void Camera::zoom(float delta) {
	pos.z -= delta;
	updateMVPMatrix();
}

void Camera::changeFov(float delta, float senstivity, int width, int height) {
	fovy += delta * senstivity;
	if (fovy < 5) fovy = 5;
	if (fovy > 160) fovy = 160;
	pos.z = distanceBase / tanf((float)fovy * 0.5 / 180.0f * M_PI);
	updatePMatrix(width, height);
}

/**
 * Call this function whenever the mouse moves while moving the model.
 */
void Camera::move(int mouse_x, int mouse_y) {
	pos.x -= (mouse_x - mouse_pos.x) * 0.1;
	pos.y += (mouse_y - mouse_pos.y) * 0.1;
	updateMVPMatrix();

	mouse_pos = glm::vec2(mouse_x, mouse_y);
}

/**
 * Update perspective projection matrix, and then, update the model view projection matrix.
 */
void Camera::updatePMatrix(int width,int height) {
	float aspect = (float)width / (float)height;
	float zfar = 1000.0f;
	float znear = 1.0f;
	float f = 1.0f / tan(fovy * M_PI / 360.0f);

	// projection行列
	// ただし、mat4はcolumn majorなので、転置した感じで初期構築する。
	// つまり、下記の一行目は、mat4の一列目に格納されるのだ。
	pMatrix = glm::mat4(
		 f/aspect,	0,								0,									0,
				0,	f,								0,						 			0,
			   -center.x,	-center.y,		(zfar+znear)/(znear-zfar),		           -1,
			    0,	0, (2.0f*zfar*znear)/(znear-zfar),									0);

	updateMVPMatrix();
}

/**
 * Update the model view projection matrix
 */
void Camera::updateMVPMatrix() {
	// create model view matrix
	mvMatrix = glm::mat4();
	//mvMatrix = glm::rotate(mvMatrix, zrot * (float)M_PI / 180.0f, glm::vec3(0, 0, 1));	// rotate around the viewing direction instead of Z axis
	mvMatrix = glm::translate(mvMatrix, -pos);
	mvMatrix = glm::rotate(mvMatrix, xrot * (float)M_PI / 180.0f, glm::vec3(1, 0, 0));
	mvMatrix = glm::rotate(mvMatrix, yrot * (float)M_PI / 180.0f, glm::vec3(0, 1, 0));
	mvMatrix = glm::rotate(mvMatrix, zrot * (float)M_PI / 180.0f, glm::vec3(0, 0, 1));

	// create model view projection matrix
	mvpMatrix = pMatrix * mvMatrix;
}

float Camera::f() {
	return 1.0f / tan(fovy / 180.0f * M_PI * 0.5);
}

void Camera::setWindowSize(int w, int h)
{
    this->w = w;
    this->h = h;
}

void Camera::mouseDown(int cx, int cy)
{
    this->cx = cx;
    this->cy = cy;
}

void Camera::rotate(int x, int y)
{
    double dx = (x - cx) / (double)w;
    double dy = (y - cy) / (double)h;

    double a = sqrt(dx * dx + dy * dy);
    if (a > 0) {
        double ar = a * 2.0 * 3.14159265 * 0.5;
        double as = sin(ar) / a;
        double dq[4] = { cos(ar), dy * as, dx * as, 0.0 };

        qmul(tq, dq, cq);
        qrot(rt, tq);
    }
}

void Camera::zoom(int x, int y)
{
    double dy = (y - cy) / (double)h * 10;
    cy = y;

    z -= dy;
}

void Camera::mouseUp()
{
    for (int i = 0; i < 4; ++i) {
        cq[i] = tq[i];
    }
}
void Camera::qmul(double r[], const double p[], const double q[])
{
    r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
    r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
    r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
    r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}

void Camera::qrot(double r[], double q[])
{
    double x2 = q[1] * q[1] * 2.0;
    double y2 = q[2] * q[2] * 2.0;
    double z2 = q[3] * q[3] * 2.0;
    double xy = q[1] * q[2] * 2.0;
    double yz = q[2] * q[3] * 2.0;
    double zx = q[3] * q[1] * 2.0;
    double xw = q[1] * q[0] * 2.0;
    double yw = q[2] * q[0] * 2.0;
    double zw = q[3] * q[0] * 2.0;

    r[ 0] = 1.0 - y2 - z2;
    r[ 1] = xy + zw;
    r[ 2] = zx - yw;
    r[ 4] = xy - zw;
    r[ 5] = 1.0 - z2 - x2;
    r[ 6] = yz + xw;
    r[ 8] = zx + yw;
    r[ 9] = yz - xw;
    r[10] = 1.0 - x2 - y2;
    r[ 3] = r[ 7] = r[11] = r[12] = r[13] = r[14] = 0.0;
    r[15] = 1.0;
}
