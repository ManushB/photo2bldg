#pragma once

#include "Classifier.h"
#include "Regression.h"
#include <boost/shared_ptr.hpp>
#include <opencv2/core.hpp>
#include <glm/glm.hpp>

namespace facarec {

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image);
	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, float width, float height, int max_NF, int max_NC);
	std::vector<float> parameterEstimation1(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation2(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation3(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation4(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation5(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation6(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation7(float width, float height, int max_NF, int max_NC, std::vector<float> params);
	std::vector<float> parameterEstimation8(float width, float height, int max_NF, int max_NC, std::vector<float> params);

}
