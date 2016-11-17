#pragma once

#include "Grammar.h"

namespace cga {

class RoofHipOperator : public Operator {
private:
	std::string angle;

public:
	RoofHipOperator(const std::string& angle);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}