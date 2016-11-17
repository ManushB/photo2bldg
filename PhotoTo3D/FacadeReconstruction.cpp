#include "FacadeReconstruction.h"
#include "Utils.h"

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
		}
		else if (grammar_id == 1) {
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
		}
		else if (grammar_id == 2) {
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
		}
		else if (grammar_id == 3) {
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
		}

		return params;
	}

}
