#pragma once

#include "Grammar.h"

namespace cga {

	class InnerArchOperator : public Operator {
	private:
		std::string inside;
		std::string border;

	public:
		InnerArchOperator(const std::string& inside, const std::string& border);

		boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack);
		QDomElement toXml(QDomDocument& doc);
	};

}