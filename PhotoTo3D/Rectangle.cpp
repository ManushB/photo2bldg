#include "Rectangle.h"
#include "GLUtils.h"
#include "Circle.h"
#include "CornerCutRectangle.h"
#include "LShape.h"
#include "Pyramid.h"
#include "HipRoof.h"
#include "GableRoof.h"
#include "Prism.h"
#include "Polygon.h"
#include "RectangleTaper.h"
#include "Cuboid.h"
#include "SemiCircle.h"
#include "UShape.h"
#include "CGA.h"

namespace cga {

Rectangle::Rectangle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color) {
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

Rectangle::Rectangle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2) {
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

boost::shared_ptr<Shape> Rectangle::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Rectangle(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> Rectangle::cornerCut(const std::string& name, int type, float length) {
	length = std::min(std::min(length, _scope.x), _scope.y);

	return boost::shared_ptr<Shape>(new CornerCutRectangle(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, type, length, _color));

	/*
	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_scope.x - length, 0));

	int slice = 32;
	if (type == CORNER_CUT_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = -M_PI * 0.5f + M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x - length + cosf(theta) * length, length + sinf(theta) * length));
		}
	} else if (type == CORNER_CUT_NEGATIVE_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = M_PI - M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x + cosf(theta) * length, sinf(theta) * length));
		}
	}

	points.push_back(glm::vec2(_scope.x, length));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));
	return boost::shared_ptr<Shape>(new Polygon(name, _grammar_type, _pivot, _modelMat, points, _color, _texture));
	*/
}

boost::shared_ptr<Shape> Rectangle::extrude(const std::string& name, float height) {
	if (_texCoords.size() >= 4) {
		return boost::shared_ptr<Shape>(new Cuboid(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y));
	}
	else {
		return boost::shared_ptr<Shape>(new Cuboid(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _color));
	}
}

boost::shared_ptr<Shape> Rectangle::hemisphere(const std::string& name) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), (_scope.x + _scope.y) * 0.25, 0, _color, _texture));
}

boost::shared_ptr<Shape> Rectangle::innerCircle(const std::string& name) {
	return boost::shared_ptr<Shape>(new Circle(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, _color));
}

boost::shared_ptr<Shape> Rectangle::innerSemiCircle(const std::string& name) {
	return boost::shared_ptr<Shape>(new SemiCircle(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, _color));
}

void Rectangle::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// inner shape
	if (!inside.empty()) {
		float offset_width = _scope.x + offsetDistance * 2.0f;
		float offset_height = _scope.y + offsetDistance * 2.0f;
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0));
		if (_textureEnabled) {
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
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(inside, _grammar_type, _pivot, mat, offset_width, offset_height, _color, _texture, offset_u1, offset_v1, offset_u2, offset_v2)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(inside, _grammar_type, _pivot, mat, offset_width, offset_height, _color)));
		}
	}

	// border shape
	if (!border.empty() && offsetDistance < 0) {
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, _modelMat, _scope.x, -offsetDistance, _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, -offsetDistance, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), _scope.y + offsetDistance * 2, -offsetDistance, _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), _scope.x, -offsetDistance, _color)));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y + offsetDistance, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), _scope.y + offsetDistance * 2, -offsetDistance, _color)));
	}
}

boost::shared_ptr<Shape> Rectangle::pyramid(const std::string& name, float height) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

boost::shared_ptr<Shape> Rectangle::roofGable(const std::string& name, float angle) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new GableRoof(name, _grammar_type, _pivot, _modelMat, points, angle, _color));
}

boost::shared_ptr<Shape> Rectangle::roofHip(const std::string& name, float angle) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new HipRoof(name, _grammar_type, _pivot, _modelMat, points, angle, _color));
}

void Rectangle::setupProjection(int axesSelector, float texWidth, float texHeight) {
	if (axesSelector == AXES_SCOPE_XY) {
		_texCoords.resize(4);
		_texCoords[0] = glm::vec2(0, 0);
		_texCoords[1] = glm::vec2(_scope.x / texWidth, 0);
		_texCoords[2] = glm::vec2(_scope.x / texWidth, _scope.y / texHeight);
		_texCoords[3] = glm::vec2(0, _scope.y / texHeight);

		_textureEnabled = true;
	}
	else {
		throw "Rectangle supports only scope.xy for setupProjection().";
	}
}

boost::shared_ptr<Shape> Rectangle::shapeL(const std::string& name, float frontWidth, float leftWidth) {
	return boost::shared_ptr<Shape>(new LShape(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, frontWidth, leftWidth, _color));
}

boost::shared_ptr<Shape> Rectangle::shapeU(const std::string& name, float frontWidth, float backDepth) {
	return boost::shared_ptr<Shape>(new UShape(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, frontWidth, backDepth, _color));
}

void Rectangle::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

void Rectangle::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	float offset = 0.0f;
	
	for (int i = 0; i < sizes.size(); ++i) {
		if (splitAxis == DIRECTION_X) {
			if (names[i] != "NIL" && sizes[i] > 0) {
				glm::mat4 mat = glm::translate(_modelMat, glm::vec3(offset, 0, 0));
				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _grammar_type, _pivot, mat, sizes[i], _scope.y, _color, _texture,
						_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * offset / _scope.x, _texCoords[0].y,
						_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (offset + sizes[i]) / _scope.x, _texCoords[2].y)));
				} else {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _grammar_type, _pivot, mat, sizes[i], _scope.y, _color)));
				}
			}
			offset += sizes[i];
		} else if (splitAxis == DIRECTION_Y) {
			if (names[i] != "NIL" && sizes[i] > 0) {
				glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, offset, 0));
				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _grammar_type, _pivot, mat, _scope.x, sizes[i], _color, _texture,
						_texCoords[0].x, _texCoords[0].y + (_texCoords[2].y - _texCoords[0].y) * offset / _scope.y,
						_texCoords[1].x, _texCoords[0].y + (_texCoords[2].y - _texCoords[0].y) * (offset + sizes[i]) / _scope.y)));
				} else {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _grammar_type, _pivot, mat, _scope.x, sizes[i], _color)));
				}
			}
			offset += sizes[i];
		} else if (splitAxis == DIRECTION_Z) {
			objects.push_back(this->clone(this->_name));
		}
	}
}

boost::shared_ptr<Shape> Rectangle::taper(const std::string& name, float height, float slope) {
	return boost::shared_ptr<Shape>(new RectangleTaper(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, slope, _color));
}

void Rectangle::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	std::vector<Vertex> vertices;

	glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(_scope.x * 0.5, _scope.y * 0.5, 0));

	if (!_texture.empty() && _texCoords.size() >= 4) {
		glutils::drawQuad(_scope.x, _scope.y, _texCoords[0], _texCoords[1], _texCoords[2], _texCoords[3], mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	} else {
		glutils::drawQuad(_scope.x, _scope.y, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}
