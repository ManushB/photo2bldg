#include "SplitOperator.h"
#include "CGA.h"
#include "Shape.h"

namespace cga {

SplitOperator::SplitOperator(int splitAxis, const std::vector<Value>& sizes, const std::vector<std::string>& output_names) {
	this->name = "split";
	this->splitAxis = splitAxis;
	this->sizes = sizes;
	this->output_names = output_names;
}

boost::shared_ptr<Shape> SplitOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
	std::vector<boost::shared_ptr<Shape> > floors;

	std::vector<Value> actual_sizes = sizes;
	for (int i = 0; i < sizes.size(); ++i) {
		actual_sizes[i].value = std::to_string(grammar.evalFloat(sizes[i].value, shape));
	}

	std::vector<float> decoded_sizes;
	std::vector<std::string> decoded_output_names;
	if (splitAxis == DIRECTION_X) {
		Rule::decodeSplitSizes(shape->_scope.x, actual_sizes, output_names, grammar, shape, decoded_sizes, decoded_output_names);
	} else if (splitAxis == DIRECTION_Y) {
		Rule::decodeSplitSizes(shape->_scope.y, actual_sizes, output_names, grammar, shape, decoded_sizes, decoded_output_names);
	} else if (splitAxis == DIRECTION_Z) {
		Rule::decodeSplitSizes(shape->_scope.z, actual_sizes, output_names, grammar, shape, decoded_sizes, decoded_output_names);
	}

	shape->split(splitAxis, decoded_sizes, decoded_output_names, floors);
	stack.insert(stack.end(), floors.begin(), floors.end());

	//delete shape;
	//return NULL;
	return boost::shared_ptr<Shape>();
}

QDomElement SplitOperator::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement(name.c_str());
	if (splitAxis == DIRECTION_X) {
		node.setAttribute("splitAxis", "x");
	}
	else if (splitAxis == DIRECTION_Y) {
		node.setAttribute("splitAxis", "y");
	}
	else if (splitAxis == DIRECTION_Z) {
		node.setAttribute("splitAxis", "z");
	}

	for (int i = 0; i < sizes.size(); ++i) {
		QDomElement child = sizes[i].toXml(doc);
		child.setAttribute("name", output_names[i].c_str());
		node.appendChild(child);
	}

	return node;
}

}
