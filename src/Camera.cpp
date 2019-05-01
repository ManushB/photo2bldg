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