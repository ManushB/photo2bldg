#pragma once

#include "Grammar.h"
#include <glm/gtc/matrix_transform.hpp>

namespace cga {

class SizeOperator : public Operator {
private:
	Value xSize;
	Value ySize;
	Value zSize;
	bool centered;

public:
	SizeOperator(const Value& xSize, const Value& ySize, const Value& zSize, bool centered);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}