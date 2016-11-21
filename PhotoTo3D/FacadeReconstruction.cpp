#include "FacadeReconstruction.h"
#include "Utils.h"
#include "facadeA.h"
#include "facadeB.h"
#include "facadeC.h"
#include "facadeD.h"
#include "facadeE.h"
#include "facadeF.h"
#include "facadeG.h"
#include "facadeH.h"

namespace facarec {

	int NUM_GRAMMARS = 8;

	int recognition(boost::shared_ptr<Classifier> classifier, const cv::Mat& input_image) {
		std::vector<Prediction> predictions = classifier->Classify(input_image, NUM_GRAMMARS);
		return predictions[0].first;
	}

	std::vector<float> parameterEstimation(int grammar_id, boost::shared_ptr<Regression> regression, const cv::Mat& input_image, int width, int height) {
		std::vector<float> params = regression->Predict(input_image);

		utils::output_vector(params);

		if (grammar_id == 0) {
			return parameterEstimation1(width, height, params);
		}
		else if (grammar_id == 1) {
			return parameterEstimation2(width, height, params);
		}
		else if (grammar_id == 2) {
			return parameterEstimation3(width, height, params);
		}
		else if (grammar_id == 3) {
			return parameterEstimation4(width, height, params);
		}
		else if (grammar_id == 4) {
			return parameterEstimation5(width, height, params);
		}
		else if (grammar_id == 5) {
			return parameterEstimation6(width, height, params);
		}
		else if (grammar_id == 6) {
			return parameterEstimation7(width, height, params);
		}
		else if (grammar_id == 7) {
			return parameterEstimation8(width, height, params);
		}

		std::cerr << "Unknown grammar id: " << grammar_id << std::endl;
		return std::vector<float>();
	}

	std::vector<float> parameterEstimation1(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 19 + 1);
		if (NF < 1) NF = 1;
		int NC = std::round(params[1] * 19 + 1);
		if (NC < 1) NC = 1;

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

		cv::Mat img = generateFacadeA(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, BS, TW, AH, FH, BH);
		cv::imwrite("estimated_facade.png", img);

		params.resize(12);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = BS / 100;
		params[8] = TW / 100;
		params[9] = AH / 100;
		params[10] = FH / 100;
		params[11] = BH / 100;

		return params;
	}

	std::vector<float> parameterEstimation2(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 18 + 2);
		if (NF < 2) NF = 2;
		int NC = std::round(params[1] * 19 + 1);
		if (NC < 1) NC = 1;

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

		cv::Mat img = generateFacadeB(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, BS, TW, AH, FH, GH, ND, DT, DH, DW, DS);
		cv::imwrite("estimated_facade.png", img);

		params.resize(17);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = BS / 100;
		params[8] = TW / 100;
		params[9] = AH / 100;
		params[10] = FH / 100;
		params[11] = GH / 100;
		params[12] = ND / 100;
		params[13] = DT / 100;
		params[14] = DH / 100;
		params[15] = DW / 100;
		params[16] = DS / 100;

		return params;
	}

	std::vector<float> parameterEstimation3(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 17 + 3);
		if (NF < 3) NF = 3;
		int NC = std::round(params[1] * 19 + 1);
		if (NC < 1) NC = 1;

		float BS = (float)width / (params[7] * 2 + params[8] * NC) * params[7];
		float TW = (float)width / (params[7] * 2 + params[8] * NC) * params[8];
		float AH = (float)height / (params[9] + params[10] * (NF - 2) + params[11]) * params[9];
		float FH = (float)height / (params[9] + params[10] * (NF - 2) + params[11]) * params[10];
		float GH = (float)height / (params[9] + params[10] * (NF - 2) + params[11]) * params[11];

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

		float TWS = TW / (params[18] + params[17] * 2) * params[17];
		float TWW = TW / (params[18] + params[17] * 2) * params[18];
		float TWT = AH / (params[19] + params[20] + params[21]) * params[19];
		float TWH = AH / (params[19] + params[20] + params[21]) * params[20];
		float TWB = AH / (params[19] + params[20] + params[21]) * params[21];

		cv::Mat img = generateFacadeC(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, BS, TW, AH, FH, GH, ND, DT, DH, DW, DS, TWS, TWW, TWT, TWH, TWB);
		cv::imwrite("estimated_facade.png", img);

		params.resize(22);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = BS / 100;
		params[8] = TW / 100;
		params[9] = AH / 100;
		params[10] = FH / 100;
		params[11] = GH / 100;
		params[12] = ND / 100;
		params[13] = DT / 100;
		params[14] = DH / 100;
		params[15] = DW / 100;
		params[16] = DS / 100;
		params[17] = TWS / 100;
		params[18] = TWW / 100;
		params[19] = TWT / 100;
		params[20] = TWH / 100;
		params[21] = TWB / 100;

		return params;
	}
	
	std::vector<float> parameterEstimation4(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 17 + 3);
		if (NF < 3) NF = 3;
		int NC = std::round(params[1] * 19 + 1);
		if (NC < 1) NC = 1;

		float BS = (float)width / (params[12] * 2 + params[13] * NC) * params[12];
		float TW = (float)width / (params[12] * 2 + params[13] * NC) * params[13];
		float AH = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[14];
		float FH = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[15];
		float FH2 = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[16];
		float GH = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[17];

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

		cv::Mat img = generateFacadeD(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, WW2, WH2, WS2, WT2, WB2, BS, TW, AH, FH, FH2, GH, ND, DT, DH, DW, DS);
		cv::imwrite("estimated_facade.png", img);

		params.resize(23);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = WW2 / 100;
		params[8] = WH2 / 100;
		params[9] = WS2 / 100;
		params[10] = WT2 / 100;
		params[11] = WB2 / 100;
		params[12] = BS / 100;
		params[13] = TW / 100;
		params[14] = AH / 100;
		params[15] = FH / 100;
		params[16] = FH2 / 100;
		params[17] = GH / 100;
		params[18] = ND / 100;
		params[19] = DT / 100;
		params[20] = DH / 100;
		params[21] = DW / 100;
		params[22] = DS / 100;

		return params;
	}

	std::vector<float> parameterEstimation5(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 15 + 5);
		if (NF < 5) NF = 5;
		int NC = std::round(params[1] * 19 + 1);
		if (NC < 1) NC = 1;

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

		cv::Mat img = generateFacadeE(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, WW2, WH2, WS2, WT2, WB2, BS, TW, AH, FH, FH2, GH, ND, DT, DH, DW, DS, TWS, TWW, TWT, TWH, TWB);
		cv::imwrite("estimated_facade.png", img);

		params.resize(28);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = WW2 / 100;
		params[8] = WH2 / 100;
		params[9] = WS2 / 100;
		params[10] = WT2 / 100;
		params[11] = WB2 / 100;
		params[12] = BS / 100;
		params[13] = TW / 100;
		params[14] = AH / 100;
		params[15] = FH / 100;
		params[16] = FH2 / 100;
		params[17] = GH / 100;
		params[18] = ND / 100;
		params[19] = DT / 100;
		params[20] = DH / 100;
		params[21] = DW / 100;
		params[22] = DS / 100;
		params[23] = TWS / 100;
		params[24] = TWW / 100;
		params[25] = TWT / 100;
		params[26] = TWH / 100;
		params[27] = TWB / 100;

		return params;
	}

	std::vector<float> parameterEstimation6(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 19 + 1);
		if (NF < 1) NF = 1;
		int NC = std::round(params[1] * 17 + 3);
		if (NC < 3) NC = 3;

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

		cv::Mat img = generateFacadeF(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, BS, TW, AH, FH, BH, WW2, WH2, WO2, WI2, WT2, WB2);
		cv::imwrite("estimated_facade.png", img);

		params.resize(18);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = BS / 100;
		params[8] = TW / 100;
		params[9] = AH / 100;
		params[10] = FH / 100;
		params[11] = BH / 100;
		params[12] = WW2 / 100;
		params[13] = WH2 / 100;
		params[14] = WO2 / 100;
		params[15] = WI2 / 100;
		params[16] = WT2 / 100;
		params[17] = WB2 / 100;

		return params;
	}

	std::vector<float> parameterEstimation7(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 19 + 1);
		if (NF < 1) NF = 1;
		int NC = std::round(params[1] * 17 + 3);
		if (NC < 3) NC = 3;

		float BS = (float)width / (params[7] * 2 + params[8] * (NC - 1) + params[12]) * params[7];
		float TW = (float)width / (params[7] * 2 + params[8] * (NC - 1) + params[12]) * params[8];
		float TW2 = (float)width / (params[7] * 2 + params[8] * (NC - 1) + params[12]) * params[12];
		float AH = (float)height / (params[9] + params[10] * NF + params[11]) * params[9];
		float FH = (float)height / (params[9] + params[10] * NF + params[11]) * params[10];
		float BH = (float)height / (params[9] + params[10] * NF + params[11]) * params[11];

		float WW = TW / (params[2] + params[4] * 2) * params[2];
		float WH = FH / (params[3] + params[5] + params[6]) * params[3];
		float WS = TW / (params[2] + params[4] * 2) * params[4];
		float WT = FH / (params[3] + params[5] + params[6]) * params[5];
		float WB = FH / (params[3] + params[5] + params[6]) * params[6];

		float WW2 = TW2 / (params[13] + params[15] * 2) * params[13];
		float WS2 = TW2 / (params[13] + params[15] * 2) * params[15];
		float WH2 = FH / (params[14] + params[16] + params[17]) * params[14];
		float WT2 = FH / (params[14] + params[16] + params[17]) * params[16];
		float WB2 = FH / (params[14] + params[16] + params[17]) * params[17];

		cv::Mat img = generateFacadeG(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, BS, TW, AH, FH, BH, TW2, WW2, WH2, WS2, WT2, WB2);
		cv::imwrite("estimated_facade.png", img);

		params.resize(18);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = BS / 100;
		params[8] = TW / 100;
		params[9] = AH / 100;
		params[10] = FH / 100;
		params[11] = BH / 100;
		params[12] = TW2 / 100;
		params[13] = WW2 / 100;
		params[14] = WH2 / 100;
		params[15] = WS2 / 100;
		params[16] = WT2 / 100;
		params[17] = WB2 / 100;

		return params;
	}

	std::vector<float> parameterEstimation8(int width, int height, std::vector<float> params) {
		int NF = std::round(params[0] * 19 + 1);
		if (NF < 1) NF = 1;
		int NC = std::round(params[1] * 14 + 6);
		if (NC < 6) NC = 6;

		if (NC % 3 != 0) NC = std::round((float)NC / 3.0f) * 3;

		float BS = (float)width / (params[7] * 2 + params[8] * NC / 3 * 2 + params[9] * NC / 3) * params[7];
		float TW = (float)width / (params[7] * 2 + params[8] * NC / 3 * 2 + params[9] * NC / 3) * params[8];
		float TW2 = (float)width / (params[7] * 2 + params[8] * NC / 3 * 2 + params[9] * NC / 3) * params[9];
		float AH = (float)height / (params[10] + params[11] * NF + params[12]) * params[10];
		float FH = (float)height / (params[10] + params[11] * NF + params[12]) * params[11];
		float BH = (float)height / (params[10] + params[11] * NF + params[12]) * params[12];

		float WW = TW / (params[2] + params[4] * 2) * params[2];
		float WH = FH / (params[3] + params[5] + params[6]) * params[3];
		float WS = TW / (params[2] + params[4] * 2) * params[4];
		float WT = FH / (params[3] + params[5] + params[6]) * params[5];
		float WB = FH / (params[3] + params[5] + params[6]) * params[6];

		float WW2 = TW2 / (params[14] + params[13] * 2) * params[14];
		float WS2 = TW2 / (params[14] + params[13] * 2) * params[13];
		float WH2 = FH / (params[15] + params[16] + params[17]) * params[16];
		float WT2 = FH / (params[15] + params[16] + params[17]) * params[15];
		float WB2 = FH / (params[15] + params[16] + params[17]) * params[17];

		cv::Mat img = generateFacadeH(100, NF, NC, width, height, 3, WW, WH, WS, WT, WB, BS, TW, TW2, AH, FH, BH, WS2, WW2, WT2, WH2, WB2);
		cv::imwrite("estimated_facade.png", img);

		params.resize(18);
		params[0] = NF / 100;
		params[1] = NC / 100;
		params[2] = WW / 100;
		params[3] = WH / 100;
		params[4] = WS / 100;
		params[5] = WT / 100;
		params[6] = WB / 100;
		params[7] = BS / 100;
		params[8] = TW / 100;
		params[9] = TW2 / 100;
		params[10] = AH / 100;
		params[11] = FH / 100;
		params[12] = BH / 100;
		params[13] = WS2 / 100;
		params[14] = WW2 / 100;
		params[15] = WT2 / 100;
		params[16] = WH2 / 100;
		params[17] = WB2 / 100;

		return params;
	}

}
