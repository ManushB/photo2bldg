#include "GrammarParser.h"
#include "CenterOperator.h"
#include "ColorOperator.h"
#include "CompOperator.h"
#include "CopyOperator.h"
#include "CornerCutOperator.h"
#include "ExtrudeOperator.h"
#include "HemisphereOperator.h"
#include "InnerCircleOperator.h"
#include "InnerSemiCircleOperator.h"
#include "InsertOperator.h"
#include "OffsetOperator.h"
#include "PyramidOperator.h"
#include "RoofGableOperator.h"
#include "RoofHipOperator.h"
#include "RotateOperator.h"
#include "SetupProjectionOperator.h"
#include "ShapeLOperator.h"
#include "ShapeUOperator.h"
#include "SizeOperator.h"
#include "SplitOperator.h"
#include "TaperOperator.h"
#include "TextureOperator.h"
#include "TranslateOperator.h"
#include "CGA.h"
#include <iostream>
#include "Grammar.h"

namespace cga {

void parseGrammar(const char* filename, Grammar& grammar) {
	QFile file(filename);

	QDomDocument doc;
	doc.setContent(&file, true);
	QDomElement root = doc.documentElement();

	parseGrammar(root, grammar);
}

void parseGrammar(const char* filename, std::vector<Grammar>& grammars) {
	QFile file(filename);

	QDomDocument doc;
	doc.setContent(&file, true);
	QDomElement root = doc.documentElement();
	QDomNode child_node = root.firstChild();
	while (!child_node.isNull()) {
		if (child_node.toElement().tagName() == "grammar") {
			Grammar grammar;
			parseGrammar(child_node.toElement(), grammar);
			grammars.push_back(grammar);
		}

		child_node = child_node.nextSibling();
	}
}

void parseGrammar(QDomElement& root, Grammar& grammar) {
	QDomNode child_node = root.firstChild();
	while (!child_node.isNull()) {
		if (child_node.toElement().tagName() == "attr") {
			if (!child_node.toElement().hasAttribute("name")) {
				throw "<attr> tag must contain name attribute.";
			}
			std::string name = child_node.toElement().attribute("name").toUtf8().constData();

			if (!child_node.toElement().hasAttribute("value")) {
				throw "<attr> tag must contain value attribute.";
			}
			std::string value = child_node.toElement().attribute("value").toUtf8().constData();

			std::vector<float> range;
			if (child_node.toElement().hasAttribute("range")) {
				QStringList value = child_node.toElement().attribute("range").split(",");
				range.push_back(value[0].toFloat());
				range.push_back(value[1].toFloat());
			}

			if (range.size() > 0) {
				grammar.addAttr(name, Attribute(name, value, range[0], range[1]));
			}
			else {
				grammar.addAttr(name, Attribute(name, value));
			}
		}
		else if (child_node.toElement().tagName() == "rule") {
			if (!child_node.toElement().hasAttribute("name")) {
				throw "<rule> tag must contain name attribute.";
			}
			std::string name = child_node.toElement().attribute("name").toUtf8().constData();

			grammar.addRule(name);

			QDomNode operator_node = child_node.firstChild();
			while (!operator_node.isNull()) {
				std::string operator_name = operator_node.toElement().tagName().toUtf8().constData();

				if (operator_name == "center") {
					grammar.addOperator(name, parseCenterOperator(operator_node));
				}
				else if (operator_name == "color") {
					grammar.addOperator(name, parseColorOperator(operator_node));
				}
				else if (operator_name == "comp") {
					grammar.addOperator(name, parseCompOperator(operator_node));
				}
				else if (operator_name == "copy") {
					grammar.addOperator(name, parseCopyOperator(operator_node));
				}
				else if (operator_name == "cornerCut") {
					grammar.addOperator(name, parseCornerCutOperator(operator_node));
				}
				else if (operator_name == "extrude") {
					grammar.addOperator(name, parseExtrudeOperator(operator_node));
				}
				else if (operator_name == "hemisphere") {
					grammar.addOperator(name, parseHemisphereOperator(operator_node));
				}
				else if (operator_name == "innerCircle") {
					grammar.addOperator(name, parseInnerCircleOperator(operator_node));
				}
				else if (operator_name == "innerSemiCircle") {
					grammar.addOperator(name, parseInnerSemiCircleOperator(operator_node));
				}
				else if (operator_name == "insert") {
					grammar.addOperator(name, parseInsertOperator(operator_node));
				}
				else if (operator_name == "offset") {
					grammar.addOperator(name, parseOffsetOperator(operator_node));
				}
				else if (operator_name == "pyramid") {
					grammar.addOperator(name, parsePyramidOperator(operator_node));
				}
				else if (operator_name == "roofGable") {
					grammar.addOperator(name, parseRoofGableOperator(operator_node));
				}
				else if (operator_name == "roofHip") {
					grammar.addOperator(name, parseRoofHipOperator(operator_node));
				}
				else if (operator_name == "rotate") {
					grammar.addOperator(name, parseRotateOperator(operator_node));
				}
				else if (operator_name == "setupProjection") {
					grammar.addOperator(name, parseSetupProjectionOperator(operator_node));
				}
				else if (operator_name == "shapeL") {
					grammar.addOperator(name, parseShapeLOperator(operator_node));
				}
				else if (operator_name == "shapeU") {
					grammar.addOperator(name, parseShapeUOperator(operator_node));
				}
				else if (operator_name == "size") {
					grammar.addOperator(name, parseSizeOperator(operator_node));
				}
				else if (operator_name == "split") {
					grammar.addOperator(name, parseSplitOperator(operator_node));
				}
				else if (operator_name == "taper") {
					grammar.addOperator(name, parseTaperOperator(operator_node));
				}
				else if (operator_name == "texture") {
					grammar.addOperator(name, parseTextureOperator(operator_node));
				}
				else if (operator_name == "translate") {
					grammar.addOperator(name, parseTranslateOperator(operator_node));
				}

				operator_node = operator_node.nextSibling();
			}
		}

		child_node = child_node.nextSibling();
	}
}

boost::shared_ptr<Operator> parseCenterOperator(const QDomNode& node) {
	int axesSelector;

	if (!node.toElement().hasAttribute("axesSelector")) {
		throw "center node has to have axesSelector attribute.";
	}

	if (node.toElement().attribute("axesSelector") == "xyz") {
		axesSelector = AXES_SELECTOR_XYZ;
	} else if (node.toElement().attribute("axesSelector") == "x") {
		axesSelector = AXES_SELECTOR_X;
	} else if (node.toElement().attribute("axesSelector") == "y") {
		axesSelector = AXES_SELECTOR_Y;
	} else if (node.toElement().attribute("axesSelector") == "z") {
		axesSelector = AXES_SELECTOR_Z;
	} else if (node.toElement().attribute("axesSelector") == "xy") {
		axesSelector = AXES_SELECTOR_XY;
	} else if (node.toElement().attribute("axesSelector") == "xz") {
		axesSelector = AXES_SELECTOR_XZ;
	} else {
		axesSelector = AXES_SELECTOR_YZ;
	}

	return boost::shared_ptr<Operator>(new CenterOperator(axesSelector));
}

boost::shared_ptr<Operator> parseColorOperator(const QDomNode& node) {
	std::string r;
	std::string g;
	std::string b;
	std::string s;

	if (node.toElement().hasAttribute("r")) {
		r = node.toElement().attribute("r").toUtf8().constData();
	}
	if (node.toElement().hasAttribute("g")) {
		g = node.toElement().attribute("g").toUtf8().constData();
	}
	if (node.toElement().hasAttribute("b")) {
		b = node.toElement().attribute("b").toUtf8().constData();
	}
	if (node.toElement().hasAttribute("s")) {
		s = node.toElement().attribute("s").toUtf8().constData();
	}

	if (s.empty()) {
		return boost::shared_ptr<Operator>(new ColorOperator(r, g, b));
	} else {
		return boost::shared_ptr<Operator>(new ColorOperator(s));
	}
}

boost::shared_ptr<Operator> parseCompOperator(const QDomNode& node) {
	std::string front_name;
	std::string side_name;
	std::string top_name;
	std::string bottom_name;
	std::string inside_name;
	std::string border_name;
	std::string vertical_name;
	std::map<std::string, std::string> name_map;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");
			std::string value = child.toElement().attribute("value").toUtf8().constData();

			if (name == "front") {
				name_map["front"] = value;
			} else if (name == "right") {
				name_map["right"] = value;
			} else if (name == "left") {
				name_map["left"] = value;
			} else if (name == "back") {
				name_map["back"] = value;
			} else if (name == "side") {
				name_map["side"] = value;
			} else if (name == "top") {
				name_map["top"] = value;
			} else if (name == "bottom") {
				name_map["bottom"] = value;
			} else if (name == "inside") {
				name_map["inside"] = value;
			} else if (name == "border") {
				name_map["border"] = value;
			} else if (name == "vertical") {
				name_map["vertical"] = value;
			}
		}

		child = child.nextSibling();
	}

	return boost::shared_ptr<Operator>(new CompOperator(name_map));
}

boost::shared_ptr<Operator> parseCopyOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("name")) {
		throw "copy node has to have name attribute.";
	}

	std::string copy_name = node.toElement().attribute("name").toUtf8().constData();

	return boost::shared_ptr<Operator>(new CopyOperator(copy_name));
}

boost::shared_ptr<Operator> parseCornerCutOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("type")) {
		throw "curnerCut node has to have type attribute.";
	}
	int type;
	if (node.toElement().attribute("type") == "straight") {
		type = CORNER_CUT_STRAIGHT;
	} else if (node.toElement().attribute("type") == "curve") {
		type = CORNER_CUT_CURVE;
	} else {
		type = CORNER_CUT_NEGATIVE_CURVE;
	}

	if (!node.toElement().hasAttribute("length")) {
		throw "curnerCut node has to have length attribute.";
	}
	std::string length = node.toElement().attribute("length").toUtf8().constData();

	return boost::shared_ptr<Operator>(new CornerCutOperator(type, length));
}

boost::shared_ptr<Operator> parseExtrudeOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("height")) {
		throw "extrude node has to have height attribute.";
	}

	std::string height = node.toElement().attribute("height").toUtf8().constData();

	return boost::shared_ptr<Operator>(new ExtrudeOperator(height));
}

boost::shared_ptr<Operator> parseHemisphereOperator(const QDomNode& node) {
	return boost::shared_ptr<Operator>(new HemisphereOperator());
}

boost::shared_ptr<Operator> parseInnerCircleOperator(const QDomNode& node) {
	return boost::shared_ptr<Operator>(new InnerCircleOperator());
}

boost::shared_ptr<Operator> parseInnerSemiCircleOperator(const QDomNode& node) {
	return boost::shared_ptr<Operator>(new InnerSemiCircleOperator());
}

boost::shared_ptr<Operator> parseInsertOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("geometryPath")) {
		throw "insert node has to have geometryPath attribute.";
	}

	std::string geometryPath = node.toElement().attribute("geometryPath").toUtf8().constData();

	return boost::shared_ptr<Operator>(new InsertOperator(geometryPath));
}

boost::shared_ptr<Operator> parseOffsetOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("offsetDistance")) {
		throw "offset node has to have offsetDistance attribute.";
	}

	std::string offsetDistance = node.toElement().attribute("offsetDistance").toUtf8().constData();
	std::string inside = node.toElement().attribute("inside").toUtf8().constData();
	std::string border = node.toElement().attribute("border").toUtf8().constData();

	return boost::shared_ptr<Operator>(new OffsetOperator(offsetDistance, inside, border));
}

boost::shared_ptr<Operator> parsePyramidOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("height")) {
		throw "pyramid node has to have height attribute.";
	}

	std::string height = node.toElement().attribute("height").toUtf8().constData();

	return boost::shared_ptr<Operator>(new PyramidOperator(height));
}

boost::shared_ptr<Operator> parseRoofGableOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("angle")) {
		throw "roofGable node has to have angle attribute.";
	}

	std::string angle = node.toElement().attribute("angle").toUtf8().constData();

	return boost::shared_ptr<Operator>(new RoofGableOperator(angle));
}

boost::shared_ptr<Operator> parseRoofHipOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("angle")) {
		throw "roofHip node has to have angle attribute.";
	}

	std::string angle = node.toElement().attribute("angle").toUtf8().constData();

	return boost::shared_ptr<Operator>(new RoofHipOperator(angle));
}

boost::shared_ptr<Operator> parseRotateOperator(const QDomNode& node) {
	float xAngle = 0.0f;
	float yAngle = 0.0f;
	float zAngle = 0.0f;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");

			if (name == "xAngle") {
				xAngle = child.toElement().attribute("value").toFloat();
			} else if (name == "yAngle") {
				yAngle = child.toElement().attribute("value").toFloat();
			} else if (name == "zAngle") {
				zAngle = child.toElement().attribute("value").toFloat();
			}
		}

		child = child.nextSibling();
	}

	return boost::shared_ptr<Operator>(new RotateOperator(xAngle, yAngle, zAngle));
}

boost::shared_ptr<Operator> parseSetupProjectionOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("axesSelector")) {
		throw "setupProjection node has to have axesSelector attribute.";
	}

	int axesSelector;
	std::string sAxesSelector = node.toElement().attribute("axesSelector").toUtf8().constData();
	if (sAxesSelector == "scope.xy") {
		axesSelector = AXES_SCOPE_XY;
	} else if (sAxesSelector == "scope.xz") {
		axesSelector = AXES_SCOPE_XZ;
	} else {
		axesSelector = AXES_SCOPE_XY;
	}

	Value texWidth;
	Value texHeight;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");

			if (name == "texWidth") {
				std::string type =  child.toElement().attribute("type").toUtf8().constData();
				std::string value =  child.toElement().attribute("value").toUtf8().constData();
				if (type == "absolute") {
					texWidth = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					texWidth = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of texWidth for texture has to be either absolute or relative.";
				}
			} else if (name == "texHeight") {
				std::string type =  child.toElement().attribute("type").toUtf8().constData();
				std::string value =  child.toElement().attribute("value").toUtf8().constData();
				if (type == "absolute") {
					texHeight = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					texHeight = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of texHeight for texture has to be either absolute or relative.";
				}
			}
		}

		child = child.nextSibling();
	}

	return boost::shared_ptr<Operator>(new SetupProjectionOperator(axesSelector, texWidth, texHeight));
}

boost::shared_ptr<Operator> parseShapeLOperator(const QDomNode& node) {
	Value frontWidth;
	Value rightWidth;
	bool frontWidthFound = false;
	bool rightWidthFound = false;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");
			QString type;

			if (!child.toElement().hasAttribute("type")) {
				throw "param node under size node has to have type attribute.";
			}

			type = child.toElement().attribute("type");
			std::string value = child.toElement().attribute("value").toUtf8().constData();

			if (name == "frontWidth") {
				frontWidthFound = true;
				if (type == "relative") {
					frontWidth = Value(Value::TYPE_RELATIVE, value);
				}
				else if (type == "absolute") {
					frontWidth = Value(Value::TYPE_ABSOLUTE, value);
				}
				else {
					throw "type attribute under shapeL node has to be either relative or absolute.";
				}
			}
			else if (name == "rightWidth") {
				rightWidthFound = true;
				if (type == "relative") {
					rightWidth = Value(Value::TYPE_RELATIVE, value);
				}
				else if (type == "absolute") {
					rightWidth = Value(Value::TYPE_ABSOLUTE, value);
				}
				else {
					throw "type attribute under shapeL node has to be either relative or absolute.";
				}
			}
		}

		child = child.nextSibling();
	}

	if (!frontWidthFound) {
		throw "shapeL node has to have frontWidth parametter.";
	}
	if (!rightWidthFound) {
		throw "shapeL node has to have rightWidth parametter.";
	}

	return boost::shared_ptr<Operator>(new ShapeLOperator(frontWidth, rightWidth));
}

boost::shared_ptr<Operator> parseShapeUOperator(const QDomNode& node) {
	Value frontWidth;
	Value backDepth;
	bool frontWidthFound = false;
	bool backDepthFound = false;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");
			QString type;

			if (!child.toElement().hasAttribute("type")) {
				throw "param node under size node has to have type attribute.";
			}

			type = child.toElement().attribute("type");
			std::string value = child.toElement().attribute("value").toUtf8().constData();

			if (name == "frontWidth") {
				frontWidthFound = true;
				if (type == "relative") {
					frontWidth = Value(Value::TYPE_RELATIVE, value);
				}
				else if (type == "absolute") {
					frontWidth = Value(Value::TYPE_ABSOLUTE, value);
				}
				else {
					throw "type attribute under shapeU node has to be either relative or absolute.";
				}
			}
			else if (name == "backDepth") {
				backDepthFound = true;
				if (type == "relative") {
					backDepth = Value(Value::TYPE_RELATIVE, value);
				}
				else if (type == "absolute") {
					backDepth = Value(Value::TYPE_ABSOLUTE, value);
				}
				else {
					throw "type attribute under shapeU node has to be either relative or absolute.";
				}
			}
		}

		child = child.nextSibling();
	}

	if (!frontWidthFound) {
		throw "shapeU node has to have frontWidth parametter.";
	}
	if (!backDepthFound) {
		throw "shapeU node has to have backDepth parametter.";
	}

	return boost::shared_ptr<Operator>(new ShapeUOperator(frontWidth, backDepth));
}

boost::shared_ptr<Operator> parseSizeOperator(const QDomNode& node) {
	Value xSize;
	Value ySize;
	Value zSize;
	bool centered = false;

	if (node.toElement().hasAttribute("centered")) {
		if (node.toElement().attribute("centered") == "true") {
			centered = true;
		}
	}

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");
			QString type;

			if (!child.toElement().hasAttribute("type")) {
				throw "param node under size node has to have type attribute.";
			}

			type = child.toElement().attribute("type");
			std::string value = child.toElement().attribute("value").toUtf8().constData();

			if (name == "xSize") {
				if (type == "relative") {
					xSize = Value(Value::TYPE_RELATIVE, value);
				} else if (type == "absolute") {
					xSize = Value(Value::TYPE_ABSOLUTE, value);
				} else {
					throw "type attribute under size node has to be either relative or absolute.";
				}
			} else if (name == "ySize") {
				if (type == "relative") {
					ySize = Value(Value::TYPE_RELATIVE, value);
				} else if (type == "absolute") {
					ySize = Value(Value::TYPE_ABSOLUTE, value);
				} else {
					throw "type attribute under size node has to be either relative or absolute.";
				}
			} else if (name == "zSize") {
				if (type == "relative") {
					zSize = Value(Value::TYPE_RELATIVE, value);
				} else if (type == "absolute") {
					zSize = Value(Value::TYPE_ABSOLUTE, value);
				} else {
					throw "type attribute under size node has to be either relative or absolute.";
				}
			}
		}

		child = child.nextSibling();
	}

	return boost::shared_ptr<Operator>(new SizeOperator(xSize, ySize, zSize, centered));
}

boost::shared_ptr<Operator> parseSplitOperator(const QDomNode& node) {
	int splitAxis;
	std::vector<Value> sizes;
	std::vector<std::string> names;

	if (!node.toElement().hasAttribute("splitAxis")) {
		throw "split node has to have splitAxis attribute.";
	}
	if (node.toElement().attribute("splitAxis") == "x") {
		splitAxis = DIRECTION_X;
	} else if (node.toElement().attribute("splitAxis") == "y") {
		splitAxis = DIRECTION_Y;
	} else {
		splitAxis = DIRECTION_Z;
	}

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString type = child.toElement().attribute("type");
			std::string value = child.toElement().attribute("value").toUtf8().constData();
			bool repeat = false;
			if (child.toElement().hasAttribute("repeat")) {
				repeat = true;
			}

			if (repeat) {
				if (type == "absolute") {
					sizes.push_back(Value(Value::TYPE_ABSOLUTE, value, true));
				} else if (type == "relative") {
					sizes.push_back(Value(Value::TYPE_RELATIVE, value, true));
				} else {
					sizes.push_back(Value(Value::TYPE_FLOATING, value, true));
				}
			} else {
				if (type == "absolute") {
					sizes.push_back(Value(Value::TYPE_ABSOLUTE, value));
				} else if (type == "relative") {
					sizes.push_back(Value(Value::TYPE_RELATIVE, value));
				} else {
					sizes.push_back(Value(Value::TYPE_FLOATING, value));
				}
			}

			names.push_back(child.toElement().attribute("name").toUtf8().constData());
		}

		child = child.nextSibling();
	}

	return boost::shared_ptr<Operator>(new SplitOperator(splitAxis, sizes, names));
}

boost::shared_ptr<Operator> parseTaperOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("height")) {
		throw "taper node has to have height attribute.";
	}
	if (!node.toElement().hasAttribute("slope")) {
		throw "taper node has to have slope attribute.";
	}

	std::string height = node.toElement().attribute("height").toUtf8().constData();
	std::string slope = node.toElement().attribute("slope").toUtf8().constData();

	return boost::shared_ptr<Operator>(new TaperOperator(height, slope));
}

boost::shared_ptr<Operator> parseTextureOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("texturePath")) {
		throw "texture node has to have texturePathtexturePath attribute.";
	}

	std::string texture = node.toElement().attribute("texturePath").toUtf8().constData();

	return boost::shared_ptr<Operator>(new TextureOperator(texture));
}

boost::shared_ptr<Operator> parseTranslateOperator(const QDomNode& node) {
	int mode;
	int coordSystem;
	Value x;
	Value y;
	Value z;

	if (!node.toElement().hasAttribute("mode")) {
		throw "translate node has to have mode attribute.";
	}
	if (node.toElement().attribute("mode") == "abs") {
		mode = MODE_ABSOLUTE;
	} else if (node.toElement().attribute("mode") == "rel") {
		mode = MODE_RELATIVE;
	} else {
		throw "mode has to be either abs or rel.";
	}

	if (!node.toElement().hasAttribute("coordSystem")) {
		throw "translate node has to have coordSystem attribute.";
	}
	if (node.toElement().attribute("coordSystem") == "world") {
		coordSystem = COORD_SYSTEM_WORLD;
	} else if (node.toElement().attribute("coordSystem") == "object") {
		coordSystem = COORD_SYSTEM_OBJECT;
	} else {
		throw "coordSystem has to be either world or object.";
	}

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			if (!child.toElement().hasAttribute("name")) {
				throw "param has to have name attribute.";
			}
			std::string name = child.toElement().attribute("name").toUtf8().constData();
			if (!child.toElement().hasAttribute("value")) {
				throw "param has to have value attribute.";
			}
			std::string value = child.toElement().attribute("value").toUtf8().constData();
			if (!child.toElement().hasAttribute("type")) {
				throw "param has to have type attribute.";
			}
			std::string type = child.toElement().attribute("type").toUtf8().constData();

			if (name == "x") {
				if (type == "absolute") {
					x = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					x = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of param for translate has to be either absolute or relative.";
				}
			} else if (name == "y") {
				if (type == "absolute") {
					y = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					y = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of param for translate has to be either absolute or relative.";
				}
			} else if (name == "z") {
				if (type == "absolute") {
					z = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					z = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of param for translate has to be either absolute or relative.";
				}
			}
		}

		child = child.nextSibling();
	}

	return boost::shared_ptr<Operator>(new TranslateOperator(mode, coordSystem, x, y, z));
}

}
