#include "FacadeSegmentation.h"
#include "CVUtils.h"
#include "Utils.h"
#include <fstream>
//#include "EdgeDetection.h"

namespace fs {

	void subdivideFacade(const cv::Mat& img, bool align_windows, std::vector<float>& y_split, std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& win_rects) {
		// compute Ver(y) and Hor(x)
		cv::Mat_<float> Ver;
		cv::Mat_<float> Hor;
		computeVerAndHor2(img, Ver, Hor);

		// smoothing
		cv::blur(Ver, Ver, cv::Size(11, 11));
		cv::blur(Hor, Hor, cv::Size(11, 11));

		// Facadeのsplit linesを求める
		getSplitLines(Ver, 3, y_split);
		//refineSplitLines(y_split);
		distributeSplitLines(y_split);
		getSplitLines(Hor, 3, x_split);
		refineSplitLines(x_split);

		// convert to grayscale
		cv::Mat gray_img;
		cvutils::grayScale(img, gray_img);

		// detect edges
		cv::Mat edge_img;
		//cv::Canny(img, edge_img, 30, 100, 3);
		cv::Canny(gray_img, edge_img, 50, 120, 3);

		// facadeの端のエッジを削除する
		int margin = 10;
		for (int r = 0; r < edge_img.rows; ++r) {
			for (int c = 0; c < edge_img.cols; ++c) {
				if (r < margin || r >= edge_img.rows - margin || c < margin || c >= edge_img.cols - margin) {
					edge_img.at<unsigned char>(r, c) = 0;

				}
			}
		}

		// 各tileの窓の位置を求める
		int window_count = 0;
		win_rects.resize(y_split.size() - 1);
		for (int i = 0; i < y_split.size() - 1; ++i) {
			win_rects[i].resize(x_split.size() - 1);
			for (int j = 0; j < x_split.size() - 1; ++j) {
				cv::Mat tile(gray_img, cv::Rect(x_split[j], y_split[i], x_split[j + 1] - x_split[j], y_split[i + 1] - y_split[i]));
				cv::Mat tile_edges(edge_img, cv::Rect(x_split[j], y_split[i], x_split[j + 1] - x_split[j], y_split[i + 1] - y_split[i]));

				if (subdivideTile(tile, tile_edges, 10, 3, win_rects[i][j])) {
					window_count++;
				}

			}
		}
		//std::cout << "Window count: " << window_count << std::endl;

		// 窓の位置をalignする
		if (align_windows) {
			//refine(y_split, x_split, win_rects, 0.3);
			align(edge_img, y_split, x_split, win_rects, 20);
		}
	}

	/**
	* 2つの領域の類似度を返却する。
	*
	* @param R1		領域1 (1-channel image)
	* @param R2		領域2 (1-channel image)
	* @return			類似度
	*/
	float MI(const cv::Mat& R1, const cv::Mat& R2) {
		cv::Mat norm_R1;
		cv::Mat norm_R2;

		return expf(-cvutils::msd(R1, R2) * 0.001f);
	}

	/**
	* Facade画像のS_max(y)、h_max(y)を計算する。
	*
	* @param img		Facade画像 (1-channel image)
	* @param SV_max		S_max(y)
	* @param h_max		h_max(y)
	* @param h_range	range of h
	*/
	void computeSV(const cv::Mat& img, cv::Mat_<float>& SV_max, cv::Mat_<float>& h_max, const std::pair<int, int>& h_range) {
		SV_max = cv::Mat_<float>(img.rows, 1, 0.0f);
		h_max = cv::Mat_<float>(img.rows, 1, 0.0f);

		/*
		std::ifstream in_SV_max("SV_max.txt");
		std::ifstream in_h("h_max.txt");
		if (in_SV_max.good() && in_h.good()) {
			in_SV_max.close();
			in_h.close();
			SV_max = cvutils::read("SV_max.txt");
			h_max = cvutils::read("h_max.txt");
		}
		else {
		*/
			printf("computing");
			for (int r = 0; r < img.rows; ++r) {
				printf("\rcomputing r = %d/%d  ", r, img.rows);

				cv::Mat_<float> SV(img.rows, 1, 0.0f);

				for (int h = h_range.first; h <= h_range.second; ++h) {
					if (r - h < 0 || r + h >= img.rows) continue;

					cv::Mat R1 = img(cv::Rect(0, r, img.cols, h));
					cv::Mat R2 = img(cv::Rect(0, r - h, img.cols, h));
					SV(h, 0) = MI(R1, R2);

					if (SV(h, 0) > SV_max(r, 0)) {
						SV_max(r, 0) = SV(h, 0);
						h_max(r, 0) = h;
					}
				}
			}
			printf("\n");

		/*
			// output SV_max(x) and h_max(x)
			cvutils::write("SV_max.txt", SV_max);
			cvutils::write("h_max.txt", h_max);
		}
		*/
	}

	/**
	* Facade画像のS_max(x)、w_max(x)を計算する。
	*
	* @param img		Facade画像 (1-channel image)
	* @param SH_max	S_max(x)
	* @param w_max		w_max(x)
	* @param w_range	range of w
	*/
	void computeSH(const cv::Mat& img, cv::Mat_<float>& SH_max, cv::Mat_<float>& w_max, const std::pair<int, int>& w_range) {
		SH_max = cv::Mat_<float>(1, img.cols, 0.0f);
		w_max = cv::Mat_<float>(1, img.cols, 0.0f);

		/*
		std::ifstream in_SH("SH_max.txt");
		std::ifstream in_w("w_max.txt");
		if (in_SH.good() && in_w.good()) {
			in_SH.close();
			in_w.close();
			SH_max = cvutils::read("SH_max.txt");
			w_max = cvutils::read("w_max.txt");
		}
		else {
		*/
			printf("computing");
			for (int c = 0; c < img.cols; ++c) {
				printf("\rcomputing c = %d/%d  ", c, img.cols);

				cv::Mat_<float> SH(1, img.cols, 0.0f);

				for (int w = w_range.first; w <= w_range.second; ++w) {
					if (c - w < 0 || c + w >= img.cols) continue;

					cv::Mat R1 = img(cv::Rect(c, 0, w, img.rows));
					cv::Mat R2 = img(cv::Rect(c - w, 0, w, img.rows));

					SH(0, w) = MI(R1, R2);

					if (SH(0, w) > SH_max(0, c)) {
						SH_max(0, c) = SH(0, w);
						w_max(0, c) = w;
					}
				}
			}
			printf("\n");

		/*
			// output SH_max(x) and w_max(x)
			cvutils::write("SH_max.txt", SH_max);
			cvutils::write("w_max.txt", w_max);
		}
		*/
	}

	/**
	* imgから、Ver(y)とHor(x)を計算する。
	*
	* @param img		image (3-channel color image)
	* @param Ver		Ver(y)
	* @param Hor		Hor(x)
	*/
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor) {
		// convert the image to grayscale
		cv::Mat grayImg;
		cvutils::grayScale(img, grayImg);

		// vertical split
		cv::Mat_<float> SV_max;
		cv::Mat_<float> h_max;
		computeSV(grayImg, SV_max, h_max, std::make_pair(10, 40));

		// estimate the floor height
		float floor_height = cvutils::getMostPopularValue(h_max, 3, 3);

		// horizontal split
		cv::Mat_<float> SH_max;
		cv::Mat_<float> w_max;
		computeSH(grayImg, SH_max, w_max, std::make_pair(10, 40));

		float tile_width = cvutils::getMostPopularValue(w_max, 3, 3);

		// compute Ver(y) and Hor(x)
		computeVerAndHor(img, Ver, Hor, floor_height * 0.1);
	}

	/**
	* imgから、Ver(y)とHor(x)を計算する。
	*
	* @param img		image (3-channel color image)
	* @param Ver		Ver(y)
	* @param Hor		Hor(x)
	*/
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float sigma) {
		cv::Mat grayImg;
		cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);

		float alpha = 0.9f;

		// compute hor(x,y) and ver(x,y) according to Equation (3)
		cv::Mat_<float> hor(grayImg.rows, grayImg.cols);
		cv::Mat_<float> ver(grayImg.rows, grayImg.cols);
		for (int r = 0; r < grayImg.rows; ++r) {
			for (int c = 0; c < grayImg.cols; ++c) {
				float dIdx;
				float dIdy;

				if (c == 0) {
					dIdx = grayImg.at<unsigned char>(r, c + 1) - grayImg.at<unsigned char>(r, c);
				}
				else if (c == grayImg.cols - 1) {
					dIdx = grayImg.at<unsigned char>(r, c) - grayImg.at<unsigned char>(r, c - 1);
				}
				else {
					dIdx = (grayImg.at<unsigned char>(r, c + 1) - grayImg.at<unsigned char>(r, c - 1)) / 2.0f;
				}

				if (r == 0) {
					dIdy = grayImg.at<unsigned char>(r + 1, c) - grayImg.at<unsigned char>(r, c);
				}
				else if (r == grayImg.rows - 1) {
					dIdy = grayImg.at<unsigned char>(r, c) - grayImg.at<unsigned char>(r - 1, c);
				}
				else {
					dIdy = (float)(grayImg.at<unsigned char>(r + 1, c) - grayImg.at<unsigned char>(r - 1, c)) / 2.0f;
				}

				hor(r, c) = std::max(dIdy * dIdy * (1.0f - alpha) - dIdx * dIdx * alpha, 0.0f);
				ver(r, c) = std::max(dIdx * dIdx * (1.0f - alpha) - dIdy * dIdy * alpha, 0.0f);
			}
		}

		// sum up the ver(x, y) horizontally and vertically, respectively
		cv::Mat ver_xtotal;
		cv::Mat ver_ytotal;
		cv::reduce(ver, ver_xtotal, 1, cv::REDUCE_SUM);
		cv::reduce(ver, ver_ytotal, 0, cv::REDUCE_SUM);

		// sum up the hor(x, y) horizontally and vertically, respectively
		cv::Mat hor_xtotal;
		cv::Mat hor_ytotal;
		cv::reduce(hor, hor_xtotal, 1, cv::REDUCE_SUM);
		cv::reduce(hor, hor_ytotal, 0, cv::REDUCE_SUM);

		// compute Ver(y) and Hor(x) according to Equation (4)
		Ver = cv::Mat_<float>(grayImg.rows, 1, 0.0f);
		Hor = cv::Mat_<float>(1, grayImg.cols, 0.0f);
		float beta = 0.1f;
		for (int r = 0; r < grayImg.rows; ++r) {
			for (int rr = 0; rr < grayImg.rows; ++rr) {
				Ver(r, 0) += (ver_xtotal.at<float>(rr, 0) - beta * hor_xtotal.at<float>(rr, 0)) * utils::gause(rr - r, sigma);
			}
		}
		for (int c = 0; c < grayImg.cols; ++c) {
			for (int cc = 0; cc < grayImg.cols; ++cc) {
				Hor(0, c) += (hor_ytotal.at<float>(0, cc) - beta * ver_ytotal.at<float>(0, cc)) * utils::gause(cc - c, sigma);
			}
		}
	}

	/**
	 * 俺の方式。
	 */
	void computeVerAndHor2(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor) {
		cv::Mat grayImg;
		cvutils::grayScale(img, grayImg);

		// smoothing
		cv::GaussianBlur(grayImg, grayImg, cv::Size(5, 5), 5, 5);

		// compute gradient magnitude
		cv::Mat sobelx;
		cv::Sobel(grayImg, sobelx, CV_32F, 1, 0);
		sobelx = cv::abs(sobelx);
		cv::Mat sobely;
		cv::Sobel(grayImg, sobely, CV_32F, 0, 1);
		sobely = cv::abs(sobely);

		// sum up the gradient magnitude horizontally and vertically
		cv::reduce(sobely, Hor, 0, CV_REDUCE_SUM);
		cv::reduce(sobelx, Ver, 1, CV_REDUCE_SUM);
	}

	/**
	 * tile内のwindowを検出し、その矩形座標を返却する。
	 * windowが検出されなかった場合はfalseを返却する。
	 *
	 * @param tile					タイル画像 (3-channel image)
	 * @param min_size
	 * @param horizontal_edge_max	水平分割線に対する、エッジの強さの最小値
	 * @param rect
	 * @return						分割する場合はtrue / false otherwise
	 */
	bool subdivideTile(const cv::Mat& tile, const cv::Mat& edges, int min_size, int tile_margin, WindowPos& winpos) {
		if (tile.cols < min_size || tile.rows < min_size) {
			winpos.valid = WindowPos::INVALID;
			return false;
		}

		// sum horizontally and vertically
		cv::Mat vertical_edges;
		cv::Mat horizontal_edges;
		cv::reduce(edges, vertical_edges, 0, CV_REDUCE_SUM, CV_32F);
		cv::reduce(edges, horizontal_edges, 1, CV_REDUCE_SUM, CV_32F);

		cv::Mat vertical_edges_max;
		cv::Mat horizonta_edges_max;
		cv::reduce(vertical_edges, vertical_edges_max, 1, CV_REDUCE_MAX, CV_32F);
		cv::reduce(horizontal_edges, horizonta_edges_max, 0, CV_REDUCE_MAX, CV_32F);

		float vertical_edge_threshold = tile.rows * 0.2f * 255;
		float horizontal_edge_threshold = tile.cols * 0.2f * 255;

		// find the  vertical edges (or x coordinates) that are closest to the side boundary
		int x1 = -1;
		float prev_x1;
		for (int c = tile_margin; c < vertical_edges.cols - tile_margin; ++c) {
			if (x1 == -1) {
				if (vertical_edges.at<float>(0, c) >= vertical_edge_threshold) {
					x1 = c;
					prev_x1 = vertical_edges.at<float>(0, c);
				}
			}
			else if (cvutils::isLocalMaximum(vertical_edges, c, 3)) {
				x1 = c;
				prev_x1 = vertical_edges.at<float>(0, c);
			}
			else {
				break;
			}

		}
		int x2 = -1;
		float prev_x2;
		for (int c = vertical_edges.cols - tile_margin - 1; c >= tile_margin; --c) {
			if (x2 == -1) {
				if (vertical_edges.at<float>(0, c) >= vertical_edge_threshold) {
					x2 = c;
					prev_x2 = vertical_edges.at<float>(0, c);
				}
			}
			else if (cvutils::isLocalMaximum(vertical_edges, c, 3)) {
				x2 = c;
				prev_x2 = vertical_edges.at<float>(0, c);
			}
			else {
				break;
			}
		}
		if (x1 == -1 || x2 == -1 || x2 - x1 <= 1) {
			winpos.valid = WindowPos::UNCERTAIN;
			return false;
		}

		// find the horizontqal edges (or y coordinates) that are closest to the top and bottom boundaries
		int y1 = -1;
		float prev_y1;
		for (int r = tile_margin; r < horizontal_edges.rows - tile_margin; ++r) {
			if (y1 == -1) {
				if (horizontal_edges.at<float>(r, 0) >= horizontal_edge_threshold) {
					y1 = r;
					prev_y1 = horizontal_edges.at<float>(r, 0);
				}
			}
			else if (cvutils::isLocalMaximum(horizontal_edges, r, 3)) {
				y1 = r;
				prev_y1 = horizontal_edges.at<float>(r, 0);
			}
			else {
				break;
			}
		}
		int y2 = -1;
		float prev_y2;
		for (int r = horizontal_edges.rows - tile_margin - 1; r >= tile_margin; --r) {
			if (y2 == -1) {
				if (horizontal_edges.at<float>(r, 0) >= horizontal_edge_threshold) {
					y2 = r;
					prev_y2 = horizontal_edges.at<float>(r, 0);
				}
			}
			else if (cvutils::isLocalMaximum(horizontal_edges, r, 3)) {
				y2 = r;
				prev_y2 = horizontal_edges.at<float>(r, 0);
			}
			else {
				break;
			}
		}
		if (y1 == -1 || y2 == -1 || y2 - y1 <= 1) {
			winpos.valid = WindowPos::UNCERTAIN;
			return false;
		}

		if ((float)(x2 - x1) / (y2 - y1) > 8.0f || (float)(y2 - y1) / (x2 - x1) > 8.0f) {
			winpos.valid = WindowPos::UNCERTAIN;
			return false;
		}

		winpos = WindowPos(x1, y1, tile.cols - 1 - x2, tile.rows - 1 - y2);

		return true;
	}

	/**
	* tile内のwindowを検出し、その矩形座標を返却する。
	* windowが検出されなかった場合はfalseを返却する。
	*
	* @param tile					タイル画像 (3-channel image)
	* @param min_size
	* @param horizontal_edge_max	水平分割線に対する、エッジの強さの最小値
	* @param rect
	* @return						分割する場合はtrue / false otherwise
	*/
	bool subdivideTile2(const cv::Mat& tile, cv::Mat Ver, cv::Mat Hor, int min_size, int tile_margin, WindowPos& winpos) {
		if (tile.cols < min_size || tile.rows < min_size) {
			winpos.valid = WindowPos::INVALID;
			return false;
		}

		//cv::imwrite("tile.png", tile);

		outputImageWithHorizontalAndVerticalGraph(tile, Ver, Hor, "graph.png");

		double Ver_min, Ver_max;
		cv::minMaxLoc(Ver, &Ver_min, &Ver_max);
		double Hor_min, Hor_max;
		cv::minMaxLoc(Hor, &Hor_min, &Hor_max);

		float vertical_edge_threshold = (Ver_max - Ver_min) * 0.25f + Ver_min;
		float horizontal_edge_threshold = (Hor_max - Hor_max) * 0.25f + Ver_min;


		// find the  vertical edges (or x coordinates) that are closest to the side boundary
		int x1 = -1;
		float prev_x1;
		for (int c = tile_margin; c < Hor.cols - tile_margin; ++c) {
			if (x1 == -1) {
				if (Hor.at<float>(0, c) >= horizontal_edge_threshold) {
					x1 = c;
					prev_x1 = Hor.at<float>(0, c);
				}
			}
			else if (Hor.at<float>(0, c) > prev_x1) {
				x1 = c;
				prev_x1 = Hor.at<float>(0, c);
			}
			else {
				break;
			}

		}
		int x2 = -1;
		float prev_x2;
		for (int c = Hor.cols - tile_margin - 1; c >= tile_margin; --c) {
			if (x2 == -1) {
				if (Hor.at<float>(0, c) >= horizontal_edge_threshold) {
					x2 = c;
					prev_x2 = Hor.at<float>(0, c);
				}
			}
			else if (Hor.at<float>(0, c) > prev_x2) {
				x2 = c;
				prev_x2 = Hor.at<float>(0, c);
			}
			else {
				break;
			}
		}
		if (x1 == -1 || x2 == -1 || x2 - x1 <= 1) {
			winpos.valid = WindowPos::UNCERTAIN;
			return false;
		}

		// find the horizontqal edges (or y coordinates) that are closest to the top and bottom boundaries
		int y1 = -1;
		float prev_y1;
		for (int r = tile_margin; r < Ver.rows - tile_margin; ++r) {
			if (y1 == -1) {
				if (Ver.at<float>(r, 0) >= vertical_edge_threshold) {
					y1 = r;
					prev_y1 = Ver.at<float>(r, 0);
				}
			}
			else if (Ver.at<float>(r, 0) > prev_y1) {
				y1 = r;
				prev_y1 = Ver.at<float>(r, 0);
			}
			else {
				break;
			}
		}
		int y2 = -1;
		float prev_y2;
		for (int r = Ver.rows - tile_margin - 1; r >= tile_margin; --r) {
			if (y2 == -1) {
				if (Ver.at<float>(r, 0) >= vertical_edge_threshold) {
					y2 = r;
					prev_y2 = Ver.at<float>(r, 0);
				}
			}
			else if (Ver.at<float>(r, 0) > prev_y2) {
				y2 = r;
				prev_y2 = Ver.at<float>(r, 0);
			}
			else {
				break;
			}
		}
		if (y1 == -1 || y2 == -1 || y2 - y1 <= 1) {
			winpos.valid = WindowPos::UNCERTAIN;
			return false;
		}

		if ((float)(x2 - x1) / (y2 - y1) > 8.0f || (float)(y2 - y1) / (x2 - x1) > 8.0f) {
			winpos.valid = WindowPos::UNCERTAIN;
			return false;
		}

		winpos = WindowPos(x1, y1, tile.cols - 1 - x2, tile.rows - 1 - y2);

		return true;
	}

	/**
	* Ver(y)の極小値をsplit lineの候補とし、S_max(y)に基づいて最適なsplit lineの組み合わせを探す。
	*
	* @param Ver			Ver(y)
	* @param min_interval	minimum floor height
	* @param max_interval	maximum floor height
	* @param y_split		最適なsplit lineの組み合わせ
	*/
	void findBestHorizontalSplitLines(const cv::Mat& img, const cv::Mat_<float>& Ver, float min_interval, float max_interval, std::vector<int>& y_split) {
		y_split.clear();

		std::vector<int> y_candidates = cvutils::getPeak(Ver, false, 1, cvutils::LOCAL_MINIMUM, 1);
		y_candidates.insert(y_candidates.begin(), 0);
		y_candidates.push_back(img.rows - 1);

		std::vector<std::vector<float>> costs;
		std::vector<std::vector<int>> indices;
		std::vector<std::vector<int>> nums;

		// 最初の行のコストを初期化
		{
			costs.push_back(std::vector<float>(y_candidates.size(), std::numeric_limits<float>::max()));
			indices.push_back(std::vector<int>(y_candidates.size(), -1));
			nums.push_back(std::vector<int>(y_candidates.size(), 0));
			costs[0][0] = 0;
			indices[0][0] = 0;
			nums[0][0] = 0;
		}

		// 2行目以降について、Dynamic Programmingで最小コストを計算
		for (int i = 1;; ++i) {
			costs.push_back(std::vector<float>(y_candidates.size(), std::numeric_limits<float>::max()));
			indices.push_back(std::vector<int>(y_candidates.size(), -1));
			nums.push_back(std::vector<int>(y_candidates.size(), 0));

			for (int k = 0; k < y_candidates.size() - 1; ++k) {
				bool found = false;

				for (int j = k + 1; j < y_candidates.size() - 1; ++j) {
					if (indices[i - 1][k] == -1) continue;

					if (y_candidates[j] - y_candidates[k] < min_interval) continue;
					if (found && y_candidates[j] - y_candidates[k] > max_interval) continue;

					found = true;
					float new_cost = costs[i - 1][k] + Ver(y_candidates[j], 0);
					if (new_cost / (nums[i - 1][k] + 1) < costs[i][j] / nums[i][j]) {
						costs[i][j] = new_cost;
						indices[i][j] = k;
						nums[i][j] = nums[i - 1][k] + 1;
					}
				}
			}

			for (int k = 0; k < y_candidates.size(); ++k) {
				if (indices[i - 1][k] == -1) continue;

				if (y_candidates.back() - y_candidates[k] > max_interval) continue;

				if (costs[i - 1][k] / nums[i - 1][k] < costs[i].back() / nums[i].back()) {
					costs[i].back() = costs[i - 1][k];
					indices[i].back() = k;
					nums[i].back() = nums[i - 1][k];
				}
			}

			// 最後のy以外がすべて-1なら、終了
			bool updated = false;
			for (int j = 0; j < indices[i].size() - 1; ++j) {
				if (indices[i][j] != -1) updated = true;
			}
			if (!updated) break;
		}

		// y_splitに、最適解を格納する
		y_split.push_back(y_candidates.back());
		int prev_index = y_candidates.size() - 1;
		for (int i = indices.size() - 1; i >= 1; --i) {
			int index = indices[i][prev_index];
			if (index == prev_index) continue;

			y_split.insert(y_split.begin(), y_candidates[index]);
			prev_index = index;
		}
	}

	/**
	* Hor(x)の極小値をsplit lineの候補とし、S_max(x)に基づいて最適なsplit lineの組み合わせを探す。
	*
	* @param Hor			Hor(x)
	* @param min_interval	minimum tile width
	* @param max_interval	maximum tile width
	* @param x_split		最適なsplit lineの組み合わせ
	*/
	void findBestVerticalSplitLines(const cv::Mat& img, const cv::Mat_<float>& Hor, float min_interval, float max_interval, std::vector<int>& x_split) {
		x_split.clear();

		std::vector<int> x_candidates = cvutils::getPeak(Hor, false, 1, cvutils::LOCAL_MINIMUM, 1);
		x_candidates.insert(x_candidates.begin(), 0);
		x_candidates.push_back(img.cols - 1);

		std::vector<std::vector<float>> costs;
		std::vector<std::vector<int>> indices;
		std::vector<std::vector<int>> nums;

		// 最初の列のコストを初期化
		{
			costs.push_back(std::vector<float>(x_candidates.size(), std::numeric_limits<float>::max()));
			indices.push_back(std::vector<int>(x_candidates.size(), -1));
			nums.push_back(std::vector<int>(x_candidates.size(), 0));
			costs[0][0] = 0;
			indices[0][0] = 0;
			nums[0][0] = 0;
		}

		// 2列目以降について、Dynamic Programmingで最小コストを計算
		for (int i = 1;; ++i) {
			costs.push_back(std::vector<float>(x_candidates.size(), std::numeric_limits<float>::max()));
			indices.push_back(std::vector<int>(x_candidates.size(), -1));
			nums.push_back(std::vector<int>(x_candidates.size(), 0));

			for (int k = 0; k < x_candidates.size() - 1; ++k) {
				bool found = false;

				for (int j = k + 1; j < x_candidates.size() - 1; ++j) {
					if (indices[i - 1][k] == -1) continue;

					if (x_candidates[j] - x_candidates[k] < min_interval) continue;
					if (found && x_candidates[j] - x_candidates[k] > max_interval) continue;

					found = true;
					float new_cost = costs[i - 1][k] + Hor(0, x_candidates[j]);
					if (new_cost / (nums[i - 1][k] + 1) < costs[i][j] / nums[i][j]) {
						costs[i][j] = new_cost;
						indices[i][j] = k;
						nums[i][j] = nums[i - 1][k] + 1;
					}
				}
			}

			for (int k = 0; k < x_candidates.size(); ++k) {
				if (indices[i - 1][k] == -1) continue;

				if (x_candidates.back() - x_candidates[k] > max_interval) continue;

				if (costs[i - 1][k] / nums[i - 1][k] < costs[i].back() / nums[i].back()) {
					costs[i].back() = costs[i - 1][k];
					indices[i].back() = k;
					nums[i].back() = nums[i - 1][k];
				}
			}

			// 最後のx以外がすべて-1なら、終了
			bool updated = false;
			for (int j = 0; j < indices[i].size() - 1; ++j) {
				if (indices[i][j] != -1) updated = true;
			}
			if (!updated) break;
		}

		// x_splitに、最適解を格納する
		x_split.push_back(x_candidates.back());
		int prev_index = x_candidates.size() - 1;
		for (int i = indices.size() - 1; i >= 1; --i) {
			int index = indices[i][prev_index];
			if (index == prev_index) continue;

			x_split.insert(x_split.begin(), x_candidates[index]);
			prev_index = index;
		}
	}

	/**
	* 与えられた関数の極小値を使ってsplit lineを決定する。
	*/
	void getSplitLines(const cv::Mat_<float>& val, int size, std::vector<float>& split_positions) {
		cv::Mat_<float> mat = val.clone();
		if (mat.rows == 1) {
			mat = mat.t();
		}

		for (int r = 0; r < mat.rows; ++r) {
			if (cvutils::isLocalMinimum(mat, r, size)) {
				split_positions.push_back(r);
			}
		}

		// 両端処理
		if (split_positions.size() == 0) {
			split_positions.insert(split_positions.begin(), 0);
		}
		else if (split_positions[0] > 0) {
			if (split_positions[0] < 5) {
				split_positions[0] = 0;
			}
			else {
				split_positions.insert(split_positions.begin(), 0);
			}
		}

		if (split_positions.back() < mat.rows) {
			if (split_positions.back() >= mat.rows - 5) {
				split_positions.back() = mat.rows;
			}
			else {
				split_positions.push_back(mat.rows);
			}
		}

		////////////////////////////////////////////////////////////
		// 一旦、極小値を取得した後、隣接する極大値との差のmedianを計算し、
		// medianより極端に小さい場合は、その極小値を削除する。
		bool updated = true;
		while (updated) {
			updated = false;

			std::vector<float> tmp_positions = split_positions;
			split_positions.clear();
			std::vector<float> diffs;
			for (int i = 1; i < tmp_positions.size() - 1; ++i) {
				int tmp = 0;
				float diff = cvutils::findNextMax(mat, tmp_positions[i], tmp) - mat.at<float>(tmp_positions[i], 0);
				diffs.push_back(diff);
			}

			float diff_median = utils::median(diffs);

			for (int i = 0; i < tmp_positions.size(); ++i) {
				if (i == 0) {
					split_positions.push_back(tmp_positions[i]);
				}
				else if (i == tmp_positions.size() - 1) {
					split_positions.push_back(tmp_positions[i]);
				}
				else {
					if (diffs[i - 1] >= diff_median * 0.5) {
						split_positions.push_back(tmp_positions[i]);
					}
					else {
						updated = true;
					}
				}
			}
		}
	}

	void refineSplitLines(std::vector<float>& split_positions) {
		// 間隔が狭すぎる場合は、分割して隣接領域にマージする
		while (true) {
			// 領域の幅を計算する
			cv::Mat intervals(split_positions.size() - 1, 1, CV_32F);
			for (int i = 0; i < split_positions.size() - 1; ++i) {
				intervals.at<float>(i, 0) = split_positions[i + 1] - split_positions[i];
			}
			float avg_interval = cvutils::getMostPopularValue(intervals, 3, 3);

			bool updated = false;
			for (int i = 0; i < split_positions.size() - 1;) {
				if (split_positions[i + 1] - split_positions[i] < avg_interval * 0.5) {
					if (i == 0) {
						split_positions.erase(split_positions.begin() + 1);
					}
					else if (i == split_positions.size() - 2) {
						split_positions.erase(split_positions.begin() + split_positions.size() - 2);
					}
					else {
						split_positions[i] = (split_positions[i + 1] + split_positions[i]) * 0.5;
						split_positions.erase(split_positions.begin() + i + 1);
					}
					updated = true;
				}
				else {
					i++;
				}
			}

			if (!updated) break;
		}
	}

	/**
	 * 分割線の一部を削除し、分割線の間隔が等間隔に近くなるようにする。
	 */
	void distributeSplitLines(std::vector<float>& split_positions) {
		std::vector<int> flags(split_positions.size() - 2, 0);

		float min_stddev = std::numeric_limits<float>::max();
		std::vector<int> min_flags;

		while (true) {
			// count 1s
			int cnt_ones = 0;
			for (int i = 0; i < flags.size(); ++i) {
				if (flags[i] == 1) cnt_ones++;
			}

			// valid only if 1s are more than half
			if ((float)(cnt_ones + 2) / split_positions.size() > 0.5) {
				// compute the distances between split lines
				std::vector<float> intervals;
				float prev_pos = split_positions[0];
				for (int i = 1; i < split_positions.size(); ++i) {
					if (i < split_positions.size() - 1 && flags[i - 1] == 0) continue;

					intervals.push_back(split_positions[i] - prev_pos);
					prev_pos = split_positions[i];
				}

				// compute the stddev of intervals
				float stddev = utils::stddev(intervals);

				// update the minimum stddev
				if (stddev < min_stddev) {
					min_stddev = stddev;
					min_flags = flags;
				}
			}

			// next permutation
			bool carried = false;
			for (int i = 0; i < flags.size(); ++i) {
				if (flags[i] == 1) {
					flags[i] = 0;
				}
				else if (flags[i] == 0) {
					flags[i] = 1;
					carried = true;
					break;
				}
			}
			if (!carried) break;
		}

		std::vector<float> tmp = split_positions;
		split_positions.clear();
		for (int i = 0; i < tmp.size(); ++i) {
			if (i == 0 || i == tmp.size() - 1) {
				split_positions.push_back(tmp[i]);
			}
			else if (min_flags[i - 1] == 1) {
				split_positions.push_back(tmp[i]);
			}
		}
	}

	void refine(std::vector<float>& y_split, std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& winpos, float threshold) {
		// 各フロアの窓の数をカウントする
		std::vector<int> win_per_row(y_split.size() - 1, 0);
		int max_win_per_row = 0;
		for (int i = 0; i < y_split.size() - 1; ++i) {
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					win_per_row[i]++;
				}
			}
			if (win_per_row[i] > max_win_per_row) {
				max_win_per_row = win_per_row[i];
			}
		}

		// 各カラムの窓の数をカウントする
		std::vector<int> win_per_col(x_split.size() - 1, 0);
		int max_win_per_col = 0;
		for (int j = 0; j < x_split.size() - 1; ++j) {
			for (int i = 0; i < y_split.size() - 1; ++i) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					win_per_col[j]++;
				}
			}
			if (win_per_col[j] > max_win_per_col) {
				max_win_per_col = win_per_col[j];
			}
		}

		// 壁のフロアかどうかチェックする
		std::vector<bool> is_wall_row(y_split.size() - 1, false);
		for (int i = 0; i < y_split.size() - 1; ++i) {
			if (win_per_row[i] < max_win_per_row * threshold) {
				is_wall_row[i] = true;

				for (int j = 0; j < x_split.size() - 1; ++j) {
					winpos[i][j].valid = WindowPos::INVALID;
				}
			}
		}

		// 壁のカラムかどうかチェックする
		std::vector<bool> is_wall_col(x_split.size() - 1, false);
		for (int j = 0; j < x_split.size() - 1; ++j) {
			if (win_per_col[j] < max_win_per_col * threshold) {
				is_wall_col[j] = true;

				for (int i = 0; i < y_split.size() - 1; ++i) {
					winpos[i][j].valid = WindowPos::INVALID;
				}
			}
		}

		// 窓のないフロアが連続している場合は、連結する
		for (int i = 0; i < is_wall_row.size() - 1;) {
			if (is_wall_row[i] && is_wall_row[i + 1]) {
				is_wall_row.erase(is_wall_row.begin() + i + 1);
				y_split.erase(y_split.begin() + i + 1);
				winpos.erase(winpos.begin() + i + 1);
			}
			else {
				i++;
			}
		}

		// 窓のないカラムが連続している場合は、連結する
		for (int j = 0; j < is_wall_col.size() - 1;) {
			if (is_wall_col[j] && is_wall_col[j + 1]) {
				is_wall_col.erase(is_wall_col.begin() + j + 1);
				x_split.erase(x_split.begin() + j + 1);
				for (int i = 0; i < y_split.size() - 1; ++i) {
					winpos[i].erase(winpos[i].begin() + j + 1);
				}
			}
			else {
				j++;
			}
		}
	}

	void align(const cv::Mat& edge_img, const std::vector<float>& y_split, const std::vector<float>& x_split, std::vector<std::vector<WindowPos>> &winpos, int max_iter) {
#if 0
		for (int iter = 0; iter < max_iter; ++iter) {
			// 各カラムについて、窓のx座標をそろえる
			for (int j = 0; j < x_split.size() - 1; ++j) {
				for (int i = 0; i < y_split.size() - 1; ++i) {
					if (winpos[i][j].valid == WindowPos::INVALID) continue;

					float E0 = computeEnergy(winpos, j, -1, y_split, x_split, edge_img);

					// 上の階の窓に揃える
					float E1 = std::numeric_limits<float>::max();
					std::vector<std::vector<WindowPos>> winpos1 = winpos;
					if (i > 0 && winpos[i - 1][j].valid == WindowPos::VALID) {
						if (winpos1[i][j].valid == WindowPos::UNCERTAIN) {
							winpos1[i][j].valid = WindowPos::VALID;

							// 可能であれば、y座標をとなりの窓からコピーする
							if (j > 0 && winpos1[i][j - 1].valid == WindowPos::VALID) {
								winpos1[i][j].top = winpos1[i][j - 1].top;
								winpos1[i][j].bottom = winpos1[i][j - 1].bottom;
							}
							else if (j < winpos1[i].size() - 1 && winpos1[i][j + 1].valid == WindowPos::VALID) {
								winpos1[i][j].top = winpos1[i][j + 1].top;
								winpos1[i][j].bottom = winpos1[i][j + 1].bottom;
							}
						}
						winpos1[i][j].left = winpos1[i - 1][j].left;
						winpos1[i][j].right = winpos1[i - 1][j].right;
						E1 = computeEnergy(winpos1, j, -1, y_split, x_split, edge_img);
					}

					// 下の階の窓に揃える
					float E2 = std::numeric_limits<float>::max();
					std::vector<std::vector<WindowPos>> winpos2 = winpos;
					if (i < y_split.size() - 2 && winpos[i + 1][j].valid == WindowPos::VALID) {
						if (winpos2[i][j].valid == WindowPos::UNCERTAIN) {
							winpos2[i][j].valid = WindowPos::VALID;

							// 可能であれば、y座標をとなりの窓からコピーする
							if (j > 0 && winpos2[i][j - 1].valid == WindowPos::VALID) {
								winpos2[i][j].top = winpos2[i][j - 1].top;
								winpos2[i][j].bottom = winpos2[i][j - 1].bottom;
							}
							else if (j < winpos2[i].size() - 1 && winpos2[i][j + 1].valid == WindowPos::VALID) {
								winpos2[i][j].top = winpos2[i][j + 1].top;
								winpos2[i][j].bottom = winpos2[i][j + 1].bottom;
							}
						}
						winpos2[i][j].left = winpos2[i + 1][j].left;
						winpos2[i][j].right = winpos2[i + 1][j].right;
						float E2 = computeEnergy(winpos2, j, -1, y_split, x_split, edge_img);
					}

					// 窓を削除する
					std::vector<std::vector<WindowPos>> winpos3 = winpos;
					winpos3[i][j].valid = WindowPos::UNCERTAIN;
					float E3 = computeEnergy(winpos3, j, -1, y_split, x_split, edge_img);

					// ステートを更新する
					if (E1 < E0 && E1 < E2) {
						winpos = winpos1;
					}
					else if (E2 < E0 && E2 < E1) {
						winpos = winpos2;
					}
				}
			}

			// 各フロアについて、窓のy座標をそろえる
			for (int i = 0; i < y_split.size() - 1; ++i) {
				for (int j = 0; j < x_split.size() - 1; ++j) {
					if (winpos[i][j].valid == WindowPos::INVALID) continue;

					float E0 = computeEnergy(winpos, -1, i, y_split, x_split, edge_img);

					// 左の窓に揃える
					float E1 = std::numeric_limits<float>::max();
					std::vector<std::vector<WindowPos>> winpos1 = winpos;
					if (j > 0 && winpos[i][j - 1].valid == WindowPos::VALID) {
						if (winpos1[i][j].valid == WindowPos::UNCERTAIN) {
							winpos1[i][j].valid = WindowPos::VALID;

							// 可能であれば、x座標を上下の窓からコピーする
							if (i > 0 && winpos1[i - 1][j].valid == WindowPos::VALID) {
								winpos1[i][j].left = winpos1[i - 1][j].left;
								winpos1[i][j].right = winpos1[i - 1][j].right;
							}
							else if (i < winpos1.size() - 1 && winpos1[i + 1][j].valid == WindowPos::VALID) {
								winpos1[i][j].left = winpos1[i + 1][j].left;
								winpos1[i][j].right = winpos1[i + 1][j].right;
							}
						}
						winpos1[i][j].top = winpos1[i][j - 1].top;
						winpos1[i][j].bottom = winpos1[i][j - 1].bottom;
						E1 = computeEnergy(winpos1, -1, i, y_split, x_split, edge_img);
					}

					// 右の窓に揃える
					float E2 = std::numeric_limits<float>::max();
					std::vector<std::vector<WindowPos>> winpos2 = winpos;
					if (j < x_split.size() - 2 && winpos[i][j + 1].valid == WindowPos::VALID) {
						if (winpos2[i][j].valid == WindowPos::UNCERTAIN) {
							winpos2[i][j].valid = WindowPos::VALID;

							// 可能であれば、x座標を上下の窓からコピーする
							if (i > 0 && winpos2[i - 1][j].valid == WindowPos::VALID) {
								winpos2[i][j].left = winpos2[i - 1][j].left;
								winpos2[i][j].right = winpos2[i - 1][j].right;
							}
							else if (i < winpos2.size() - 1 && winpos2[i + 1][j].valid == WindowPos::VALID) {
								winpos2[i][j].left = winpos2[i + 1][j].left;
								winpos2[i][j].right = winpos2[i + 1][j].right;
							}
						}
						winpos2[i][j].top = winpos2[i][j + 1].top;
						winpos2[i][j].bottom = winpos2[i][j + 1].bottom;
						float E2 = computeEnergy(winpos2, -1, i, y_split, x_split, edge_img);
					}

					// 窓を削除する。
					std::vector<std::vector<WindowPos>> winpos3 = winpos;
					winpos3[i][j].valid = WindowPos::UNCERTAIN;
					float E3 = computeEnergy(winpos3, -1, i, y_split, x_split, edge_img);

					// ステートを更新する
					if (E1 < E0 && E1 < E2 && E1 < E3) {
						winpos = winpos1;
					}
					else if (E2 < E0 && E2 < E1 && E2 < E3) {
						winpos = winpos2;
					}
					else if (E3 < E0 && E3 < E1 && E3 < E2) {
						winpos = winpos3;
					}
				}
			}
		}
#endif


#if 1
		// 窓のX座標をvoteする
		for (int j = 0; j < x_split.size() - 1; ++j) {
			int max_left, max_right;

			// voteする
			std::vector<float> histogram1(x_split[j + 1] - x_split[j], 0);
			std::vector<float> histogram2(x_split[j + 1] - x_split[j], 0);
			int count = 0;
			for (int i = 0; i < y_split.size() - 1; ++i) {
				if (!winpos[i][j].valid) continue;

				count++;
				for (int c = 0; c < histogram1.size(); ++c) {
					histogram1[c] += utils::gause(winpos[i][j].left - c, 2);
					histogram2[c] += utils::gause(winpos[i][j].right - c, 2);
				}
			}

			if (count == 0) continue;

			// max voteを探す
			float max_val1 = 0.0f;
			float max_val2 = 0.0f;
			for (int c = 0; c < histogram1.size(); ++c) {
				if (histogram1[c] > max_val1) {
					max_val1 = histogram1[c];
					max_left = c;
				}
				if (histogram2[c] > max_val2) {
					max_val2 = histogram2[c];
					max_right = c;
				}
			}

			// 全てのフロアの窓のX座標をそろえる
			for (int r = 0; r < y_split.size() - 1; ++r) {
				if (!winpos[r][j].valid) continue;

				if (r == 0 || r == y_split.size() - 1) {
					if (abs(winpos[r][j].left - max_left) < 5) {
						winpos[r][j].left = max_left;
					}
					if (abs(winpos[r][j].right - max_right) < 5) {
						winpos[r][j].right = max_right;
					}
				}
				else {
					winpos[r][j].left = max_left;
					winpos[r][j].right = max_right;
				}
			}
		}

		// 窓のY座標をvoteする
		for (int i = 0; i < y_split.size() - 1; ++i) {
			int max_top, max_bottom;

			// voteする
			std::vector<float> histogram1(y_split[i + 1] - y_split[i], 0);
			std::vector<float> histogram2(y_split[i + 1] - y_split[i], 0);
			int count = 0;
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (!winpos[i][j].valid) continue;

				count++;
				for (int r = 0; r < histogram1.size(); ++r) {
					histogram1[r] += utils::gause(winpos[i][j].top - r, 2);
					histogram2[r] += utils::gause(winpos[i][j].bottom - r, 2);
				}
			}

			if (count == 0) continue;

			// max voteを探す
			float max_val1 = 0.0f;
			float max_val2 = 0.0f;
			for (int r = 0; r < histogram1.size(); ++r) {
				if (histogram1[r] > max_val1) {
					max_val1 = histogram1[r];
					max_top = r;
				}
				if (histogram2[r] > max_val2) {
					max_val2 = histogram2[r];
					max_bottom = r;
				}
			}

			// 全てのカラムの窓のY座標をそろえる
			for (int c = 0; c < x_split.size() - 1; ++c) {
				if (!winpos[i][c].valid) continue;

				winpos[i][c].top = max_top;
				winpos[i][c].bottom = max_bottom;
			}
		}
#endif
	}

	float computeEnergy(std::vector<std::vector<WindowPos>> &winpos, int u, int v, const std::vector<float>& y_split, const std::vector<float>& x_split, const cv::Mat& edge_img) {
		float E = 0.0f;

		if (v == -1) { // energy of column u
			for (int i = 0; i < winpos.size(); ++i) {
				// get the edge image of this tile
				cv::Mat tile_edge(edge_img, cv::Rect(x_split[u], y_split[i], x_split[u + 1] - x_split[u], y_split[i + 1] - y_split[i]));

				// reduce
				cv::Mat edge_hist;
				cv::reduce(tile_edge, edge_hist, 0, CV_REDUCE_SUM, CV_32F);

				// get the max of edge hist
				cv::Mat edge_max;
				cv::reduce(edge_hist, edge_max, 1, CV_REDUCE_MAX);

				// normalize the edge hist
				edge_hist /= edge_max.at<float>(0, 0);

				// data cost
				float data_cost = 0.0f;
				if (winpos[i][u].valid == WindowPos::VALID) {
					// the gradient magnitude should be high
					data_cost += (1 - edge_hist.at<float>(0, winpos[i][u].left));
					data_cost += (1 - edge_hist.at<float>(0, edge_hist.cols - 1 - winpos[i][u].right));

					// the edge should be close to the boundary of the tile
					data_cost += (float)winpos[i][u].left / tile_edge.cols;
					data_cost += (float)winpos[i][u].right / tile_edge.cols;
				}
				else {
					data_cost += edge_max.at<float>(0, 0) * 2;
				}

				// smoothness cost
				float smooth_cost = 0.0f;
				if (i > 0) {
					if (winpos[i][u].valid == WindowPos::VALID) {
						if (winpos[i - 1][u].valid == WindowPos::VALID) {
							if (winpos[i][u].left != winpos[i - 1][u].left) smooth_cost++;
							if (winpos[i][u].right != winpos[i - 1][u].right) smooth_cost++;
						}
						else {
							smooth_cost += 2;
						}
					}
					else {
						if (winpos[i - 1][u].valid == WindowPos::VALID) {
							smooth_cost += 2;
						}
						else {
							// no cost
						}
					}
				}
				if (i < winpos.size() - 1) {
					if (winpos[i][u].valid == WindowPos::VALID) {
						if (winpos[i + 1][u].valid == WindowPos::VALID) {
							if (winpos[i][u].left != winpos[i + 1][u].left) smooth_cost++;
							if (winpos[i][u].right != winpos[i + 1][u].right) smooth_cost++;
						}
						else {
							smooth_cost += 2;
						}
					}
					else {
						if (winpos[i + 1][u].valid == WindowPos::VALID) {
							smooth_cost += 2;
						}
						else {
							// no cost
						}
					}
				}

				E += data_cost + smooth_cost;
			}
		}
		else { // energy of row v
			for (int i = 0; i < winpos[v].size(); ++i) {
				// get the edge image of this tile
				cv::Mat tile_edge(edge_img, cv::Rect(x_split[i], y_split[v], x_split[i + 1] - x_split[i], y_split[v + 1] - y_split[v]));

				// reduce
				cv::Mat edge_hist;
				cv::reduce(tile_edge, edge_hist, 1, CV_REDUCE_SUM, CV_32F);

				// get the max of edge hist
				cv::Mat edge_max;
				cv::reduce(edge_hist, edge_max, 0, CV_REDUCE_MAX);

				// normalize the edge hist
				edge_hist /= edge_max.at<float>(0, 0);

				// data cost
				float data_cost = 0.0f;
				if (winpos[v][i].valid == WindowPos::VALID) {
					// the gradient magnitude should be high
					data_cost += (1 - edge_hist.at<float>(0, winpos[v][i].top));
					data_cost += (1 - edge_hist.at<float>(0, edge_hist.rows - 1 - winpos[v][i].bottom));

					// the edge should be close to the boundary of the tile
					data_cost += (float)winpos[v][i].top / tile_edge.cols;
					data_cost += (float)winpos[v][i].bottom / tile_edge.cols;
				}
				else {
					data_cost += edge_max.at<float>(0, 0) * 2;
				}

				// smoothness cost
				float smooth_cost = 0.0f;
				if (i > 0) {
					if (winpos[v][i].valid == WindowPos::VALID) {
						if (winpos[v][i - 1].valid == WindowPos::VALID) {
							if (winpos[v][i].left != winpos[v][i - 1].left) smooth_cost++;
							if (winpos[v][i].right != winpos[v][i - 1].right) smooth_cost++;
						}
						else {
							smooth_cost += 2;
						}
					}
					else {
						if (winpos[v][i - 1].valid == WindowPos::VALID) {
							smooth_cost += 2;
						}
						else {
							// no cost
						}
					}
				}
				if (i < winpos[v].size() - 1) {
					if (winpos[v][i].valid == WindowPos::VALID) {
						if (winpos[v][i + 1].valid == WindowPos::VALID) {
							if (winpos[v][i].left != winpos[v][i + 1].left) smooth_cost++;
							if (winpos[v][i].right != winpos[v][i + 1].right) smooth_cost++;
						}
						else {
							smooth_cost += 2;
						}
					}
					else {
						if (winpos[v][i + 1].valid == WindowPos::VALID) {
							smooth_cost += 2;
						}
						else {
							// no cost
						}
					}
				}

				E += data_cost + smooth_cost;
			}
		}

		return E;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// visualization

	void outputFacadeStructure(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result = img.clone();

		for (int i = 0; i < y_split.size(); ++i) {
			if (i < y_split.size() - 1) {
				cv::line(result, cv::Point(0, y_split[i]), cv::Point(img.cols, y_split[i]), lineColor, lineWidth);
			}
			else {
				// For the last line, we need to move the line upward by 1px to make it inside the image.
				cv::line(result, cv::Point(0, y_split[i] - 1), cv::Point(img.cols, y_split[i] - 1), lineColor, lineWidth);
			}
		}
		for (int i = 0; i < x_split.size(); ++i) {
			if (i < x_split.size() - 1) {
				cv::line(result, cv::Point(x_split[i], 0), cv::Point(x_split[i], img.rows), lineColor, lineWidth);
			}
			else {
				// For the last line, we need to move the line upward by 1px to make it inside the image.
				cv::line(result, cv::Point(x_split[i] - 1, 0), cv::Point(x_split[i] - 1, img.rows), lineColor, lineWidth);
			}
		}
		cv::imwrite(filename, result);
	}

	void outputFacadeAndWindows(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result = img.clone();
		for (int i = 0; i < y_split.size(); ++i) {
			if (i < y_split.size() - 1) {
				cv::line(result, cv::Point(0, y_split[i]), cv::Point(result.cols - 1, y_split[i]), cv::Scalar(0, 0, 255), lineWidth);
			}
			else {
				cv::line(result, cv::Point(0, y_split[i] - 1), cv::Point(result.cols - 1, y_split[i] - 1), cv::Scalar(0, 0, 255), lineWidth);
			}
		}
		for (int i = 0; i < x_split.size(); ++i) {
			if (i < x_split.size() - 1) {
				cv::line(result, cv::Point(x_split[i], 0), cv::Point(x_split[i], result.rows - 1), cv::Scalar(0, 0, 255), lineWidth);
			}
			else {
				cv::line(result, cv::Point(x_split[i] - 1, 0), cv::Point(x_split[i] - 1, result.rows - 1), cv::Scalar(0, 0, 255), lineWidth);
			}
		}
		for (int i = 0; i < y_split.size() - 1; ++i) {
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					int x1 = x_split[j] + winpos[i][j].left;
					int y1 = y_split[i] + winpos[i][j].top;
					int x2 = x_split[j + 1] - 1 - winpos[i][j].right;
					int y2 = y_split[i + 1] - 1 - winpos[i][j].bottom;
					cv::rectangle(result, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1), lineColor, lineWidth);
				}
			}
		}
		cv::imwrite(filename, result);
	}

	void outputWindows(const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result(y_split.back(), x_split.back(), CV_8UC3, cv::Scalar(255, 255, 255));
		for (int i = 0; i < y_split.size() - 1; ++i) {
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					int x1 = x_split[j] + winpos[i][j].left;
					int y1 = y_split[i] + winpos[i][j].top;
					int x2 = x_split[j + 1] - 1 - winpos[i][j].right;
					int y2 = y_split[i + 1] - 1 - winpos[i][j].bottom;
					cv::rectangle(result, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1), lineColor, lineWidth);
				}
			}
		}
		cv::imwrite(filename, result);
	}

	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat& ver, const std::vector<float>& ys, const cv::Mat& hor, const std::vector<float>& xs, const std::string& filename, int lineWidth) {
		int graphSize = std::max(10.0, std::max(img.rows, img.cols) * 0.3);

		cv::Mat result;
		cv::Scalar graph_color;
		cv::Scalar peak_color;

		result = cv::Mat(img.rows + graphSize + 3, img.cols + graphSize + 3, CV_8UC3, cv::Scalar(255, 255, 255));
		graph_color = cv::Scalar(0, 0, 0);
		peak_color = cv::Scalar(0, 0, 255);

		// copy img to result
		cv::Mat color_img;
		if (img.channels() == 1) {
			cv::cvtColor(img, color_img, cv::COLOR_GRAY2BGR);
		}
		else if (img.channels() == 3) {
			color_img = img;
		}
		cv::Mat roi(result, cv::Rect(0, 0, color_img.cols, color_img.rows));
		color_img.copyTo(roi);

		// get the maximum value of Ver(y) and Hor(x)
		float max_ver = cvutils::max(ver);
		float min_ver = cvutils::min(ver);
		float max_hor = cvutils::max(hor);
		float min_hor = cvutils::min(hor);

		// draw vertical graph
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + (cvutils::get(ver, r, 0) - min_ver) / (max_ver - min_ver) * graphSize;
			int x2 = img.cols + (cvutils::get(ver, r + 1, 0) - min_ver) / (max_ver - min_ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), graph_color, 1, cv::LINE_8);
		}
		for (int i = 0; i < ys.size(); ++i) {
			cv::line(result, cv::Point(0, ys[i]), cv::Point(img.cols - 1, ys[i]), peak_color, lineWidth);
		}

		// draw horizontal graph
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + (cvutils::get(hor, 0, c) - min_hor) / (max_hor - min_hor) * graphSize;
			int y2 = img.rows + (cvutils::get(hor, 0, c + 1) - min_hor) / (max_hor - min_hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), graph_color, 1, cv::LINE_8);
		}
		for (int i = 0; i < xs.size(); ++i) {
			cv::line(result, cv::Point(xs[i], 0), cv::Point(xs[i], img.rows - 1), peak_color, lineWidth);
		}

		cv::imwrite(filename, result);
	}

	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat& ver, const cv::Mat& hor, const std::string& filename) {
		int graphSize = std::max(10.0, std::max(img.rows, img.cols) * 0.3);

		cv::Mat result;
		cv::Scalar graph_color;
		cv::Scalar peak_color;

		result = cv::Mat(img.rows + graphSize + 3, img.cols + graphSize + 3, CV_8UC3, cv::Scalar(255, 255, 255));
		graph_color = cv::Scalar(0, 0, 0);
		peak_color = cv::Scalar(0, 0, 255);

		// copy img to result
		cv::Mat color_img;
		if (img.channels() == 1) {
			cv::cvtColor(img, color_img, cv::COLOR_GRAY2BGR);
		}
		else if (img.channels() == 3) {
			color_img = img;
		}
		cv::Mat roi(result, cv::Rect(0, 0, color_img.cols, color_img.rows));
		color_img.copyTo(roi);

		// get the maximum value of Ver(y) and Hor(x)
		float max_ver = cvutils::max(ver);
		float min_ver = cvutils::min(ver);
		float max_hor = cvutils::max(hor);
		float min_hor = cvutils::min(hor);

		// draw vertical graph
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + (cvutils::get(ver, r, 0) - min_ver) / (max_ver - min_ver) * graphSize;
			int x2 = img.cols + (cvutils::get(ver, r + 1, 0) - min_ver) / (max_ver - min_ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), graph_color, 1, cv::LINE_8);
		}

		// draw horizontal graph
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + (cvutils::get(hor, 0, c) - min_hor) / (max_hor - min_hor) * graphSize;
			int y2 = img.rows + (cvutils::get(hor, 0, c + 1) - min_hor) / (max_hor - min_hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), graph_color, 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

}