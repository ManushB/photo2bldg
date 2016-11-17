#include "RotateOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

RotateOperator::RotateOperator(float xAngle, float yAngle, float zAngle) {
	this->name = "rotate";
	this->xAngle = xAngle;
	this->yAngle = yAngle;
	this->zAngle = zAngle;
}

boost::shared_ptr<Shape> RotateOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	shape->rotate(shape->_name, xAngle, yAngle, zAngle);
	return shape;
}

}
