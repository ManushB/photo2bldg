#include "OffsetOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

OffsetOperator::OffsetOperator(const std::string& offsetDistance, const std::string& inside, const std::string& border) {
	this->name = "offset";
	this->offsetDistance = offsetDistance;
	this->inside = inside;
	this->border = border;
}

boost::shared_ptr<Shape> OffsetOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar,  std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_offsetDistancet = grammar.evalFloat(offsetDistance, shape);

	std::vector<boost::shared_ptr<Shape> > shapes;
	shape->offset(shape->_name, actual_offsetDistancet, inside, border, shapes);
	stack.insert(stack.end(), shapes.begin(), shapes.end());

	return boost::shared_ptr<Shape>();
}

QDomElement OffsetOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	node.setAttribute("offsetDistance", offsetDistance.c_str());
	if (!inside.empty()) {
		node.setAttribute("inside", inside.c_str());
	}
	if (!border.empty()) {
		node.setAttribute("border", border.c_str());
	}

	return node;
}

}
