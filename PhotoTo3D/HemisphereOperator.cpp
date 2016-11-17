#include "HemisphereOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

HemisphereOperator::HemisphereOperator() {
	this->name = "hemisphere";
}

boost::shared_ptr<Shape> HemisphereOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	return shape->hemisphere(shape->_name);
}

}
