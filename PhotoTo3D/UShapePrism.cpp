#include "UShapePrism.h"
#include "GLUtils.h"
#include "Circle.h"
#include "Cuboid.h"
#include "UShape.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

UShapePrism::UShapePrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, float front_width, float back_depth, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_front_width = front_width;
	this->_back_depth = back_depth;
	this->_color = color;
	this->_textureEnabled = false;
}

boost::shared_ptr<Shape> UShapePrism::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new UShapePrism(*this));
	copy->_name = name;
	return copy;
}

void UShapePrism::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, 0, _scope.z));
		shapes.push_back(boost::shared_ptr<Shape>(new UShape(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x, _scope.y, _front_width, _back_depth, _color)));
	}

	// bottom face
	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL" && _scope.z >= 0) {
		shapes.push_back(boost::shared_ptr<Shape>(new UShape(name_map.at("bottom"), _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, _front_width, _back_depth, _color)));
	}

	// front face
	if (name_map.find("front") != name_map.end() && name_map.at("front") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _front_width, fabs(_scope.z), _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0)), _scope.y - _back_depth, fabs(_scope.z), _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, _scope.y - _back_depth, 0)), rot_angle, glm::vec3(1, 0, 0)), _scope.x - _front_width * 2, fabs(_scope.z), _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _front_width, _scope.y - _back_depth, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0)), _scope.y - _back_depth, fabs(_scope.z), _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _front_width, 0, 0)), rot_angle, glm::vec3(1, 0, 0)), _front_width, fabs(_scope.z), _color)));
	}

	// right face
	if (name_map.find("right") != name_map.end() && name_map.at("right") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
	}

	// left face
	if (name_map.find("left") != name_map.end() && name_map.at("left") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
	}

	// back face
	if (name_map.find("back") != name_map.end() && name_map.at("back") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}

		// front face
		if (name_map.find("front") == name_map.end()) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _front_width, fabs(_scope.z), _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0)), _scope.y - _back_depth, fabs(_scope.z), _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, _scope.y - _back_depth, 0)), rot_angle, glm::vec3(1, 0, 0)), _scope.x - _front_width * 2, fabs(_scope.z), _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _front_width, _scope.y - _back_depth, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0)), _scope.y - _back_depth, fabs(_scope.z), _color)));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _front_width, 0, 0)), rot_angle, glm::vec3(1, 0, 0)), _front_width, fabs(_scope.z), _color)));
		}

		// right face
		if (name_map.find("right") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
		}

		// left face
		if (name_map.find("left") == name_map.end()) {
			glm::mat4 mat = glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
		}

		// back face
		if (name_map.find("back") == name_map.end()) {
			glm::mat4 mat = glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
		}
	}
}

void UShapePrism::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	if (offsetDistance >= 0) {
		shapes.push_back(boost::shared_ptr<Shape>(new UShapePrism(inside, _grammar_type, _pivot, glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0)), _scope.x + offsetDistance * 2, _scope.y + offsetDistance * 2, _scope.z, _front_width + offsetDistance * 2, _back_depth + offsetDistance * 2, _color)));
	}
	else {
		// not supported
	}
}

void UShapePrism::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_front_width *= xSize / _scope.x;
	_back_depth *= ySize / _scope.y;

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

void UShapePrism::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	if (splitAxis == DIRECTION_X) {
		// not supported!!
	}
	else if (splitAxis == DIRECTION_Y) {
		// not supported!!
	}
	else {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new UShapePrism(names[i], _grammar_type, _pivot, mat, _scope.x, _scope.y, sizes[i], _front_width, _back_depth, _color)));
			}
			mat = glm::translate(mat, glm::vec3(0, 0, sizes[i]));
		}
	}
}

void UShapePrism::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	// top
	{
		std::vector<Vertex> vertices;
		glutils::drawQuad(_front_width, _scope.y - _back_depth, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_front_width * 0.5, (_scope.y - _back_depth) * 0.5, _scope.z)), vertices);
		glutils::drawQuad(_scope.x, _back_depth, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_scope.x * 0.5, _scope.y - _back_depth * 0.5, _scope.z)), vertices);
		glutils::drawQuad(_front_width, _scope.y - _back_depth, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_scope.x - _front_width * 0.5, (_scope.y - _back_depth) * 0.5, _scope.z)), vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// base
	if (_scope.z >= 0) {
		std::vector<Vertex> vertices;
		glutils::drawQuad(_front_width, _scope.y - _back_depth, glm::vec4(_color, opacity), glm::rotate(glm::translate(_pivot * _modelMat, glm::vec3(_front_width * 0.5, (_scope.y - _back_depth) * 0.5, 0)), M_PI, glm::vec3(1, 0, 0)), vertices);
		glutils::drawQuad(_scope.x, _back_depth, glm::vec4(_color, opacity), glm::rotate(glm::translate(_pivot * _modelMat, glm::vec3(_scope.x * 0.5, _scope.y - _back_depth * 0.5, 0)), M_PI, glm::vec3(1, 0, 0)), vertices);
		glutils::drawQuad(_front_width, _scope.y - _back_depth, glm::vec4(_color, opacity), glm::rotate(glm::translate(_pivot * _modelMat, glm::vec3(_scope.x - _front_width * 0.5, (_scope.y - _back_depth) * 0.5, 0)), M_PI, glm::vec3(1, 0, 0)), vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// front
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width * 0.5, 0, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_front_width, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y - _back_depth, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x - _front_width * 2, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _front_width * 0.5, 0, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_front_width, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, (_scope.y - _back_depth) * 0.5, _scope.z * 0.5)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y - _back_depth, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _front_width, (_scope.y - _back_depth) * 0.5, _scope.z * 0.5)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y - _back_depth, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// back
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, 0, _scope.z * 0.5)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// right
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y * 0.5, _scope.z * 0.5)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// left
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y * 0.5, 0, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}