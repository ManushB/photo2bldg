#include "GeneralObject.h"
#include "CGA.h"
#include "GLUtils.h"

namespace cga {

GeneralObject::GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points.push_back(points);
	this->_normals.push_back(normals);
	this->_color = color;
	this->_textureEnabled = false;
}

GeneralObject::GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<std::vector<glm::vec3> >& points, const std::vector<std::vector<glm::vec3> >& normals, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_normals = normals;
	this->_color = color;
	this->_textureEnabled = false;
}

GeneralObject::GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const glm::vec3& color, const std::vector<glm::vec2>& texCoords, const std::string& texture) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points.push_back(points);
	this->_normals.push_back(normals);
	this->_color = color;
	this->_texCoords.push_back(texCoords);
	this->_texture = texture;
	this->_textureEnabled = true;
}

GeneralObject::GeneralObject(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<std::vector<glm::vec3> >& points, const std::vector<std::vector<glm::vec3> >& normals, const glm::vec3& color, const std::vector<std::vector<glm::vec2> >& texCoords, const std::string& texture) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_normals = normals;
	this->_color = color;
	this->_texCoords = texCoords;
	this->_texture = texture;
	this->_textureEnabled = true;
}

boost::shared_ptr<Shape> GeneralObject::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new GeneralObject(*this));
	copy->_name = name;
	return copy;
}

void GeneralObject::size(float xSize, float ySize, float zSize) {
	_prev_scope = _scope;

	glutils::BoundingBox bbox(_points);
	float scale_x = xSize / bbox.sx();
	float scale_y = xSize / bbox.sy();
	float scale_z = xSize / bbox.sz();

	for (int i =  0; i < _points.size(); ++i) {
		for (int k = 0; k < _points[i].size(); ++k) {
			_points[i][k].x *= scale_x;
			_points[i][k].y *= scale_y;
			_points[i][k].z *= scale_z;
		}
	}
}

void GeneralObject::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	if (!_active) return;

	for (int i = 0; i < _points.size(); ++i) {
		std::vector<Vertex> vertices;
		if (_textureEnabled) {
			glutils::drawPolygon(_points[i], glm::vec4(_color, opacity), _texCoords[i], _pivot * _modelMat, vertices);
			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices, _texture)));
		} else {
			glutils::drawPolygon(_points[i], glm::vec4(_color, opacity), _pivot * _modelMat, vertices);
			faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
		}
	}
}

}
