#include "ShapeLOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

ShapeLOperator::ShapeLOperator(const Value& frontWidth, const Value& leftWidth) {
	this->name = "shapeL";
	this->frontWidth = frontWidth;
	this->leftWidth = leftWidth;
}

boost::shared_ptr<Shape> ShapeLOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	float actual_frontWidth;
	float actual_leftWidth;

	if (frontWidth.type == Value::TYPE_RELATIVE) {
		actual_frontWidth = shape->_scope.x * grammar.evalFloat(frontWidth.value, shape);
	}
	else {
		actual_frontWidth = grammar.evalFloat(frontWidth.value, shape);
	}

	if (leftWidth.type == Value::TYPE_RELATIVE) {
		actual_leftWidth = shape->_scope.y * grammar.evalFloat(leftWidth.value, shape);
	}
	else {
		actual_leftWidth = grammar.evalFloat(leftWidth.value, shape);
	}

	return shape->shapeL(shape->_name, actual_frontWidth, actual_leftWidth);
}

}
