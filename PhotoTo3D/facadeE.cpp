#include "facadeE.h"
#include "Utils.h"

cv::Mat generateFacadeE(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, int max_NF, int max_NC, const std::vector<float>& params) {
	// #floors has to be at least 4 for this facade.
	if (range_NF.first < 5) range_NF.first = 5;
	if (max_NF < 5) max_NF = 5;

	// #columns has to be at least 1 for this facade.
	if (range_NC.first < 1) range_NC.first = 1;
	if (max_NC < 1) max_NC = 1;

	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF < range_NF.first) NF = range_NF.first;
	if (NF > max_NF && max_NF <= 5) NF = max_NF;
	int NC = std::round(params[1] * (range_NC.second - range_NC.first) + range_NC.first);
	if (NC < range_NC.first) NC = range_NC.first;
	if (NC > max_NC && max_NC <= 5) NC = max_NC;

	float BS = (float)width / (params[12] * 2 + params[13] * NC) * params[12];
	float TW = (float)width / (params[12] * 2 + params[13] * NC) * params[13];
	float AH = (float)height / (params[14] + params[15] * (NF - 3) + params[16] + params[17]) * params[14];
	float FH = (float)height / (params[14] + params[15] * (NF - 3) + params[16] + params[17]) * params[15];
	float FH2 = (float)height / (params[14] + params[15] * (NF - 3) + params[16] + params[17]) * params[16];
	float GH = (float)height / (params[14] + params[15] * (NF - 3) + params[16] + params[17]) * params[17];

	float WW = TW / (params[2] + params[4] * 2) * params[2];
	float WH = FH / (params[3] + params[5] + params[6]) * params[3];
	float WS = TW / (params[2] + params[4] * 2) * params[4];
	float WT = FH / (params[3] + params[5] + params[6]) * params[5];
	float WB = FH / (params[3] + params[5] + params[6]) * params[6];

	float WW2 = TW / (params[7] + params[9] * 2) * params[7];
	float WH2 = FH2 / (params[8] + params[10] + params[11]) * params[8];
	float WS2 = TW / (params[7] + params[9] * 2) * params[9];
	float WT2 = FH2 / (params[8] + params[10] + params[11]) * params[10];
	float WB2 = FH2 / (params[8] + params[10] + params[11]) * params[11];

	// １Fのドアの数 {1, NC/4, NC/3, NC/2, NC}
	int ND_type = std::round(params[18] * 4);
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

	float DT = GH / (params[19] + params[20]) * params[19];
	float DH = GH / (params[19] + params[20]) * params[20];
	float DW = TW * NC / ND / (params[21] + params[22] * 2) * params[21];
	float DS = TW * NC / ND / (params[21] + params[22] * 2) * params[22];

	float TWS = TW / (params[24] + params[23] * 2) * params[23];
	float TWW = TW / (params[24] + params[23] * 2) * params[24];
	float TWT = AH / (params[25] + params[26] + params[27]) * params[25];
	float TWH = AH / (params[25] + params[26] + params[27]) * params[26];
	float TWB = AH / (params[25] + params[26] + params[27]) * params[27];

	return generateFacadeE(1, NF, NC, width, height, thickness, WW, WH, WS, WT, WB, WW2, WH2, WS2, WT2, WB2, BS, TW, AH, FH, FH2, GH, ND, DT, DH, DW, DS, TWS, TWW, TWT, TWH, TWB);
}

cv::Mat generateRandomFacadeE(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	// #floors has to be at least 4 for this facade.
	if (range_NF.first < 5) range_NF.first = 5;

	// #columns has to be at least 1 for this facade.
	if (range_NC.first < 1) range_NC.first = 1;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	// 各フロアの高さ
	float FH = utils::genRand(2.5, 4);

	// 最上階の高さ
	float AH = FH + utils::genRand(0.2, 2);

	// １Fの高さ
	float GH = FH + utils::genRand(0, 2);

	// ２Fの高さ
	float FH2;
	if (utils::genRand() < 0.333) {
		FH2 = FH + utils::genRand(-1, 0.2);
	}
	else {
		FH2 = FH + utils::genRand(0.2, 2);
	}

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

	// 2Fの窓上部から天井までの高さ
	float WT2 = utils::genRand(0.2, 1);

	// 2Fの窓下部からフロア底面までの高さ
	float WB2 = utils::genRand(0.2, 1);

	// 2Fの窓の高さ
	float WH2 = utils::genRand(1, 2.5);

	// 2Fの各種高さをnormalize
	ratio = FH2 / (WT2 + WB2 + WH2);
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;

	// 2Fの窓の横マージン
	float WS2 = utils::genRand(0.2, 1);

	// 2Fの窓の幅
	float WW2 = utils::genRand(0.5, 2.5);

	// 2Fの各種幅をnormalize
	ratio = TW / (WS2 * 2 + WW2);
	WS2 *= ratio;
	WW2 *= ratio;

	// 最上階のドア上部から天井までの高さ
	float TWT = utils::genRand(0.2, 2);

	// 最上階の窓下部からフロア底面までの高さ
	float TWB = utils::genRand(0.2, 1);

	// 最上階のドアの高さ
	float TWH = utils::genRand(1, 2.5);

	// 最上階の各種高さをnormalize
	ratio = AH / (TWT + TWB + TWH);
	TWT *= ratio;
	TWB *= ratio;
	TWH *= ratio;

	// 最上階の窓の横マージン
	float TWS = utils::genRand(0.2, 1);

	// 最上階の窓の幅
	float TWW = utils::genRand(0.5, 2.5);

	// 最上階の各種幅をnormalize
	ratio = TW / (TWS * 2 + TWW);
	TWS *= ratio;
	TWW *= ratio;
	
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
	ratio = (float)height / (GH + AH + FH * (NF - 3) + FH2);
	GH *= ratio;
	FH *= ratio;
	FH2 *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;
	AH *= ratio;
	TWT *= ratio;
	TWH *= ratio;
	TWB *= ratio;
	DH *= ratio;
	DT *= ratio;
	ratio = (float)width / (BS * 2 + TW * NC);
	BS *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;
	WS2 *= ratio;
	WW2 *= ratio;
	TWS *= ratio;
	TWW *= ratio;
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
	params.push_back(WW2 / TW);
	params.push_back(WH2 / FH2);
	params.push_back(WS2 / TW);
	params.push_back(WT2 / FH2);
	params.push_back(WB2 / FH2);
	params.push_back(BS / width);
	params.push_back(TW / width);
	params.push_back(AH / height);
	params.push_back(FH / height);
	params.push_back(FH2 / height);
	params.push_back(GH / height);
	params.push_back((float)ND_type / 4);
	params.push_back(DT / GH);
	params.push_back(DH / GH);
	params.push_back(DW / (DW + DS * 2));
	params.push_back(DS / (DW + DS * 2));
	params.push_back(TWS / TW);
	params.push_back(TWW / TW);
	params.push_back(TWT / AH);
	params.push_back(TWH / AH);
	params.push_back(TWB / AH);

	return generateFacadeE(1, NF, NC, width, height, thickness, WW, WH, WS, WT, WB, WW2, WH2, WS2, WT2, WB2, BS, TW, AH, FH, FH2, GH, ND, DT, DH, DW, DS, TWS, TWW, TWT, TWH, TWB, window_displacement, window_prob);
}

cv::Mat generateFacadeE(float scale, int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float WW2, float WH2, float WS2, float WT2, float WB2, float BS, float TW, float AH, float FH, float FH2, float GH, int ND, float DT, float DH, float DW, float DS, float TWS, float TWW, float TWT, float TWH, float TWB, int window_displacement, float window_prob) {
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

	// 2Fの窓を描画
	for (int j = 0; j < NC; ++j) {
		int x1 = (BS + TW * j + WS2) * scale;
		int y1 = (height - GH - WB2 - WH2) * scale;
		int x2 = (BS + TW * j + WS2 + WW2) * scale;
		int y2 = (height - GH - WB2) * scale;

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

	// 3F以上の窓を描画
	for (int i = 0; i < NF - 3; ++i) {
		for (int j = 0; j < NC; ++j) {
			int x1 = (BS + TW * j + WS) * scale;
			int y1 = (height - GH - FH2 - FH * i - WB - WH) * scale;
			int x2 = (BS + (WS * 2 + WW) * j + WS + WW) * scale;
			int y2 = (height - GH - FH2 - FH * i - WB) * scale;

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

	// 最上階の窓を描画
	for (int j = 0; j < NC; ++j) {
		int x1 = (BS + TW * j + TWS) * scale;
		int y1 = (height - GH - FH2 - FH * (NF - 3) - TWB - TWH) * scale;
		int x2 = (BS + TW * j + TWS + TWW) * scale;
		int y2 = (height - GH - FH2 - FH * (NF - 3) - TWB) * scale;

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

	return result;
}

int clusterWindowTypesE(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int j = 0; j < win_rects[0].size(); ++j) {
		win_rects[0][j].type = 0;
	}
	for (int i = 1; i < win_rects.size() - 2; ++i) {
		for (int j = 0; j < win_rects[i].size(); ++j) {
			win_rects[i][j].type = 1;
		}
	}
	for (int j = 0; j < win_rects[win_rects.size() - 2].size(); ++j) {
		win_rects[win_rects.size() - 2][j].type = 2;
	}
	for (int j = 0; j < win_rects.back().size(); ++j) {
		win_rects.back()[j].type = 3;
	}

	return 4;
}
