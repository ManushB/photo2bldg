#include "InsertOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

InsertOperator::InsertOperator(const std::string& geometryPath) {
	this->name = "insert";
	this->geometryPath = geometryPath;
}

boost::shared_ptr<Shape> InsertOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	return shape->insert(shape->_name, grammar.evalString(geometryPath, shape));
}

}
