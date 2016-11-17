#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

namespace cga {

class CornerCutPrism : public Shape {
private:
	int _cut_type;
	float _cut_length;

public:
	CornerCutPrism() {}
	CornerCutPrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, int cut_type, float cut_length, const glm::vec3& color);
	CornerCutPrism(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float depth, float height, int cut_type, float cut_length, const glm::vec3& color, const std::string& texture, float s1, float t1, float s2, float t2);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes);
	void size(float xSize, float ySize, float zSize, bool centered);
	void split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;
};

}
