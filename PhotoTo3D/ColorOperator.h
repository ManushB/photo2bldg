#pragma once

#include "Grammar.h"
#include <glm/gtc/matrix_transform.hpp>

namespace cga {

class ColorOperator : public Operator {
private:
	std::string r;
	std::string g;
	std::string b;
	std::string s;

public:
	ColorOperator(const std::string& r, const std::string& g, const std::string& b);
	ColorOperator(const std::string& s);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);

private:
	static void decodeRGB(const std::string& str, float& r, float& g, float& b);
};

}