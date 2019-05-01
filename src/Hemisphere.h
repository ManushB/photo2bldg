#pragma once

#include "Shape.h"

namespace cga {

class Hemisphere : public Shape {
private:
	float _texWidth;
	float _texHeight;

public:
	Hemisphere() {}
	Hemisphere(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, const glm::vec3& color);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes);
	void setupProjection(int axesSelector, float texWidth, float texHeight);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
