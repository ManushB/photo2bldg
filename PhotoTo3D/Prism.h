#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class Prism : public Shape {
private:
	std::vector<glm::vec2> _points;
	float _texWidth;
	float _texHeight;

public:
	Prism() {}
	Prism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float height, const glm::vec3& color);
	Prism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float height, const glm::vec3& color, const std::string& texture, float texWidth, float texHieght);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes);
	void setupProjection(float texWidth, float texHeight);
	void size(float xSize, float ySize, float zSize);
	void split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
