#include "Asset.h"

namespace cga {

Asset::Asset() {
}

Asset::Asset(const std::vector<std::vector<glm::vec3> >& points, const std::vector<std::vector<glm::vec3> >& normals, const std::vector<std::vector<glm::vec2> >& texCoords) {
	this->points = points;
	this->normals = normals;
	this->texCoords = texCoords;
}

}