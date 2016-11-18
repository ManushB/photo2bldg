#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat generateFacadeE(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, const std::vector<float>& params);
cv::Mat generateRandomFacadeE(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement = 0, float window_prob = 1);
cv::Mat generateFacadeE(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float WW2, float WH2, float WS2, float WT2, float WB2, float BS, float TW, float AH, float FH, float FH2, float GH, int ND, float DT, float DH, float DW, float DS, float TWS, float TWW, float TWT, float TWH, float TWB, int window_displacement = 0, float window_prob = 1);
