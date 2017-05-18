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

	obj_function::obj_function(int grammar_id, cv::Mat initial_facade_parsing, int num_floors, int num_columns, const std::vector<int>& selected_win_types) {
		this->grammar_id = grammar_id;
		this->initial_facade_parsing = initial_facade_parsing;
		this->num_floors = num_floors;
		this->num_columns = num_columns;
		this->selected_win_types = selected_win_types;
	}

	double obj_function::operator() (const column_vector& arg) const {
		std::vector<float> params;
		for (int k = 0; k < arg.size(); ++k) {
			params.push_back(std::max(0.0, arg(k, 0)));
		}

		cv::Mat result_img;
		if (grammar_id == 0) {
			parameterEstimation1(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 1) {
			parameterEstimation2(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 2) {
			parameterEstimation3(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 3) {
			parameterEstimation4(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 4) {
			parameterEstimation5(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 5) {
			parameterEstimation6(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 6) {
			parameterEstimation7(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}
		else if (grammar_id == 7) {
			parameterEstimation8(initial_facade_parsing.cols, initial_facade_parsing.rows, num_floors, num_columns, params, selected_win_types, -1, cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), result_img);
		}

		// TODO
		// calculate the distance between facade_dist_map and result_dist_map
		// ...
		float diff = 0.0f;
		for (int r = 0; r < result_img.rows; ++r) {
			for (int c = 0; c < result_img.cols; ++c) {
				cv::Vec3b col1 = result_img.at<cv::Vec3b>(r, c);
				cv::Vec3b col2 = initial_facade_parsing.at<cv::Vec3b>(r, c);
				if (col1[0] != col2[0] || col1[1] != col2[1] || col1[2] != col2[2]) {
					diff += 1;
				}
			}
		}

		//std::cout << "dist=" << (float)diff / result_img.rows / result_img.cols << std::endl;
		return (float)diff / result_img.rows / result_img.cols;
	}

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

	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, float width, float height, int num_floors, int num_columns, const cv::Mat& initial_facade_parsing, std::vector<int>& selected_win_types) {
		std::vector<float> params = regression->Predict(input_image);
		for (int i = 2; i < params.size(); i++) {
			params[i] = std::max(0.0f, params[i]);
		}
		utils::output_vector(params);

#if 1
		/////////////////////////////////////////////////////////////////////////////
		// refine the parameters based on the initial facade parsing
		column_vector starting_point(params.size());
		column_vector lower_bound(params.size());
		column_vector upper_bound(params.size());
		for (int k = 0; k < params.size(); ++k) {
			starting_point(k, 0) = params[k];
			lower_bound(k, 0) = params[k] - 0.1;
			upper_bound(k, 0) = params[k] + 0.1;
		}

		try {
			double dist = find_min_bobyqa(obj_function(grammar_id, initial_facade_parsing, num_floors, num_columns, selected_win_types),
				starting_point,
				params.size() * 4,
				lower_bound,
				upper_bound,
				0.03,
				0.001,
				3000
				);
			std::cout << "Final dist after optimization: " << dist << std::endl;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		for (int k = 0; k < params.size(); ++k) {
			params[k] = starting_point(k, 0);
		}
#endif

		for (int i = 2; i < params.size(); i++) {
			params[i] = std::max(0.0f, params[i]);
		}


		cv::Mat result_img;
		if (grammar_id == 0) {
			FacadeA::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 1) {
			FacadeB::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 2) {
			FacadeC::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 3) {
			FacadeD::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 4) {
			FacadeE::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 5) {
			FacadeF::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 6) {
			FacadeG::attachDoors(params, selected_win_types);
		}
		else if (grammar_id == 7) {
			FacadeH::attachDoors(params, selected_win_types);
		}

		return params;
	}

	std::vector<float> parameterEstimation1(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeA::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeA::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9]);

		return decoded_params;
	}

	std::vector<float> parameterEstimation2(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeB::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeB::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15]);

		return decoded_params;
	}

	std::vector<float> parameterEstimation3(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeC::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeC::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20]);

		return decoded_params;
	}
	
	std::vector<float> parameterEstimation4(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeD::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeD::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26]);

		return decoded_params;
	}

	std::vector<float> parameterEstimation5(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeE::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeE::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15]);

		return decoded_params;
	}

	std::vector<float> parameterEstimation6(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeF::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeF::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26]);

		return decoded_params;
	}

	std::vector<float> parameterEstimation7(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeG::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeG::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26], decoded_params[27], decoded_params[28]);

		return decoded_params;
	}

	std::vector<float> parameterEstimation8(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img) {
		std::vector<float> decoded_params;
		FacadeH::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

		result_img = FacadeH::generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21]);

		return decoded_params;
	}

	void generateFacadeImage(int grammar_id, int width, int height, int num_floors, int num_columns, std::vector<float>& params, std::vector<int>& selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& image) {
		if (grammar_id == 0) {
			image = FacadeA::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 1) {
			image = FacadeB::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 2) {
			image = FacadeC::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 3) {
			image = FacadeD::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 4) {
			image = FacadeE::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 5) {
			image = FacadeF::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 6) {
			image = FacadeG::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
		else if (grammar_id == 7) {
			image = FacadeH::generateFacade(width, height, thickness, num_floors, num_columns, params, selected_win_types, bg_color, fg_color);
		}
	}

	std::vector<float> decodeParameters(int grammar_id, float width, float height, int num_floors, int num_columns, std::vector<float>& params, std::vector<int>& selected_win_types) {
		std::vector<float> decoded_params;

		if (grammar_id == 0) {
			FacadeA::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 1) {
			FacadeB::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 2) {
			FacadeC::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 3) {
			FacadeD::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 4) {
			FacadeE::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 5) {
			FacadeF::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 6) {
			FacadeG::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}
		else if (grammar_id == 7) {
			FacadeH::decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
		}

		return decoded_params;
	}

}
