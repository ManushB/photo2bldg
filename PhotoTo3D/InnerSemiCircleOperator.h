#pragma once

#include "Grammar.h"

namespace cga {

class InnerSemiCircleOperator : public Operator {
public:
	InnerSemiCircleOperator();

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}