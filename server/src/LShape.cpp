#include "LShape.h"
#include "GLUtils.h"
#include "Pyramid.h"
#include "HipRoof.h"
#include "GableRoof.h"
#include "LShapePrism.h"
#include "LShapeTaper.h"
#include "Cuboid.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

LShape::LShape(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, float front_width, float right_width, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, height, 0);
	this->_front_width = front_width;
	this->_right_width = right_width;
	this->_color = color;
	this->_textureEnabled = false;
}

boost::shared_ptr<Shape> LShape::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new LShape(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> LShape::extrude(const std::string& name, float height) {
	return boost::shared_ptr<Shape>(new LShapePrism(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _front_width, _right_width, _color));
}

boost::shared_ptr<Shape> LShape::hemisphere(const std::string& name) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), (_scope.x + _scope.y) * 0.25, 0, _color, _texture));
}

void LShape::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// inner shape
	if (!inside.empty()) {
		float offset_width = _scope.x + offsetDistance * 2.0f;
		float offset_height = _scope.y + offsetDistance * 2.0f;
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0));
		/*if (_textureEnabled) {
		float offset_u1 = _texCoords[0].x;
		float offset_v1 = _texCoords[0].y;
		float offset_u2 = _texCoords[2].x;
		float offset_v2 = _texCoords[2].y;
		if (offsetDistance < 0) {
		float offset_u1 = (_texCoords[2].x - _texCoords[0].x) * (-offsetDistance) / _scope.x + _texCoords[0].x;
		float offset_v1 = (_texCoords[2].y - _texCoords[0].y) * (-offsetDistance) / _scope.y + _texCoords[0].y;
		float offset_u2 = (_texCoords[2].x - _texCoords[0].x) * (_scope.x + offsetDistance) / _scope.x + _texCoords[0].x;
		float offset_v2 = (_texCoords[2].y - _texCoords[0].y) * (_scope.y + offsetDistance) / _scope.y + _texCoords[0].y;
		}
		shapes.push_back(boost::shared_ptr<Shape>(new UShape(inside, _grammar_type, _pivot, mat, offset_width, offset_height, _color, _texture, offset_u1, offset_v1, offset_u2, offset_v2)));
		}
		else {*/
		shapes.push_back(boost::shared_ptr<Shape>(new LShape(inside, _grammar_type, _pivot, mat, offset_width, offset_height, _front_width + offsetDistance * 2.0f, _right_width + offsetDistance * 2.0f, _color)));
		//}
	}

	// border shape
	if (!border.empty()) {
		if (offsetDistance < 0) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::translate(_modelMat, glm::vec3(0, 0, 0)), _front_width, -offsetDistance, _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, -offsetDistance, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _scope.y - _right_width + offsetDistance, -offsetDistance, _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::translate(_modelMat, glm::vec3(_front_width + offsetDistance, _scope.y - _right_width, 0)), _scope.x - _front_width - offsetDistance, -offsetDistance, _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y - _right_width - offsetDistance, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _right_width + offsetDistance * 2.0f, -offsetDistance, _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _scope.x, -offsetDistance, _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y + offsetDistance, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), _scope.y + offsetDistance * 2.0f, -offsetDistance, _color)));
		}
		else {
			// not supported
		}
	}
}

boost::shared_ptr<Shape> LShape::pyramid(const std::string& name, float height) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

boost::shared_ptr<Shape> LShape::roofGable(const std::string& name, float angle) {
	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_front_width, 0));
	points.push_back(glm::vec2(_front_width, _scope.y - _right_width));
	points.push_back(glm::vec2(_scope.x, _scope.y - _right_width));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));

	return boost::shared_ptr<Shape>(new GableRoof(name, _grammar_type, _pivot, _modelMat, points, angle, _color));
}

boost::shared_ptr<Shape> LShape::roofHip(const std::string& name, float angle) {
	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_front_width, 0));
	points.push_back(glm::vec2(_front_width, _scope.y - _right_width));
	points.push_back(glm::vec2(_scope.x, _scope.y - _right_width));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));

	return boost::shared_ptr<Shape>(new HipRoof(name, _grammar_type, _pivot, _modelMat, points, angle, _color));
}

void LShape::setupProjection(int axesSelector, float texWidth, float texHeight) {
	_texCoords.clear();;
	_texCoords.push_back(glm::vec2(0, 0));
	_texCoords.push_back(glm::vec2(_front_width / texWidth, 0));
	_texCoords.push_back(glm::vec2(_front_width / texWidth, (_scope.y - _right_width) / texHeight));
	_texCoords.push_back(glm::vec2(_scope.x / texWidth, (_scope.y - _right_width) / texHeight));
	_texCoords.push_back(glm::vec2(_scope.x / texWidth, _scope.y / texHeight));
	_texCoords.push_back(glm::vec2(0, _scope.y / texHeight));
}

void LShape::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_front_width *= xSize / _scope.x;
	_right_width *= ySize / _scope.y;

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

boost::shared_ptr<Shape> LShape::taper(const std::string& name, float height, float slope) {
	return boost::shared_ptr<Shape>(new LShapeTaper(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, slope, _front_width, _right_width, _color));
}

void LShape::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	if (_textureEnabled) {
		std::vector<Vertex> vertices;
		glutils::drawQuad(_front_width, _scope.y - _right_width, _texCoords[0], _texCoords[1], _texCoords[2], (_texCoords[0] + _texCoords[5]) * 0.5f, glm::translate(_pivot * _modelMat, glm::vec3(_front_width * 0.5f, (_scope.y - _right_width) * 0.5f, 0)), vertices);
		glutils::drawQuad(_scope.x, _right_width, (_texCoords[0] + _texCoords[5]) * 0.5f, _texCoords[3], _texCoords[4], _texCoords[5], glm::translate(_pivot * _modelMat, glm::vec3(_scope.x * 0.5, _scope.y - _right_width * 0.5, 0)), vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}
	else {
		std::vector<Vertex> vertices;
		glutils::drawQuad(_front_width, _scope.y - _right_width, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_front_width * 0.5f, (_scope.y - _right_width) * 0.5f, 0)), vertices);
		glutils::drawQuad(_scope.x, _right_width, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_scope.x * 0.5, _scope.y - _right_width * 0.5, 0)), vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}
}

}