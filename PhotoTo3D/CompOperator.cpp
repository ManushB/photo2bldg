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

QDomElement CompOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());

	for (auto it = name_map.begin(); it != name_map.end(); ++it) {
		QDomElement child = doc.createElement("param");
		child.setAttribute("name", it->first.c_str());
		child.setAttribute("value", it->second.c_str());
		node.appendChild(child);
	}

	return node;
}

}
