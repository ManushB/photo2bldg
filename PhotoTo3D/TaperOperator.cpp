#include "TaperOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

TaperOperator::TaperOperator(const std::string& height, const std::string& slope) {
	this->name = "taper";
	this->height = height;
	this->slope = slope;
}

boost::shared_ptr<Shape> TaperOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_height = grammar.evalFloat(height, shape);
	float actual_slope = grammar.evalFloat(slope, shape);
	
	return shape->taper(shape->_name, actual_height, actual_slope);
}

QDomElement TaperOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	node.setAttribute("height", height.c_str());
	node.setAttribute("slope", slope.c_str());

	return node;
}

}
