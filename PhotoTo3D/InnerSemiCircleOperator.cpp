#include "InnerSemiCircleOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

InnerSemiCircleOperator::InnerSemiCircleOperator() {
	this->name = "semiCircle";
}

boost::shared_ptr<Shape> InnerSemiCircleOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	return shape->innerSemiCircle(shape->_name);
}

}
