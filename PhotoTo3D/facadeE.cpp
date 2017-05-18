#include "facadeE.h"
#include "Utils.h"

std::pair<int, int> FacadeE::range_NF = std::make_pair(1, 20);
std::pair<int, int> FacadeE::range_NC = std::make_pair(5, 20);

void FacadeE::attachDoors(std::vector<float>& params, const std::vector<int>& selected_win_types) {
	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF > 1 || selected_win_types[1] < 25) {
		// do nothing
	}
	else {
		// remove the gap between the door and the ground
		params[9] = 0;
	}

	if (NF > 1 || selected_win_types[0] < 25) {
		// do nothing
	}
	else {
		// remove the gap between the door and the ground
		params[14] = 0;
	}
}

cv::Mat FacadeE::generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, const std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color) {
	std::vector<float> decoded_params;
	decodeParams(width, height, num_floors, num_columns, params, selected_win_types, -1, decoded_params);

	return generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15]);
}

void FacadeE::decodeParams(float width, float height, int num_floors, int num_columns, std::vector<float> params, const std::vector<int>& selected_win_types, int mass_grammar_id, std::vector<float>& decoded_params) {
	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF < range_NF.first) NF = range_NF.first;
	int NC = std::round(params[1] * (range_NC.second - range_NC.first) + range_NC.first);
	if (NC < range_NC.first) NC = range_NC.first;

	////////////////////////////////////////////////////////////////////////////////////////////
	// use the known #floors/#columns if they are provided
	if (num_floors > 0 && num_columns > 0) {
		NF = num_floors;
		NC = num_columns;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// HACK
	// enforce the minimum value for some parameters
	params[3] = std::max(0.02f, params[3]);	// FH
	params[6] = std::max(0.02f, params[6]);	// TW

	float GH = (float)height / (params[2] + params[3] * NF + params[4]) * params[2];
	float FH = (float)height / (params[2] + params[3] * NF + params[4]) * params[3];
	float AH = (float)height / (params[2] + params[3] * NF + params[4]) * params[4];
	float SW = (float)width / (params[5] * 2 + params[6] * (NC - 2)) * params[5];
	float TW = (float)width / (params[5] * 2 + params[6] * (NC - 2)) * params[6];

	float WT = FH / (params[7] + params[8] + params[9]) * params[7];
	float WH = FH / (params[7] + params[8] + params[9]) * params[8];
	float WB = FH / (params[7] + params[8] + params[9]) * params[9];
	float WS = TW / (params[10] * 2 + params[11]) * params[10];
	float WW = TW / (params[10] * 2 + params[11]) * params[11];

	float WT2 = FH / (params[12] + params[13] + params[14]) * params[12];
	float WH2 = FH / (params[12] + params[13] + params[14]) * params[13];
	float WB2 = FH / (params[12] + params[13] + params[14]) * params[14];
	float WO2 = SW / (params[15] + params[16] + params[17]) * params[15];
	float WW2 = SW / (params[15] + params[16] + params[17]) * params[16];
	float WI2 = SW / (params[15] + params[16] + params[17]) * params[17];

	decoded_params.resize(16);
	decoded_params[0] = GH;
	decoded_params[1] = FH;
	decoded_params[2] = AH;
	decoded_params[3] = SW;
	decoded_params[4] = TW;
	decoded_params[5] = WT;
	decoded_params[6] = WH;
	decoded_params[7] = WB;
	decoded_params[8] = WS;
	decoded_params[9] = WW;
	decoded_params[10] = WT2;
	decoded_params[11] = WH2;
	decoded_params[12] = WB2;
	decoded_params[13] = WO2;
	decoded_params[14] = WW2;
	decoded_params[15] = WI2;
}

cv::Mat FacadeE::generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement, float window_prob) {
	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	float GH = utils::genRand(0, 0.28);
	float FH = 1;
	float AH = utils::genRand(0, 0.018);
	float SW;
	if (utils::genRand() < 0.2) {
		SW = utils::genRand(0.8, 0.9);
	}
	else {
		SW = utils::genRand(1.2, 1.9);
	}
	float TW = 1;
	float WT = utils::genRand(0.005, 0.7);
	float WH = 1;
	float WB = utils::genRand(0.005, 0.5);
	float WS = utils::genRand(0.01, 0.5);
	float WW = 1;
	float WT2 = utils::genRand(0.1, 0.7);
	float WH2 = 1;
	float WB2 = utils::genRand(0.1, 0.5);
	float WO2 = utils::genRand(0.3, 2.2);
	float WW2 = 1;
	float WI2 = utils::genRand(0.3, 2.2);
	

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	
	// 左・右端の各種高さをnormalize
	ratio = FH / (WT2 + WB2 + WH2);
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;
	
	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;
	
	// 左・右端の各種幅をnormalize
	ratio = SW / (WO2 + WW2 + WI2);
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * NF);
	GH *= ratio;
	FH *= ratio;
	AH *= ratio;
	WT *= ratio;
	WH *= ratio;
	WB *= ratio;
	WT2 *= ratio;
	WH2 *= ratio;
	WB2 *= ratio;
	ratio = (float)width / (SW * 2 + TW * (NC - 2));
	SW *= ratio;
	TW *= ratio;
	WS *= ratio;
	WW *= ratio;
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータ値を格納
	params.push_back((float)(NF - range_NF.first) / (range_NF.second - range_NF.first));
	params.push_back((float)(NC - range_NC.first) / (range_NC.second - range_NC.first));
	params.push_back(GH / height);
	params.push_back(FH / height);
	params.push_back(AH / height);
	params.push_back(SW / width);
	params.push_back(TW / width);
	params.push_back(WT / FH);
	params.push_back(WH / FH);
	params.push_back(WB / FH);
	params.push_back(WS / TW);
	params.push_back(WW / TW);
	params.push_back(WT2 / FH);
	params.push_back(WH2 / FH);
	params.push_back(WB2 / FH);
	params.push_back(WO2 / SW);
	params.push_back(WW2 / SW);
	params.push_back(WI2 / SW);

	return generateFacade(width, height, thickness, GH, FH, AH, SW, TW, WT, WH, WB, WS, WW, WT2, WH2, WB2, WO2, WW2, WI2, window_displacement, window_prob);
}

cv::Mat FacadeE::generateFacade(int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float AH, float SW, float TW, float WT, float WH, float WB, float WS, float WW, float WT2, float WH2, float WB2, float WO2, float WW2, float WI2, float window_displacement, float window_prob) {
	cv::Mat result(height, width, CV_8UC3, bg_color);
	
	int NF = std::round((float)(height - AH - GH) / FH);
	int NC = std::round((float)(width - SW * 2) / TW) + 2;

	window_prob = 1 - utils::genRand(0, 1 - window_prob);

	// 窓を描画
	for (int i = 0; i < NF; ++i) {
		// 左端
		{
			float x1 = WO2;
			float y1 = height - GH - FH * i - WB2 - WH2;
			float x2 = WO2 + WW2;
			float y2 = height - GH - FH * i - WB2;

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-FH * window_displacement, FH * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-FH * window_displacement, FH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			float x1 = SW + TW * j + WS;
			float y1 = height - GH - FH * i - WB - WH;
			float x2 = SW + TW * j + WS + WW;
			float y2 = height - GH - FH * i - WB;

			if (window_displacement > 0) {
				x1 += utils::genRand(-TW * window_displacement, TW * window_displacement);
				y1 += utils::genRand(-FH * window_displacement, FH * window_displacement);
				x2 += utils::genRand(-TW * window_displacement, TW * window_displacement);
				y2 += utils::genRand(-FH * window_displacement, FH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}

		// 右端
		{
			float x1 = SW + TW * (NC - 2) + WI2;
			float y1 = height - GH - FH * i - WB2 - WH2;
			float x2 = SW + TW * (NC - 2) + WI2 + WW2;
			float y2 = height - GH - FH * i - WB2;

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-FH * window_displacement, FH * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-FH * window_displacement, FH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}
	}

	return result;
}

int FacadeE::clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects) {
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
