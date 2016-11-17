#pragma once

#include "Grammar.h"

namespace cga {

class RotateOperator : public Operator {
private:
	float xAngle;
	float yAngle;
	float zAngle;

public:
	RotateOperator(float xAngle, float yAngle, float zAngle);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}