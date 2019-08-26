#include "SizeOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

SizeOperator::SizeOperator(const Value& xSize, const Value& ySize, const Value& zSize, bool centered) {
	this->name = "size";
	this->xSize = xSize;
	this->ySize = ySize;
	this->zSize = zSize;
	this->centered = centered;
}

boost::shared_ptr<Shape> SizeOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_xSize;
	float actual_ySize;
	float actual_zSize;

	if (xSize.type == Value::TYPE_RELATIVE) {
		actual_xSize = shape->_scope.x * grammar.evalFloat(xSize.value, shape);
	} else {
		actual_xSize = grammar.evalFloat(xSize.value, shape);
	}

	if (ySize.type == Value::TYPE_RELATIVE) {
		actual_ySize = shape->_scope.y * grammar.evalFloat(ySize.value, shape);
	} else {
		actual_ySize = grammar.evalFloat(ySize.value, shape);
	}

	if (zSize.type == Value::TYPE_RELATIVE) {
		actual_zSize = shape->_scope.z * grammar.evalFloat(zSize.value, shape);
	} else {
		actual_zSize = grammar.evalFloat(zSize.value, shape);
	}

	shape->size(actual_xSize, actual_ySize, actual_zSize, centered);
	return shape;
}

QDomElement SizeOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	if (centered) {
		node.setAttribute("centered", "true");
	}

	QDomElement child1 = xSize.toXml(doc);
	child1.setAttribute("name", "xSize");
	node.appendChild(child1);

	QDomElement child2 = ySize.toXml(doc);
	child2.setAttribute("name", "ySize");
	node.appendChild(child2);

	QDomElement child3 = zSize.toXml(doc);
	child3.setAttribute("name", "zSize");
	node.appendChild(child3);

	return node;
}

}
