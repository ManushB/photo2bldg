#pragma once

#include "Grammar.h"

namespace cga {

class ShapeUOperator : public Operator {
private:
	Value frontWidth;
	Value backDepth;

public:
	ShapeUOperator(const Value& frontWidth, const Value& backDepth);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
	QDomElement toXml(QDomDocument& doc);
};

}