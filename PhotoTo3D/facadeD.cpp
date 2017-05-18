#include "facadeD.h"
#include "Utils.h"

std::pair<int, int> FacadeD::range_NF = std::make_pair(4, 20);
std::pair<int, int> FacadeD::range_NC = std::make_pair(3, 20);

void FacadeD::attachDoors(std::vector<float>& params, const std::vector<int>& selected_win_types) {
	if (selected_win_types[3] < 25) {
		// do nothing
	}
	else {
		// remove the gap between the door and the ground
		params[21] = 0;
	}
}

cv::Mat FacadeD::generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, const std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color) {
	std::vector<float> decoded_params;
	decodeParams(width, height, num_floors, num_columns, params, selected_win_types, -1, decoded_params);

	return generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26]);
}

void FacadeD::decodeParams(float width, float height, int num_floors, int num_columns, std::vector<float> params, const std::vector<int>& selected_win_types, int mass_grammar_id, std::vector<float>& decoded_params) {
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
	params[7] = std::max(0.02f, params[7]);	// GW
	params[8] = std::max(0.02f, params[8]);	// TW

	float GH = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[2];
	float FH = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[3];
	float FH2 = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[4];
	float AH = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[5];
	float SW = (float)width / (params[6] * 2 + params[8] * NC) * params[6];
	float TW = (float)width / (params[6] * 2 + params[8] * NC) * params[8];
	int ND = std::max(1.0f, std::round((float)(width - SW * 2) / width / params[7]));
	float GW = (float)(width - SW * 2) / ND;

	float WT = FH / (params[9] + params[10] + params[11]) * params[9];
	float WH = FH / (params[9] + params[10] + params[11]) * params[10];
	float WB = FH / (params[9] + params[10] + params[11]) * params[11];
	float WS = TW / (params[12] * 2 + params[13]) * params[12];
	float WW = TW / (params[12] * 2 + params[13]) * params[13];

	float WT2 = FH2 / (params[14] + params[15] + params[16]) * params[14];
	float WH2 = FH2 / (params[14] + params[15] + params[16]) * params[15];
	float WB2 = FH2 / (params[14] + params[15] + params[16]) * params[16];
	float WS2 = TW / (params[17] * 2 + params[18]) * params[17];
	float WW2 = TW / (params[17] * 2 + params[18]) * params[18];

	float DT, DH, DB;
	if (selected_win_types[3] < 25) {
		DT = GH / (params[19] + params[20] + params[21]) * params[19];
		DH = GH / (params[19] + params[20] + params[21]) * params[20];
		DB = GH / (params[19] + params[20] + params[21]) * params[21];
	}
	else {
		// remove the gap between the door and the ground
		DT = GH / (params[19] + params[20]) * params[19];
		DH = GH / (params[19] + params[20]) * params[20];
		DB = 0.0f;
	}
	float DS = GW / (params[22] * 2 + params[23]) * params[22];
	float DW = GW / (params[22] * 2 + params[23]) * params[23];

	float TWT = AH / (params[24] + params[25] + params[26]) * params[24];
	float TWH = AH / (params[24] + params[25] + params[26]) * params[25];
	float TWB = AH / (params[24] + params[25] + params[26]) * params[26];
	float TWS = TW / (params[27] * 2 + params[28]) * params[27];
	float TWW = TW / (params[27] * 2 + params[28]) * params[28];

	////////////////////////////////////////////////////////////////////////////////////////////
	// HACK:
	// For cylinder, remove the side margin
	if (mass_grammar_id == 1) {
		SW = 0.0f;
	}

	decoded_params.resize(27);
	decoded_params[0] = GH;
	decoded_params[1] = FH;
	decoded_params[2] = FH2;
	decoded_params[3] = AH;
	decoded_params[4] = SW;
	decoded_params[5] = GW;
	decoded_params[6] = TW;
	decoded_params[7] = WT;
	decoded_params[8] = WH;
	decoded_params[9] = WB;
	decoded_params[10] = WS;
	decoded_params[11] = WW;
	decoded_params[12] = WT2;
	decoded_params[13] = WH2;
	decoded_params[14] = WB2;
	decoded_params[15] = WS2;
	decoded_params[16] = WW2;
	decoded_params[17] = DT;
	decoded_params[18] = DH;
	decoded_params[19] = DB;
	decoded_params[20] = DS;
	decoded_params[21] = DW;
	decoded_params[22] = TWT;
	decoded_params[23] = TWH;
	decoded_params[24] = TWB;
	decoded_params[25] = TWS;
	decoded_params[26] = TWW;
}

cv::Mat FacadeD::generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement, float window_prob) {
	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	float GH;
	if (utils::genRand() < 0.5) {
		GH = utils::genRand(0.4, 0.9);
	}
	else {
		GH = utils::genRand(1.1, 2.8);
	}
	float FH = 1;
	float FH2;
	if (utils::genRand() < 0.5) {
		FH2 = utils::genRand(0.3, 0.9);
	}
	else {
		FH2 = utils::genRand(1.1, 2);
	}
	float AH;
	if (utils::genRand() < 0.5) {
		AH = utils::genRand(0.3, 0.9);
	}
	else {
		AH = utils::genRand(1.1, 3);
	}
	float SW = utils::genRand(0, 0.8);
	float GW = utils::genRand(1, 2);
	float TW = 1;
	float WT = utils::genRand(0.005, 0.65);
	float WH = 1;
	float WB = utils::genRand(0.005, 0.5);	
	float WS = utils::genRand(0.01, 0.5);
	float WW = 1;
	float WT2 = utils::genRand(0.1, 0.65);
	float WH2 = 1;
	float WB2 = utils::genRand(0.1, 0.5);
	float WS2 = utils::genRand(0.04, 0.5);
	float WW2 = 1;
	float DT = utils::genRand(0.1, 0.25);
	float DH = 1;
	float DB = utils::genRand(0, 0.01);
	float DS = utils::genRand(0.1, 0.5);
	float DW = 1;
	float TWT = utils::genRand(0.05, 1.5);
	float TWH = 1;
	float TWB = utils::genRand(0.05, 0.6);
	float TWS = utils::genRand(0.01, 0.6);
	float TWW = 1;
		

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	
	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;
	
	// 2Fの各種高さをnormalize
	ratio = FH2 / (WT2 + WB2 + WH2);
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;
	
	// 2Fの各種幅をnormalize
	ratio = TW / (WS2 * 2 + WW2);
	WS2 *= ratio;
	WW2 *= ratio;
	
	// 最上階の各種高さをnormalize
	ratio = AH / (TWT + TWB + TWH);
	TWT *= ratio;
	TWB *= ratio;
	TWH *= ratio;
	
	// 最上階の各種幅をnormalize
	ratio = TW / (TWS * 2 + TWW);
	TWS *= ratio;
	TWW *= ratio;
	
	// １Fの各種高さをnormalize
	ratio = GH / (DT + DH + DB);
	DT *= ratio;
	DH *= ratio;
	DB *= ratio;
	
	// １Fの各種幅をnormalize
	ratio = GW / (DS * 2 + DW);
	DS *= ratio;
	DW *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * (NF - 3) + FH2);
	GH *= ratio;
	FH *= ratio;
	FH2 *= ratio;
	AH *= ratio;
	WT *= ratio;
	WH *= ratio;
	WB *= ratio;
	WT2 *= ratio;
	WH2 *= ratio;
	WB2 *= ratio;
	DT *= ratio;
	DH *= ratio;
	DB *= ratio;
	TWT *= ratio;
	TWH *= ratio;
	TWB *= ratio;
	ratio = (float)width / (SW * 2 + TW * NC);
	SW *= ratio;
	GW *= ratio;
	TW *= ratio;
	WS *= ratio;
	WW *= ratio;
	WS2 *= ratio;
	WW2 *= ratio;
	DS *= ratio;
	DW *= ratio;
	TWS *= ratio;
	TWW *= ratio;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータ値を格納
	params.push_back((float)(NF - range_NF.first) / (range_NF.second - range_NF.first));
	params.push_back((float)(NC - range_NC.first) / (range_NC.second - range_NC.first));
	params.push_back(GH / height);
	params.push_back(FH / height);
	params.push_back(FH2 / height);
	params.push_back(AH / height);
	params.push_back(SW / width);
	params.push_back(GW / width);
	params.push_back(TW / width);
	params.push_back(WT / FH);
	params.push_back(WH / FH);
	params.push_back(WB / FH);
	params.push_back(WS / TW);
	params.push_back(WW / TW);
	params.push_back(WT2 / FH2);
	params.push_back(WH2 / FH2);
	params.push_back(WB2 / FH2);
	params.push_back(WS2 / TW);
	params.push_back(WW2 / TW);
	params.push_back(DT / GH);
	params.push_back(DH / GH);
	params.push_back(DB / GH);
	params.push_back(DS / GW);
	params.push_back(DW / GW);
	params.push_back(TWT / AH);
	params.push_back(TWH / AH);
	params.push_back(TWB / AH);
	params.push_back(TWS / TW);
	params.push_back(TWW / TW);

	return generateFacade(width, height, thickness, GH, FH, FH2, AH, SW, GW, TW, WT, WH, WB, WS, WW, WT2, WH2, WB2, WS2, WW2, DT, DH, DB, DS, DW, TWT, TWH, TWB, TWS, TWW, window_displacement, window_prob);
}

cv::Mat FacadeD::generateFacade(int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float FH2, float AH, float SW, float GW, float TW, float WT, float WH, float WB, float WS, float WW, float WT2, float WH2, float WB2, float WS2, int WW2, float DT, float DH, float DB, float DS, float DW, float TWT, float TWH, float TWB, float TWS, float TWW, float window_displacement, float window_prob) {
	cv::Mat result(height, width, CV_8UC3, bg_color);

	int NF = std::round((float)(height - AH - GH - FH2) / FH) + 3;
	int NC = std::round((float)(width - SW * 2) / TW);
	int ND = std::round((float)(width - SW * 2) / GW);

	window_prob = 1 - utils::genRand(0, 1 - window_prob);

	// １Fのドアを描画
	for (int j = 0; j < ND; ++j) {
		float x1 = SW + GW * j + DS;
		float y1 = height - DB - DH;
		float x2 = SW + GW * j + DS + DW;
		float y2 = height - DB;

		if (window_displacement > 0) {
			x1 += utils::genRand(-GW * window_displacement, GW * window_displacement);
			y1 += utils::genRand(-GH * window_displacement, GH * window_displacement);
			x2 += utils::genRand(-GW * window_displacement, GW * window_displacement);
			y2 += utils::genRand(-GH * window_displacement, GH * window_displacement);
		}

		if (utils::genRand() < window_prob) {
			cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
		}
	}

	// 2Fの窓を描画
	for (int j = 0; j < NC; ++j) {
		float x1 = (SW + TW * j + WS2);
		float y1 = (height - GH - WB2 - WH2);
		float x2 = (SW + TW * j + WS2 + WW2);
		float y2 = (height - GH - WB2);

		if (window_displacement > 0) {
			x1 += utils::genRand(-TW * window_displacement, TW * window_displacement);
			y1 += utils::genRand(-FH2 * window_displacement, FH2 * window_displacement);
			x2 += utils::genRand(-TW * window_displacement, TW * window_displacement);
			y2 += utils::genRand(-FH2 * window_displacement, FH2 * window_displacement);
		}

		if (utils::genRand() < window_prob) {
			cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
		}
	}

	// 3F以上の窓を描画
	for (int i = 0; i < NF - 3; ++i) {
		for (int j = 0; j < NC; ++j) {
			float x1 = (SW + TW * j + WS);
			float y1 = (height - GH - FH2 - FH * i - WB - WH);
			float x2 = (SW + TW * j + WS + WW);
			float y2 = (height - GH - FH2 - FH * i - WB);

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

	// 最上階の窓を描画
	for (int j = 0; j < NC; ++j) {
		float x1 = (SW + TW * j + TWS);
		float y1 = (height - GH - FH2 - FH * (NF - 3) - TWB - TWH);
		float x2 = (SW + TW * j + TWS + TWW);
		float y2 = (height - GH - FH2 - FH * (NF - 3) - TWB);

		if (window_displacement > 0) {
			x1 += utils::genRand(-TW * window_displacement, TW * window_displacement);
			y1 += utils::genRand(-AH * window_displacement, AH * window_displacement);
			x2 += utils::genRand(-TW * window_displacement, TW * window_displacement);
			y2 += utils::genRand(-AH * window_displacement, AH * window_displacement);
		}

		if (utils::genRand() < window_prob) {
			cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
		}
	}

	return result;
}

int FacadeD::clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects) {
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
