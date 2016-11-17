#pragma once

#include <glew.h>
#include <QGLWidget>
#include <glm/glm.hpp>

class GLWidget3D;

class ShadowMapping {
public:
	int width;
	int height;

	int programId;

	uint fboDepth;
	uint textureDepth;

public:
	ShadowMapping();

	void init(int programId, int width, int height);
	void update(GLWidget3D* glWidget3D, const glm::vec3& light_dir, const glm::mat4& light_mvpMatrix);
};



