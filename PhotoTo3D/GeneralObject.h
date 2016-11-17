#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class GeneralObject : public Shape {
private:
	std::vector<std::vector<glm::vec3> > _points;
	std::vector<std::vector<glm::vec3> > _normals;
	std::vector<std::vector<glm::vec2> > _texCoords;

public:
	GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const glm::vec3& color);
	GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<std::vector<glm::vec3> >& points, const std::vector<std::vector<glm::vec3> >& normals, const glm::vec3& color);
	GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const glm::vec3& color, const std::vector<glm::vec2>& texCoords, const std::string& texture);
	GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<std::vector<glm::vec3> >& points, const std::vector<std::vector<glm::vec3> >& normals, const glm::vec3& color, const std::vector<std::vector<glm::vec2> >& texCoords, const std::string& texture);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void size(float xSize, float ySize, float zSize);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
