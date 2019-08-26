#include "SemiCircle.h"
#include "CGA.h"
#include "GLUtils.h"

namespace cga {

SemiCircle::SemiCircle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, height, 0);
	this->_color = color;
	this->_textureEnabled = false;
}

boost::shared_ptr<Shape> SemiCircle::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new SemiCircle(*this));
	copy->_name = name;
	return copy;
}

void SemiCircle::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// inner shape
	if (!inside.empty()) {
		float offset_width = _scope.x + offsetDistance * 2.0f;
		float offset_height = _scope.y + offsetDistance;
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new SemiCircle(name, _grammar_type, _pivot, mat, offset_width, offset_height, _color)));
	}
	
	// border shape
	if (!border.empty()) {
		throw "border of offset is not supported by semicircle.";
	}
}

void SemiCircle::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	std::vector<Vertex> vertices;

	glm::vec3 p0 = glm::vec3(_pivot * _modelMat * glm::vec4(_scope.x * 0.5, 0, 0, 1));

	glm::vec3 normal = glm::vec3(_pivot * _modelMat * glm::vec4(0, 0, 1, 0));

	int numSlices = 12;
	for (int i = 0; i < numSlices; ++i) {
		float theta1 = (float)i / numSlices * M_PI;
		float theta2 = (float)(i + 1) / numSlices * M_PI;

		glm::vec4 p1(_scope.x * 0.5 * cosf(theta1) + _scope.x * 0.5, _scope.y * sinf(theta1), 0.0f, 1.0f);
		p1 = _pivot * _modelMat * p1;
		glm::vec4 p2(_scope.x * 0.5 * cosf(theta2) + _scope.x * 0.5, _scope.y * sinf(theta2), 0.0f, 1.0f);
		p2 = _pivot * _modelMat * p2;

		vertices.push_back(Vertex(p0, normal, glm::vec4(_color, opacity)));
		if (i < numSlices) {
			vertices.push_back(Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity), 1));
		} else {
			vertices.push_back(Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity)));
		}
		if (i > 0) {
			vertices.push_back(Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), 1));
		} else {
			vertices.push_back(Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity)));
		}
	}

	faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
}

}
