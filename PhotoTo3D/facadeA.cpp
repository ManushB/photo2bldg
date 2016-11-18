#include "facadeA.h"
#include "Utils.h"

cv::Mat generateFacadeA(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, const std::vector<float>& params) {
	// #floors has to be at least 1 for this facade.
	if (range_NF.first < 1) range_NF.first = 1;

	// #columns has to be at least 1 for this facade.
	if (range_NC.first < 1) range_NC.first = 1;

	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF < range_NF.first) NF = range_NF.first;
	int NC = std::round(params[1] * (range_NC.second - range_NC.first) + range_NC.first);
	if (NC < range_NC.first) NC = range_NC.first;

	float BS = (float)width / (params[7] * 2 + params[8] * NC) * params[7];
	float TW = (float)width / (params[7] * 2 + params[8] * NC) * params[8];
	float AH = (float)height / (params[9] + params[10] * NF + params[11]) * params[9];
	float FH = (float)height / (params[9] + params[10] * NF + params[11]) * params[10];
	float BH = (float)height / (params[9] + params[10] * NF + params[11]) * params[11];

	float WW = TW / (params[2] + params[4] * 2) * params[2];
	float WH = FH / (params[3] + params[5] + params[6]) * params[3];
	float WS = TW / (params[2] + params[4] * 2) * params[4];
	float WT = FH / (params[3] + params[5] + params[6]) * params[5];
	float WB = FH / (params[3] + params[5] + params[6]) * params[6];
	
	return generateFacadeA(1, NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, BH);
}

cv::Mat generateRandomFacadeA(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	// #floors has to be at least 1 for this facade.
	if (range_NF.first < 1) range_NF.first = 1;

	// #columns has to be at least 1 for this facade.
	if (range_NC.first < 1) range_NC.first = 1;

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

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (BH + AH + FH * NF);
	BH *= ratio;
	FH *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	AH *= ratio;
	ratio = (float)width / (BS * 2 + TW * NC);
	BS *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;

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

	return generateFacadeA(NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, BH, window_displacement, window_prob);
}

cv::Mat generateFacadeA(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float BH, int window_displacement, float window_prob) {
	cv::Mat result(height * scale, width * scale, CV_8UC3, cv::Scalar(255, 255, 255));

	// 窓を描画
	for (int i = 0; i < NF; ++i) {
		for (int j = 0; j < NC; ++j) {
			int x1 = (BS + TW * j + WS) * scale;
			int y1 = (height - BH - FH * i - WB - WH) * scale;
			int x2 = (BS + (WS * 2 + WW) * j + WS + WW) * scale;
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
	}

	return result;
}