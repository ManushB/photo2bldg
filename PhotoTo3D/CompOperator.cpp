#include "CompOperator.h"
#include "CGA.h"
#include "Rectangle.h"
#include "Polygon.h"

namespace cga {

CompOperator::CompOperator(const std::map<std::string, std::string>& name_map) {
	this->name = "comp";
	this->name_map = name_map;
}

boost::shared_ptr<Shape> CompOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	std::vector<boost::shared_ptr<Shape> > shapes;
	
	shape->comp(name_map, shapes);
	stack.insert(stack.end(), shapes.begin(), shapes.end());

	return boost::shared_ptr<Shape>();
}

}
