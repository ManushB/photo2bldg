#include "CGA.h"
#include "GLUtils.h"
#include "OBJLoader.h"
#include <map>
#include <iostream>
#include <random>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace cga {

/**
 * Randomly select parameter values if the range is specified for the parameter
 */
std::vector<float> randomParamValues(Grammar& grammar) {
	std::vector<float> param_values;

	for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it) {
		if (it->second.hasRange) {
			float r = (float)rand() / RAND_MAX;
			float v = r * (it->second.range_end - it->second.range_start) + it->second.range_start;
			it->second.value = boost::lexical_cast<std::string>(v);
			//param_values.push_back(v);
			param_values.push_back(r);
		}
	}

	return param_values;
}

std::vector<std::pair<float, float> > getParamRanges(const Grammar& grammar) {
	std::vector<std::pair<float, float> > ranges;

	for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it) {
		if (it->second.hasRange) {
			ranges.push_back(std::make_pair(it->second.range_start, it->second.range_end));
		}
	}

	return ranges;

}

/**
* Set parameter values.
* Each value is normalized to [0, 1], so it has to be populated based on the range.
* If the parameter value is out of [0, 1], it is forced to be between [0, 1].
*/
void setParamValues(Grammar& grammar, const std::vector<float>& params) {
	int count = 0;
	for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it, ++count) {
		if (it->second.hasRange) {
			float param = std::min(1.0f, std::max(0.0f, params[count]));

			grammar.attrs[it->first].value = boost::lexical_cast<std::string>((it->second.range_end - it->second.range_start) * param + it->second.range_start);
		}
	}
}

CGA::CGA() {
}


/**
 * Execute a derivation of the grammar
 */
void CGA::derive(const Grammar& grammar, bool suppressWarning) {
	shapes.clear();

	while (!stack.empty()) {
		boost::shared_ptr<Shape> shape = stack.front();
		stack.pop_front();

		if (grammar.contain(shape->_name)) {
			grammar.getRule(shape->_name).apply(shape, grammar, stack, shapes);
		} else {
			if (!suppressWarning && shape->_name.back() != '!' && shape->_name.back() != '.') {
				std::cout << "Warning: " << "no rule is found for " << shape->_name << "." << std::endl;
			}
			shapes.push_back(shape);
		}
	}
}

void CGA::derive(const std::vector<Grammar>& grammars, bool suppressWarning) {
	shapes.clear();

	while (!stack.empty()) {
		boost::shared_ptr<Shape> shape = stack.front();
		stack.pop_front();

		bool found = false;
		for (int i = 0; i < grammars.size(); ++i) {
			if (grammars[i].contain(shape->_name)) {
				grammars[i].getRule(shape->_name).apply(shape, grammars[i], stack, shapes);
				found = true;
				break;
			}
		}

		if (!found) {
			if (!suppressWarning && shape->_name.back() != '!' && shape->_name.back() != '.') {
				std::cout << "Warning: " << "no rule is found for " << shape->_name << "." << std::endl;
			}
			shapes.push_back(shape);
		}
	}
}

/**
 * Generate a geometry and add it to the render manager.
 */
void CGA::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, bool center) {
	for (int i = 0; i < shapes.size(); ++i) {
		shapes[i]->generateGeometry(faces, 1.0f);
	}

	if (center) {
		glutils::BoundingBox bbox;
		for (int i = 0; i < faces.size(); ++i) {
			for (int j = 0; j < faces[i]->vertices.size(); ++j) {
				bbox.addPoint(faces[i]->vertices[j].position);
			}
		}

		glm::vec3 centroid = bbox.center();

		for (int i = 0; i < faces.size(); ++i) {
			for (int j = 0; j < faces[i]->vertices.size(); ++j) {
				faces[i]->vertices[j].position -= centroid;
			}
		}
	}
}

}
