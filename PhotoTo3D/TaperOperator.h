#pragma once

#include "Grammar.h"

namespace cga {

class TaperOperator : public Operator {
private:
	std::string height;
	std::string slope;

public:
	TaperOperator(const std::string& height, const std::string& slope);

	boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
	QDomElement toXml(QDomDocument& doc);
};

}