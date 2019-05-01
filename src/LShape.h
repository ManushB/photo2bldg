#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

namespace cga {

class LShape : public Shape {
private:
	float _front_width;
	float _right_width;

public:
	LShape() {}
	LShape(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, float front_width, float right_width, const glm::vec3& color);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	boost::shared_ptr<Shape> hemisphere(const std::string& name);
	void offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes);
	boost::shared_ptr<Shape> pyramid(const std::string& name, float height);
	boost::shared_ptr<Shape> roofGable(const std::string& name, float angle);
	boost::shared_ptr<Shape> roofHip(const std::string& name, float angle);
	void setupProjection(int axesSelector, float texWidth, float texHeight);
	void size(float xSize, float ySize, float zSize, bool centered);
	boost::shared_ptr<Shape> taper(const std::string& name, float height, float slope);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
