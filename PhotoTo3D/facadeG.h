#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "FacadeSegmentation.h"

class FacadeG {
private:
	static std::pair<int, int> range_NF;
	static std::pair<int, int> range_NC;

protected:
	FacadeG() {}

public:
	static cv::Mat generateFacade(int width, int height, int thickness, int max_NF, int max_NC, const std::vector<float>& params);
	static void decodeParams(float width, float height, int max_NF, int max_NC, const std::vector<float>& params, std::vector<float>& decoded_params);
	static cv::Mat generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement = 0, float window_prob = 1);
	static cv::Mat generateFacade(float scale, int width, int height, int thickness, float GH, float FH, float FH2, float AH, float SW, float TW, float WT, float WH, float WB, float WS, float WW, float WT2, float WH2, float WB2, float WO2, float WW2, float WI2, float DT, float DH, float DB, float DT2, float DH2, float DB2, float DO2, float DW2, float DI2, float TWT, float TWH, float TWB, float window_displacement = 0, float window_prob = 1);

	static int clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects);
};