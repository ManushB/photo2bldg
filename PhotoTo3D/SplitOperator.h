#pragma once

#include "Grammar.h"

namespace cga {

class SplitOperator : public Operator {
private:
	int splitAxis;
	std::vector<Value> sizes;
	std::vector<std::string> output_names;

public:
	SplitOperator(int splitAxis, const std::vector<Value>& sizes, const std::vector<std::string>& output_names);
	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}
