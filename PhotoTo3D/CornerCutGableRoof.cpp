#include "CornerCutGableRoof.h"
#include "GLUtils.h"
#include "Cuboid.h"
#include "CornerCutRectangle.h"
#include "CylinderSide.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

CornerCutGableRoof::CornerCutGableRoof(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float slope, int cut_type, float cut_length, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, depth * 0.5f * tanf(slope / 180.0f * M_PI));
	this->_slope = slope;
	this->_cut_type = cut_type;
	this->_cut_length = cut_length;
	this->_color = color;
	this->_textureEnabled = false;
}

boost::shared_ptr<Shape> CornerCutGableRoof::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new CornerCutGableRoof(*this));
	copy->_name = name;
	return copy;
}

void CornerCutGableRoof::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	float longer_slope = _scope.y * 0.5f / cosf(_slope / 180.0f * M_PI);
	float short_side_half = (_scope.y - _cut_length) * 0.5f;
	float short_height = short_side_half * tanf(_slope / 180.0f * M_PI);
	float shorter_slope = short_side_half / cosf(_slope / 180.0f * M_PI);

	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - _cut_length, longer_slope, _color)));

		if (_cut_type == CORNER_CUT_CURVE) {
			mat = glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0));
			std::vector<glm::vec2> points;

			int slices = CIRCLE_SLICES / 4;
			for (int i = 0; i <= slices; ++i) {
				float theta = (float)i / slices * M_PI * 0.5f - M_PI * 0.5f;

				points.push_back(glm::vec2(_cut_length * cosf(theta), _cut_length + _cut_length * sinf(theta)));
			}
			points.push_back(glm::vec2(0, _cut_length));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("top"), _grammar_type, _pivot, mat, points, _color, _texture)));
		}
		else if (_cut_type == CORNER_CUT_STRAIGHT) {
			mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.25f, glm::vec3(0, 0, 1));
			std::vector<glm::vec2> points;
			points.push_back(glm::vec2(0, 0));
			points.push_back(glm::vec2(_cut_length * sqrt(2.0f), 0));
			points.push_back(glm::vec2(_cut_length * sqrt(2.0f) * 0.5f, _cut_length * 0.5f * sqrt(2.0f)));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("top"), _grammar_type, _pivot, mat, points, _color, _texture)));
		}

		mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, _cut_length, 0)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _cut_length, shorter_slope, _color)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - _cut_length, longer_slope, _color)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _cut_length, shorter_slope, _color)));
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));

		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y * 0.5f, _scope.z));

		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y - _cut_length, 0));
		points.push_back(glm::vec2(short_side_half, short_height));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_cut_length, 0));
		points.push_back(glm::vec2((_scope.y + _cut_length) * 0.5f, short_height));
		points.push_back(glm::vec2(_scope.y * 0.5f, _scope.z));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));
	}
}

void CornerCutGableRoof::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;

	if (_cut_length > _scope.x) {
		_cut_length = _scope.x;
	}
	if (_cut_length > _scope.y) {
		_cut_length = _scope.y;
	}
}

void CornerCutGableRoof::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	float longer_slope = _scope.y * 0.5f / cosf(_slope / 180.0f * M_PI);
	float short_side_half = (_scope.y - _cut_length) * 0.5f;
	float short_height = short_side_half * tanf(_slope / 180.0f * M_PI);

	// top
	{
		std::vector<Vertex> vertices;

		
	}

	// side
	{
		std::vector<Vertex> vertices;

		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));

		std::vector<glm::vec2> points;
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y, 0));
		points.push_back(glm::vec2(_scope.y * 0.5f, _scope.z));

		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));
		points.clear();
		points.push_back(glm::vec2(0, 0));
		points.push_back(glm::vec2(_scope.y - _cut_length, 0));
		points.push_back(glm::vec2(short_side_half, short_height));
		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}