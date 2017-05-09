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

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image, int mass_grammar_id) {
		std::vector<Prediction> predictions = classifier->Classify(input_image, NUM_GRAMMARS);

		for (int i = 0; i < predictions.size(); ++i) {
			// HACK
			// if the building mass is a cylinder shape, we use only the first 5 facade grammars as valid.
			if (mass_grammar_id != 1 || predictions[i].first < 5) {
				return predictions[i].first;
			}
		}

		return 0;
	}

	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<int>& selected_win_types) {
		std::vector<float> params = regression->Predict(input_image);

		if (grammar_id == 0) {
			params = parameterEstimation1(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 1) {
			params = parameterEstimation2(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 2) {
			params = parameterEstimation3(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 3) {
			params = parameterEstimation4(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 4) {
			params = parameterEstimation5(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 5) {
			params = parameterEstimation6(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 6) {
			params = parameterEstimation7(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}
		else if (grammar_id == 7) {
			params = parameterEstimation8(width, height, mass_grammar_id, max_NF, max_NC, num_floors, num_columns, params, selected_win_types);
		}

		for (int i = 0; i < params.size(); ++i) {
			params[i] = params[i] / 100;
		}

		return params;
	}

	std::vector<float> parameterEstimation1(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeA::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeA::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation2(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeB::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeB::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation3(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeC::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeC::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}
	
	std::vector<float> parameterEstimation4(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeD::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeD::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation5(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeE::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeE::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation6(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeF::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeF::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation7(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeG::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeG::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26], decoded_params[27], decoded_params[28]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

	std::vector<float> parameterEstimation8(float width, float height, int mass_grammar_id, int max_NF, int max_NC, int num_floors, int num_columns, std::vector<float> params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;
		FacadeH::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		cv::Mat img = FacadeH::generateFacade(100, width, height, 3, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21]);
		cv::imwrite("estimated_facade.png", img);

		return decoded_params;
	}

}
