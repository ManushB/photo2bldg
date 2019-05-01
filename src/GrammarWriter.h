#pragma once

#include "Grammar.h"

namespace cga {

	void save(const char* filename, const Grammar& grammar);
	void save(const char* filename, const std::vector<Grammar>& grammars);
	void save(QDomDocument& doc, QDomElement& root, const Grammar& grammar);

}