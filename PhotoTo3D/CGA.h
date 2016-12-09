#pragma once

#include "RenderManager.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Vertex.h"
#include "Grammar.h"
#include "Shape.h"

namespace cga {

enum { DIRECTION_X = 0, DIRECTION_Y, DIRECTION_Z, SCOPE_SX, SCOPE_SY };
enum { REVOLVE_X = 0, REVOLVE_Y };
enum { MODE_ABSOLUTE = 0, MODE_RELATIVE };
enum { COORD_SYSTEM_WORLD = 0, COORD_SYSTEM_OBJECT };
enum { AXES_SCOPE_XY = 0, AXES_SCOPE_XZ, AXES_SCOPE_YX, AXES_SCOPE_YZ, AXES_SCOPE_ZX, AXES_SCOPE_ZY, AXES_WORLD_XY, AXES_WORLD_XZ, AXES_WORLD_YX, AXES_WORLD_YZ, AXES_WORLD_ZX, AXES_WORLD_ZY };
enum { SELECTOR_ALL = 0, SELECTOR_INSIDE, SELECTOR_BORDER };
enum { AXES_SELECTOR_XYZ = 0, AXES_SELECTOR_X, AXES_SELECTOR_Y, AXES_SELECTOR_Z, AXES_SELECTOR_XY, AXES_SELECTOR_XZ, AXES_SELECTOR_YZ };
enum { CORNER_CUT_STRAIGHT = 0, CORNER_CUT_CURVE, CORNER_CUT_NEGATIVE_CURVE };

const float M_PI = 3.14159265358979f;
const int CIRCLE_SLICES = 36;

std::vector<float> randomParamValues(Grammar& grammar);
std::vector<std::pair<float, float> > getParamRanges(const Grammar& grammar);
void setParamValues(Grammar& grammar, const std::vector<float>& params);

class CGA {
public:
	glm::mat4 modelMat;
	std::list<boost::shared_ptr<Shape> > stack;
	std::vector<boost::shared_ptr<Shape> > shapes;

public:
	CGA();
	void derive(const Grammar& grammar, bool suppressWarning = false);
	void derive(const std::vector<Grammar>& grammars, bool suppressWarning = false);
	void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, bool center);
};

}
