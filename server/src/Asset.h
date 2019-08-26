#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

namespace cga {

class Asset {
public:
	std::vector<std::vector<glm::vec3> > points;
	std::vector<std::vector<glm::vec3> > normals;
	std::vector<std::vector<glm::vec2> > texCoords;

public:
	Asset();
	Asset(const std::vector<std::vector<glm::vec3> >& points, const std::vector<std::vector<glm::vec3> >& normals, const std::vector<std::vector<glm::vec2> >& texCoords);
};

}
