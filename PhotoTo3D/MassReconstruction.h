#pragma once

#include "Regression.h"
#include <dlib/optimization.h>
#include "CGA.h"
#include "VanishingPoint.h"

class GLWidget3D;

namespace massrec {

	typedef dlib::matrix<double, 0, 1> column_vector;

	class obj_function {
	public:
		obj_function(GLWidget3D* glWidget, cga::Grammar* grammar, cv::Mat silhouette_dist_map, float xrotMax, float xrotMin, float yrotMax, float yrotMin, float zrotMax, float zrotMin, float fovMax, float fovMin, float oxMax, float oxMin, float oyMax, float oyMin, float xMax, float xMin, float yMax, float yMin);
		double operator() (const column_vector& arg) const;

	private:
		GLWidget3D* glWidget;
		cga::Grammar* grammar;
		cv::Mat silhouette_dist_map;
		float xrotMin;
		float xrotMax;
		float yrotMin;
		float yrotMax;
		float zrotMin;
		float zrotMax;
		float fovMin;
		float fovMax;
		float oxMin;
		float oxMax;
		float oyMin;
		float oyMax;
		float xMin;
		float xMax;
		float yMin;
		float yMax;
	};

	std::vector<float> parameterEstimation(GLWidget3D* glWidget, boost::shared_ptr<Regression> regression, cga::Grammar* grammar, const std::vector<vp::VanishingLine>& silhouette, int image_size, float cameraDistanceBase, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float zrotMin, float zrotMax, float fovMin, float fovMax, float oxMin, float oxMax, float oyMin, float oyMax, float xMin, float xMax, float yMin, float yMax, int silhouette_line_type, bool imageBlur, int imageBlurSize, bool refinement, int maxIters, int refinement_method);

}

