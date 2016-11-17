#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"

class OBJLoader {
protected:
	OBJLoader() {}

public:
	static void load(const char* filename, std::vector<Vertex>& vertices);
	static bool load(const char* filename, std::vector<std::vector<glm::vec3> >& points, std::vector<std::vector<glm::vec3> >& normals, std::vector<std::vector<glm::vec2> >& texCoords);
};

