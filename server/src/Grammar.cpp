#include "Grammar.h"
#include "CGA.h"
#include "Shape.h"
#include "NumberEval.h"
#include <sstream>
#include <boost/algorithm/string/replace.hpp>

namespace cga {

Attribute::Attribute(const std::string& name, const std::string& value) {
	this->name = name;
	this->value = value;
	this->hasRange = false;
}

Attribute::Attribute(const std::string& name, const std::string& value, float range_start, float range_end) {
	this->name = name;
	this->value = value;
	this->hasRange = true;
	this->range_start = range_start;
	this->range_end = range_end;
}


float Value::getEstimateValue(float size, const Grammar& grammar, const boost::shared_ptr<Shape>& shape) const {
	if (type == Value::TYPE_ABSOLUTE) {
		return grammar.evalFloat(value, shape);
	} else if (type == Value::TYPE_RELATIVE) {
		return grammar.evalFloat(value, shape) * size;
	} else {
		return grammar.evalFloat(value, shape);
	}
}

QDomElement Value::toXml(QDomDocument& doc) {
	QDomElement node = doc.createElement("param");
	if (type == TYPE_ABSOLUTE) {
		node.setAttribute("type", "absolute");
	}
	else if (type == TYPE_RELATIVE) {
		node.setAttribute("type", "relative");
	}
	else if (type == TYPE_FLOATING) {
		node.setAttribute("type", "floating");
	}
	node.setAttribute("value", value.c_str());
	if (repeat) {
		node.setAttribute("repeat", "true");
	}

	return node;
}

/**
 * このルールを指定されたshapeに適用する。
 * いくつかのオペレーション (compやsplitなど)は、適用後のshapeをstackに格納する。
 *
 * @param shape		shape
 * @param grammar	このshapeに適用されるルールセット (nugetに相当)
 * @param stack		stack
 */
void Rule::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack, std::vector<boost::shared_ptr<Shape> >& shapes) const {
	for (int i = 0; i < operators.size(); ++i) {
		shape = operators[i]->apply(shape, grammar, stack);
		if (shape == NULL) break;
	}
	
	if (shape != NULL) {
		if (operators.size() == 0 || operators.back()->name == "copy") {
			// copyで終わる場合、このshapeはもう必要ないので削除
			shape->_active = false;
			//shape->_grammar_type = grammar.type;
			shapes.push_back(shape);
		} else {
			// copyで終わらない場合、このshapeは描画する必要があるので、
			// shapesリストに格納する。
			shape->_active = true;
			//shape->_grammar_type = grammar.type;
			shapes.push_back(shape);
		}
	}
}

/**
 * 指定されたsizeをsplitした後の、各断片のサイズを計算する。
 *
 * @param size							もとのsize
 * @param sizes							指定された、各断片のサイズ
 * @param output_names					指定された、各断片の名前
 * @param ruleSet						ルール (sizeなどで変数が使用されている場合、解決するため)
 * @param decoded_sizes	[OUT]			計算された、各断片のサイズ
 * @param decoded_output_names [OUT]	計算された、各断片の名前
 */
void Rule::decodeSplitSizes(float size, const std::vector<Value>& sizes, const std::vector<std::string>& output_names, const Grammar& grammar, const boost::shared_ptr<Shape>& shape, std::vector<float>& decoded_sizes, std::vector<std::string>& decoded_output_names) {
	float regular_sum = 0.0f;
	float floating_sum = 0.0f;
	int repeat_count = 0;
	float repeat_unit = 0.0f;
	int repeat_num = 0;
	float repeat_scale = 1.0f;

	for (int i = 0; i < sizes.size(); ++i) {
		if (sizes[i].repeat) {
			repeat_count++;
		}
		else {
			if (sizes[i].type == Value::TYPE_ABSOLUTE) {
				regular_sum += grammar.evalFloat(sizes[i].value, shape);
			}
			else if (sizes[i].type == Value::TYPE_RELATIVE) {
				regular_sum += size * grammar.evalFloat(sizes[i].value, shape);
			}
			else if (sizes[i].type == Value::TYPE_FLOATING) {
				floating_sum += grammar.evalFloat(sizes[i].value, shape);
			}
		}
	}

	float floating_scale = 1.0f;
	if (floating_sum > 0 && repeat_count == 0) {
		floating_scale = std::max(0.0f, size - regular_sum) / floating_sum;
	}

	if (repeat_count > 0) {
		for (int i = 0; i < sizes.size(); ++i) {
			if (sizes[i].repeat) {
				repeat_unit += sizes[i].getEstimateValue(size - regular_sum - floating_sum * floating_scale, grammar, shape);
			}
		}

		repeat_num = std::max(0.0f, (size - regular_sum - floating_sum * floating_scale) / repeat_unit + 0.5f);
		if (repeat_num == 0) {
			if (size - regular_sum - floating_sum * floating_scale > 0) {
				repeat_num = 1;
			}
		}
		if (repeat_num > 0) {
			repeat_scale = std::max(0.0f, (size - regular_sum - floating_sum * floating_scale) / (float)repeat_num / repeat_unit);
		}

		if (floating_sum > 0) {
			floating_scale = std::max(0.0f, size - regular_sum - repeat_unit * repeat_scale * repeat_num) / floating_sum;
		}
	}

	for (int i = 0; i < sizes.size(); ++i) {
		if (sizes[i].repeat) {
			float s = sizes[i].getEstimateValue(size - regular_sum - floating_sum * floating_scale, grammar, shape);
			s *= repeat_scale;
			for (int k = 0; k < repeat_num; ++k) {
				decoded_sizes.push_back(s);
				decoded_output_names.push_back(output_names[i]);
			}
		}
		else {
			if (sizes[i].type == Value::TYPE_ABSOLUTE) {
				decoded_sizes.push_back(grammar.evalFloat(sizes[i].value, shape));
				decoded_output_names.push_back(output_names[i]);
			}
			else if (sizes[i].type == Value::TYPE_RELATIVE) {
				decoded_sizes.push_back(grammar.evalFloat(sizes[i].value, shape) * size);
				decoded_output_names.push_back(output_names[i]);
			}
			else if (sizes[i].type == Value::TYPE_FLOATING) {
				decoded_sizes.push_back(grammar.evalFloat(sizes[i].value, shape) * floating_scale);
				decoded_output_names.push_back(output_names[i]);
			}
		}
	}
}

bool Grammar::contain(const std::string& name) const {
	if (rules.find(name) == rules.end()) return false;
	else return true;
}

/**
 * 変数とその値を追加する。
 *
 * @param name		変数名
 * @param value		値
 */
void Grammar::addAttr(const std::string& name, const Attribute& value) {
	attrs[name] = value;
}

/**
 * ルールを追加する。
 *
 * @param name		ルール名 (左辺に来るnonterminalの名前)
 */
void Grammar::addRule(const std::string& name) {
	rules[name].operators.clear();
}

/**
 * ルールにオペレーションを追加する。
 *
 * @param name		ルール名
 * @param op		オペレーション
 */
void Grammar::addOperator(const std::string& name, const boost::shared_ptr<Operator>& op) {
	rules[name].operators.push_back(op);
}

/**
 * 指定された変数を、数値に変換する。
 *
 * @param attr_name		変数名
 * @param shape			shape
 * @return				変換された数値
 */
float Grammar::evalFloat(const std::string& attr_name, const boost::shared_ptr<Shape>& shape) const {
	myeval::calculator<std::string::const_iterator> calc;

	myeval::variables.clear();
	myeval::variables.add("scope.sx", shape->_scope.x);
	myeval::variables.add("scope.sy", shape->_scope.y);
	myeval::variables.add("scope.sz", shape->_scope.z);

	for (auto it = attrs.begin(); it != attrs.end(); ++it) {
		float val;
		if (sscanf(it->second.value.c_str(), "%f", &val) != EOF) {
			myeval::variables.add(it->first, val);
		}
	}

	float result;
	std::string::const_iterator iter = attr_name.begin();
	std::string::const_iterator end = attr_name.end();
	bool r = phrase_parse(iter, end, calc, boost::spirit::ascii::space, result);
	if (r && iter == end) {
		return result;
	} else {
		std::string rest(iter, end);
		std::cout << "Parsing failed\n";
		std::cout << "stopped at: \": " << rest << "\"\n";
		throw "Parsing failed\nstpped at: \": " + rest + "\"\n";
	}

	/*
	// To be fixed
	// 置換だと、変数BCが、変数ABCを置換してしまう。
	// 対策は？

	// scope.sx|y|zを置換
	std::string decoded_str = attr_name;
	std::string scope_x = std::to_string((long double)shape->_scope.x);
	std::string scope_y = std::to_string((long double)shape->_scope.y);
	std::string scope_z = std::to_string((long double)shape->_scope.z);
	boost::replace_all(decoded_str, "scope.sx", scope_x);
	boost::replace_all(decoded_str, "scope.sy", scope_y);
	boost::replace_all(decoded_str, "scope.sz", scope_z);

	// 変数を置換
	for (auto it = attrs.begin(); it != attrs.end(); ++it) {
		boost::replace_all(decoded_str, it->first, it->second);
	}

	return calculate(decoded_str);
	*/
}

/**
 * 指定された変数を、文字列に変換する。
 *
 * @param attr_name		変数名
 * @param shape			shape
 * @return				変換された文字列
 */
std::string Grammar::evalString(const std::string& attr_name, const boost::shared_ptr<Shape>& shape) const {
	if (attrs.find(attr_name) == attrs.end()) {
		return attr_name;
	} else {
		return attrs.at(attr_name).value;
	}
}

}
