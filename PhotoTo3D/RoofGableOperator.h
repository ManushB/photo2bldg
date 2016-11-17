#pragma once

#include "Grammar.h"

namespace cga {

class RoofGableOperator : public Operator {
private:
	std::string angle;

public:
	RoofGableOperator(const std::string& angle);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}