#include "ColorOperator.h"
#include "CGA.h"
#include "Shape.h"
#include <sstream>

namespace cga {

ColorOperator::ColorOperator(const std::string& r, const std::string& g, const std::string& b) {
	this->name = "color";
	this->r = r;
	this->g = g;
	this->b = b;
	this->s = "";
}

ColorOperator::ColorOperator(const std::string& s) {
	this->name = "color";
	this->r = "";
	this->g = "";
	this->b = "";
	this->s = s;
}

boost::shared_ptr<Shape> ColorOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	if (s.empty()) {
		shape->_color.r = grammar.evalFloat(r, shape);
		shape->_color.g = grammar.evalFloat(g, shape);
		shape->_color.b = grammar.evalFloat(b, shape);
	} else {
		decodeRGB(grammar.evalString(s, shape), shape->_color.r, shape->_color.g, shape->_color.b);
	}

	return shape;
}

void ColorOperator::decodeRGB(const std::string& str, float& r, float& g, float& b) {
	int ir, ig, ib;
    std::istringstream(str.substr(1, 2)) >> std::hex >> ir;
    std::istringstream(str.substr(3, 2)) >> std::hex >> ig;
    std::istringstream(str.substr(5, 2)) >> std::hex >> ib;

	r = (float)ir / 255;
	g = (float)ig / 255;
	b = (float)ib / 255;
}

}
