#pragma once

#include "Grammar.h"

namespace cga {

class OffsetOperator : public Operator {
private:
	std::string offsetDistance;
	std::string inside;
	std::string border;

public:
	OffsetOperator(const std::string& offsetDistance, const std::string& inside, const std::string& border);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
	QDomElement toXml(QDomDocument& doc);
};

}