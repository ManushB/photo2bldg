#include "CornerCutRectangle.h"
#include "GLUtils.h"
#include "CornerCutGableRoof.h"
#include "CornerCutPrism.h"
#include "CornerCutTaper.h"
#include "Pyramid.h"
#include "HipRoof.h"
#include "GableRoof.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

CornerCutRectangle::CornerCutRectangle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, int cut_type, float cut_length, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, height, 0);
	this->_cut_type = cut_type;
	this->_cut_length = cut_length;
	this->_color = color;
	this->_textureEnabled = false;
}

CornerCutRectangle::CornerCutRectangle(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, int cut_type, float cut_length, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, height, 0);
	this->_cut_type = cut_type;
	this->_cut_length = cut_length;
	this->_color = color;
	this->_texture = texture;

	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(u1, v1);
	_texCoords[1] = glm::vec2(u2, v1);
	_texCoords[2] = glm::vec2(u2, v2);
	_texCoords[3] = glm::vec2(u1, v2);
	this->_textureEnabled = true;
}

boost::shared_ptr<Shape> CornerCutRectangle::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new CornerCutRectangle(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> CornerCutRectangle::extrude(const std::string& name, float height) {
	if (_texCoords.size() >= 4) {
		return boost::shared_ptr<Shape>(new CornerCutPrism(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _cut_type, _cut_length, _color, _texture, _texCoords[0].x, _texCoords[0].y, _texCoords[2].x, _texCoords[2].y));
	}
	else {
		return boost::shared_ptr<Shape>(new CornerCutPrism(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, _cut_type, _cut_length, _color));
	}
}

boost::shared_ptr<Shape> CornerCutRectangle::hemisphere(const std::string& name) {
	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_scope.x - _cut_length, 0));

	int slice = 32;
	if (_cut_type == CORNER_CUT_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = -M_PI * 0.5f + M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x - _cut_length + cosf(theta) * _cut_length, _cut_length + sinf(theta) * _cut_length));
		}
	}
	else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = M_PI - M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x + cosf(theta) * _cut_length, sinf(theta) * _cut_length));
		}
	}

	points.push_back(glm::vec2(_scope.x, _cut_length));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));

	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), (_scope.x + _scope.y) * 0.25, 0, _color, _texture));
}

void CornerCutRectangle::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	float max_offset;
	if (_cut_type == CORNER_CUT_CURVE) {
	}
	else if (_cut_type == CORNER_CUT_STRAIGHT) {
	}

	// inner shape
	if (!inside.empty()) {
		float offset_width = _scope.x + offsetDistance * 2.0f;
		float offset_height = _scope.y + offsetDistance * 2.0f;
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0));
		float new_cut_length = _cut_length + offsetDistance * tanf(M_PI * 0.125f) * 2.0f;
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
			shapes.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(inside, _grammar_type, _pivot, mat, offset_width, offset_height, _cut_type, new_cut_length, _color, _texture, offset_u1, offset_v1, offset_u2, offset_v2)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(inside, _grammar_type, _pivot, mat, offset_width, offset_height, _cut_type, new_cut_length, _color)));
		}
	}

	// border shape
	if (!border.empty() && offsetDistance < 0.0f) {
		if (_cut_type == CORNER_CUT_CURVE) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, _modelMat, _scope.x - _cut_length, -offsetDistance, _color)));

			glm::mat4 mat = glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0));
			int slices = CIRCLE_SLICES / 4;
			for (int i = 0; i < slices; ++i) {
				float theta1 = (float)i / slices * M_PI * 0.5f - M_PI * 0.5f;
				float theta2 = (float)(i + 1) / slices * M_PI * 0.5f - M_PI * 0.5f;

				std::vector<glm::vec2> pts;
				pts.push_back(glm::vec2(_cut_length * cosf(theta1), _cut_length + _cut_length * sinf(theta1)));
				pts.push_back(glm::vec2(_cut_length * cosf(theta2), _cut_length + _cut_length * sinf(theta2)));
				pts.push_back(glm::vec2((_cut_length + offsetDistance) * cosf(theta2), _cut_length + (_cut_length + offsetDistance) * sinf(theta2)));
				pts.push_back(glm::vec2((_cut_length + offsetDistance) * cosf(theta1), _cut_length + (_cut_length + offsetDistance) * sinf(theta1)));
				shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, mat, pts, _color, _texture)));
			}

			mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat, _scope.y - _cut_length + offsetDistance, -offsetDistance, _color)));
		}
		else if (_cut_type == CORNER_CUT_STRAIGHT) {
			std::vector<glm::vec2> pts;
			pts.push_back(glm::vec2(0, 0));
			pts.push_back(glm::vec2(_scope.x - _cut_length, 0));
			pts.push_back(glm::vec2(_scope.x - _cut_length + offsetDistance * tanf(M_PI * 0.125f), -offsetDistance));
			pts.push_back(glm::vec2(0, -offsetDistance));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, _modelMat, pts, _color, _texture)));

			glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.25f, glm::vec3(0, 0, 1));
			pts.clear();
			pts.push_back(glm::vec2(0, 0));
			pts.push_back(glm::vec2(_cut_length * sqrt(2.0f), 0));
			pts.push_back(glm::vec2(_cut_length * sqrt(2.0f) + offsetDistance * tanf(M_PI * 0.125f), -offsetDistance));
			pts.push_back(glm::vec2(-offsetDistance * tanf(M_PI * 0.125f), -offsetDistance));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, mat, pts, _color, _texture)));

			mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1));
			pts.clear();
			pts.push_back(glm::vec2(0, 0));
			pts.push_back(glm::vec2(_scope.y - _cut_length + offsetDistance, 0));
			pts.push_back(glm::vec2(_scope.y - _cut_length + offsetDistance, -offsetDistance));
			pts.push_back(glm::vec2(-offsetDistance * tanf(M_PI * 0.125f), -offsetDistance));
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, mat, pts, _color, _texture)));
		}

		glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat, _scope.x, -offsetDistance, _color)));
		
		mat = glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y + offsetDistance, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(border, _grammar_type, _pivot, mat, _scope.y + offsetDistance * 2.0f, -offsetDistance, _color)));
	}
}

boost::shared_ptr<Shape> CornerCutRectangle::pyramid(const std::string& name, float height) {
	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_scope.x - _cut_length, 0));

	int slice = 32;
	if (_cut_type == CORNER_CUT_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = -M_PI * 0.5f + M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x - _cut_length + cosf(theta) * _cut_length, _cut_length + sinf(theta) * _cut_length));
		}
	}
	else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = M_PI - M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x + cosf(theta) * _cut_length, sinf(theta) * _cut_length));
		}
	}

	points.push_back(glm::vec2(_scope.x, _cut_length));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));

	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

boost::shared_ptr<Shape> CornerCutRectangle::roofGable(const std::string& name, float angle) {
	return boost::shared_ptr<Shape>(new CornerCutGableRoof(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, angle, _cut_type, _cut_length, _color));
}

boost::shared_ptr<Shape> CornerCutRectangle::roofHip(const std::string& name, float angle) {
	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_scope.x - _cut_length, 0));

	int slice = 32;
	if (_cut_type == CORNER_CUT_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = -M_PI * 0.5f + M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x - _cut_length + cosf(theta) * _cut_length, _cut_length + sinf(theta) * _cut_length));
		}
	}
	else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = M_PI - M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x + cosf(theta) * _cut_length, sinf(theta) * _cut_length));
		}
	}

	points.push_back(glm::vec2(_scope.x, _cut_length));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));
	return boost::shared_ptr<Shape>(new HipRoof(name, _grammar_type, _pivot, _modelMat, points, angle, _color));
}

void CornerCutRectangle::setupProjection(int axesSelector, float texWidth, float texHeight) {
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

void CornerCutRectangle::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

void CornerCutRectangle::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	float offset = 0.0f;
	
	for (int i = 0; i < sizes.size(); ++i) {
		if (splitAxis == DIRECTION_X) {
			// not supported
		} else if (splitAxis == DIRECTION_Y) {
			if (names[i] != "NIL" && sizes[i] > 0) {
				glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, offset, 0));
				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(names[i], _grammar_type, _pivot, mat, _scope.x, sizes[i], _cut_type, _cut_length, _color, _texture,
						_texCoords[0].x, _texCoords[0].y + (_texCoords[2].y - _texCoords[0].y) * offset / _scope.y,
						_texCoords[1].x, _texCoords[0].y + (_texCoords[2].y - _texCoords[0].y) * (offset + sizes[i]) / _scope.y)));
				} else {
					objects.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(names[i], _grammar_type, _pivot, mat, _scope.x, sizes[i], _cut_type, _cut_length, _color)));
				}
			}
			offset += sizes[i];
		} else if (splitAxis == DIRECTION_Z) {
			objects.push_back(this->clone(this->_name));
		}
	}
}

boost::shared_ptr<Shape> CornerCutRectangle::taper(const std::string& name, float height, float slope) {
	return boost::shared_ptr<Shape>(new CornerCutTaper(name, _grammar_type, _pivot, _modelMat, _scope.x, _scope.y, height, slope, _cut_type, _cut_length, _color));
}

void CornerCutRectangle::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	std::vector<Vertex> vertices;

	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(0, 0));
	points.push_back(glm::vec2(_scope.x - _cut_length, 0));

	int slice = 32;
	if (_cut_type == CORNER_CUT_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = -M_PI * 0.5f + M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x - _cut_length + cosf(theta) * _cut_length, _cut_length + sinf(theta) * _cut_length));
		}
	}
	else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
		for (int i = 1; i < slice; ++i) {
			float theta = M_PI - M_PI * 0.5f * i / slice;
			points.push_back(glm::vec2(_scope.x + cosf(theta) * _cut_length, sinf(theta) * _cut_length));
		}
	}

	points.push_back(glm::vec2(_scope.x, _cut_length));
	points.push_back(glm::vec2(_scope.x, _scope.y));
	points.push_back(glm::vec2(0, _scope.y));

	if (!_texture.empty() && _texCoords.size() >= 4) {
		std::vector<glm::vec2> texs;
		for (int i = 0; i < points.size(); ++i) {
			texs.push_back(glm::vec2(points[i].x / _scope.x * (_texCoords[1].x - _texCoords[0].x) + _texCoords[0].x, points[i].y / _scope.y * (_texCoords[2].y - _texCoords[0].y) + _texCoords[0].y));
		}
		glutils::drawPolygon(points, glm::vec4(_color, opacity), texs, _pivot * _modelMat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	} else {
		glutils::drawPolygon(points, glm::vec4(_color, opacity), _pivot * _modelMat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}

}
