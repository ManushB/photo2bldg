#include "InnerCircleOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

InnerCircleOperator::InnerCircleOperator() {
	this->name = "circle";
}

boost::shared_ptr<Shape> InnerCircleOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	return shape->innerCircle(shape->_name);
}

}
