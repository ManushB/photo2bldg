#pragma once

#include "Grammar.h"

namespace cga {

class ShapeLOperator : public Operator {
private:
	Value frontWidth;
	Value rightWidth;

public:
	ShapeLOperator(const Value& frontWidth, const Value& rightWidth);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
	QDomElement toXml(QDomDocument& doc);
};

}