#include "Polygon.h"
#include "Pyramid.h"
#include "GableRoof.h"
#include "HipRoof.h"
#include "Prism.h"
#include "CGA.h"
#include "GLUtils.h"
#include <iostream>

namespace cga {

Polygon::Polygon(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, const glm::vec3& color, const std::string& texture) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	//this->_points = points;
	for (int i = 0; i < points.size(); ++i) {
		if (i == 0) {
			_points.push_back(points[i]);
		}
		else {
			if (glm::length(points[i] - points[i - 1]) > 0.001f) {
				_points.push_back(points[i]);
			}
		}
	}
	this->_color = color;
	this->_texture = texture;
	this->_textureEnabled = false;


	glutils::BoundingBox bbox(points);
	this->_scope = glm::vec3(bbox.maxPt.x, bbox.maxPt.y, 0);

	this->_center = glm::vec2(0, 0);
	for (int i = 0; i < points.size(); ++i) {
		_center += points[i];
	}
	_center /= points.size();
}

Polygon::Polygon(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, const glm::vec3& color, const std::string& texture, const std::vector<glm::vec2>& texCoords) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	//this->_points = points;
	for (int i = 0; i < points.size(); ++i) {
		if (i == 0) {
			_points.push_back(points[i]);
		}
		else {
			if (glm::length(points[i] - points[i - 1]) > 0.0f) {
				_points.push_back(points[i]);
			}
		}
	}

	this->_color = color;
	this->_texture = texture;
	this->_textureEnabled = true;
	_texCoords = texCoords;

	glutils::BoundingBox bbox(points);
	this->_scope = glm::vec3(bbox.maxPt.x, bbox.maxPt.y, 0);

	this->_center = glm::vec2(0, 0);
	for (int i = 0; i < points.size(); ++i) {
		_center += points[i];
	}
	_center /= points.size();
}

Polygon::Polygon(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, const glm::vec3& color, const std::string& texture, float texWidth, float texHeight) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	//this->_points = points;
	for (int i = 0; i < points.size(); ++i) {
		if (i == 0) {
			_points.push_back(points[i]);
		}
		else {
			if (glm::length(points[i] - points[i - 1]) > 0.0f) {
				_points.push_back(points[i]);
			}
		}
	}

	this->_color = color;
	this->_texture = texture;
	this->_texWidth = texWidth;
	this->_texHeight = texHeight;
	this->_textureEnabled = true;

	_texCoords.resize(_points.size());
	for (int i = 0; i < _points.size(); ++i) {
		_texCoords[i] = glm::vec2(_points[i].x / texWidth, _points[i].y / texHeight);
	}

	glutils::BoundingBox bbox(points);
	this->_scope = glm::vec3(bbox.maxPt.x, bbox.maxPt.y, 0);

	this->_center = glm::vec2(0, 0);
	for (int i = 0; i < points.size(); ++i) {
		_center += points[i];
	}
	_center /= points.size();
}

boost::shared_ptr<Shape> Polygon::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Polygon(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> Polygon::extrude(const std::string& name, float height) {
	if (_textureEnabled) {
		return boost::shared_ptr<Shape>(new Prism(name, _grammar_type, _pivot, _modelMat, _points, height, _color, _texture, _texWidth, _texHeight));
	}
	else {
		return boost::shared_ptr<Shape>(new Prism(name, _grammar_type, _pivot, _modelMat, _points, height, _color));
	}
}

boost::shared_ptr<Shape> Polygon::hemisphere(const std::string& name) {
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, _points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), (_scope.x + _scope.y) * 0.25, 0, _color, _texture));
}

boost::shared_ptr<Shape> Polygon::inscribeCircle(const std::string& name) {
	return NULL;
}

void Polygon::offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes) {
	std::vector<glm::vec2> offset_points;
	glutils::offsetPolygon(_points, offsetDistance, offset_points);

	// inner shape
	if (!inside.empty()) {
		std::vector<glm::vec2> new_pts;
		glm::vec2 diff = offset_points[0] - _points[0];
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(diff, 0));
		for (int i = 0; i < offset_points.size(); ++i) {
			new_pts.push_back(offset_points[i] - diff);
		}

		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(inside, _grammar_type, _pivot, mat, new_pts, _color, _texture)));
	}

	// border shape
	if (!border.empty()) {
		std::vector<glm::vec3> normals;
		for (int i = 0; i < _points.size(); ++i) {
			std::vector<glm::vec3> pts;
			pts.push_back(glm::vec3(offset_points[i], 0));
			pts.push_back(glm::vec3(_points[i], 0));
			pts.push_back(glm::vec3(_points[(i+1) % _points.size()], 0));
			pts.push_back(glm::vec3(offset_points[(i+1) % offset_points.size()], 0));

			float rot_z = atan2f(_points[(i + 1) % _points.size()].y - _points[i].y, _points[(i + 1) % _points.size()].x - _points[i].x);
			glm::mat4 convMat = glm::rotate(glm::translate(glm::mat4(), glm::vec3(_points[i], 0)), rot_z, glm::vec3(0, 0, 1));
			glm::mat4 invMat = glm::inverse(convMat);
			glm::mat4 mat = _modelMat * convMat;

			std::vector<glm::vec2> pts2d;
			pts2d.push_back(glm::vec2(invMat * glm::vec4(pts[0], 1)));
			pts2d.push_back(glm::vec2(invMat * glm::vec4(pts[1], 1)));
			pts2d.push_back(glm::vec2(invMat * glm::vec4(pts[2], 1)));
			pts2d.push_back(glm::vec2(invMat * glm::vec4(pts[3], 1)));

			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(border, _grammar_type, _pivot, mat, pts2d, _color, _texture)));
		}		
	}
}

boost::shared_ptr<Shape> Polygon::pyramid(const std::string& name, float height) {
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, _points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, 0, _color, _texture));
}

boost::shared_ptr<Shape> Polygon::roofHip(const std::string& name, float angle) {
	return boost::shared_ptr<Shape>(new HipRoof(name, _grammar_type, _pivot, _modelMat, _points, angle, _color));
}

boost::shared_ptr<Shape> Polygon::roofGable(const std::string& name, float angle) {
	return boost::shared_ptr<Shape>(new GableRoof(name, _grammar_type, _pivot, _modelMat, _points, angle, _color));
}

void Polygon::setupProjection(int axesSelector, float texWidth, float texHeight) {
	this->_texWidth = texWidth;
	this->_texHeight = texHeight;

	if (axesSelector == AXES_SCOPE_XY) {
		_texCoords.resize(_points.size());
		for (int i = 0; i < _points.size(); ++i) {
			_texCoords[i] = glm::vec2(_points[i].x / texWidth, _points[i].y / texHeight);
		}
		_textureEnabled = true;
	}
	else {
		throw "Polygon supports only scope.xy for setupProjection().";
	}
}

void Polygon::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	float scaleX = xSize / _scope.x;
	float scaleY = ySize / _scope.y;
	for (int i = 0; i < _points.size(); ++i) {
		_points[i].x *= scaleX;
		_points[i].y *= scaleY;
	}
	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = 0.0f;
}

boost::shared_ptr<Shape> Polygon::taper(const std::string& name, float height, float top_ratio) {
	return boost::shared_ptr<Shape>(new Pyramid(name, _grammar_type, _pivot, _modelMat, _points, _center, height, top_ratio, _color, _texture));
}

void Polygon::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	bool valid = true;
	for (int i = 0; i < _points.size(); ++i) {
		if (i == 0) continue;
		if (glm::length(_points[i] - _points[i - 1]) == 0.0f) valid = false;
	}

	if (valid) {
		if (!_texture.empty() && _texCoords.size() >= _points.size()) {
			std::vector<Vertex> vertices;
			glutils::drawConcavePolygon(_points, glm::vec4(_color, opacity), _texCoords, _pivot * _modelMat, vertices);

			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
		}
		else {
			std::vector<Vertex> vertices;
			glutils::drawConcavePolygon(_points, glm::vec4(_color, opacity), _pivot * _modelMat, vertices);

			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
		}

	}
}

}
