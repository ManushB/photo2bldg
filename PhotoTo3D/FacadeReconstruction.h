#pragma once

#include "Classifier.h"
#include "Regression.h"
#include <boost/shared_ptr.hpp>
#include <opencv2/core.hpp>
#include <glm/glm.hpp>

namespace facarec {

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image);
	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, int width, int height);

}
