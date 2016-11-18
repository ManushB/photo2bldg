#pragma once

#include "Classifier.h"
#include "Regression.h"
#include <boost/shared_ptr.hpp>
#include <opencv2/core.hpp>
#include <glm/glm.hpp>

namespace facarec {

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image);
	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, int width, int height);
	std::vector<float> parameterEstimation1(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation2(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation3(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation4(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation5(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation6(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation7(int width, int height, std::vector<float> params);
	std::vector<float> parameterEstimation8(int width, int height, std::vector<float> params);

}
