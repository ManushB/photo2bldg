#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include "Shape.h"

namespace cga {

class Grammar;

class Attribute {
public:
	std::string name;
	std::string value;
	bool hasRange;
	float range_start;
	float range_end;

public:
	Attribute() {}
	Attribute(const std::string& name, const std::string& value);
	Attribute(const std::string& name, const std::string& value, float range_start, float range_end);
};

class Value  {
public:
	static enum { TYPE_ABSOLUTE = 0, TYPE_RELATIVE, TYPE_FLOATING };

public:
	int type;
	std::string value;
	bool repeat;

public:
	Value() : type(TYPE_ABSOLUTE), value(""), repeat(false) {}
	Value(int type, const std::string& value, bool repeat = false) : type(type), value(value), repeat(repeat) {}
	
	float getEstimateValue(float size, const Grammar& grammar, const boost::shared_ptr<Shape>& shape) const;
};

class Operator {
public:
	std::string name;

public:
	Operator() {}

	virtual boost::shared_ptr<Shape> apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) = 0;
};

class Rule {
public:
	std::vector<boost::shared_ptr<Operator> > operators;

public:
	Rule() {}

	void apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack, std::vector<boost::shared_ptr<Shape> >& shapes) const;
	static void decodeSplitSizes(float size, const std::vector<Value>& sizes, const std::vector<std::string>& output_names, const Grammar& grammar, const boost::shared_ptr<Shape>& shape, std::vector<float>& decoded_sizes, std::vector<std::string>& decoded_output_names);
};

class Grammar {
public:
	//std::string type;
	std::map<std::string, Attribute> attrs;
	std::map<std::string, cga::Rule> rules;

public:
	Grammar() {}

	bool contain(const std::string& name) const;
	Rule getRule(const std::string& name) const { return rules.at(name); }
	Rule& getRule(const std::string& name) { return rules[name]; }
	void addAttr(const std::string& name, const Attribute& value);
	void addRule(const std::string& name);
	void addOperator(const std::string& name, const boost::shared_ptr<Operator>& op);
	float evalFloat(const std::string& attr_name, const boost::shared_ptr<Shape>& shape) const;
	std::string evalString(const std::string& attr_name, const boost::shared_ptr<Shape>& shape) const;
};

}
