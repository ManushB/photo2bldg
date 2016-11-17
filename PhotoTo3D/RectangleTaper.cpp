#include "RectangleTaper.h"
#include "GLUtils.h"
#include "Cuboid.h"
#include "CylinderSide.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

RectangleTaper::RectangleTaper(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, float slope, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_slope = slope;
	this->_color = color;
	this->_textureEnabled = false;

	if (height / tanf(_slope / 180.0f * M_PI) > std::min(width, depth) * 0.5f) {
		this->_scope.z = std::min(width, depth) * 0.5f * tanf(_slope / 180.0f * M_PI);
	}
}

boost::shared_ptr<Shape> RectangleTaper::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new RectangleTaper(*this));
	copy->_name = name;
	return copy;
}

void RectangleTaper::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	float offset = _scope.z / tanf(_slope / 180.0f * M_PI);
	
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		if (_scope.x > offset * 2 && _scope.y > offset * 2) {
			glm::mat4 mat = glm::translate(_modelMat, glm::vec3(offset, offset, _scope.z));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _scope.y - offset * 2, _color)));
		}
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		glm::mat4 mat = glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.x, 0));
		points.push_back(glm::vec2(_scope.x - offset, offset / cosf(_slope / 180.0f * M_PI)));
		if (_scope.x > offset * 2) {
			points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y - offset, offset / cosf(_slope / 180.0f * M_PI)));
		if (_scope.y > offset * 2) {
			points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.x, 0));
		points.push_back(glm::vec2(_scope.x - offset, offset / cosf(_slope / 180.0f * M_PI)));
		if (_scope.x > offset * 2) {
			points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y - offset, offset / cosf(_slope / 180.0f * M_PI)));
		if (_scope.y > offset * 2) {
			points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));
	}
}

void RectangleTaper::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

void RectangleTaper::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	float offset = _scope.z / tanf(_slope / 180.0f * M_PI);

	// top face
	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5f, _scope.y * 0.5f, _scope.z));
		glutils::drawQuad(_scope.x - offset * 2, _scope.y - offset * 2, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// side faces
	{
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
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
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y - offset, offset / cosf(_slope / 180.0f * M_PI)));
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