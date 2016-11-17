#pragma once

#include "Grammar.h"
#include <map>

namespace cga {

class CompOperator : public Operator {
private:
	std::map<std::string, std::string> name_map;

public:
	CompOperator(const std::map<std::string, std::string>& name_map);
	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
};

}
