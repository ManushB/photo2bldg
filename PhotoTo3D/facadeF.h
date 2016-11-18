#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat generateFacadeF(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, const std::vector<float>& params);
cv::Mat generateRandomFacadeF(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement = 0, float window_prob = 1);
cv::Mat generateFacadeF(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float BH, float WW2, float WH2, float WO2, float WI2, float WT2, float WB2, int window_displacement = 0, float window_prob = 1);
