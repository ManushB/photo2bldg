#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class CornerCutRectangle : public Shape {
private:
	int _cut_type;
	float _cut_length;

public:
	CornerCutRectangle() {}
	CornerCutRectangle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, int cut_type, float cut_length, const glm::vec3& color);
	CornerCutRectangle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, int cut_type, float cut_length, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	boost::shared_ptr<Shape> hemisphere(const std::string& name);
	void offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes);
	boost::shared_ptr<Shape> pyramid(const std::string& name, float height);
	boost::shared_ptr<Shape> roofGable(const std::string& name, float angle);
	boost::shared_ptr<Shape> roofHip(const std::string& name, float angle);
	void setupProjection(int axesSelector, float texWidth, float texHeight);
	void size(float xSize, float ySize, float zSize, bool centered);
	void split(int splitAxis, const std::vector<float>& ratios, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects);
	boost::shared_ptr<Shape> taper(const std::string& name, float height, float top_ratio = 0.0f);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}

