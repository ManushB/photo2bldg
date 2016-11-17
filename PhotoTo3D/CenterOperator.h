#pragma once

#include "Grammar.h"

namespace cga {

class CenterOperator : public Operator {
private:
	int axesSelector;

public:
	CenterOperator(int axesSelector);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}