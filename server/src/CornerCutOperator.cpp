#include "CornerCutOperator.h"
#include "CGA.h"
#include "Shape.h"
#include <sstream>

namespace cga {

CornerCutOperator::CornerCutOperator(int type, const std::string& length) {
	this->name = "cornerCut";
	this->type = type;
	this->length = length;
}


boost::shared_ptr<Shape> CornerCutOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_length = grammar.evalFloat(length, shape);
	return shape->cornerCut(shape->_name, type, actual_length);
}

QDomElement CornerCutOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());

	if (type == CORNER_CUT_STRAIGHT) {
		node.setAttribute("type", "straight");
	}
	else if (type == CORNER_CUT_CURVE) {
		node.setAttribute("type", "curve");
	}
	else if (type == CORNER_CUT_NEGATIVE_CURVE) {
		node.setAttribute("type", "negative_curve");
	}
	node.setAttribute("length", length.c_str());

	return node;
}

}
