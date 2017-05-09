#include "facadeA.h"
#include "Utils.h"

std::pair<int, int> FacadeA::range_NF = std::make_pair(1, 20);
std::pair<int, int> FacadeA::range_NC = std::make_pair(1, 20);

cv::Mat FacadeA::generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color) {
	std::vector<float> decoded_params;
	decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);
	
	return generateFacade(1, width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9]);
}

void FacadeA::decodeParams(float width, float height, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, std::vector<float>& decoded_params) {
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

	float GH = (float)height / (params[2] + params[3] * NF + params[4]) * params[2];
	float FH = (float)height / (params[2] + params[3] * NF + params[4]) * params[3];
	float AH = (float)height / (params[2] + params[3] * NF + params[4]) * params[4];
	float SW = (float)width / (params[5] * 2 + params[6] * NC) * params[5];
	float TW = (float)width / (params[5] * 2 + params[6] * NC) * params[6];

	float WT = FH / (params[7] + params[8] + params[9]) * params[7];
	float WH = FH / (params[7] + params[8] + params[9]) * params[8];
	float WB = FH / (params[7] + params[8] + params[9]) * params[9];
	float WS = TW / (params[10] * 2 + params[11]) * params[10];
	float WW = TW / (params[10] * 2 + params[11]) * params[11];

	decoded_params.resize(10);
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
}

cv::Mat FacadeA::generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement, float window_prob) {
	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	float GH = utils::genRand(0, 0.28);
	float FH = 1.0;
	float AH = utils::genRand(0, 0.018);	
	float SW = utils::genRand(0, 0.31);
	float TW = 1.0;
	float WT = utils::genRand(0.005, 0.41);
	float WH = 1.0;
	float WB = utils::genRand(0.005, 0.4);
	float WS = utils::genRand(0.01, 0.4);
	float WW = 1.0;


	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	
	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * NF);
	GH *= ratio;
	FH *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	AH *= ratio;
	ratio = (float)width / (SW * 2 + TW * NC);
	SW *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;

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

	return generateFacade(1, width, height, thickness, GH, FH, AH, SW, TW, WT, WH, WB, WS, WW, window_displacement, window_prob);
}

cv::Mat FacadeA::generateFacade(float scale, int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float AH, float SW, float TW, float WT, float WH, float WB, float WS, float WW, float window_displacement, float window_prob) {
	cv::Mat result(height * scale, width * scale, CV_8UC3, bg_color);

	int NF = std::round((float)(height - AH - GH) / FH);
	int NC = std::round((float)(width - SW * 2) / TW);

	window_prob = 1 - utils::genRand(0, 1 - window_prob);

	// 窓を描画
	for (int i = 0; i < NF; ++i) {
		for (int j = 0; j < NC; ++j) {
			float x1 = (SW + TW * j + WS) * scale;
			float y1 = (height - GH - FH * i - WB - WH) * scale;
			float x2 = (SW + (WS * 2 + WW) * j + WS + WW) * scale;
			float y2 = (height - GH - FH * i - WB) * scale;

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
	}

	return result;
}

int FacadeA::clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int i = 0; i < win_rects.size(); ++i) {
		for (int j = 0; j < win_rects[i].size(); ++j) {
			win_rects[i][j].type = 0;
		}
	}

	return 1;
}