#include "Pyramid.h"
#include "Rectangle.h"
#include "CGA.h"
#include "GLUtils.h"
#include "Polygon.h"

namespace cga {

Pyramid::Pyramid(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, const glm::vec2& center, float height, float top_ratio, const glm::vec3& color, const std::string& texture) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_center = center;
	this->_height = height;
	this->_top_ratio = top_ratio;
	this->_color = color;
	this->_texture = texture;

	glutils::BoundingBox bbox(points);
	this->_scope = glm::vec3(bbox.maxPt.x, bbox.maxPt.y, height);
}

boost::shared_ptr<Shape> Pyramid::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Pyramid(*this));
	copy->_name = name;
	return copy;
}

void Pyramid::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	if (_top_ratio == 0.0f) {
		if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
			glm::vec3 p2(_center, _height);

			for (int i = 0; i < _points.size(); ++i) {
				glm::vec3 p0(_points[i], 0);
				glm::vec3 p1(_points[(i + 1) % _points.size()], 0);

				glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));

				// set the conversion matrix
				float rot_x = atan2f(sqrt(normal.x * normal.x + normal.y * normal.y), normal.z);
				float rot_z = atan2f(p1.y - p0.y, p1.x - p0.x);
				glm::mat4 convMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(), p0), rot_z, glm::vec3(0, 0, 1)), rot_x, glm::vec3(1, 0, 0));
				glm::mat4 invMat = glm::inverse(convMat);
				glm::mat4 mat = _modelMat * convMat;

				// convert the coordinates
				std::vector<glm::vec2> pts;
				pts.push_back(glm::vec2(invMat * glm::vec4(p0, 1)));
				pts.push_back(glm::vec2(invMat * glm::vec4(p1, 1)));
				pts.push_back(glm::vec2(invMat * glm::vec4(p2, 1)));

				shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, pts, _color, _texture)));
			}
		}
	}
	else {
		if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
			for (int i = 0; i < _points.size(); ++i) {
				glm::vec3 p0(_points[i], 0);
				glm::vec3 p3(_points[i] * _top_ratio + _center * (1.0f - _top_ratio), _height);

				glm::vec3 p1(_points[(i + 1) % _points.size()], 0);
				glm::vec3 p2(_points[(i + 1) % _points.size()] * _top_ratio + _center * (1.0f - _top_ratio), _height);

				glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(p1 - p0), glm::vec3(p2 - p0)));

				// set the conversion matrix
				float rot_x = atan2f(sqrt(normal.x * normal.x + normal.y * normal.y), normal.z);
				float rot_z = atan2f(p1.y - p0.y, p1.x - p0.x);
				glm::mat4 convMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(), p0), rot_z, glm::vec3(0, 0, 1)), rot_x, glm::vec3(1, 0, 0));
				glm::mat4 invMat = glm::inverse(convMat);
				glm::mat4 mat = _modelMat * convMat;

				// convert the coordinates
				std::vector<glm::vec2> pts;
				pts.push_back(glm::vec2(invMat * glm::vec4(p0, 1)));
				pts.push_back(glm::vec2(invMat * glm::vec4(p1, 1)));
				pts.push_back(glm::vec2(invMat * glm::vec4(p2, 1)));
				pts.push_back(glm::vec2(invMat * glm::vec4(p3, 1)));

				shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, pts, _color, _texture)));
			}
		}

		if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
			glm::mat4 mat = glm::translate(_modelMat, glm::vec3(_points[0] * _top_ratio + _center * (1.0f - _top_ratio), _height));

			glm::vec2 offset = _points[0] * _top_ratio + _center * (1.0f - _top_ratio);
			std::vector<glm::vec2> pts;
			for (auto point : _points) {
				pts.push_back(point * _top_ratio + _center * (1.0f - _top_ratio) - offset);
			}

			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("top"), _grammar_type, _pivot, mat, pts, _color, _texture)));
		}
	}

	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL") {
		// set the conversion matrix
		glm::mat4 convMat = glm::rotate(glm::mat4(), M_PI, glm::vec3(1, 0, 0));
		glm::mat4 invMat = glm::inverse(convMat);
		glm::mat4 mat = _modelMat * convMat;
		
		// convert the coordinates
		std::vector<glm::vec2> pts;
		for (int i = _points.size() - 1; i >= 0; --i) {
			pts.push_back(glm::vec2(invMat * glm::vec4(_points[i], 0, 1)));
		}

		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("bottom"), _grammar_type, _pivot, mat, pts, _color, _texture)));
	}
}

void Pyramid::setupProjection(int axesSelector, float texWidth, float texHeight) {
	_texWidth = texWidth;
	_texHeight = texHeight;
	_textureEnabled = true;
}

void Pyramid::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	if (_top_ratio == 0.0f) {
		std::vector<Vertex> vertices(_points.size() * 3);

		glm::vec4 p0(_center, _height, 1);
		p0 = _pivot * _modelMat * p0;

		glm::vec4 p1(_points.back(), 0, 1);
		p1 = _pivot * _modelMat * p1;

		glm::vec2 t0(0.5, _height / _texHeight);
		glm::vec2 t1(0, 0);

		for (int i = 0; i < _points.size(); ++i) {
			glm::vec4 p2(_points[i], 0, 1);
			p2 = _pivot * _modelMat * p2;

			glm::vec3 normal = glm::cross(glm::vec3(p1 - p0), glm::vec3(p2 - p0));

			vertices[i * 3] = Vertex(glm::vec3(p0), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));
			vertices[i * 3 + 1] = Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));
			vertices[i * 3 + 2] = Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));

			if (_textureEnabled) {
				glm::vec2 t2(glm::length(p2 - p1) / _texWidth, 0);
				
				vertices[i * 3] = Vertex(glm::vec3(p0), normal, glm::vec4(_color, opacity), t0);
				vertices[i * 3 + 1] = Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity), t1);
				vertices[i * 3 + 2] = Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), t2);
			}
			else {
				vertices[i * 3] = Vertex(glm::vec3(p0), normal, glm::vec4(_color, opacity));
				vertices[i * 3 + 1] = Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity));
				vertices[i * 3 + 2] = Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity));
			}

			p1 = p2;
		}

		if (_textureEnabled) {
			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
		}
		else {
			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
		}
	} else {
		std::vector<Vertex> vertices(_points.size() * 6);

		glm::vec4 p0(_points.back(), 0, 1);
		p0 = _pivot * _modelMat * p0;

		glm::vec4 p1(_points.back() * _top_ratio + _center * (1.0f - _top_ratio), _height, 1);
		p1 = _pivot * _modelMat * p1;

		std::vector<glm::vec3> pts3(_points.size());
		for (int i = 0; i < _points.size(); ++i) {
			glm::vec4 p2(_points[i], 0, 1);
			p2 = _pivot * _modelMat * p2;

			glm::vec4 p3(_points[i] * _top_ratio + _center * (1.0f - _top_ratio), _height, 1);
			pts3[i] = glm::vec3(p3);
			p3 = _pivot * _modelMat * p3;

			glm::vec3 normal = glm::cross(glm::vec3(p2 - p0), glm::vec3(p3 - p0));

			vertices[i * 6 + 0] = Vertex(glm::vec3(p0), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));
			vertices[i * 6 + 1] = Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));
			vertices[i * 6 + 2] = Vertex(glm::vec3(p3), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));

			vertices[i * 6 + 3] = Vertex(glm::vec3(p0), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));
			vertices[i * 6 + 4] = Vertex(glm::vec3(p3), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));
			vertices[i * 6 + 5] = Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity), glm::vec2(0, 0));

			p0 = p2;
			p1 = p3;
		}
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

		vertices.clear();
		glutils::drawPolygon(pts3, glm::vec4(_color, opacity), _pivot * _modelMat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}
}


}
