#include "Hemisphere.h"
#include "CGA.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "GLUtils.h"

namespace cga {

Hemisphere::Hemisphere(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope.x = width;
	this->_scope.y = depth;
	this->_scope.z = width * 0.5;
	this->_color = color;
}

boost::shared_ptr<Shape> Hemisphere::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Hemisphere(*this));
	copy->_name = name;
	return copy;
}

void Hemisphere::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	int slices = 30;
	int stacks = 7;

	float radius = _scope.x * 0.5f;

	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		float theta = (float)(stacks - 1) / stacks * M_PI * 0.5f;
		float height = radius * sinf(theta);
		float r = radius * cosf(theta);

		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(radius - r, radius - r, height));
		shapes.push_back(boost::shared_ptr<Shape>(new Circle(name_map.at("top"), _grammar_type, _pivot, mat, r * 2, r * 2, _color)));
	}

	// side face
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		for (int i = 0; i < stacks - 1; ++i) {
			float theta1 = (float)i / stacks * M_PI * 0.5f;
			float theta2 = (float)(i + 1) / stacks * M_PI * 0.5f;

			for (int j = 0; j < slices; ++j) {
				float phi1 = (float)j / slices * M_PI * 2.0f;
				float phi2 = (float)(j + 1) / slices * M_PI * 2.0f;

				glm::vec3 p1(radius * cosf(theta1) * cosf(phi1) + radius, radius * cosf(theta1) * sinf(phi1) + radius, radius * sinf(theta1));
				glm::vec3 p2(radius * cosf(theta1) * cosf(phi2) + radius, radius * cosf(theta1) * sinf(phi2) + radius, radius * sinf(theta1));
				glm::vec3 p3(radius * cosf(theta2) * cosf(phi2) + radius, radius * cosf(theta2) * sinf(phi2) + radius, radius * sinf(theta2));
				glm::vec3 p4(radius * cosf(theta2) * cosf(phi1) + radius, radius * cosf(theta2) * sinf(phi1) + radius, radius * sinf(theta2));

				glm::mat4 convMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(), glm::vec3(p1)), M_PI * 0.5f + (phi1 + phi2) * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f - (theta1 + theta2) * 0.5f, glm::vec3(1, 0, 0));
				glm::mat4 invMat = glm::inverse(convMat);
				glm::mat4 mat = _modelMat * convMat;

				// convert the coordinates
				p1 = glm::vec3(invMat * glm::vec4(p1, 1));
				p2 = glm::vec3(invMat * glm::vec4(p2, 1));
				p3 = glm::vec3(invMat * glm::vec4(p3, 1));
				p4 = glm::vec3(invMat * glm::vec4(p4, 1));

				std::vector<glm::vec2> pts;
				pts.push_back(glm::vec2(0, 0));
				if (p2.x > 0) {
					pts.push_back(glm::vec2(p2.x, 0));
					pts.push_back(glm::vec2(p3));
					if (glm::length(p3 - p4) > 0.1f) {
						pts.push_back(glm::vec2(p4));
					}

					shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, pts, _color, _texture)));
				}
			}
		}
	}
}

void Hemisphere::setupProjection(int axesSelector, float texWidth, float texHeight) {
	this->_texWidth = texWidth;
	this->_texHeight = texHeight;
	this->_textureEnabled = true;
}

void Hemisphere::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	int slices = 20;
	int stacks = 7;

	float radius = _scope.x * 0.5f;

	for (int i = 0; i < stacks; ++i) {
		float theta1 = (float)i / stacks * M_PI * 0.5f;
		float theta2 = (float)(i + 1) / stacks * M_PI * 0.5f;

		for (int j = 0; j < slices; ++j) {
			float phi1 = (float)j / slices * M_PI * 2.0f;
			float phi2 = (float)(j + 1) / slices * M_PI * 2.0f;

			glm::vec3 p1(radius * cosf(theta1) * cosf(phi1) + radius, radius * cosf(theta1) * sinf(phi1) + radius, radius * sinf(theta1));
			glm::vec3 p2(radius * cosf(theta1) * cosf(phi2) + radius, radius * cosf(theta1) * sinf(phi2) + radius, radius * sinf(theta1));
			glm::vec3 p3(radius * cosf(theta2) * cosf(phi2) + radius, radius * cosf(theta2) * sinf(phi2) + radius, radius * sinf(theta2));
			glm::vec3 p4(radius * cosf(theta2) * cosf(phi1) + radius, radius * cosf(theta2) * sinf(phi1) + radius, radius * sinf(theta2));

			p1 = glm::vec3(_pivot * _modelMat * glm::vec4(p1, 1));
			p2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, 1));
			p3 = glm::vec3(_pivot * _modelMat * glm::vec4(p3, 1));
			p4 = glm::vec3(_pivot * _modelMat * glm::vec4(p4, 1));

			glm::vec3 n1(cosf(theta1) * cosf(phi1), cosf(theta1) * sinf(phi1), sinf(theta1));
			glm::vec3 n2(cosf(theta1) * cosf(phi2), cosf(theta1) * sinf(phi2), sinf(theta1));
			glm::vec3 n3(cosf(theta2) * cosf(phi2), cosf(theta2) * sinf(phi2), sinf(theta1));
			glm::vec3 n4(cosf(theta2) * cosf(phi1), cosf(theta2) * sinf(phi1), sinf(theta1));

			n1 = glm::vec3(_pivot * _modelMat * glm::vec4(n1, 0));
			n2 = glm::vec3(_pivot * _modelMat * glm::vec4(n2, 0));
			n3 = glm::vec3(_pivot * _modelMat * glm::vec4(n3, 0));
			n4 = glm::vec3(_pivot * _modelMat * glm::vec4(n4, 0));

			if (_textureEnabled) {
				glm::vec2 t1((float)j / slices * radius * 2.0f * M_PI / _texWidth, (float)i / stacks * radius * 2.0f * M_PI * 0.25f / _texHeight);
				glm::vec2 t2((float)(j + 1) / slices * radius * 2.0f * M_PI / _texWidth, (float)i / stacks * radius * 2.0f * M_PI * 0.25f / _texHeight);
				glm::vec2 t3((float)(j + 1) / slices * radius * 2.0f * M_PI / _texWidth, (float)(i + 1) / stacks * radius * 2.0f * M_PI * 0.25f / _texHeight);
				glm::vec2 t4((float)j / slices * radius * 2.0f * M_PI / _texWidth, (float)(i + 1) / stacks * radius * 2.0f * M_PI * 0.25f / _texHeight);

				std::vector<Vertex> vertices;
				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity), t1));
				vertices.push_back(Vertex(p2, n2, glm::vec4(_color, opacity), t2));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity), t3));
				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity), t1));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity), t3));
				vertices.push_back(Vertex(p4, n4, glm::vec4(_color, opacity), t4));
				faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
			}
			else {
				std::vector<Vertex> vertices;
				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p2, n2, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p4, n4, glm::vec4(_color, opacity)));
				faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
			}
		}
	}
}

}