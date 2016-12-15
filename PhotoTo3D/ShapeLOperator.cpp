#include "ShapeLOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

ShapeLOperator::ShapeLOperator(const Value& frontWidth, const Value& rightWidth) {
	this->name = "shapeL";
	this->frontWidth = frontWidth;
	this->rightWidth = rightWidth;
}

boost::shared_ptr<Shape> ShapeLOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_frontWidth;
	float actual_rightWidth;

	if (frontWidth.type == Value::TYPE_RELATIVE) {
		actual_frontWidth = shape->_scope.x * grammar.evalFloat(frontWidth.value, shape);
	}
	else {
		actual_frontWidth = grammar.evalFloat(frontWidth.value, shape);
	}

	if (rightWidth.type == Value::TYPE_RELATIVE) {
		actual_rightWidth = shape->_scope.y * grammar.evalFloat(rightWidth.value, shape);
	}
	else {
		actual_rightWidth = grammar.evalFloat(rightWidth.value, shape);
	}

	return shape->shapeL(shape->_name, actual_frontWidth, actual_rightWidth);
}

QDomElement ShapeLOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());

	QDomElement child1 = frontWidth.toXml(doc);
	child1.setAttribute("name", "frontWidth");
	node.appendChild(child1);

	QDomElement child2 = rightWidth.toXml(doc);
	child2.setAttribute("name", "rightWidth");
	node.appendChild(child2);

	return node;
}

}
