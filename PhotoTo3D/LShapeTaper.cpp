#include "LShapeTaper.h"
#include "GLUtils.h"
#include "Cuboid.h"
#include "CornerCutRectangle.h"
#include "CylinderSide.h"
#include "LShape.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

LShapeTaper::LShapeTaper(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, float slope, float front_width, float right_width, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_slope = slope;
	this->_front_width = front_width;
	this->_right_width = right_width;
	this->_color = color;
	this->_textureEnabled = false;

	if (height / tanf(_slope / 180.0f * M_PI) > std::min(front_width, right_width) * 0.5f) {
		this->_scope.z = std::min(front_width, right_width) * 0.5f * tanf(_slope / 180.0f * M_PI);
	}
}

boost::shared_ptr<Shape> LShapeTaper::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new LShapeTaper(*this));
	copy->_name = name;
	return copy;
}

void LShapeTaper::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	float offset = _scope.z / tanf(_slope / 180.0f * M_PI);
	
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		if (std::min(_front_width, _right_width) * 0.5f > offset) {
			glm::mat4 mat = glm::translate(_modelMat, glm::vec3(offset, offset, _scope.z));
			shapes.push_back(boost::shared_ptr<Shape>(new LShape(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _scope.y - offset * 2, _front_width - offset * 2, _right_width - offset * 2, _color)));
		}
		else if (_front_width * 0.5f > offset) {
			glm::mat4 mat = glm::translate(_modelMat, glm::vec3(offset, offset, _scope.z));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _front_width - offset * 2, _scope.y - offset * 2, _color)));
		}
		else if (_right_width * 0.5f > offset) {
			glm::mat4 mat = glm::translate(_modelMat, glm::vec3(offset, _scope.y - _right_width + offset, _scope.z));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _right_width - offset * 2, _color)));
		}
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		glm::mat4 mat = glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_front_width, 0));
		points.push_back(glm::vec2(_front_width - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y - _right_width, 0));
		points.push_back(glm::vec2(_scope.y - _right_width + offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, _scope.y - _right_width, 0)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.x - _front_width, 0));
		points.push_back(glm::vec2(_scope.x - _front_width - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(-offset, offset / cosf(_slope / 180.0f * M_PI)));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y - _right_width, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_right_width, 0));
		points.push_back(glm::vec2(_right_width - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.x, 0));
		points.push_back(glm::vec2(_scope.x - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));
	}
}

void LShapeTaper::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

void LShapeTaper::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	float offset = _scope.z / tanf(_slope / 180.0f * M_PI);

	// top face
	if (std::min(_front_width, _right_width) * 0.5f > offset) {
		std::vector<Vertex> vertices;

		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_front_width - offset * 2, 0));
		points.push_back(glm::vec2(_front_width - offset * 2, _scope.y - _right_width));
		points.push_back(glm::vec2(_scope.x - offset * 2, _scope.y - _right_width));
		points.push_back(glm::vec2(_scope.x - offset * 2, _scope.y - offset * 2));
		points.push_back(glm::vec2(0, _scope.y - offset * 2));

		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(offset, offset, _scope.z));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
	else if (_front_width * 0.5f > offset) {
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_front_width * 0.5f, _scope.y * 0.5f, _scope.z));
		glutils::drawQuad(_front_width - offset * 2, _scope.y - offset * 2, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
	else if (_right_width * 0.5f > offset) {
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5f, _scope.y - _right_width * 0.5f, _scope.z));
		glutils::drawQuad(_scope.x - offset * 2, _right_width - offset * 2, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
	
	// side faces
	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_front_width, 0));
		points.push_back(glm::vec2(_front_width - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y - _right_width, 0));
		points.push_back(glm::vec2(_scope.y - _right_width + offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, _scope.y - _right_width, 0)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.x - _front_width, 0));
		points.push_back(glm::vec2(_scope.x - _front_width - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(-offset, offset / cosf(_slope / 180.0f * M_PI)));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y - _right_width, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_right_width, 0));
		points.push_back(glm::vec2(_right_width - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.x, 0));
		points.push_back(glm::vec2(_scope.x - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y - offset, offset / cosf(_slope / 180.0f * M_PI)));
		points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}