#include "FacadeReconstruction.h"

namespace facarec {

	int NUM_GRAMMARS = 8;

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image) {
		std::vector<Prediction> predictions = classifier->Classify(input_image, NUM_GRAMMARS);
		return predictions[0].first;
	}

	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image) {
		std::vector<float> params = regression->Predict(input_image);

		return params;
	}

}
