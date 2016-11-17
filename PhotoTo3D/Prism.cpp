#include "Prism.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "CGA.h"
#include "GLUtils.h"

namespace cga {

Prism::Prism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float height, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_color = color;
	this->_textureEnabled = false;

	glutils::BoundingBox bbox(points);
	this->_scope = glm::vec3(bbox.maxPt.x, bbox.maxPt.y, height);
}

Prism::Prism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float height, const glm::vec3& color, const std::string& texture, float texWidth, float texHeight) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_color = color;
	this->_texture = texture;
	this->_textureEnabled = true;
	this->_texWidth = texWidth;
	this->_texHeight = texHeight;

	glutils::BoundingBox bbox(points);
	this->_scope = glm::vec3(bbox.maxPt.x, bbox.maxPt.y, height);
}

boost::shared_ptr<Shape> Prism::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Prism(*this));
	copy->_name = name;
	return copy;
}

void Prism::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		if (_textureEnabled) {
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("top"), _grammar_type, _pivot, glm::translate(_modelMat, glm::vec3(0, 0, _scope.z)), _points, _color, _texture, _texWidth, _texHeight)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("top"), _grammar_type, _pivot, glm::translate(_modelMat, glm::vec3(0, 0, _scope.z)), _points, _color, _texture)));
		}
	}

	// bottom face
	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL") {
		if (_textureEnabled) {
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("bottom"), _grammar_type, _pivot, _modelMat, _points, _color, _texture, _texWidth, _texHeight)));
		}
		else {
			shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("bottom"), _grammar_type, _pivot, _modelMat, _points, _color, _texture)));
		}
	}

	// front face
	if (name_map.find("front") != name_map.end() && name_map.at("front") != "NIL") {
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(_modelMat, M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::length(_points[1] - _points[0]), _scope.z, _color)));
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		// front face
		if (name_map.find("front") == name_map.end()) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(_modelMat, M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::length(_points[1] - _points[0]), _scope.z, _color)));
		}

		glm::mat4 mat;
		for (int i = 1; i < _points.size(); ++i) {
			glm::vec2 a = _points[i] - _points[i - 1];
			glm::vec2 b = _points[(i + 1) % _points.size()] - _points[i];

			mat = glm::translate(mat, glm::vec3(glm::length(a), 0, 0));
			float theta = acos(glm::dot(a, b) / glm::length(a) / glm::length(b));
			mat = glm::rotate(mat, theta, glm::vec3(0, 0, 1));
			glm::mat4 mat2 = glm::rotate(mat, M_PI * 0.5f, glm::vec3(1, 0, 0));
			glm::mat4 invMat = glm::inverse(mat2);

			std::vector<glm::vec2> sidePoints(4);
			sidePoints[0] = glm::vec2(invMat * glm::vec4(_points[i], 0, 1));
			sidePoints[1] = glm::vec2(invMat * glm::vec4(_points[(i + 1) % _points.size()], 0, 1));
			sidePoints[2] = glm::vec2(invMat * glm::vec4(_points[(i + 1) % _points.size()], _scope.z, 1));
			sidePoints[3] = glm::vec2(invMat * glm::vec4(_points[i], _scope.z, 1));

			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, _modelMat * mat2, glm::length(_points[(i + 1) % _points.size()] - _points[i]), _scope.z, _color)));
		}
	}


}

void Prism::setupProjection(float texWidth, float texHeight) {
}

void Prism::size(float xSize, float ySize, float zSize) {
	_prev_scope = _scope;

	float scale_x = xSize / _scope.x;
	float scale_y = ySize / _scope.y;
	_scope.z = zSize;

	for (int i = 0; i < _points.size(); ++i) {
		_points[i].x *= scale_x;
		_points[i].y *= scale_y;
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
}

/**
 * To be fixed:
 * Z方向のsplitしか対応していない。
 */
void Prism::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	glm::mat4 modelMat = this->_modelMat;

	for (int i = 0; i < sizes.size(); ++i) {
		Prism* obj = new Prism(*this);
		obj->_name = names[i];
		obj->_modelMat = modelMat;
		obj->_scope.z = sizes[i];
		objects.push_back(boost::shared_ptr<Shape>(obj));

		modelMat = glm::translate(modelMat, glm::vec3(0, 0, obj->_scope.z));
	}
}

void Prism::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	// top
	if (_scope.z >= 0) {
		std::vector<Vertex> vertices;
		glm::mat4 mat = _pivot * glm::translate(_modelMat, glm::vec3(0, 0, _scope.z));
		glutils::drawConcavePolygon(_points, glm::vec4(_color, opacity), mat, vertices);

		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// bottom
	{
		std::vector<Vertex> vertices;
		glutils::drawConcavePolygon(_points, glm::vec4(_color, opacity), _pivot * _modelMat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
	}

	// side
	{
		glm::vec4 p1(_points.back(), 0, 1);
		glm::vec4 p2(_points.back(), _scope.z, 1);
		p1 = _pivot * _modelMat * p1;
		p2 = _pivot * _modelMat * p2;

		for (int i = 0; i < _points.size(); ++i) {
			std::vector<Vertex> vertices;

			glm::vec4 p3(_points[i], 0, 1);
			glm::vec4 p4(_points[i], _scope.z, 1);
			p3 = _pivot * _modelMat * p3;
			p4 = _pivot * _modelMat * p4;

			glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(p3) - glm::vec3(p1), glm::vec3(p2) - glm::vec3(p1)));
	
			vertices.push_back(Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity)));
			vertices.push_back(Vertex(glm::vec3(p3), normal, glm::vec4(_color, opacity), 1));
			vertices.push_back(Vertex(glm::vec3(p4), normal, glm::vec4(_color, opacity)));

			vertices.push_back(Vertex(glm::vec3(p1), normal, glm::vec4(_color, opacity)));
			vertices.push_back(Vertex(glm::vec3(p4), normal, glm::vec4(_color, opacity)));
			vertices.push_back(Vertex(glm::vec3(p2), normal, glm::vec4(_color, opacity), 1));

			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));

			p1 = p3;
			p2 = p4;
		}
	}
}

}
