#include "CopyOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

CopyOperator::CopyOperator(const std::string& copy_name) {
	this->name = "copy";
	this->copy_name = copy_name;
}

boost::shared_ptr<Shape> CopyOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	boost::shared_ptr<Shape> copy = shape->clone(copy_name);
	stack.push_back(copy);

	return shape;
}

}
