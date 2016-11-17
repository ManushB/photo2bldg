#pragma once

#include "Grammar.h"
#include <glm/gtc/matrix_transform.hpp>

namespace cga {

class TranslateOperator : public Operator {
private:
	int mode;
	int coordSystem;
	Value x;
	Value y;
	Value z;

public:
	TranslateOperator(int mode, int coordSystem, const Value& x, const Value& y, const Value& z);
	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}
