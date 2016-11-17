#pragma once

#include "Grammar.h"

namespace cga {

class ExtrudeOperator : public Operator {
private:
	std::string height;

public:
	ExtrudeOperator(const std::string& height);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}