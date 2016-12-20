#include "ArchPrism.h"
#include "GLUtils.h"
#include "Arch.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

ArchPrism::ArchPrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, const glm::vec3& color) {
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

ArchPrism::ArchPrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, const glm::vec3& color, const std::string& texture, float s1, float t1, float s2, float t2) {
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

boost::shared_ptr<Shape> ArchPrism::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new ArchPrism(*this));
	copy->_name = name;
	return copy;
}

void ArchPrism::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, 0, _scope.z));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Arch(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x, _scope.y, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Arch(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x, _scope.y, _color)));
		}
	}

	// bottom face
	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL" && _scope.z >= 0) {
		glm::mat4 mat1 = glm::translate(glm::rotate(_modelMat, M_PI, glm::vec3(1, 0, 0)), glm::vec3(0, -_scope.y, 0));
		glm::mat4 mat2 = glm::translate(glm::rotate(mat1, (float)(M_PI * 0.5), glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.x, 0));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Arch(name_map.at("bottom"), _grammar_type, _pivot, mat2, _scope.y, _scope.x, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Arch(name_map.at("bottom"), _grammar_type, _pivot, mat2, _scope.y, _scope.x, _color)));
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

	float h1 = _scope.y - _scope.x * 0.5;
	if (h1 < 0) h1 = _scope.y;

	// right face
	if (name_map.find("right") != name_map.end() && name_map.at("right") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color, _texture, _texCoords[1].x, _texCoords[1].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color)));
		}
	}

	// left face
	if (name_map.find("left") != name_map.end() && name_map.at("left") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, h1, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		if (_texCoords.size() >= 4) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color, _texture, _texCoords[1].x, _texCoords[1].y, _texCoords[2].x, _texCoords[2].y)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color)));
		}
	}

	// side of the arch face
	if (name_map.find("back") != name_map.end() && name_map.at("back") != "NIL") {
		if (_scope.y > h1) {
			float r = _scope.y - h1;

			for (int i = 0; i < CIRCLE_SLICES / 2; ++i) {
				float theta1 = (float)i / CIRCLE_SLICES * 2 * M_PI;
				float theta2 = (float)(i + 1) / CIRCLE_SLICES * 2 * M_PI;

				float l = r * sinf((theta2 - theta1) * 0.5f) * 2;

				glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(r + r * cosf(theta1), h1 + r * sinf(theta1), _scope.z)), (theta1 + theta2) * 0.5f, glm::vec3(0, 0, 1)), -M_PI * 0.5f, glm::vec3(0, 1, 0));
				if (_texCoords.size() >= 4) {
					float x1 = r * cosf(theta1) + r;
					float y1 = h1 + sinf(theta1);
					float x2 = r * cosf(theta2) + r;
					float y2 = h1 + sinf(theta2);

					float u1 = x1 / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
					float v1 = y1 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
					float u2 = x2 / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
					float v2 = y2 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;

					shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _grammar_type, _pivot, mat, fabs(_scope.z), l, _color, _texture, u1, v1, u2, v2)));
				}
				else {
					shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _grammar_type, _pivot, mat, fabs(_scope.z), l, _color)));
				}
			}
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
			glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			if (_texCoords.size() >= 4) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color, _texture, _texCoords[1].x, _texCoords[1].y, _texCoords[2].x, _texCoords[2].y)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color)));
			}
		}

		// left face
		if (name_map.find("left") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, h1, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			if (_texCoords.size() >= 4) {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color, _texture, _texCoords[1].x, _texCoords[1].y, _texCoords[2].x, _texCoords[2].y)));
			}
			else {
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, h1, fabs(_scope.z), _color)));
			}
		}

		// back face
		if (name_map.find("back") == name_map.end()) {
			if (_scope.y > h1) {
				float r = _scope.y - h1;

				for (int i = 0; i < CIRCLE_SLICES / 2; ++i) {
					float theta1 = (float)i / CIRCLE_SLICES * 2 * M_PI;
					float theta2 = (float)(i + 1) / CIRCLE_SLICES * 2 * M_PI;

					float l = r * sinf((theta2 - theta1) * 0.5f) * 2;

					glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(r + r * cosf(theta1), h1 + r * sinf(theta1), _scope.z)), (theta1 + theta2) * 0.5f, glm::vec3(0, 0, 1)), -M_PI * 0.5f, glm::vec3(0, 1, 0));
					if (_texCoords.size() >= 4) {
						float x1 = r * cosf(theta1) + r;
						float y1 = h1 + sinf(theta1);
						float x2 = r * cosf(theta2) + r;
						float y2 = h1 + sinf(theta2);

						float u1 = x1 / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
						float v1 = y1 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
						float u2 = x2 / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
						float v2 = y2 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;

						shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, fabs(_scope.z), l, _color, _texture, u1, v1, u2, v2)));
					}
					else {
						shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, fabs(_scope.z), l, _color)));
					}
				}
			}
		}
	}
}

void ArchPrism::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

#if 0
	// top
	{
		std::vector<Vertex> vertices;
		glutils::drawQuad(_front_width, _scope.y - _right_width, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_front_width * 0.5f, (_scope.y - _right_width) * 0.5f, _scope.z)), vertices);
		glutils::drawQuad(_scope.x, _right_width, glm::vec4(_color, opacity), glm::translate(_pivot * _modelMat, glm::vec3(_scope.x * 0.5, _scope.y - _right_width * 0.5, _scope.z)), vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}

	// base
	if (_scope.z >= 0) {
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), M_PI, glm::vec3(1, 0, 0));
		std::vector<Vertex> vertices;
		glutils::drawQuad(_scope.x, _right_width, glm::vec4(_color, opacity), glm::translate(mat, glm::vec3(_scope.x * 0.5f, _right_width * 0.5f, 0)), vertices);
		glutils::drawQuad(_front_width, _scope.y - _right_width, glm::vec4(_color, opacity), glm::translate(mat, glm::vec3(_front_width * 0.5f, _scope.y - _right_width * 0.5f, 0)), vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}

	// front
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width * 0.5f, 0, _scope.z * 0.5f)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_front_width, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_front_width, (_scope.y - _right_width) * 0.5f, _scope.z * 0.5f)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y - _right_width, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3((_scope.x +_front_width) * 0.5f, _scope.y - _right_width, _scope.z * 0.5f)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x - _front_width, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// back
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y, _scope.z * 0.5)), M_PI, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
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
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y - _right_width * 0.5, _scope.z * 0.5)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_right_width, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// left
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y * 0.5f, _scope.z * 0.5f)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y, _scope.z, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
#endif
}

}