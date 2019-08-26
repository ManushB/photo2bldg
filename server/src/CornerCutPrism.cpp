#include "CornerCutPrism.h"
#include "GLUtils.h"
#include "Cuboid.h"
#include "CornerCutRectangle.h"
#include "CylinderSide.h"
#include "Rectangle.h"
#include "CGA.h"

namespace cga {

CornerCutPrism::CornerCutPrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, int cut_type, float cut_length, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_cut_type = cut_type;
	this->_cut_length = cut_length;
	this->_color = color;
	this->_textureEnabled = false;
}

CornerCutPrism::CornerCutPrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, int cut_type, float cut_length, const glm::vec3& color, const std::string& texture, float s1, float t1, float s2, float t2) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope = glm::vec3(width, depth, height);
	this->_cut_type = cut_type;
	this->_cut_length = cut_length;
	this->_color = color;

	this->_texture = texture;
	this->_texCoords.push_back(glm::vec2(s1, t1));
	this->_texCoords.push_back(glm::vec2(s2, t1));
	this->_texCoords.push_back(glm::vec2(s2, t2));
	this->_texCoords.push_back(glm::vec2(s1, t2));
	this->_textureEnabled = true;
}
boost::shared_ptr<Shape> CornerCutPrism::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new CornerCutPrism(*this));
	copy->_name = name;
	return copy;
}

void CornerCutPrism::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// top face
	if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(0, 0, _scope.z));
		shapes.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(name_map.at("top"), _grammar_type, _pivot, mat, _scope.x, _scope.y, _cut_type, _cut_length, _color)));
	}

	// bottom face
	if (name_map.find("bottom") != name_map.end() && name_map.at("bottom") != "NIL" && _scope.z >= 0) {
		glm::mat4 mat1 = glm::translate(glm::rotate(_modelMat, M_PI, glm::vec3(1, 0, 0)), glm::vec3(0, -_scope.y, 0));
		glm::mat4 mat2 = glm::translate(glm::rotate(mat1, (float)(M_PI * 0.5), glm::vec3(0, 0, 1)), glm::vec3(0, -_scope.x, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new CornerCutRectangle(name_map.at("bottom"), _grammar_type, _pivot, mat2, _scope.y, _scope.x, _cut_type, _cut_length, _color)));
	}

	// front face
	if (name_map.find("front") != name_map.end() && name_map.at("front") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("front"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x - _cut_length, fabs(_scope.z), _color)));
	}

	// corner face
	if (name_map.find("right") != name_map.end() && name_map.at("right") != "NIL") {
		if (_cut_type == CORNER_CUT_CURVE) {
			glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.5f, glm::vec3(1, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new CylinderSide(name_map.at("right"), _grammar_type, _pivot, mat, _cut_length, _cut_length, _scope.z, M_PI * 0.5f, _color)));
		}
		else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
			// not supported
		}
		else {
			float rot_angle = M_PI * 0.5f;
			if (_scope.z < 0) {
				rot_angle = -rot_angle;
			}
			glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.25f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, mat, _cut_length * sqrt(2.0f), fabs(_scope.z), _color)));
		}
	}

	// right face
	if (name_map.find("right") != name_map.end() && name_map.at("right") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("right"), _grammar_type, _pivot, mat, _scope.y - _cut_length, fabs(_scope.z), _color)));
	}

	// left face
	if (name_map.find("left") != name_map.end() && name_map.at("left") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("left"), _grammar_type, _pivot, mat, _scope.y, fabs(_scope.z), _color)));
	}

	// back face
	if (name_map.find("back") != name_map.end() && name_map.at("back") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("back"), _grammar_type, _pivot, mat, _scope.x, fabs(_scope.z), _color)));
	}

	// side faces
	if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}

		// front face
		if (name_map.find("front") == name_map.end()) {
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, glm::rotate(_modelMat, rot_angle, glm::vec3(1, 0, 0)), _scope.x - _cut_length, fabs(_scope.z), _color)));
		}

		// corner face
		if (name_map.find("right") == name_map.end()) {
			if (_cut_type == CORNER_CUT_CURVE) {
				glm::mat4 mat = glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.5f, glm::vec3(1, 0, 0));
				shapes.push_back(boost::shared_ptr<Shape>(new CylinderSide(name_map.at("side"), _grammar_type, _pivot, mat, _cut_length, _cut_length, _scope.z, M_PI * 0.5f, _color)));
			}
			else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
				// not supported
			}
			else {
				glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length, 0, 0)), M_PI * 0.25f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
				shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, _cut_length * sqrt(2.0f), fabs(_scope.z), _color)));
			}
		}

		// right face
		if (name_map.find("right") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _cut_length, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, _scope.y - _cut_length, fabs(_scope.z), _color)));
		}

		// left face
		if (name_map.find("left") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, _scope.y, fabs(_scope.z), _color)));
		}

		// back face
		if (name_map.find("back") == name_map.end()) {
			glm::mat4 mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, _scope.y, 0)), M_PI, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("side"), _grammar_type, _pivot, mat, _scope.x, fabs(_scope.z), _color)));
		}
	}
}

void CornerCutPrism::size(float xSize, float ySize, float zSize, bool centered) {
	_prev_scope = _scope;

	if (centered) {
		_modelMat = glm::translate(_modelMat, glm::vec3((_scope.x - xSize) * 0.5, (_scope.y - ySize) * 0.5, (_scope.z - zSize) * 0.5));
	}

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;

	if (_cut_length > _scope.x) {
		_cut_length = _scope.x;
	}
	if (_cut_length > _scope.y) {
		_cut_length = _scope.y;
	}
}

void CornerCutPrism::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	if (splitAxis == DIRECTION_X) {
		// not supported!!
	}
	else if (splitAxis == DIRECTION_Y) {
		// not supported!!
	}
	else {
		glm::mat4 mat = this->_modelMat;
		for (int i = 0; i < sizes.size(); ++i) {
			if (names[i] != "NIL") {
				objects.push_back(boost::shared_ptr<Shape>(new CornerCutPrism(names[i], _grammar_type, _pivot, mat, _scope.x, _scope.y, sizes[i], _cut_type, _cut_length, _color)));
			}
			mat = glm::translate(mat, glm::vec3(0, 0, sizes[i]));
		}
	}
}

void CornerCutPrism::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	// top
	{
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

		glutils::drawPolygon(points, glm::vec4(_color, opacity), _pivot * _modelMat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}

	// base
	if (_scope.z >= 0) {
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3(0, _scope.y, 0)), M_PI, glm::vec3(1, 0, 0));
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

		glutils::drawPolygon(points, glm::vec4(_color, opacity), mat, vertices);
		faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
	}

	// front
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}
		glm::mat4 mat = _pivot * glm::rotate(glm::translate(_modelMat, glm::vec3((_scope.x - _cut_length) * 0.5f, 0, _scope.z * 0.5f)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.x - _cut_length, _scope.z, glm::vec4(_color, opacity), mat, vertices);
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

	// corner
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}

		if (_cut_type == CORNER_CUT_CURVE) {
			for (int i = 0; i < CIRCLE_SLICES / 4; ++i) {
				float theta1 = (float)i / CIRCLE_SLICES * 2.0f * M_PI - M_PI * 0.5f;
				float theta2 = (float)(i + 1) / CIRCLE_SLICES * 2.0f * M_PI - M_PI * 0.5f;

				glm::vec3 p1(_scope.x - _cut_length + _cut_length * cosf(theta1), _cut_length + _cut_length * sinf(theta1), 0.0f);
				glm::vec3 p2(_scope.x - _cut_length + _cut_length * cosf(theta2), _cut_length + _cut_length * sinf(theta2), 0.0f);
				glm::vec3 p3(_scope.x - _cut_length + _cut_length * cosf(theta2), _cut_length + _cut_length * sinf(theta2), _scope.z);
				glm::vec3 p4(_scope.x - _cut_length + _cut_length * cosf(theta1), _cut_length + _cut_length * sinf(theta1), _scope.z);

				glm::vec3 n1(cosf(theta1), sinf(theta1), 0.0f);
				glm::vec3 n2(cosf(theta2), sinf(theta2), 0.0f);
				glm::vec3 n3(cosf(theta2), sinf(theta2), 0.0f);
				glm::vec3 n4(cosf(theta1), sinf(theta1), 0.0f);

				p1 = glm::vec3(_pivot * _modelMat * glm::vec4(p1, 1));
				p2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, 1));
				p3 = glm::vec3(_pivot * _modelMat * glm::vec4(p3, 1));
				p4 = glm::vec3(_pivot * _modelMat * glm::vec4(p4, 1));

				n1 = glm::vec3(_pivot * _modelMat * glm::vec4(p1, 0));
				n2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, 0));
				n3 = glm::vec3(_pivot * _modelMat * glm::vec4(p3, 0));
				n4 = glm::vec3(_pivot * _modelMat * glm::vec4(p4, 0));

				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p2, n2, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p4, n4, glm::vec4(_color, opacity)));
				faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
			}
		}
		else if (_cut_type == CORNER_CUT_NEGATIVE_CURVE) {
			for (int i = 0; i < CIRCLE_SLICES / 4; ++i) {
				float theta1 = M_PI - (float)i / CIRCLE_SLICES * 2.0f * M_PI;
				float theta2 = M_PI - (float)(i + 1) / CIRCLE_SLICES * 2.0f * M_PI;

				glm::vec3 p1(_scope.x + _cut_length * cosf(theta1), _cut_length * sinf(theta1), 0.0f);
				glm::vec3 p2(_scope.x + _cut_length * cosf(theta2), _cut_length * sinf(theta2), 0.0f);
				glm::vec3 p3(_scope.x + _cut_length * cosf(theta2), _cut_length * sinf(theta2), _scope.z);
				glm::vec3 p4(_scope.x + _cut_length * cosf(theta1), _cut_length * sinf(theta1), _scope.z);

				glm::vec3 n1(-cosf(theta1), -sinf(theta1), 0.0f);
				glm::vec3 n2(-cosf(theta2), -sinf(theta2), 0.0f);
				glm::vec3 n3(-cosf(theta2), -sinf(theta2), 0.0f);
				glm::vec3 n4(-cosf(theta1), -sinf(theta1), 0.0f);

				p1 = glm::vec3(_pivot * _modelMat * glm::vec4(p1, 1));
				p2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, 1));
				p3 = glm::vec3(_pivot * _modelMat * glm::vec4(p3, 1));
				p4 = glm::vec3(_pivot * _modelMat * glm::vec4(p4, 1));

				n1 = glm::vec3(_pivot * _modelMat * glm::vec4(p1, 0));
				n2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, 0));
				n3 = glm::vec3(_pivot * _modelMat * glm::vec4(p3, 0));
				n4 = glm::vec3(_pivot * _modelMat * glm::vec4(p4, 0));

				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p2, n2, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p1, n1, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p3, n3, glm::vec4(_color, opacity)));
				vertices.push_back(Vertex(p4, n4, glm::vec4(_color, opacity)));
				faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
			}
		}
		else {
			glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x - _cut_length * 0.5f, _cut_length * 0.5, _scope.z * 0.5)), M_PI * 0.25f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
			glutils::drawQuad(_cut_length * sqrt(2.0f), _scope.z, glm::vec4(_color, opacity), mat, vertices);
			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
		}
	}

	// right
	{
		std::vector<Vertex> vertices;
		float rot_angle = M_PI * 0.5f;
		if (_scope.z < 0) {
			rot_angle = -rot_angle;
		}

		glm::mat4 mat = _pivot * glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(_scope.x, (_scope.y + _cut_length) * 0.5f, _scope.z * 0.5)), M_PI * 0.5f, glm::vec3(0, 0, 1)), rot_angle, glm::vec3(1, 0, 0));
		glutils::drawQuad(_scope.y - _cut_length, _scope.z, glm::vec4(_color, opacity), mat, vertices);
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
}

}