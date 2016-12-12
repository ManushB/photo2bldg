#include "FacadeReconstruction.h"
#include "Utils.h"
#include "facadeA.h"
#include "facadeB.h"
#include "facadeC.h"
#include "facadeD.h"
#include "facadeE.h"
#include "facadeF.h"
#include "facadeG.h"
#include "facadeH.h"

namespace facarec {

	int NUM_GRAMMARS = 8;

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image) {
		std::vector<Prediction> predictions = classifier->Classify(input_image, NUM_GRAMMARS);
		return predictions[0].first;
	}

	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, float width, float height, int max_NF, int max_NC) {
		std::vector<float> params = regression->Predict(input_image);

		if (grammar_id == 0) {
			params = parameterEstimation1(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 1) {
			params = parameterEstimation2(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 2) {
			params = parameterEstimation3(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 3) {
			params = parameterEstimation4(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 4) {
			params = parameterEstimation5(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 5) {
			params = parameterEstimation6(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 6) {
			params = parameterEstimation7(width, height, max_NF, max_NC, params);
		}
		else if (grammar_id == 7) {
			params = parameterEstimation8(width, height, max_NF, max_NC, params);
		}

		for (int i = 0; i < params.size(); ++i) {
			params[i] = params[i] / 100;
		}

		return params;
	}

	std::vector<float> parameterEstimation1(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsA(width, height, std::make_pair(1, 20), std::make_pair(1, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeA(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation2(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsB(width, height, std::make_pair(2, 20), std::make_pair(1, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeB(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation3(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsC(width, height, std::make_pair(3, 20), std::make_pair(1, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeC(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}
	
	std::vector<float> parameterEstimation4(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsD(width, height, std::make_pair(3, 20), std::make_pair(1, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeD(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation5(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsE(width, height, std::make_pair(5, 20), std::make_pair(1, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeE(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26], decoded_params[27]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation6(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsF(width, height, std::make_pair(1, 20), std::make_pair(3, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeF(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation7(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsG(width, height, std::make_pair(1, 20), std::make_pair(3, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeG(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation8(float width, float height, int max_NF, int max_NC, std::vector<float> params) {
		std::vector<float> decoded_params;
		decodeParamsH(width, height, std::make_pair(3, 20), std::make_pair(3, 20), max_NF, max_NC, params, decoded_params);

		cv::Mat img = generateFacadeH(100, decoded_params[0], decoded_params[1], width, height, 3, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

}
