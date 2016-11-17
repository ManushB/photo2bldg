#include "CornerCutTaper.h"
#include "GLUtils.h"
#include "Cuboid.h"
#include "CornerCutRectangle.h"
#include "CylinderSide.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

CornerCutTaper::CornerCutTaper(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, float slope, int cut_type, float cut_length, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_slope = slope;
	this->_cut_type = cut_type;
	this->_cut_length = cut_length;
	this->_color = color;
	this->_textureEnabled = false;

	if (height / tanf(_slope / 180.0f * M_PI) > _cut_length) {
		this->_scope.z = _cut_length * tanf(_slope / 180.0f * M_PI);
	}
}

boost::shared_ptr<Shape> CornerCutTaper::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new CornerCutTaper(*this));
	copy->_name = name;
	return copy;
}

void CornerCutTaper::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	float offset = _scope.z / tanf(_slope / 180.0f * M_PI);
	
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(offset, offset, _scope.z));
		if (_cut_type == CORNER_CUT_CURVE) {
			if (fabs(_cut_length - offset) < 0.01f) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _scope.y - offset * 2, _color)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _scope.y - offset * 2, _cut_type, _cut_length - offset, _color)));
			}
		}
		else if (_cut_type == CORNER_CUT_STRAIGHT) {
			float new_cut_length = _cut_length - offset * tanf(M_PI * 0.125f) * sqrt(2.0f);
			if (fabs(new_cut_length) < 0.01f) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _scope.y - offset * 2, _color)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x - offset * 2, _scope.y - offset * 2, _cut_type, new_cut_length, _color)));
			}
		}
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		if (_cut_type == CORNER_CUT_CURVE) {
			glm::mat4 mat = glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
			std::vector<glm::vec2> points;
			points.push_back(glm::vec2(0, 0));
			points.push_back(glm::vec2(_scope.x - _cut_length, 0));
			points.push_back(glm::vec2(_scope.x - _cut_length, offset / cosf(_slope / 180.0f * M_PI)));
			points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

			int slices = CIRCLE_SLICES / 4;
			for (int i = 0; i <= slices; ++i) {
				float theta1 = (float)i / slices * M_PI * 0.5f - M_PI * 0.5f;
				float theta2 = (float)(i + 1) / slices * M_PI * 0.5f - M_PI * 0.5f;

				glm::vec2 p1 = glm::vec2(_cut_length * cosf(theta1), _cut_length * sinf(theta1));
				glm::vec2 p2 = glm::vec2(_cut_length * cosf(theta2), _cut_length * sinf(theta2));
				glm::vec2 p3 = glm::vec2((_cut_length - offset) * cosf(theta1), (_cut_length - offset) * sinf(theta1));
				glm::vec2 p4 = glm::vec2((_cut_length - offset) * cosf(theta2), (_cut_length - offset) * sinf(theta2));
				float len = glm::length(p1 - p2);

				mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length + p1.x, _cut_length + p1.y, 0)), (theta1 + theta2) * 0.5f + M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
				points.clear();
				points.push_back(glm::vec2(0, 0));
				points.push_back(glm::vec2(len, 0));
				points.push_back(glm::vec2(len * 0.5f + len * 0.5f * (_cut_length - offset) / _cut_length, offset / cosf(_slope / 180.0f * M_PI)));
				points.push_back(glm::vec2(len * 0.5f - len * 0.5f * (_cut_length - offset) / _cut_length, offset / cosf(_slope / 180.0f * M_PI)));
				shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));
			}

			mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
			points.clear();
			points.push_back(glm::vec2(0, 0));
			points.push_back(glm::vec2(_scope.y - _cut_length, 0));
			points.push_back(glm::vec2(_scope.y - _cut_length - offset, offset / cosf(_slope / 180.0f * M_PI)));
			points.push_back(glm::vec2(0, offset / cosf(_slope / 180.0f * M_PI)));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

		}
		else if (_cut_type == CORNER_CUT_STRAIGHT) {
			glm::mat4 mat = glm::rotate(_modelMat, _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
			std::vector<glm::vec2> points;
			points.push_back(glm::vec2(0, 0));
			points.push_back(glm::vec2(_scope.x - _cut_length, 0));
			points.push_back(glm::vec2(_scope.x - _cut_length - offset * tanf(M_PI * 0.125f), offset / cosf(_slope / 180.0f * M_PI)));
			points.push_back(glm::vec2(offset, offset / cosf(_slope / 180.0f * M_PI)));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

			mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.25f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
			points.clear();
			points.push_back(glm::vec2(0, 0));
			points.push_back(glm::vec2(_cut_length * sqrt(2.0f), 0));
			points.push_back(glm::vec2(_cut_length * sqrt(2.0f) - offset * tanf(M_PI * 0.125f), offset / cosf(_slope / 180.0f * M_PI)));
			points.push_back(glm::vec2(offset * tanf(M_PI * 0.125f), offset / cosf(_slope / 180.0f * M_PI)));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));

			mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
			points.clear();
			points.push_back(glm::vec2(0, 0));
			points.push_back(glm::vec2(_scope.y - _cut_length, 0));
			points.push_back(glm::vec2(_scope.y - _cut_length - offset * tanf(M_PI * 0.25f), offset / cosf(_slope / 180.0f * M_PI)));
			points.push_back(glm::vec2(offset * tanf(M_PI * 0.125f), offset / cosf(_slope / 180.0f * M_PI)));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, points, _color, _texture)));
		}

		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _slope / 180.0f * M_PI, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> points;
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

void CornerCutTaper::size(float xSize, float ySize, float zSize, bool centered) {
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

void CornerCutTaper::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
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