#include "CenterOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

CenterOperator::CenterOperator(int axesSelector) {
	this->name = "center";
	this->axesSelector = axesSelector;
}

boost::shared_ptr<Shape> CenterOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	shape->center(axesSelector);

	return shape;
}

QDomElement CenterOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	if (axesSelector == AXES_SELECTOR_XYZ) {
		node.setAttribute("axesSelector", "xyz");
	}
	else if (axesSelector == AXES_SELECTOR_X) {
		node.setAttribute("axesSelector", "x");
	}
	else if (axesSelector == AXES_SELECTOR_X) {
		node.setAttribute("axesSelector", "x");
	}
	else if (axesSelector == AXES_SELECTOR_Y) {
		node.setAttribute("axesSelector", "y");
	}
	else if (axesSelector == AXES_SELECTOR_Z) {
		node.setAttribute("axesSelector", "z");
	}
	else if (axesSelector == AXES_SELECTOR_XY) {
		node.setAttribute("axesSelector", "xy");
	}
	else if (axesSelector == AXES_SELECTOR_XZ) {
		node.setAttribute("axesSelector", "xz");
	}
	else if (axesSelector == AXES_SELECTOR_YZ) {
		node.setAttribute("axesSelector", "yz");
	}

	return node;
}

}
