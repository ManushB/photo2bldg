#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "FacadeSegmentation.h"

class FacadeF {
private:
	static std::pair<int, int> range_NF;
	static std::pair<int, int> range_NC;

protected:
	FacadeF() {}

public:
	static void attachDoors(std::vector<float>& params, const std::vector<int>& selected_win_types);
	static cv::Mat generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, const std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color);
	static void decodeParams(float width, float height, int num_floors, int num_columns, std::vector<float> params, const std::vector<int>& selected_win_types, int mass_grammar_id, std::vector<float>& decoded_params);
	static cv::Mat generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement = 0, float window_prob = 1);
	static cv::Mat generateFacade(int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float AH, float SW, float TW, float WT, float WH, float WB, float WS, float WW, float WT2, float WH2, float WB2, float WO2, float WW2, float WI2, float DT, float DH, float DB, float DS, float DW, float DT2, float DH2, float DB2, float DO2, float DW2, float DI2, float window_displacement = 0, float window_prob = 1);

	static int clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects);
};
