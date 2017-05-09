#include "facadeF.h"
#include "Utils.h"

std::pair<int, int> FacadeF::range_NF = std::make_pair(2, 20);
std::pair<int, int> FacadeF::range_NC = std::make_pair(3, 20);

cv::Mat FacadeF::generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color) {
	std::vector<float> decoded_params;
	decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

	return generateFacade(1, width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26]);
}

void FacadeF::decodeParams(float width, float height, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, std::vector<float>& decoded_params) {
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

	float GH = (float)height / (params[2] + params[3] * (NF - 1) + params[4]) * params[2];
	float FH = (float)height / (params[2] + params[3] * (NF - 1) + params[4]) * params[3];
	float AH = (float)height / (params[2] + params[3] * (NF - 1) + params[4]) * params[4];
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

	float DT, DH, DB;
	if (selected_win_types[4] < 25) {
		DT = GH / (params[18] + params[19] + params[20]) * params[18];
		DH = GH / (params[18] + params[19] + params[20]) * params[19];
		DB = GH / (params[18] + params[19] + params[20]) * params[20];
	}
	else {
		// remove the gap between the door and the ground
		DT = GH / (params[18] + params[19]) * params[18];
		DH = GH / (params[18] + params[19]) * params[19];
		DB = 0.0f;
	}
	float DS = TW / (params[21] * 2 + params[22]) * params[21];
	float DW = TW / (params[21] * 2 + params[22]) * params[22];

	float DT2, DH2, DB2;
	if (selected_win_types[3] < 25) {
		DT2 = GH / (params[23] + params[24] + params[25]) * params[23];
		DH2 = GH / (params[23] + params[24] + params[25]) * params[24];
		DB2 = GH / (params[23] + params[24] + params[25]) * params[25];
	}
	else {
		// remove the gap between the door and the ground
		DT2 = GH / (params[23] + params[24]) * params[23];
		DH2 = GH / (params[23] + params[24]) * params[24];
		DB2 = 0.0f;
	}
	float DO2 = SW / (params[26] + params[27] + params[28]) * params[26];
	float DW2 = SW / (params[26] + params[27] + params[28]) * params[27];
	float DI2 = SW / (params[26] + params[27] + params[28]) * params[28];

	decoded_params.resize(27);
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
	decoded_params[16] = DT;
	decoded_params[17] = DH;
	decoded_params[18] = DB;
	decoded_params[19] = DS;
	decoded_params[20] = DW;
	decoded_params[21] = DT2;
	decoded_params[22] = DH2;
	decoded_params[23] = DB2;
	decoded_params[24] = DO2;
	decoded_params[25] = DW2;
	decoded_params[26] = DI2;
}

cv::Mat FacadeF::generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement, float window_prob) {
	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	float GH = utils::genRand(0.4, 5);
	float FH = 1;
	float AH = utils::genRand(0, 0.001);
	float SW;
	if (utils::genRand() < 0.3) {
		SW = utils::genRand(0.8, 0.9);
	}
	else {
		SW = utils::genRand(1.2, 3);
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
	float DT = utils::genRand(0.1, 0.22);
	float DH = 1;
	float DB = utils::genRand(0, 0.35);
	float DS = utils::genRand(0.1, 0.4);
	float DW = 1;
	float DT2 = utils::genRand(0.1, 1.3);
	float DH2 = 1;
	float DB2 = utils::genRand(0, 1);
	float DO2 = utils::genRand(0.3, 2.2);
	float DW2 = 1;
	float DI2 = utils::genRand(0.3, 2.2);


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

	// １Fの各種高さをnormalize
	ratio = GH / (DT + DB + DH);
	DT *= ratio;
	DH *= ratio;
	DB *= ratio;

	// １Fの左右の各種高さをnormalize
	ratio = GH / (DT2 + DB2 + DH2);
	DT2 *= ratio;
	DH2 *= ratio;
	DB2 *= ratio;

	// １Fの各種幅をnormalize
	ratio = TW / (DS * 2 + DW);
	DS *= ratio;
	DW *= ratio;

	// １Fの左右の各種幅をnormalize
	ratio = SW / (DO2 + DW2 + DI2);
	DO2 *= ratio;
	DW2 *= ratio;
	DI2 *= ratio;
	
	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * (NF - 1));
	GH *= ratio;
	FH *= ratio;
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
	DT2 *= ratio;
	DH2 *= ratio;
	DB2 *= ratio;
	ratio = (float)width / (SW * 2 + TW * (NC - 2));
	SW *= ratio;
	TW *= ratio;
	WS *= ratio;
	WW *= ratio;
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;
	DS *= ratio;
	DW *= ratio;
	DO2 *= ratio;
	DW2 *= ratio;
	DI2 *= ratio;

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
	params.push_back(DT / GH);
	params.push_back(DH / GH);
	params.push_back(DB / GH);
	params.push_back(DS / TW);
	params.push_back(DW / TW);
	params.push_back(DT2 / GH);
	params.push_back(DH2 / GH);
	params.push_back(DB2 / GH);
	params.push_back(DO2 / SW);
	params.push_back(DW2 / SW);
	params.push_back(DI2 / SW);

	return generateFacade(1, width, height, thickness, GH, FH, AH, SW, TW, WT, WH, WB, WS, WW, WT2, WH2, WB2, WO2, WW2, WI2, DT, DH, DB, DS, DW, DT2, DH2, DB2, DO2, DW2, DI2, window_displacement, window_prob);
}

cv::Mat FacadeF::generateFacade(float scale, int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float AH, float SW, float TW, float WT, float WH, float WB, float WS, float WW, float WT2, float WH2, float WB2, float WO2, float WW2, float WI2, float DT, float DH, float DB, float DS, float DW, float DT2, float DH2, float DB2, float DO2, float DW2, float DI2, float window_displacement, float window_prob) {
	cv::Mat result(height * scale, width * scale, CV_8UC3, bg_color);

	int NF = std::round((float)(height - AH - GH) / FH) + 1;
	int NC = std::round((float)(width - SW * 2) / TW) + 2;

	window_prob = 1 - utils::genRand(0, 1 - window_prob);

	// １Fのドアを描画
	{
		// 左端
		{
			float x1 = DO2 * scale;
			float y1 = (height - DB2 - DH2) * scale;
			float x2 = (DO2 + DW2) * scale;
			float y2 = (height - DB2) * scale;

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-GH * window_displacement, GH * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-GH * window_displacement, GH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			float x1 = (SW + TW * j + DS) * scale;
			float y1 = (height - DB - DH) * scale;
			float x2 = (SW + TW * j + DS + DW) * scale;
			float y2 = (height - DB) * scale;

			if (window_displacement > 0) {
				x1 += utils::genRand(-TW * window_displacement, TW * window_displacement);
				y1 += utils::genRand(-GH * window_displacement, GH * window_displacement);
				x2 += utils::genRand(-TW * window_displacement, TW * window_displacement);
				y2 += utils::genRand(-GH * window_displacement, GH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}

		// 右端
		{
			float x1 = (SW + TW * (NC - 2) + DI2) * scale;
			float y1 = (height - DB2 - DH2) * scale;
			float x2 = (SW + TW * (NC - 2) + DI2 + DW2) * scale;
			float y2 = (height - DB2) * scale;

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-GH * window_displacement, GH * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-GH * window_displacement, GH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}
	}

	// ２F以上の窓を描画
	for (int i = 0; i < NF - 1; ++i) {
		// 左端
			{
				float x1 = WO2 * scale;
				float y1 = (height - GH - FH * i - WB2 - WH2) * scale;
				float x2 = (WO2 + WW2) * scale;
				float y2 = (height - GH - FH * i - WB2) * scale;

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

		// 右端
		{
			float x1 = (SW + TW * (NC - 2) + WI2) * scale;
			float y1 = (height - GH - FH * i - WB2 - WH2) * scale;
			float x2 = (SW + TW * (NC - 2) + WI2 + WW2) * scale;
			float y2 = (height - GH - FH * i - WB2) * scale;

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

int FacadeF::clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int i = 0; i < win_rects.size() - 1; ++i) {
		if (win_rects[i].size() > 0) {
			win_rects[i][0].type = 0;
		}

		for (int j = 1; j < win_rects[i].size() - 1; ++j) {
			win_rects[i][j].type = 1;
		}

		if (win_rects[i].size() > 0) {
			win_rects[i].back().type = 2;
		}
	}

	for (int j = 0; j < win_rects.back().size(); j += win_rects.back().size() - 1) {
		win_rects.back()[j].type = 3;
	}

	for (int j = 1; j < win_rects.back().size() - 1; ++j) {
		win_rects.back()[j].type = 4;
	}

	return 5;
}
