#include "CylinderSide.h"
#include "GLUtils.h"
#include "Circle.h"
#include "Cylinder.h"
#include "LShape.h"
#include "Pyramid.h"
#include "HipRoof.h"
#include "GableRoof.h"
#include "Prism.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "Cuboid.h"
#include "SemiCircle.h"
#include "UShape.h"
#include "CGA.h"

namespace cga {

CylinderSide::CylinderSide(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float radius_x, float radius_y, float height, float angle, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_radius_x = radius_x;
	this->_radius_y = radius_y;
	this->_angle = angle;
	this->_scope = glm::vec3(radius_x * angle, height, 0);
	this->_color = color;

	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(0, 0);
	_texCoords[1] = glm::vec2(1, 0);
	_texCoords[2] = glm::vec2(1, 1);
	_texCoords[3] = glm::vec2(0, 1);
	this->_textureEnabled = false;
}

CylinderSide::CylinderSide(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float radius_x, float radius_y, float height, float angle, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_radius_x = radius_x;
	this->_radius_y = radius_y;
	this->_angle = angle;
	this->_scope = glm::vec3(radius_x * angle, height, 0);
	this->_color = color;

	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(u1, v1);
	_texCoords[1] = glm::vec2(u2, v1);
	_texCoords[2] = glm::vec2(u2, v2);
	_texCoords[3] = glm::vec2(u1, v2);
	this->_textureEnabled = true;
}

boost::shared_ptr<Shape> CylinderSide::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new CylinderSide(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> CylinderSide::extrude(const std::string& name, float height) {
	if (_angle < M_PI * 0.499) {
		// if the angle is small enough, approximate it by a flat surface
		glm::vec3 p2(_radius_x * sinf(_angle), 0, _radius_y * cosf(_angle) - _radius_y);
		float new_sx = glm::length(p2 - glm::vec3(0, 0, 0));

		glm::mat4 mat = glm::rotate(_modelMat, _angle * 0.5f, glm::vec3(0, 1, 0));

		if (_texCoords.size() >= 4) {
			return boost::shared_ptr<Shape>(new Cuboid(name, _grammar_type, _pivot, mat, new_sx, _scope.y, height, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y));
		}
		else {
			return boost::shared_ptr<Shape>(new Cuboid(name, _grammar_type, _pivot, mat, new_sx, _scope.y, height, _color));
		}
	}
	else {
		// HACK: use cylinder
		glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(-_radius_x - height, 0, height)), -M_PI * 0.5f, glm::vec3(1, 0, 0));
		return boost::shared_ptr<Shape>(new Cylinder(name, _grammar_type, _pivot, mat, (_radius_x + height) * 2, (_radius_y + height) * 2, _scope.y, _color));
	}
}

void CylinderSide::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// inner shape
	if (!inside.empty()) {
		// approximate by a flat surface
		float delta_theta = -offsetDistance / _radius_x;
		glm::vec3 p1(-offsetDistance * cosf(_angle * 0.5f), -offsetDistance, offsetDistance * sinf(_angle * 0.5f));

		glm::mat4 mat = glm::rotate(glm::translate(_modelMat, p1), _angle * 0.5f, glm::vec3(0, 1, 0));
		float width = _scope.x + offsetDistance * 2.0f;
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(inside, _grammar_type, _pivot, mat, width, _scope.y + offsetDistance * 2, _color)));
	}

	// border shape
	if (!border.empty() && offsetDistance < 0) {
		// approximate by a flat surface
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(_modelMat, _angle * 0.5f, glm::vec3(0, 1, 0)), _scope.x, -offsetDistance, _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y + offsetDistance, 0)), _angle * 0.5f, glm::vec3(0, 1, 0)), _scope.x, -offsetDistance, _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y + offsetDistance, 0)), _angle * 0.5f, glm::vec3(0, 1, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), _scope.y + offsetDistance * 2, -offsetDistance, _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(glm::rotate(_modelMat, _angle * 0.5f, glm::vec3(0, 1, 0)), glm::vec3(_scope.x, -offsetDistance, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _scope.y + offsetDistance * 2, -offsetDistance, _color)));
	}
}

void CylinderSide::setupProjection(int axesSelector, float texWidth, float texHeight) {
	if (axesSelector == AXES_SCOPE_XY) {
		_texCoords.resize(4);
		_texCoords[0] = glm::vec2(0, 0);
		_texCoords[1] = glm::vec2(_scope.x / texWidth, 0);
		_texCoords[2] = glm::vec2(_scope.x / texWidth, _scope.y / texHeight);
		_texCoords[3] = glm::vec2(0, _scope.y / texHeight);
	}
	else {
		throw "CylinderSide supports only scope.xy for setupProjection().";
	}
}

void CylinderSide::size(float xSize, float ySize, float zSize, bool centered) {
	/*if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}*/

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

void CylinderSide::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	float rot_y = 0.0f;
	float offset = 0.0f;

	if (splitAxis == DIRECTION_X) {
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL" && sizes[i] > 0) {
				glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_radius_x * sinf(rot_y), 0, _radius_y * cosf(rot_y) - _radius_y)), rot_y, glm::vec3(0, 1, 0));
				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new CylinderSide(names[i], _grammar_type, _pivot, mat, _radius_x, _radius_y, _scope.y, sizes[i] / _radius_x, _color, _texture,
						_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * offset / _scope.x, _texCoords[0].y,
						_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (offset + sizes[i]) / _scope.x, _texCoords[2].y)));
				}
				else {
					objects.push_back(boost::shared_ptr<Shape>(new CylinderSide(names[i], _grammar_type, _pivot, mat, _radius_x, _radius_y, _scope.y, sizes[i] / _radius_x, _color)));
				}
			}
			rot_y += (sizes[i] / _scope.x) * _angle;
			offset += sizes[i];
		}
	} else if (splitAxis == DIRECTION_Y) {
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL" && sizes[i] > 0) {
				glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, offset, 0));

				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new CylinderSide(names[i], _grammar_type, _pivot, mat, _radius_x, _radius_y, sizes[i], _angle, _color, _texture, _texCoords[0].x, _texCoords[1].y + (_texCoords[2].y - _texCoords[1].y) / _scope.y * offset, _texCoords[1].x, _texCoords[1].y + (_texCoords[2].y - _texCoords[1].y) / _scope.y * (offset + sizes[i]))));
				}
				else {
					objects.push_back(boost::shared_ptr<Shape>(new CylinderSide(names[i], _grammar_type, _pivot, mat, _radius_x, _radius_y, sizes[i], _angle, _color)));
				}
			}
			offset += sizes[i];
		}
	}
}

void CylinderSide::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	int slices = _angle / M_PI / 2.0f * CIRCLE_SLICES;
	if (slices <= 0) slices = 1;

	std::vector<Vertex> vertices;

	for (int i = 0; i < slices; ++i) {
		float theta1 = (float)i / slices * _angle;
		float theta2 = (float)(i + 1) / slices * _angle;

		glm::vec3 p1(_radius_x * sinf(theta1), 0, _radius_y * cosf(theta1) - _radius_y);
		glm::vec3 p2(_radius_x * sinf(theta2), 0, _radius_y * cosf(theta2) - _radius_y);
		glm::vec3 p3(_radius_x * sinf(theta2), _scope.y, _radius_y * cosf(theta2) - _radius_y);
		glm::vec3 p4(_radius_x * sinf(theta1), _scope.y, _radius_y * cosf(theta1) - _radius_y);

		p1 = glm::vec3(_pivot * _modelMat * glm::vec4(p1, 1));
		p2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, 1));
		p3 = glm::vec3(_pivot * _modelMat * glm::vec4(p3, 1));
		p4 = glm::vec3(_pivot * _modelMat * glm::vec4(p4, 1));

		glm::vec3 n1(sinf(theta1), 0, cosf(theta1));
		glm::vec3 n2(sinf(theta2), 0, cosf(theta2));
		glm::vec3 n3(sinf(theta2), 0, cosf(theta2));
		glm::vec3 n4(sinf(theta1), 0, cosf(theta1));

		n1 = glm::vec3(_pivot * _modelMat * glm::vec4(n1, 0));
		n2 = glm::vec3(_pivot * _modelMat * glm::vec4(n2, 0));
		n3 = glm::vec3(_pivot * _modelMat * glm::vec4(n3, 0));
		n4 = glm::vec3(_pivot * _modelMat * glm::vec4(n4, 0));

		glm::vec2 t1(_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (float)i / slices, _texCoords[0].y);
		glm::vec2 t2(_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (float)(i + 1) / slices, _texCoords[0].y);
		glm::vec2 t3(_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (float)(i + 1) / slices, _texCoords[2].y);
		glm::vec2 t4(_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (float)i / slices, _texCoords[2].y);

		vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity), t1));
		vertices.push_back(Vertex(p2, n2, glm::vec4(_color, opacity), t2));
		vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity), t3));
		vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity), t1));
		vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity), t3));
		vertices.push_back(Vertex(p4, n4, glm::vec4(_color, opacity), t4));
	}

	if (!_texture.empty() && _texCoords.size() >= 4) {
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	} else {
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}
