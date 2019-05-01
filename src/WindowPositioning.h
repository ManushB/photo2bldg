#pragma once

#include "Regression.h"
#include <dlib/optimization.h>
#include <vector>

namespace wp {

	typedef dlib::matrix<double, 0, 1> column_vector;

	class obj_function {
	public:
		obj_function(cv::Mat dist_map, float initial_x1, float initial_y1, float initial_x2, float initial_y2);
		double operator() (const column_vector& arg) const;

	private:
		cv::Mat dist_map;
		float initial_x1;
		float initial_y1;
		float initial_x2;
		float initial_y2;
	};

	std::vector<float> parameterEstimation(boost::shared_ptr<Regression> regression, const cv::Mat& tile, bool refine, int maxIters);
	float distance(const cv::Mat& dist_map, float initial_x1, float initial_y1, float initial_x2, float initial_y2, float x1, float y1, float x2, float y2);
}
