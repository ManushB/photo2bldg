#include "facadeB.h"
#include "Utils.h"

cv::Mat generateFacadeB(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, int max_NF, int max_NC, const std::vector<float>& params) {
	// #floors has to be at least 2 for this facade.
	if (range_NF.first < 2) range_NF.first = 2;
	if (max_NF < 2) max_NF = 2;

	// #columns has to be at least 1 for this facade.
	if (range_NC.first < 1) range_NC.first = 1;
	if (max_NC < 1) max_NC = 1;

	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF < range_NF.first) NF = range_NF.first;
	if (NF > max_NF && max_NF <= 5) NF = max_NF;
	int NC = std::round(params[1] * (range_NC.second - range_NC.first) + range_NC.first);
	if (NC < range_NC.first) NC = range_NC.first;
	if (NC > max_NC && max_NC <= 5) NC = max_NC;

	float BS = (float)width / (params[7] * 2 + params[8] * NC) * params[7];
	float TW = (float)width / (params[7] * 2 + params[8] * NC) * params[8];
	float AH = (float)height / (params[9] + params[10] * (NF - 1) + params[11]) * params[9];
	float FH = (float)height / (params[9] + params[10] * (NF - 1) + params[11]) * params[10];
	float GH = (float)height / (params[9] + params[10] * (NF - 1) + params[11]) * params[11];

	float WW = TW / (params[2] + params[4] * 2) * params[2];
	float WH = FH / (params[3] + params[5] + params[6]) * params[3];
	float WS = TW / (params[2] + params[4] * 2) * params[4];
	float WT = FH / (params[3] + params[5] + params[6]) * params[5];
	float WB = FH / (params[3] + params[5] + params[6]) * params[6];

	// １Fのドアの数 {1, NC/4, NC/3, NC/2, NC}
	int ND_type = std::round(params[12] * 4);
	if (ND_type < 0) ND_type = 0;
	if (ND_type > 4) ND_type = 4;

	int ND;
	if (ND_type == 0) {
		ND = 1;
	}
	else if (ND_type == 1) {
		ND = std::ceil((float)NC / 4);
	}
	else if (ND_type == 2) {
		ND = std::ceil((float)NC / 3);
	}
	else if (ND_type == 3) {
		ND = std::ceil((float)NC / 2);
	}
	else if (ND_type == 4) {
		ND = NC;
	}

	float DT = GH / (params[13] + params[14]) * params[13];
	float DH = GH / (params[13] + params[14]) * params[14];
	float DW = TW * NC / ND / (params[15] + params[16] * 2) * params[15];
	float DS = TW * NC / ND / (params[15] + params[16] * 2) * params[16];

	return generateFacadeB(1, NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, GH, ND, DT, DH, DW, DS);
}

cv::Mat generateRandomFacadeB(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	// #floors has to be at least 2 for this facade.
	if (range_NF.first < 2) range_NF.first = 2;

	// #columns has to be at least 1 for this facade.
	if (range_NC.first < 1) range_NC.first = 1;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	// 屋根の高さ
	float AH = utils::genRand(0, 0.5);

	// 各フロアの高さ
	float FH = utils::genRand(2.5, 4);

	// １Fの高さ
	float GH = FH + utils::genRand(0, 2);

	// 各タイルの幅
	float TW = utils::genRand(2, 4);

	// ビルの横マージン
	float BS = utils::genRand(0, 1);

	// 各フロアの窓上部から天井までの高さ
	float WT = utils::genRand(0.2, 1);

	// 各フロアの窓下部からフロア底面までの高さ
	float WB = utils::genRand(0.2, 1);

	// 各フロアの窓の高さ
	float WH = utils::genRand(1, 2.5);

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;

	// 各フロアの窓の横マージン
	float WS = utils::genRand(0.2, 1);

	// 各フロアの窓の幅
	float WW = utils::genRand(0.5, 2.5);

	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;

	// １Fのドア上部から天井までの高さ
	float DT = utils::genRand(0.2, 1.5);

	// １Fのドアの高さ
	float DH = utils::genRand(2, 3);

	// １Fの各種高さをnormalize
	ratio = GH / (DT + DH);
	DT *= ratio;
	DH *= ratio;

	// １Fのドアの数 {1, NC/4, NC/3, NC/2, N}
	int ND;
	int ND_type = utils::genRand(0, 5);
	if (ND_type == 0) {
		ND = 1;
	}
	else if (ND_type == 1) {
		ND = std::ceil((float)NC / 4);
	}
	else if (ND_type == 2) {
		ND = std::ceil((float)NC / 3);
	}
	else if (ND_type == 3) {
		ND = std::ceil((float)NC / 2);
	}
	else if (ND_type == 4) {
		ND = NC;
	}

	// １Fのドアの横マージン
	float DS = utils::genRand(0.2, 1);

	// １Fのドアの幅
	float DW = utils::genRand(0.5, 4);

	// １Fの各種幅をnormalize
	ratio = TW * NC / ND / (DS * 2 + DW);
	DS *= ratio;
	DW *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * (NF - 1));
	GH *= ratio;
	FH *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	AH *= ratio;
	DH *= ratio;
	DT *= ratio;
	ratio = (float)width / (BS * 2 + TW * NC);
	BS *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;
	DS *= ratio;
	DW *= ratio;

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
	params.push_back(GH / height);
	params.push_back((float)ND_type / 4);
	params.push_back(DT / GH);
	params.push_back(DH / GH);
	params.push_back(DW / (DW + DS * 2));
	params.push_back(DS / (DW + DS * 2));

	return generateFacadeB(NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, GH, ND, DT, DH, DW, DS, window_displacement, window_prob);
}

cv::Mat generateFacadeB(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float GH, int ND, float DT, float DH, float DW, float DS, int window_displacement, float window_prob) {
	cv::Mat result(height * scale, width * scale, CV_8UC3, cv::Scalar(255, 255, 255));

	// １Fのドアを描画
	for (int j = 0; j < ND; ++j) {
		int x1 = (BS + (DS * 2 + DW) * j + DS) * scale;
		int y1 = (height - DH) * scale;
		int x2 = (BS + (DS * 2 + DW) * j + DS + DW) * scale;
		int y2 = height * scale - 1;

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

	// ２F以上の窓を描画
	for (int i = 0; i < NF - 1; ++i) {
		for (int j = 0; j < NC; ++j) {
			int x1 = (BS + TW * j + WS) * scale;
			int y1 = (height - GH - FH * i - WB - WH) * scale;
			int x2 = (BS + (WS * 2 + WW) * j + WS + WW) * scale;
			int y2 = (height - GH - FH * i - WB) * scale;

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

int clusterWindowTypesB(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int i = 0; i < win_rects.size() - 1; ++i) {
		for (int j = 0; j < win_rects[i].size(); ++j) {
			win_rects[i][j].type = 0;
		}
	}
	for (int j = 0; j < win_rects.back().size(); ++j) {
		win_rects.back()[j].type = 1;
	}

	return 2;
}