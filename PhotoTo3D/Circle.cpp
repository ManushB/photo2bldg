#include "Circle.h"
#include "CGA.h"
#include "Cylinder.h"
#include "Polygon.h"
#include "Pyramid.h"
#include "Hemisphere.h"

namespace cga {

Circle::Circle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, height, 0);
	this->_color = color;

	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(0, 0);
	_texCoords[1] = glm::vec2(1, 0);
	_texCoords[2] = glm::vec2(1, 1);
	_texCoords[3] = glm::vec2(0, 1);
	this->_textureEnabled = false;
}

boost::shared_ptr<Shape> Circle::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Circle(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> Circle::extrude(const std::string& name, float height) {
	return boost::shared_ptr<Shape>(new Cylinder(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _color));
}

boost::shared_ptr<Shape> Circle::hemisphere(const std::string& name) {
	return boost::shared_ptr<Shape>(new Hemisphere(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, _color));
}

void Circle::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// inner shape
	if (!inside.empty()) {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0));

		shapes.push_back(boost::shared_ptr<Shape>(new Circle(inside, _grammar_type, _pivot, mat, _scope.x + offsetDistance * 2, _scope.y + offsetDistance * 2, _color)));
	}

	// border shape
	if (!border.empty()) {
		for (int i = 0; i < CIRCLE_SLICES; ++i) {
			float theta1 = (float)i / CIRCLE_SLICES * M_PI * 2.0f;
			float theta2 = (float)(i + 1) / CIRCLE_SLICES * M_PI * 2.0f;

			std::vector<glm::vec2> pts;
			pts.push_back(glm::vec2(cosf(theta1) * _scope.x * 0.5 + _scope.x * 0.5, sinf(theta1) * _scope.y * 0.5 + _scope.y * 0.5));
			pts.push_back(glm::vec2(cosf(theta2) * _scope.x * 0.5 + _scope.x * 0.5, sinf(theta2) * _scope.y * 0.5 + _scope.y * 0.5));
			pts.push_back(glm::vec2(cosf(theta2) * (_scope.x * 0.5 + offsetDistance) + _scope.x * 0.5, sinf(theta2) * (_scope.y * 0.5 + offsetDistance) + _scope.y * 0.5));
			pts.push_back(glm::vec2(cosf(theta1) * (_scope.x * 0.5 + offsetDistance) + _scope.x * 0.5, sinf(theta1) * (_scope.y * 0.5 + offsetDistance) + _scope.y * 0.5));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, _modelMat, pts, _color, _texture)));
		}

	}
}

boost::shared_ptr<Shape> Circle::pyramid(const std::string& name, float height) {
	std::vector<glm::vec2> points;
	for (int i = 0; i < CIRCLE_SLICES; ++i) {
		float theta = (float)i / CIRCLE_SLICES * M_PI * 2.0f;
		points.push_back(glm::vec2(cosf(theta) * _scope.x * 0.5 + _scope.x * 0.5, sinf(theta) * _scope.y * 0.5 + _scope.y * 0.5));
	}
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

boost::shared_ptr<Shape> Circle::roofGable(const std::string& name, float angle) {
	float height = (_scope.x + _scope.y) * 0.25f * tanf(angle / 180.0f * M_PI);
	
	std::vector<glm::vec2> points;
	for (int i = 0; i < CIRCLE_SLICES; ++i) {
		float theta = (float)i / CIRCLE_SLICES * M_PI * 2.0f;
		points.push_back(glm::vec2(cosf(theta) * _scope.x * 0.5 + _scope.x * 0.5, sinf(theta) * _scope.y * 0.5 + _scope.y * 0.5));
	}
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

boost::shared_ptr<Shape> Circle::roofHip(const std::string& name, float angle) {
	float height = (_scope.x + _scope.y) * 0.25f * tanf(angle / 180.0f * M_PI);

	std::vector<glm::vec2> points;
	for (int i = 0; i < CIRCLE_SLICES; ++i) {
		float theta = (float)i / CIRCLE_SLICES * M_PI * 2.0f;
		points.push_back(glm::vec2(cosf(theta) * _scope.x * 0.5 + _scope.x * 0.5, sinf(theta) * _scope.y * 0.5 + _scope.y * 0.5));
	}
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

void Circle::setupProjection(int axesSelector, float texWidth, float texHeight) {
	this->_texWidth = texWidth;
	this->_texHeight = texHeight;

	if (axesSelector == AXES_SCOPE_XY) {
		_texCoords.resize(4);
		_texCoords[0] = glm::vec2(0, 0);
		_texCoords[1] = glm::vec2(_scope.x / texWidth, 0);
		_texCoords[2] = glm::vec2(_scope.x / texWidth, _scope.y / texHeight);
		_texCoords[3] = glm::vec2(0, _scope.y / texHeight);
		_textureEnabled = true;
	}
	else {
		throw "Circle supports only scope.xy for setupProjection().";
	}
}

boost::shared_ptr<Shape> Circle::taper(const std::string& name, float height, float slope) {
	float top_ratio = std::min(1.0f, std::max(0.0f, 1.0f - height * 2.0f / tanf(slope / 180.0f * M_PI) / std::min(_scope.x, _scope.y)));

	std::vector<glm::vec2> points;
	for (int i = 0; i < CIRCLE_SLICES; ++i) {
		float theta = (float)i / CIRCLE_SLICES * M_PI * 2.0f;
		points.push_back(glm::vec2(cosf(theta) * _scope.x * 0.5 + _scope.x * 0.5, sinf(theta) * _scope.y * 0.5 + _scope.y * 0.5));
	}
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, top_ratio, _color, _texture));
}

void Circle::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	std::vector<Vertex> vertices;

	glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5f, _scope.y * 0.5f, 0));
	if (!_texture.empty()) {
		glutils::drawCircle(_scope.x * 0.5f, _scope.y * 0.5f, _texWidth, _texHeight, mat, vertices, CIRCLE_SLICES);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}
	else {
		glutils::drawCircle(_scope.x * 0.5f, _scope.y * 0.5f, glm::vec4(_color, opacity), mat, vertices, CIRCLE_SLICES);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}
