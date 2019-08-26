#include "InnerArchOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

	InnerArchOperator::InnerArchOperator(const std::string& inside, const std::string& border) {
		this->name = "innerArch";
		this->inside = inside;
		this->border = border;
	}

	boost::shared_ptr<Shape> InnerArchOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
		std::vector<boost::shared_ptr<Shape> > shapes;
		shape->innerArch(shape->_name, inside, border, shapes);
		stack.insert(stack.end(), shapes.begin(), shapes.end());

		return boost::shared_ptr<Shape>();
	}

	QDomElement InnerArchOperator::toXml(QDomDocument& doc) {
		QDomElement node = doc.createElement(name.c_str());
		if (!inside.empty()) {
			node.setAttribute("inside", inside.c_str());
		}
		if (!border.empty()) {
			node.setAttribute("border", border.c_str());
		}

		return node;
	}

}
