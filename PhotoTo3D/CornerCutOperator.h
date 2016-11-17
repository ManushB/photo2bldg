#pragma once

#include "Grammar.h"
#include <glm/gtc/matrix_transform.hpp>

namespace cga {

class CornerCutOperator : public Operator {
private:
	int type;
	std::string length;

public:
	CornerCutOperator(int type, const std::string& length);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}