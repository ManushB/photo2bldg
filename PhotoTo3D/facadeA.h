#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "FacadeSegmentation.h"

cv::Mat generateFacadeA(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, int max_NF, int max_NC, const std::vector<float>& params);
void decodeParamsA(float width, float height, std::pair<int, int> range_NF, std::pair<int, int> range_NC, int max_NF, int max_NC, const std::vector<float>& params, std::vector<float>& decoded_params);
cv::Mat generateRandomFacadeA(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement = 0, float window_prob = 1);
cv::Mat generateFacadeA(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float BH, int window_displacement = 0, float window_prob = 1);

int clusterWindowTypesA(std::vector<std::vector<fs::WindowPos>>& win_rects);
