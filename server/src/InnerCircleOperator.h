#pragma once

#include "Grammar.h"

namespace cga {

	class InnerCircleOperator : public Operator {
	public:
		InnerCircleOperator();

		boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
		QDomElement toXml(QDomDocument& doc);
	};

}