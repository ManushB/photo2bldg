#pragma once

#include "Classifier.h"
#include "Regression.h"
#include <boost/shared_ptr.hpp>
#include <opencv2/core.hpp>
#include <dlib/optimization.h>
#include <glm/glm.hpp>

namespace facarec {

	typedef dlib::matrix<double, 0, 1> column_vector;

	class obj_function {
	public:
		obj_function(int grammar_id, cv::Mat initial_facade_parsing, int num_floors, int num_columns, const std::vector<int>& selected_win_types);
		double operator() (const column_vector& arg) const;

	private:
		int grammar_id;
		cv::Mat initial_facade_parsing;
		int num_floors;
		int num_columns;
		std::vector<int> selected_win_types;
	};

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image, int mass_grammar_id);
	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, float width, float height, int num_floors, int num_columns, const cv::Mat& initial_facade_parsing, std::vector<int>& selected_win_types);
	std::vector<float> parameterEstimation1(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation2(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation3(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation4(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation5(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation6(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation7(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);
	std::vector<float> parameterEstimation8(float width, float height, int num_floors, int num_columns, std::vector<float> params, std::vector<int> selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& result_img);

	void generateFacadeImage(int grammar_id, int width, int height, int num_floors, int num_columns, std::vector<float>& params, std::vector<int>& selected_win_types, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, cv::Mat& image);
	std::vector<float> decodeParameters(int grammar_id, float width, float height, int num_floors, int num_columns, std::vector<float>& params, std::vector<int>& selected_win_types);
}
