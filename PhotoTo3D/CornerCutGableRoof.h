#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

namespace cga {

class CornerCutGableRoof : public Shape {
private:
	float _slope;
	int _cut_type;
	float _cut_length;

public:
	CornerCutGableRoof() {}
	CornerCutGableRoof(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float slope, int cut_type, float cut_length, const glm::vec3& color);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes);
	void size(float xSize, float ySize, float zSize, bool centered);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
