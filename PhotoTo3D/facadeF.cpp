﻿#include "facadeF.h"
#include "Utils.h"

cv::Mat generateFacadeF(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, int max_NF, int max_NC, const std::vector<float>& params) {
	// #floors has to be at least 1 for this facade.
	if (range_NF.first < 1) range_NF.first = 1;

	// #columns has to be at least 3 for this facade.
	if (range_NC.first < 3) range_NC.first = 3;

	std::vector<float> decoded_params;
	decodeParamsF(width, height, range_NF, range_NC, max_NF, max_NC, params, decoded_params);

	return generateFacadeF(1, decoded_params[0], decoded_params[1], width, height, thickness, decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17]);
}

void decodeParamsF(float width, float height, std::pair<int, int> range_NF, std::pair<int, int> range_NC, int max_NF, int max_NC, const std::vector<float>& params, std::vector<float>& decoded_params) {
	if (max_NF < 1) max_NF = 1;
	if (max_NC < 3) max_NC = 3;

	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF < range_NF.first) NF = range_NF.first;
	if (NF > max_NF && max_NF <= 5) NF = max_NF;
	int NC = std::round(params[1] * (range_NC.second - range_NC.first) + range_NC.first);
	if (NC < range_NC.first) NC = range_NC.first;
	if (NC > max_NC && max_NC <= 5) NC = max_NC;

	float BS = (float)width / (params[7] * 2 + params[8] * (NC - 2)) * params[7];
	float TW = (float)width / (params[7] * 2 + params[8] * (NC - 2)) * params[8];
	float AH = (float)height / (params[9] + params[10] * NF + params[11]) * params[9];
	float FH = (float)height / (params[9] + params[10] * NF + params[11]) * params[10];
	float BH = (float)height / (params[9] + params[10] * NF + params[11]) * params[11];

	float WW = TW / (params[2] + params[4] * 2) * params[2];
	float WH = FH / (params[3] + params[5] + params[6]) * params[3];
	float WS = TW / (params[2] + params[4] * 2) * params[4];
	float WT = FH / (params[3] + params[5] + params[6]) * params[5];
	float WB = FH / (params[3] + params[5] + params[6]) * params[6];

	float WW2 = BS / (params[12] + params[14] + params[15]) * params[12];
	float WO2 = BS / (params[12] + params[14] + params[15]) * params[14];
	float WI2 = BS / (params[12] + params[14] + params[15]) * params[15];
	float WH2 = FH / (params[13] + params[16] + params[17]) * params[13];
	float WT2 = FH / (params[13] + params[16] + params[17]) * params[16];
	float WB2 = FH / (params[13] + params[16] + params[17]) * params[17];

	decoded_params.resize(18);
	decoded_params[0] = NF;
	decoded_params[1] = NC;
	decoded_params[2] = WW;
	decoded_params[3] = WH;
	decoded_params[4] = WS;
	decoded_params[5] = WT;
	decoded_params[6] = WB;
	decoded_params[7] = BS;
	decoded_params[8] = TW;
	decoded_params[9] = AH;
	decoded_params[10] = FH;
	decoded_params[11] = BH;
	decoded_params[12] = WW2;
	decoded_params[13] = WH2;
	decoded_params[14] = WO2;
	decoded_params[15] = WI2;
	decoded_params[16] = WT2;
	decoded_params[18] = WB2;
}

cv::Mat generateRandomFacadeF(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	// #floors has to be at least 1 for this facade.
	if (range_NF.first < 1) range_NF.first = 1;

	// #columns has to be at least 3 for this facade.
	if (range_NC.first < 3) range_NC.first = 3;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	// ベースの高さ
	float BH = utils::genRand(0, 0.5);

	// 屋根の高さ
	float AH = utils::genRand(0, 0.5);

	// 各フロアの高さ
	float FH = utils::genRand(2.5, 4);

	// 各タイルの幅
	float TW = utils::genRand(2, 4);

	// ビルの横マージン
	float BS;
	if (utils::genRand() < 0.2) {
		BS = utils::genRand(TW * 0.5, TW * 0.7);
	}
	else {
		BS = utils::genRand(TW * 1.3, TW * 3);
	}

	// 各フロアの窓上部から天井までの高さ
	float WT = utils::genRand(0.1, 0.35);

	// 各フロアの窓下部からフロア底面までの高さ
	float WB = utils::genRand(0.1, 0.35);

	// 各フロアの窓の高さ
	float WH = utils::genRand(0.3, 0.8);

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;

	// 左・右端の窓上部から天井までの高さ
	float WT2 = utils::genRand(0.1, 0.35);

	// 左・右端の窓下部からフロア底面までの高さ
	float WB2 = utils::genRand(0.1, 0.35);

	// 左・右端の窓の高さ
	float WH2 = utils::genRand(0.3, 0.8);

	// 左・右端の各種高さをnormalize
	ratio = FH / (WT2 + WB2 + WH2);
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;

	// 各フロアの窓の横マージン
	float WS = utils::genRand(0.05, 0.4);

	// 各フロアの窓の幅
	float WW = utils::genRand(0.2, 0.9);

	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;

	// 左・右端の窓の横のマージン
	float WO2 = utils::genRand(0.2, 0.6);
	float WI2 = utils::genRand(0.2, 0.6);

	// 左・右端の窓の幅
	float WW2 = utils::genRand(0.1, 0.3);

	// 左・右端の各種幅をnormalize
	ratio = BS / (WO2 + WW2 + WI2);
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (BH + AH + FH * NF);
	BH *= ratio;
	FH *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	WT2 *= ratio;
	WH2 *= ratio;
	WB2 *= ratio;
	AH *= ratio;
	ratio = (float)width / (BS * 2 + TW * (NC - 2));
	BS *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータ値を格納
	params.push_back((float)(NF - range_NF.first) / (float)(range_NF.second - range_NF.first));
	params.push_back((float)(NC - range_NC.first) / (float)(range_NC.second - range_NC.first));
	params.push_back(WW / TW);
	params.push_back(WH / FH);
	params.push_back(WS / TW);
	params.push_back(WT / FH);
	params.push_back(WB / FH);
	params.push_back(BS / width);
	params.push_back(TW / width);
	params.push_back(AH / height);
	params.push_back(FH / height);
	params.push_back(BH / height);
	params.push_back(WW2 / BS);
	params.push_back(WH2 / FH);
	params.push_back(WO2 / BS);
	params.push_back(WI2 / BS);
	params.push_back(WT2 / FH);
	params.push_back(WB2 / FH);

	return generateFacadeF(1, NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, BH, WW2, WH2, WO2, WI2, WT2, WB2, window_displacement, window_prob);
}

cv::Mat generateFacadeF(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float BH, float WW2, float WH2, float WO2, float WI2, float WT2, float WB2, int window_displacement, float window_prob) {
	cv::Mat result(height * scale, width * scale, CV_8UC3, cv::Scalar(255, 255, 255));

	// 窓を描画
	for (int i = 0; i < NF; ++i) {
		// 左端
		{
			int x1 = WO2 * scale;
			int y1 = (height - BH - FH * i - WB2 - WH2) * scale;
			int x2 = (WO2 + WW2) * scale;
			int y2 = (height - BH - FH * i - WB2) * scale;

			if (window_displacement > 0) {
				x1 += utils::genRand(-window_displacement, window_displacement + 1);
				y1 += utils::genRand(-window_displacement, window_displacement + 1);
				x2 += utils::genRand(-window_displacement, window_displacement + 1);
				y2 += utils::genRand(-window_displacement, window_displacement + 1);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			int x1 = (BS + TW * j + WS) * scale;
			int y1 = (height - BH - FH * i - WB - WH) * scale;
			int x2 = (BS + TW * j + WS + WW) * scale;
			int y2 = (height - BH - FH * i - WB) * scale;

			if (window_displacement > 0) {
				x1 += utils::genRand(-window_displacement, window_displacement + 1);
				y1 += utils::genRand(-window_displacement, window_displacement + 1);
				x2 += utils::genRand(-window_displacement, window_displacement + 1);
				y2 += utils::genRand(-window_displacement, window_displacement + 1);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		// 右端
		{
			int x1 = (BS + TW * (NC - 2) + WI2) * scale;
			int y1 = (height - BH - FH * i - WB2 - WH2) * scale;
			int x2 = (BS + TW * (NC - 2) + WI2 + WW2) * scale;
			int y2 = (height - BH - FH * i - WB2) * scale;

			if (window_displacement > 0) {
				x1 += utils::genRand(-window_displacement, window_displacement + 1);
				y1 += utils::genRand(-window_displacement, window_displacement + 1);
				x2 += utils::genRand(-window_displacement, window_displacement + 1);
				y2 += utils::genRand(-window_displacement, window_displacement + 1);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}
	}

	return result;
}

int clusterWindowTypesF(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int i = 0; i < win_rects.size(); ++i) {
		for (int j = 0; j < win_rects[i].size(); j += win_rects[i].size() - 1) {
			win_rects[i][j].type = 0;
		}
		for (int j = 1; j < win_rects[i].size() - 1; ++j) {
			win_rects[i][j].type = 1;
		}
	}

	return 2;
}
