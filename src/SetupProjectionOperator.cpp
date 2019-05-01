#include "SetupProjectionOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

SetupProjectionOperator::SetupProjectionOperator(int axesSelector, const Value& texWidth, const Value& texHeight) {
	this->name = "setupProjection";
	this->axesSelector = axesSelector;
	this->texWidth = texWidth;
	this->texHeight = texHeight;
}

boost::shared_ptr<Shape> SetupProjectionOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_texWidth;
	float actual_texHeight;

	if (texWidth.type == Value::TYPE_RELATIVE) {
		actual_texWidth = shape->_scope.x * grammar.evalFloat(texWidth.value, shape);
	} else {
		actual_texWidth = grammar.evalFloat(texWidth.value, shape);
	}
	if (texHeight.type == Value::TYPE_RELATIVE) {
		actual_texHeight = shape->_scope.y * grammar.evalFloat(texHeight.value, shape);
	} else {
		actual_texHeight = grammar.evalFloat(texHeight.value, shape);
	}


	shape->setupProjection(axesSelector, actual_texWidth, actual_texHeight);

	return shape;
}

QDomElement SetupProjectionOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	if (axesSelector == AXES_SCOPE_XY) {
		node.setAttribute("axesSelector", "scope.xy");
	}
	else if (axesSelector == AXES_SCOPE_XZ) {
		node.setAttribute("axesSelector", "scope.xz");
	}
	else if (axesSelector == AXES_SCOPE_YZ) {
		node.setAttribute("axesSelector", "scope.yz");
	}

	QDomElement child1 = texWidth.toXml(doc);
	child1.setAttribute("name", "texWidth");
	node.appendChild(child1);

	QDomElement child2 = texHeight.toXml(doc);
	child2.setAttribute("name", "texHeight");
	node.appendChild(child2);

	return node;
}

}
