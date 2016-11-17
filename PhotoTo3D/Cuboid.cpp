#include "Cuboid.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "CGA.h"
#include "GLUtils.h"

namespace cga {

Cuboid::Cuboid(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_color = color;

	this->_textureEnabled = false;
}

Cuboid::Cuboid(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, const glm::vec3& color, const std::string& texture, float s1, float t1, float s2, float t2) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_color = color;

	this->_texture = texture;
	this->_texCoords.push_back(glm::vec2(s1, t1));
	this->_texCoords.push_back(glm::vec2(s2, t1));
	this->_texCoords.push_back(glm::vec2(s2, t2));
	this->_texCoords.push_back(glm::vec2(s1, t2));
	this->_textureEnabled = true;
}

boost::shared_ptr<Shape> Cuboid::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Cuboid(*this));
	copy->_name = name;
	return copy;
}

void Cuboid::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, 0, _scope.z));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x, _scope.y, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x, _scope.y, _color)));
		}
	}

	// bottom face
	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL" && _scope.z >= 0) {
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("bottom"), _grammar_type, _pivot, glm::translate(glm::rotate(_modelMat, M_PI, glm::vec3(1, 0, 0)), glm::vec3(0, -_scope.y, 0)), _scope.x, _scope.y, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("bottom"), _grammar_type, _pivot, glm::translate(glm::rotate(_modelMat, M_PI, glm::vec3(1, 0, 0)), glm::vec3(0, -_scope.y, 0)), _scope.x, _scope.y, _color)));
		}
	}

	// front face
	if (name_map.find("front") != name_map.end() && name_map.at("front") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[1].x, _texCoords[1].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
		}
	}

	// right face
	if (name_map.find("right") != name_map.end() && name_map.at("right") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color, _texture, _texCoords[1].x, _texCoords[1].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
		}
	}

	// left face
	if (name_map.find("left") != name_map.end() && name_map.at("left") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y, 0, 0));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color, _texture, _texCoords[3].x, _texCoords[3].y, _texCoords[0].x, _texCoords[0].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
		}
	}

	// back face
	if (name_map.find("back") != name_map.end() && name_map.at("back") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color, _texture, _texCoords[2].x, _texCoords[2].y, _texCoords[3].x, _texCoords[3].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
		}
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}

		// front face
		if (name_map.find("front") == name_map.end()) {
			if (_texCoords.size() >= 4) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[1].x, _texCoords[1].y)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
			}
		}

		// right face
		if (name_map.find("right") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
			if (_texCoords.size() >= 4) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color, _texture, _texCoords[1].x, _texCoords[1].y, _texCoords[2].x, _texCoords[2].y)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
			}
		}

		// left face
		if (name_map.find("left") == name_map.end()) {
			glm::mat4 mat = glm::translate(glm::rotate(_modelMat, -M_PI * 0.5f, glm::vec3(0, 0, 1)), glm::vec3(-_scope.y, 0, 0));
			if (_texCoords.size() >= 4) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color, _texture, _texCoords[3].x, _texCoords[3].y, _texCoords[0].x, _texCoords[0].y)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.y, fabs(_scope.z), _color)));
			}
		}

		// back face
		if (name_map.find("back") == name_map.end()) {
			glm::mat4 mat = glm::translate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI, glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.y, 0));
			if (_texCoords.size() >= 4) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color, _texture, _texCoords[2].x, _texCoords[2].y, _texCoords[3].x, _texCoords[3].y)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(mat, rot_angle, glm::vec3(1, 0, 0)), _scope.x, fabs(_scope.z), _color)));
			}
		}
	}
}

void Cuboid::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	if (offsetDistance >= 0) {
		shapes.push_back(boost::shared_ptr<Shape>(new Cuboid(inside, _grammar_type, _pivot, glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0)), _scope.x + offsetDistance * 2, _scope.y + offsetDistance * 2, _scope.z, _color)));
	}
	else {
		// not supported
	}
}

void Cuboid::setupProjection(float texWidth, float texHeight) {
}

void Cuboid::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

/**
 */
void Cuboid::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	if (splitAxis == DIRECTION_X) {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new Cuboid(names[i], _grammar_type, _pivot, mat, sizes[i], _scope.y, _scope.z, _color)));
			}
			mat = glm::translate(mat, glm::vec3(sizes[i], 0, 0));
		}
	} else if (splitAxis == DIRECTION_Y) {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new Cuboid(names[i], _grammar_type, _pivot, mat, _scope.x, sizes[i], _scope.z, _color)));
			}
			mat = glm::translate(mat, glm::vec3(0, sizes[i], 0));
		}
	} else {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new Cuboid(names[i], _grammar_type, _pivot, mat, _scope.x, _scope.y, sizes[i], _color)));
			}
			mat = glm::translate(mat, glm::vec3(0, 0, sizes[i]));
		}
	}
}

void Cuboid::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	if (_scope.z == 0) return;
	int num = 0;
	
	// top
	if (_scope.x >= 0) {
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y * 0.5, _scope.z));
		glutils::drawQuad(_scope.x, _scope.y, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
	else {
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y * 0.5, _scope.z)), M_PI, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x, _scope.y, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// base
	if (_scope.z >= 0) {
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::translate(glm::rotate(_modelMat, M_PI, glm::vec3(1, 0, 0)), glm::vec3(_scope.x * 0.5, -_scope.y * 0.5, 0));
		glutils::drawQuad(_scope.x, _scope.y, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// front
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, 0, _scope.z * 0.5)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x, _scope.z, glm::vec4(_color, opacity), mat, vertices);
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
