#include "facadeC.h"
#include "Utils.h"

std::pair<int, int> FacadeC::range_NF = std::make_pair(3, 20);
std::pair<int, int> FacadeC::range_NC = std::make_pair(4, 20);

cv::Mat FacadeC::generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color) {
	std::vector<float> decoded_params;
	decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

	return generateFacade(1, width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20]);
}

void FacadeC::decodeParams(float width, float height, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, std::vector<float>& decoded_params) {
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

	float GH = (float)height / (params[2] + params[3] * (NF - 2) + params[4]) * params[2];
	float FH = (float)height / (params[2] + params[3] * (NF - 2) + params[4]) * params[3];
	float AH = (float)height / (params[2] + params[3] * (NF - 2) + params[4]) * params[4];
	float SW = (float)width / (params[5] * 2 + params[6] * NC) * params[5];
	float TW = (float)width / (params[5] * 2 + params[6] * NC) * params[6];
	int ND = std::round((float)(width - SW * 2) / width / params[7]);
	float GW = (float)(width - SW * 2) / ND;

	float WT = FH / (params[8] + params[9] + params[10]) * params[8];
	float WH = FH / (params[8] + params[9] + params[10]) * params[9];
	float WB = FH / (params[8] + params[9] + params[10]) * params[10];
	float WS = TW / (params[11] * 2 + params[12]) * params[11];
	float WW = TW / (params[11] * 2 + params[12]) * params[12];
	
	float DT, DH, DB;
	if (selected_win_types[2] < 25) {
		DT = GH / (params[13] + params[14] + params[15]) * params[13];
		DH = GH / (params[13] + params[14] + params[15]) * params[14];
		DB = GH / (params[13] + params[14] + params[15]) * params[15];
	}
	else {
		// remove the gap between the door and the ground
		DT = GH / (params[13] + params[14]) * params[13];
		DH = GH / (params[13] + params[14]) * params[14];
		DB = 0.0f;
	}
	float DS = GW / (params[16] * 2 + params[17]) * params[16];
	float DW = GW / (params[16] * 2 + params[17]) * params[17];

	float TWT = AH / (params[18] + params[19] + params[20]) * params[18];
	float TWH = AH / (params[18] + params[19] + params[20]) * params[19];
	float TWB = AH / (params[18] + params[19] + params[20]) * params[20];
	float TWS = TW / (params[21] * 2 + params[22]) * params[21];
	float TWW = TW / (params[21] * 2 + params[22]) * params[22];

	decoded_params.resize(21);
	decoded_params[0] = GH;
	decoded_params[1] = FH;
	decoded_params[2] = AH;
	decoded_params[3] = SW;
	decoded_params[4] = TW;
	decoded_params[5] = GW;
	decoded_params[6] = WT;
	decoded_params[7] = WH;
	decoded_params[8] = WB;
	decoded_params[9] = WS;
	decoded_params[10] = WW;
	decoded_params[11] = DT;
	decoded_params[12] = DH;
	decoded_params[13] = DB;
	decoded_params[14] = DS;
	decoded_params[15] = DW;
	decoded_params[16] = TWT;
	decoded_params[17] = TWH;
	decoded_params[18] = TWB;
	decoded_params[19] = TWS;
	decoded_params[20] = TWW;
}

cv::Mat FacadeC::generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement, float window_prob) {
	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	float GH = utils::genRand(0.4, 5);
	float FH = 1;
	float AH = utils::genRand(0.3, 3);	
	float SW = utils::genRand(0, 0.7);
	float TW = 1;
	float GW = utils::genRand(1, 2);
	float WT = utils::genRand(0.005, 0.45);
	float WH = 1;
	float WB = utils::genRand(0.005, 0.2);	
	float WS = utils::genRand(0.01, 0.34);
	float WW = 1;
	float DT = utils::genRand(0.01, 0.45);
	float DH = 1;
	float DB = utils::genRand(0, 0.1);	
	float DS = utils::genRand(0.01, 0.5);
	float DW = 1;
	float TWT = utils::genRand(0.01, 0.6);
	float TWH = 1;
	float TWB = utils::genRand(0.01, 1);
	float TWS = utils::genRand(0.01, 0.5);
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
	ratio = (float)height / (GH + AH + FH * (NF - 2));
	GH *= ratio;
	FH *= ratio;
	AH *= ratio;
	WT *= ratio;
	WH *= ratio;
	WB *= ratio;
	DT *= ratio;
	DH *= ratio;
	DB *= ratio;
	TWT *= ratio;
	TWH *= ratio;
	TWB *= ratio;
	ratio = (float)width / (SW * 2 + TW * NC);
	SW *= ratio;
	TW *= ratio;
	GW *= ratio;
	WS *= ratio;
	WW *= ratio;
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
	params.push_back(AH / height);
	params.push_back(SW / width);
	params.push_back(TW / width);
	params.push_back(GW / width);
	params.push_back(WT / FH);
	params.push_back(WH / FH);
	params.push_back(WB / FH);
	params.push_back(WS / TW);
	params.push_back(WW / TW);
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

	return generateFacade(1, width, height, thickness, GH, FH, AH, SW, TW, GW, WT, WH, WB, WS, WW, DT, DH, DB, DS, DW, TWT, TWH, TWB, TWS, TWW, window_displacement, window_prob);
}

cv::Mat FacadeC::generateFacade(float scale, int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float AH, float SW, float TW, float GW, float WT, float WH, float WB, float WS, int WW, float DT, float DH, float DB, float DS, float DW, float TWT, float TWH, float TWB, float TWS, float TWW, float window_displacement, float window_prob) {
	cv::Mat result(height * scale, width * scale, CV_8UC3, bg_color);

	int NF = std::round((float)(height - AH - GH) / FH) + 2;
	int NC = std::round((float)(width - SW * 2) / TW);
	int ND = std::round((float)(width - SW * 2) / GW);

	window_prob = 1 - utils::genRand(0, 1 - window_prob);

	// １Fのドアを描画
	for (int j = 0; j < ND; ++j) {
		float x1 = (SW + GW * j + DS) * scale;
		float y1 = (height - DB - DH) * scale;
		float x2 = (SW + GW * j + DS + DW) * scale;
		float y2 = (height - DB) * scale;

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

	// ２F以上の窓を描画
	for (int i = 0; i < NF - 2; ++i) {
		for (int j = 0; j < NC; ++j) {
			float x1 = (SW + TW * j + WS) * scale;
			float y1 = (height - GH - FH * i - WB - WH) * scale;
			float x2 = (SW + TW * j + WS + WW) * scale;
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

	// 最上階の窓を描画
	for (int j = 0; j < NC; ++j) {
		float x1 = (SW + TW * j + TWS) * scale;
		float y1 = (height - GH - FH * (NF - 2) - TWB - TWH) * scale;
		float x2 = (SW + TW * j + TWS + TWW) * scale;
		float y2 = (height - GH - FH * (NF - 2) - TWB) * scale;

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

int FacadeC::clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int j = 0; j < win_rects[0].size(); ++j) {
		win_rects[0][j].type = 0;
	}
	for (int i = 1; i < win_rects.size() - 1; ++i) {
		for (int j = 0; j < win_rects[i].size(); ++j) {
			win_rects[i][j].type = 1;
		}
	}
	for (int j = 0; j < win_rects.back().size(); ++j) {
		win_rects.back()[j].type = 2;
	}

	return 3;
}
