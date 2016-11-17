#pragma once

#include "Grammar.h"

namespace cga {

class InsertOperator : public Operator {
private:
	std::string geometryPath;

public:
	InsertOperator(const std::string& geometryPath);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}