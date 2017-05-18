#include "facadeG.h"
#include "Utils.h"

std::pair<int, int> FacadeG::range_NF = std::make_pair(6, 20);
std::pair<int, int> FacadeG::range_NC = std::make_pair(5, 20);

void FacadeG::attachDoors(std::vector<float>& params, const std::vector<int>& selected_win_types) {
	if (selected_win_types[7] < 25) {
		// do nothing
	}
	else {
		// remove the gap between the door and the ground
		params[21] = 0;
	}

	if (selected_win_types[6] < 25) {
		// do nothing
	}
	else {
		// remove the gap between the door and the ground
		params[24] = 0;
	}
}

cv::Mat FacadeG::generateFacade(int width, int height, int thickness, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, const cv::Scalar& bg_color, const cv::Scalar& fg_color) {
	std::vector<float> decoded_params;
	decodeParams(width, height, num_floors, num_columns, params, selected_win_types, decoded_params);

	return generateFacade(width, height, thickness, bg_color, fg_color, decoded_params[0], decoded_params[1], decoded_params[2], decoded_params[3], decoded_params[4], decoded_params[5], decoded_params[6], decoded_params[7], decoded_params[8], decoded_params[9], decoded_params[10], decoded_params[11], decoded_params[12], decoded_params[13], decoded_params[14], decoded_params[15], decoded_params[16], decoded_params[17], decoded_params[18], decoded_params[19], decoded_params[20], decoded_params[21], decoded_params[22], decoded_params[23], decoded_params[24], decoded_params[25], decoded_params[26], decoded_params[27], decoded_params[28]);
}

void FacadeG::decodeParams(float width, float height, int num_floors, int num_columns, const std::vector<float>& params, std::vector<int>& selected_win_types, std::vector<float>& decoded_params) {
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

	float GH = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[2];
	float FH = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[3];
	float FH2 = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[4];
	float AH = (float)height / (params[2] + params[3] * (NF - 3) + params[4] + params[5]) * params[5];
	float SW = (float)width / (params[6] * 2 + params[7] * (NC - 2)) * params[6];
	float TW = (float)width / (params[6] * 2 + params[7] * (NC - 2)) * params[7];

	float WT = FH / (params[8] + params[9] + params[10]) * params[8];
	float WH = FH / (params[8] + params[9] + params[10]) * params[9];
	float WB = FH / (params[8] + params[9] + params[10]) * params[10];
	float WS = TW / (params[11] * 2 + params[12]) * params[11];
	float WW = TW / (params[11] * 2 + params[12]) * params[12];

	float WT2 = FH2 / (params[13] + params[14] + params[15]) * params[13];
	float WH2 = FH2 / (params[13] + params[14] + params[15]) * params[14];
	float WB2 = FH2 / (params[13] + params[14] + params[15]) * params[15];
	float WO2 = SW / (params[16] + params[17] + params[18]) * params[16];
	float WW2 = SW / (params[16] + params[17] + params[18]) * params[17];
	float WI2 = SW / (params[16] + params[17] + params[18]) * params[18];

	float DT, DH, DB;
	if (selected_win_types[7] < 25) {
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
	float DT2, DH2, DB2;
	if (selected_win_types[6] < 25) {
		DT2 = GH / (params[22] + params[23] + params[24]) * params[22];
		DH2 = GH / (params[22] + params[23] + params[24]) * params[23];
		DB2 = GH / (params[22] + params[23] + params[24]) * params[24];
	}
	else {
		// remove the gap between the door and the ground
		DT2 = GH / (params[22] + params[23]) * params[22];
		DH2 = GH / (params[22] + params[23]) * params[23];
		DB2 = 0.0f;
	}
	float DO2 = SW / (params[25] + params[26] + params[27]) * params[25];
	float DW2 = SW / (params[25] + params[26] + params[27]) * params[26];
	float DI2 = SW / (params[25] + params[26] + params[27]) * params[27];
	float TWT = AH / (params[28] + params[29] + params[30]) * params[28];
	float TWH = AH / (params[28] + params[29] + params[30]) * params[29];
	float TWB = AH / (params[28] + params[29] + params[30]) * params[30];

	decoded_params.resize(29);
	decoded_params[0] = GH;
	decoded_params[1] = FH;
	decoded_params[2] = FH2;
	decoded_params[3] = AH;
	decoded_params[4] = SW;
	decoded_params[5] = TW;
	decoded_params[6] = WT;
	decoded_params[7] = WH;
	decoded_params[8] = WB;
	decoded_params[9] = WS;
	decoded_params[10] = WW;
	decoded_params[11] = WT2;
	decoded_params[12] = WH2;
	decoded_params[13] = WB2;
	decoded_params[14] = WO2;
	decoded_params[15] = WW2;
	decoded_params[16] = WI2;
	decoded_params[17] = DT;
	decoded_params[18] = DH;
	decoded_params[19] = DB;
	decoded_params[20] = DT2;
	decoded_params[21] = DH2;
	decoded_params[22] = DB2;
	decoded_params[23] = DO2;
	decoded_params[24] = DW2;
	decoded_params[25] = DI2;
	decoded_params[26] = TWT;
	decoded_params[27] = TWH;
	decoded_params[28] = TWB;
}

cv::Mat FacadeG::generateRandomFacade(int width, int height, int thickness, std::vector<float>& params, float window_displacement, float window_prob) {
	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::genRand(range_NF.first, range_NF.second + 1);
	int NC = utils::genRand(range_NC.first, range_NC.second + 1);

	float GH = utils::genRand(0.4, 5);
	float FH = 1;
	float FH2 = utils::genRand(0.3, 2);
	float AH;
	if (utils::genRand() < 0.5) {
		AH = utils::genRand(0.3, 0.9);
	}
	else {
		AH = utils::genRand(1.1, 3);
	}
	float SW;
	if (utils::genRand() < 0.3) {
		SW = utils::genRand(0.8, 0.9);
	}
	else {
		SW = utils::genRand(1.1, 3);
	}
	float TW = 1;
	float WT = utils::genRand(0.005, 0.4);
	float WH = 1;
	float WB = utils::genRand(0.005, 0.4);
	float WS = utils::genRand(0.01, 0.5);
	float WW = 1;
	float WT2 = utils::genRand(0.15, 0.6);
	float WH2 = 1;
	float WB2 = utils::genRand(0.15, 0.4);
	float WO2 = utils::genRand(0.3, 2.2);
	float WW2 = 1;
	float WI2 = utils::genRand(0.3, 2.2);
	float DT = utils::genRand(0.1, 0.5);
	float DH = 1;
	float DB = utils::genRand(0, 0.7);
	float DT2 = utils::genRand(0.14, 0.35);
	float DH2 = 1;
	float DB2 = utils::genRand(0, 0.5);
	float DO2 = utils::genRand(0.2, 2.2);
	float DW2 = 1;
	float DI2 = utils::genRand(0.2, 2.2);
	float TWT = utils::genRand(0.1, 0.7);
	float TWH = 1;
	float TWB = utils::genRand(0.1, 0.7);
	

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	
	// ２Fのフロアの各種高さをnormalize
	ratio = FH2 / (WT2 + WB2 + WH2);
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
	ratio = GH / (DT + DH + DB);
	DT *= ratio;
	DH *= ratio;
	DB *= ratio;

	// １Fの左右の各種高さをnormalize
	ratio = GH / (DT2 + DH2 + DB2);
	DT2 *= ratio;
	DH2 *= ratio;
	DB2 *= ratio;

	// １Fの左右の各種幅をnormalize
	ratio = SW / (DO2 + DW2 + DI2);
	DO2 *= ratio;
	DW2 *= ratio;
	DI2 *= ratio;

	// 最上階の各種高さをnormalize
	ratio = AH / (TWT + TWB + TWH);
	TWT *= ratio;
	TWB *= ratio;
	TWH *= ratio;
	

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH2 + FH * (NF - 3));
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
	DT2 *= ratio;
	DH2 *= ratio;
	DB2 *= ratio;
	TWT *= ratio;
	TWH *= ratio;
	TWB *= ratio;
	ratio = (float)width / (SW * 2 + TW * (NC - 2));
	SW *= ratio;
	TW *= ratio;
	WS *= ratio;
	WW *= ratio;
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;
	DO2 *= ratio;
	DW2 *= ratio;
	DI2 *= ratio;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータ値を格納
	params.push_back((float)(NF - range_NF.first) / (range_NF.second - range_NF.first));
	params.push_back((float)(NC - range_NC.first) / (range_NC.second - range_NC.first));
	params.push_back(GH / height);
	params.push_back(FH / height);
	params.push_back(FH2 / height);
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
	params.push_back(DT2 / GH);
	params.push_back(DH2 / GH);
	params.push_back(DB2 / GH);
	params.push_back(DO2 / GH);
	params.push_back(DW2 / GH);
	params.push_back(DI2 / GH);
	params.push_back(TWT / AH);
	params.push_back(TWH / AH);
	params.push_back(TWB / AH);

	return generateFacade(width, height, thickness, GH, FH, FH2, AH, SW, TW, WT, WH, WB, WS, WW, WT2, WH2, WB2, WO2, WW2, WI2, DT, DH, DB, DT2, DH2, DB2, DO2, DW2, DI2, TWT, TWH, TWB, window_displacement, window_prob);
}

cv::Mat FacadeG::generateFacade(int width, int height, int thickness, const cv::Scalar& bg_color, const cv::Scalar& fg_color, float GH, float FH, float FH2, float AH, float SW, float TW, float WT, float WH, float WB, float WS, float WW, float WT2, float WH2, float WB2, float WO2, float WW2, float WI2, float DT, float DH, float DB, float DT2, float DH2, float DB2, float DO2, float DW2, float DI2, float TWT, float TWH, float TWB, float window_displacement, float window_prob) {
	cv::Mat result(height, width, CV_8UC3, bg_color);

	int NF = std::round((float)(height - AH - GH - FH2) / FH) + 3;
	int NC = std::round((float)(width - SW * 2) / TW) + 2;

	window_prob = 1 - utils::genRand(0, 1 - window_prob);

	// １Fのドアを描画
	{
		// 左端
		{
			float x1 = DO2;
			float y1 = height - DB2 - DH2;
			float x2 = DO2 + DW2;
			float y2 = height - DB2;

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
			float x1 = SW + TW * j + WS;
			float y1 = height - DB - DH;
			float x2 = SW + TW * j + WS + WW;
			float y2 = height - DB;

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
			float x1 = SW + TW * (NC - 2) + DI2;
			float y1 = height - DB2 - DH2;
			float x2 = SW + TW * (NC - 2) + DI2 + DW2;
			float y2 = height - DB2;

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

	// 2Fのドアを描画
	{
		// 左端
		{
			float x1 = WO2;
			float y1 = height - GH - WB2 - WH2;
			float x2 = WO2 + WW2;
			float y2 = height - GH - WB2;

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-FH2 * window_displacement, FH2 * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-FH2 * window_displacement, FH2 * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			float x1 = SW + TW * j + WS;
			float y1 = height - GH - WB2 - WH2;
			float x2 = SW + TW * j + WS + WW;
			float y2 = height - GH - WB2;

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

		// 右端
		{
			float x1 = SW + TW * (NC - 2) + WI2;
			float y1 = height - GH - WB2 - WH2;
			float x2 = SW + TW * (NC - 2) + WI2 + WW2;
			float y2 = height - GH - WB2;

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-FH2 * window_displacement, FH2 * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-FH2 * window_displacement, FH2 * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}
	}

	// 3F以上の窓を描画
	for (int i = 0; i < NF - 3; ++i) {
		// 左端
			{
				float x1 = WO2;
				float y1 = height - GH - FH2 - FH * i - WB - WH;
				float x2 = WO2 + WW2;
				float y2 = height - GH - FH2 - FH * i - WB;

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
			float y1 = height - GH - FH2 - FH * i - WB - WH;
			float x2 = SW + TW * j + WS + WW;
			float y2 = height - GH - FH2 - FH * i - WB;

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
			float y1 = height - GH - FH2 - FH * i - WB - WH;
			float x2 = SW + TW * (NC - 2) + WI2 + WW2;
			float y2 = height - GH - FH2 - FH * i - WB;

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

	// 最上階の窓を描画
	{
		// 左端
		{
			float x1 = WO2;
			float y1 = (height - GH - FH2 - FH * (NF - 3) - TWB - TWH);
			float x2 = (WO2 + WW2);
			float y2 = (height - GH - FH2 - FH * (NF - 3) - TWB);

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-AH * window_displacement, AH * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-AH * window_displacement, AH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			float x1 = (SW + TW * j + WS);
			float y1 = (height - GH - FH2 - FH * (NF - 3) - TWB - TWH);
			float x2 = (SW + TW * j + WS + WW);
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

		// 右端
		{
			float x1 = (SW + TW * (NC - 2) + WI2);
			float y1 = (height - GH - FH2 - FH * (NF - 3) - TWB - TWH);
			float x2 = (SW + TW * (NC - 2) + WI2 + WW2);
			float y2 = (height - GH - FH2 - FH * (NF - 3) - TWB);

			if (window_displacement > 0) {
				x1 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y1 += utils::genRand(-AH * window_displacement, AH * window_displacement);
				x2 += utils::genRand(-SW * window_displacement, SW * window_displacement);
				y2 += utils::genRand(-AH * window_displacement, AH * window_displacement);
			}

			if (utils::genRand() < window_prob) {
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), fg_color, thickness);
			}
		}
	}

	return result;
}

int FacadeG::clusterWindowTypes(std::vector<std::vector<fs::WindowPos>>& win_rects) {
	for (int j = 0; j < win_rects[0].size(); j += win_rects[0].size() - 1) {
		win_rects[0][j].type = 0;
	}

	for (int j = 1; j < win_rects[0].size() - 1; ++j) {
		win_rects[0][j].type = 1;
	}

	for (int i = 1; i < win_rects.size() - 2; ++i) {
		for (int j = 0; j < win_rects[i].size(); j += win_rects[i].size() - 1) {
			win_rects[i][j].type = 2;
		}

		for (int j = 1; j < win_rects[i].size() - 1; ++j) {
			win_rects[i][j].type = 3;
		}
	}

	for (int j = 0; j < win_rects[win_rects.size() - 2].size(); j += win_rects[win_rects.size() - 2].size() - 1) {
		win_rects[win_rects.size() - 2][j].type = 4;
	}

	for (int j = 1; j < win_rects[win_rects.size() - 2].size() - 1; ++j) {
		win_rects[win_rects.size() - 2][j].type = 5;
	}

	for (int j = 0; j < win_rects.back().size(); j += win_rects.back().size() - 1) {
		win_rects.back()[j].type = 6;
	}

	for (int j = 1; j < win_rects.back().size() - 1; ++j) {
		win_rects.back()[j].type = 7;
	}

	return 8;
}
