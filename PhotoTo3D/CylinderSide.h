#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class CylinderSide : public Shape {
private:
	float _radius_x;
	float _radius_y;
	float _angle;

public:
	CylinderSide() {}
	CylinderSide(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float radius_x, float radius_y, float height, float angle, const glm::vec3& color);
	CylinderSide(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float radius_x, float radius_y, float height, float angle, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	void offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes);
	void setupProjection(int axesSelector, float texWidth, float texHeight);
	void size(float xSize, float ySize, float zSize, bool centered);
	void split(int splitAxis, const std::vector<float>& ratios, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}

