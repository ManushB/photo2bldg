#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class Circle : public Shape {
private:
	float _texWidth;
	float _texHeight;

public:
	Circle() {}
	Circle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	boost::shared_ptr<Shape> hemisphere(const std::string& name);
	void offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes);
	boost::shared_ptr<Shape> pyramid(const std::string& name, float height);
	boost::shared_ptr<Shape> roofGable(const std::string& name, float angle);
	boost::shared_ptr<Shape> roofHip(const std::string& name, float angle);
	void setupProjection(int axesSelector, float texWidth, float texHeight);
	boost::shared_ptr<Shape> taper(const std::string& name, float height, float slope);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
