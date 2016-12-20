#include "Arch.h"
#include "CGA.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "ArchPrism.h"

namespace cga {

Arch::Arch(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color) {
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

Arch::Arch(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, height, 0);
	this->_color = color;
	this->_texture = texture;

	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(u1, v1);
	_texCoords[1] = glm::vec2(u2, v1);
	_texCoords[2] = glm::vec2(u2, v2);
	_texCoords[3] = glm::vec2(u1, v2);
	this->_textureEnabled = true;
}

boost::shared_ptr<Shape> Arch::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Arch(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> Arch::extrude(const std::string& name, float height) {
	if (_texCoords.size() >= 4) {
		return boost::shared_ptr<Shape>(new ArchPrism(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y));
	}
	else {
		return boost::shared_ptr<Shape>(new ArchPrism(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _color));
	}
}

void Arch::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// inner shape
	if (!inside.empty()) {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0));

		if (_texCoords.size() >= 4) {
			float u1 = -offsetDistance / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			float v1 = -offsetDistance / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
			float u2 = (_scope.x + offsetDistance) / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			float v2 = (_scope.y + offsetDistance) / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
			shapes.push_back(boost::shared_ptr<Shape>(new Arch(inside, _grammar_type, _pivot, mat, _scope.x + offsetDistance * 2, _scope.y + offsetDistance * 2, _color, _texture, u1, v1, u2, v2)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Arch(inside, _grammar_type, _pivot, mat, _scope.x + offsetDistance * 2, _scope.y + offsetDistance * 2, _color)));
		}
	}

	// border shape
	if (!border.empty()) {
		glm::vec2 new_scope(_scope.x + offsetDistance * 2, _scope.y + offsetDistance * 2);

		// left border
		float h1 = new_scope.y - new_scope.x * 0.5;
		if (h1 < 0) h1 = new_scope.y;
		if (_texCoords.size() >= 4) {
			float u1 = _texCoords[0].x;
			float v1 = _texCoords[0].y;
			float u2 = -offsetDistance / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			float v2 = h1 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, _modelMat, abs(offsetDistance), h1 - offsetDistance, _color, _texture, u1, v1, u2, v2)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, _modelMat, abs(offsetDistance), h1 - offsetDistance, _color)));
		}

		// right border
		glm::mat4 mat2 = glm::translate(_modelMat, glm::vec3(_scope.x + offsetDistance, 0, 0));
		if (_texCoords.size() >= 4) {
			float u1 = (_scope.x+offsetDistance) / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			float v1 = _texCoords[0].y;
			float u2 = _texCoords[1].x;
			float v2 = h1 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat2, abs(offsetDistance), h1 - offsetDistance, _color, _texture, u1, v1, u2, v2)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat2, abs(offsetDistance), h1 - offsetDistance, _color)));
		}

		// bottom border
		glm::mat4 mat3 = glm::translate(_modelMat, glm::vec3(-offsetDistance, 0, 0));
		if (_texCoords.size() >= 4) {
			float u1 = -offsetDistance / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			float v1 = _texCoords[0].y;
			float u2 = (_scope.x + offsetDistance) / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			float v2 = -offsetDistance / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat2, new_scope.x, abs(offsetDistance), _color, _texture, u1, v1, u2, v2)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat2, new_scope.x, abs(offsetDistance), _color)));
		}

		// arch border
		if (new_scope.y > h1) {
			float r1 = new_scope.x * 0.5;
			float r2 = _scope.x * 0.5;
			for (int i = 0; i < CIRCLE_SLICES / 2; ++i) {
				float theta1 = (float)i / CIRCLE_SLICES * 2 * M_PI;
				float theta2 = (float)(i + 1) / CIRCLE_SLICES * 2 * M_PI;

				std::vector<glm::vec2> pts;
				pts.push_back(glm::vec2(cosf(theta1) * r1 + r2, sinf(theta1) * r1 + h1 - offsetDistance));
				pts.push_back(glm::vec2(cosf(theta1) * r2 + r2, sinf(theta1) * r2 + h1 - offsetDistance));
				pts.push_back(glm::vec2(cosf(theta2) * r2 + r2, sinf(theta2) * r2 + h1 - offsetDistance));
				pts.push_back(glm::vec2(cosf(theta2) * r1 + r2, sinf(theta2) * r1 + h1 - offsetDistance));

				if (_texCoords.size() >= 4) {
					std::vector<glm::vec2> tex(pts.size());
					for (int k = 0; k < pts.size(); ++k) {
						tex[k].x = pts[k].x / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
						tex[k].y = pts[k].y / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
					}
					shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, _modelMat, pts, _color, _texture, tex)));
				}
				else {
					shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, _modelMat, pts, _color, _texture)));
				}
			}
		}
	}
}

void Arch::setupProjection(int axesSelector, float texWidth, float texHeight) {
	if (axesSelector == AXES_SCOPE_XY) {
		_texCoords.resize(4);
		_texCoords[0] = glm::vec2(0, 0);
		_texCoords[1] = glm::vec2(_scope.x / texWidth, 0);
		_texCoords[2] = glm::vec2(_scope.x / texWidth, _scope.y / texHeight);
		_texCoords[3] = glm::vec2(0, _scope.y / texHeight);
		_textureEnabled = true;
	}
	else {
		throw "Arch supports only scope.xy for setupProjection().";
	}
}

void Arch::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	std::vector<Vertex> vertices;

	// bottom part with rectangle shape
	float h1 = _scope.y - _scope.x * 0.5;
	if (h1 < 0) h1 = _scope.y;

	glm::mat4 mat1 = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, h1 * 0.5, 0));

	if (!_texture.empty() && _textureEnabled) {
		float v1 = _texCoords[1].y + (_texCoords[2].y - _texCoords[1].y) / _scope.y * h1;
		glutils::drawQuad(_scope.x, h1, _texCoords[0], _texCoords[1], glm::vec2(_texCoords[2].x, v1), glm::vec2(_texCoords[0].x, v1), mat1, vertices);
	}
	else {
		glutils::drawQuad(_scope.x, h1, glm::vec4(_color, opacity), mat1, vertices);
	}

	if (_scope.y > h1) {
		float r = _scope.y - h1;

		// upper part with semicircle shape
		glm::vec3 p0 = glm::vec3(_pivot * _modelMat * glm::vec4(_scope.x * 0.5, h1, 0, 1));
		glm::vec2 tex0;
		tex0.x = (_texCoords[0].x + _texCoords[1].x) * 0.5;
		tex0.y = h1 / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;

		glm::vec3 normal = glm::vec3(_pivot * _modelMat * glm::vec4(0, 0, 1, 0));

		for (int i = 0; i < CIRCLE_SLICES / 2; ++i) {
			float theta1 = (float)i / CIRCLE_SLICES * 2 * M_PI;
			float theta2 = (float)(i + 1) / CIRCLE_SLICES * 2 * M_PI;

			glm::vec4 p1(r * cosf(theta1) + r, r * sinf(theta1) + h1, 0.0f, 1.0f);
			glm::vec4 p2(r * cosf(theta2) + r, r * sinf(theta2) + h1, 0.0f, 1.0f);

			glm::vec2 tex1;
			tex1.x = p1.x / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			tex1.y = p1.y / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;
			glm::vec2 tex2;
			tex2.x = p2.x / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x;
			tex2.y = p2.y / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y;

			p1 = _pivot * _modelMat * p1;
			p2 = _pivot * _modelMat * p2;

			vertices.push_back(Vertex(p0, normal, glm::vec4(_color, opacity), tex0, 0));
			if (i < CIRCLE_SLICES / 2) {
				vertices.push_back(Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity), tex1, 1));
			}
			else {
				vertices.push_back(Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity), tex1, 0));
			}
			if (i > 0) {
				vertices.push_back(Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), tex2, 1));
			}
			else {
				vertices.push_back(Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), tex2, 0));
			}
		}
	}

	if (!_texture.empty() && _textureEnabled) {
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}
	else {
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}
