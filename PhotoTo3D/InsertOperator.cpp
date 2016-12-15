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

QDomElement InsertOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	node.setAttribute("geometryPath", geometryPath.c_str());
	return node;
}

}
