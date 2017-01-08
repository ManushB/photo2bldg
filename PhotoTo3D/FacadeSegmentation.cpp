#include "FacadeSegmentation.h"
#include "CVUtils.h"
#include "Utils.h"
#include <fstream>
#include <list>

namespace fs {
	int seq = 0;

	void subdivideFacade(cv::Mat img, float average_floor_height, float average_column_width, std::vector<float>& y_splits, std::vector<float>& x_splits, std::vector<std::vector<WindowPos>>& win_rects) {
		// gray scale
		cv::Mat gray_img;
		cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
		
		// compute kernel size
		int kernel_size_V = average_floor_height / 8;
		if (kernel_size_V % 2 == 0) kernel_size_V++;
		int kernel_size_H = average_column_width / 8;
		if (kernel_size_H % 2 == 0) kernel_size_H++;

		// blur the image according to the average floor height
		cv::Mat blurred_gray_img;
		if (kernel_size_V > 1) {
			cv::GaussianBlur(gray_img, blurred_gray_img, cv::Size(kernel_size_V, kernel_size_V), kernel_size_V);
		}
		else {
			blurred_gray_img = gray_img.clone();
		}

		// compute Ver and Hor
		cv::Mat_<float> Ver, Hor;
		computeVerAndHor2(blurred_gray_img, Ver, Hor, 0.0);

		// smooth Ver and Hor
		if (kernel_size_V > 1) {
			cv::blur(Ver, Ver, cv::Size(kernel_size_V, kernel_size_V));
		}
		if (kernel_size_H > 1) {
			cv::blur(Hor, Hor, cv::Size(kernel_size_H, kernel_size_H));
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////
		// subdivide vertically
		
		// find the floor boundaries
		cv::Range h_range1 = cv::Range(average_floor_height * 0.7, average_floor_height * 1.5);
		cv::Range h_range2 = cv::Range(average_floor_height * 0.5, average_floor_height * 1.95);
		y_splits = findBoundaries(blurred_gray_img, h_range1, h_range2, std::round(img.rows / average_floor_height) + 1, Ver);
		
		////////////////////////////////////////////////////////////////////////////////////////////////
		// subdivide horizontally
		
		// find the floor boundaries
		cv::Range w_range1 = cv::Range(average_column_width * 0.6, average_column_width * 1.3);
		cv::Range w_range2 = cv::Range(average_column_width * 0.3, average_column_width * 1.95);
		x_splits = findBoundaries(blurred_gray_img.t(), w_range1, w_range2, std::round(img.cols / average_column_width) + 1, Hor);
		
		extractWindows(gray_img, y_splits, x_splits, win_rects);
	}

	std::vector<float> findBoundaries(const cv::Mat& img, cv::Range range1, cv::Range range2, int num_splits, const cv::Mat_<float>& Ver) {
		std::vector<std::vector<float>> good_candidates;

		// find the local minima of Ver
		std::vector<float> y_splits_strong;
		getSplitLines(Ver, 0.5, y_splits_strong);

		// ignore the split lines that are too close to the border
		if (y_splits_strong.size() > 0 && y_splits_strong[0] < range2.start) {
			y_splits_strong.erase(y_splits_strong.begin());
		}
		if (y_splits_strong.size() > 0 && img.rows - 1 - y_splits_strong.back() < range2.start) {
			y_splits_strong.pop_back();
		}

		for (int iter = 0; iter < 2; ++iter) {
			// find the local minima of Ver
			std::vector<float> y_splits;
			getSplitLines(Ver, 0.05 / (iter + 1), y_splits);

			// check whether each split is strong or not
			std::map<float, bool> is_strong;
			for (int i = 0; i < y_splits.size(); ++i) {
				if (std::find(y_splits_strong.begin(), y_splits_strong.end(), y_splits[i]) != y_splits_strong.end()) {
					is_strong[y_splits[i]] = true;
				}
				else {
					is_strong[y_splits[i]] = false;
				}
			}

			std::list<std::vector<float>> queue;
			queue.push_back(std::vector<float>(1, 0));
			while (!queue.empty()) {
				std::vector<float> list = queue.back();
				queue.pop_back();

				if (img.rows - list.back() >= range2.start && img.rows - list.back() <= range2.end) {
					std::vector<float> new_list = list;
					new_list.push_back(img.rows - 1);

					// Only if the number of splits so far does not exceed the limit,
					// add this to the candidate list.
					if (std::abs((int)new_list.size() - num_splits) <= 1) {
						good_candidates.push_back(new_list);
					}
				}

				for (int i = 0; i < y_splits.size(); ++i) {
					if (y_splits[i] <= list.back()) continue;
					if (list.size() == 1) {
						// for the top floor, use the wider range to check
						if (y_splits[i] - list.back() < range2.start || y_splits[i] - list.back() > range2.end) continue;
					}
					else {
						// for other floors, use the narrower range to check
						if (y_splits[i] - list.back() < range1.start || y_splits[i] - list.back() > range1.end) continue;
					}

					std::vector<float> new_list = list;
					new_list.push_back(y_splits[i]);

					// Only if the number of splits so far does not exceed the limit,
					// add this to the queue.
					if ((int)new_list.size() + 1 - num_splits <= 1) {
						queue.push_back(new_list);
					}

					// If this split is a strong one, you cannot skip this, so stop here.
					if (is_strong[y_splits[i]]) {
						break;
					}
				}

			}

			// discard the candidates that have too few strong splits
			for (int i = 0; i < good_candidates.size(); ) {
				int num_strong_splits = 0;
				for (int j = 0; j < good_candidates[i].size(); ++j) {
					if (is_strong[good_candidates[i][j]]) num_strong_splits++;
				}

				if ((float)num_strong_splits < y_splits_strong.size() * 0.8) {
					good_candidates.erase(good_candidates.begin() + i);
				}
				else {
					++i;
				}
			}

			// if there is no good candidate found, increase the range and start over the process
			if (good_candidates.size() == 0) {
				continue;
			}

			// if there is only one option, select it.
			if (good_candidates.size() == 1) {
				return good_candidates[0];
			}

#if 1
			// find the best candidate
			float min_val = std::numeric_limits<float>::max();
			int best_id = -1;
			float alpha = 0.5;
			for (int i = 0; i < good_candidates.size(); ++i) {
				// compute the average Ver/Hor
				float total_Ver = 0;
				for (int k = 1; k < (int)good_candidates[i].size() - 1; ++k) {
					total_Ver += Ver(good_candidates[i][k]);
				}

				float avg_Ver = total_Ver / ((int)good_candidates[i].size() - 2);

				// compute stddev of heights
				std::vector<float> heights;
				for (int k = 0; k < (int)good_candidates[i].size() - 1; ++k) {
					int h = good_candidates[i][k + 1] - good_candidates[i][k];
					heights.push_back(h);
				}
				float stddev = utils::stddev(heights);
				if (heights.size() > 0) {
					float avg_h = utils::mean(heights);
					stddev /= avg_h;
				}

				if (avg_Ver * alpha + stddev * (1 - alpha) < min_val) {
					min_val = avg_Ver  * alpha + stddev * (1 - alpha);
					best_id = i;
				}
			}

			return good_candidates[best_id];
#endif

#if 0
			// compute S
			float min_S = std::numeric_limits<float>::max();
			int best_id = -1;
			for (int i = 0; i < good_candidates.size(); ++i) {
				float avg_S = 0;
				if (good_candidates[i].size() >= 3) {
					float total_S = 0;
					for (int j = 0; j < good_candidates[i].size() - 2; ++j) {
						int h1 = good_candidates[i][j + 1] - good_candidates[i][j];
						int h2 = good_candidates[i][j + 2] - good_candidates[i][j + 1];
						if (std::abs(h1 - h2) / h1 < 0.1) {
							cv::Mat roi1(img, cv::Rect(0, good_candidates[i][j], img.cols, h1));
							cv::Mat roi2(img, cv::Rect(0, good_candidates[i][j + 1], img.cols, h2));
							total_S += MI(roi1, roi2);
						}
					}

					avg_S = total_S / (good_candidates[i].size() - 2);
				}

				if (avg_S < min_S) {
					min_S = avg_S;
					best_id = i;
				}
			}

			return good_candidates[best_id];
#endif
		}
		
		// if there is no good splits found, use the original candidate splits
		std::cerr << "-----------------------------------" << std::endl;
		std::cerr << "No good split is found!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		std::vector<float> y_splits;
		getSplitLines(Ver, 0.1, y_splits);
		y_splits.insert(y_splits.begin(), 0);
		y_splits.push_back(img.rows - 1);

		return y_splits;
	}

	bool sortBySecondValue(const std::pair<float, float>& a, const std::pair<float, float>& b) {
		return a.second < b.second;
	}

	void sortByS(std::vector<float>& splits, std::map<int, float>& S_max) {
		std::vector<std::pair<float, float>> list;
		for (int i = 0; i < splits.size(); ++i) {
			list.push_back(std::make_pair(splits[i], S_max[splits[i]]));
		}

		std::sort(list.begin(), list.end(), sortBySecondValue);

		splits.clear();
		for (int i = list.size() - 1; i >= 0; --i) {
			splits.push_back(list[i].first);
		}
	}

	void extractWindows(cv::Mat gray_img, const std::vector<float>& y_splits, const std::vector<float>& x_splits, std::vector<std::vector<WindowPos>>& win_rects) {
		// compute gradient magnitude
		cv::Mat sobelx;
		cv::Sobel(gray_img, sobelx, CV_32F, 1, 0);
		cv::multiply(sobelx, sobelx, sobelx);

		cv::Mat sobely;
		cv::Sobel(gray_img, sobely, CV_32F, 0, 1);
		cv::multiply(sobely, sobely, sobely);

		cv::Mat_<float> grad_mag;
		cv::sqrt(sobelx + sobely, grad_mag);

		win_rects.resize(y_splits.size() - 1);
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			win_rects[i].resize(x_splits.size() - 1);
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				int x1 = x_splits[j];
				int x2 = x_splits[j + 1] - 1;
				int y1 = y_splits[i];
				int y2 = y_splits[i + 1] - 1;
				int w = x2 - x1 + 1;
				int h = y2 - y1 + 1;

				int min_w = w * 0.1;
				int min_h = h * 0.1;

				// tile image
				cv::Mat tile_img(gray_img, cv::Rect(x1, y1, w, h));

				// Update: 2016/12/07
				// use Ver/Hor of this tile instead of the global ones
				cv::Mat_<float> Ver, Hor;
				computeVerAndHor2(tile_img, Ver, Hor, 0.0);


				// compute max/min of Ver/Hor
				float top_Ver = Ver(0);
				float bottom_Ver = Ver(h - 1);
				float max_Ver = 0;
				for (int k = 0; k < h; ++k) {
					if (Ver(k) > max_Ver) {
						max_Ver = Ver(k);
					}
				}
				float left_Hor = Hor(0);
				float right_Hor = Hor(w - 1);
				float max_Hor = 0;
				for (int k = 0; k < w; ++k) {
					if (Hor(k) > max_Hor) {
						max_Hor = Hor(k);
					}
				}

				// define the threshold of Ver/Hor
				float top_threshold_Ver = (max_Ver - top_Ver) * 0.2 + top_Ver;
				float bottom_threshold_Ver = (max_Ver - bottom_Ver) * 0.2 + bottom_Ver;
				float left_threshold_Hor = (max_Hor - left_Hor) * 0.2 + left_Hor;
				float right_threshold_Hor = (max_Hor - right_Hor) * 0.2 + right_Hor;

				// defind a threshold
				float threshold1 = 50;
				float threshold2 = 150;

				// detect edge
				//cv::Mat roi(gray_img, cv::Rect(x1, y1, w, h));
				cv::Mat edges;
				cv::Canny(tile_img, edges, threshold1, threshold2);
				
				// sum up edge horizontally and vertically
				cv::Mat edgeV, edgeH;
				cv::reduce(edges, edgeV, 1, cv::REDUCE_SUM, CV_32F);
				cv::reduce(edges, edgeH, 0, cv::REDUCE_SUM, CV_32F);
				edgeH = edgeH.t();




				// find the left edge of the window
				int left = -1;
				bool flag = false;
				for (int xx = 0; xx < w; ++xx) {
					if (!flag) {
						if (Hor(xx) < left_threshold_Hor) continue;
						flag = true;
					}
					if (edgeH.at<float>(xx, 0) >= 255 * h * 0.1) {
						left = xx;
						break;
					}
				}

				// find the right edge of the window
				int right = -1;
				flag = false;
				for (int xx = w - 1; xx >= 0; --xx) {
					if (!flag) {
						if (Hor(xx) < right_threshold_Hor) continue;
						flag = true;
					}
					if (edgeH.at<float>(xx, 0) >= 255 * h * 0.1) {
						right = xx;
						break;
					}
				}

				// find the top edge of the window
				int top = -1;
				flag = false;
				for (int yy = 0; yy < h; ++yy) {
					if (!flag) {
						if (Ver(yy) < top_threshold_Ver) continue;
						flag = true;
					}
					if (edgeV.at<float>(yy, 0) >= 255 * w * 0.1) {
						top = yy;
						break;
					}
				}

				// find the bottom edge of the window
				int bottom = -1;
				flag = false;
				for (int yy = h - 1; yy >= 0; --yy) {
					if (!flag) {
						if (Ver(yy) < top_threshold_Ver) continue;
						flag = true;
					}
					if (edgeV.at<float>(yy, 0) >= 255 * w * 0.1) {
						bottom = yy;
						break;
					}
				}

				if (left >= 0 && right >= 0 && right - left + 1 >= min_w && top >= 0 && bottom >= 0 && bottom - top + 1 > min_h && (right - left + 1) / (bottom - top + 1) < 8 && (bottom - top + 1) / (right - left + 1) < 8) {
					win_rects[i][j] = WindowPos(left, top, right, bottom);
				}
				else {
					win_rects[i][j].valid = WindowPos::INVALID;
				}
			}
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
#if 1
		cv::Mat_<float> Pab(256, 256, 0.0f);
		cv::Mat_<float> Pa(256, 1, 0.0f);
		cv::Mat_<float> Pb(256, 1, 0.0f);

		// create a histogram of intensities
		for (int r = 0; r < R1.rows; ++r) {
			for (int c = 0; c < R1.cols; ++c) {
				int a = R1.at<unsigned char>(r, c);
				int b = R2.at<unsigned char>(r, c);

				Pab(a, b)++;
				Pa(a, 0)++;
				Pb(b, 0)++;
			}
		}

		// normalize the historgram
		cv::Mat Pab_max;
		cv::Mat Pa_max;
		cv::Mat Pb_max;
		cv::reduce(Pab, Pab_max, 0, CV_REDUCE_SUM);
		cv::reduce(Pab_max, Pab_max, 1, CV_REDUCE_SUM);
		cv::reduce(Pa, Pa_max, 0, CV_REDUCE_SUM);
		cv::reduce(Pb, Pb_max, 0, CV_REDUCE_SUM);
		Pab /= Pab_max.at<float>(0, 0);
		Pa /= Pa_max.at<float>(0, 0);
		Pb /= Pb_max.at<float>(0, 0);

		float result = 0.0f;
		for (int a = 0; a < 256; ++a) {
			for (int b = 0; b < 256; ++b) {
				float v = Pab(a, b);
				if (v == 0) {
					v = 0.001f * 0.001f;
				}
				float v1 = Pa(a, 0);
				if (v1 == 0) v1 = 0.001f;
				float v2 = Pb(b, 0);
				if (v2 == 0) v2 = 0.001f;

				float hoge = Pab(a, b) * log(v / v1 / v2);
				result += Pab(a, b) * log(v / v1 / v2);
			}
		}

		return result;
#endif
#if 0
		cv::Mat norm_R1;
		cv::Mat norm_R2;

		return expf(-cvutils::msd(R1, R2) * 0.001f);
#endif
	}

	/**
	* Facade画像のS_max(y)、h_max(y)を計算する。
	*
	* @param img		Facade画像 (1-channel image)
	* @param SV_max		S_max(y)
	* @param h_max		h_max(y)
	* @param h_range	range of h
	*/
	void computeSV(const cv::Mat& img, cv::Mat_<float>& SV_max, cv::Mat_<int>& h_max, const cv::Range& h_range) {
		SV_max = cv::Mat_<float>(img.rows, 1, 0.0f);
		h_max = cv::Mat_<float>(img.rows, 1, 0.0f);

		printf("computing");
		for (int r = 0; r < img.rows; ++r) {
			printf("\rcomputing r = %d/%d  ", r, img.rows);

			computeSV(img, r, SV_max(r), h_max(r), h_range);
		}
		printf("\n");
	}

	void computeSV(const cv::Mat& img, int r, float& SV_max, int& h_max, const cv::Range& h_range) {
		SV_max = 0;
		h_max = 0;

		for (int h = h_range.start; h <= h_range.end; ++h) {
			if (r - h < 0 || r + h >= img.rows) continue;

			cv::Mat R1 = img(cv::Rect(0, r, img.cols, h));
			cv::Mat R2 = img(cv::Rect(0, r - h, img.cols, h));
			float SV = MI(R1, R2);

			if (SV > SV_max) {
				SV_max = SV;
				h_max = h;
			}
		}
	}

	/**
	* Facade画像のS_max(x)、w_max(x)を計算する。
	*
	* @param img		Facade画像 (1-channel image)
	* @param SH_max	S_max(x)
	* @param w_max		w_max(x)
	* @param w_range	range of w
	*/
	void computeSH(const cv::Mat& img, cv::Mat_<float>& SH_max, cv::Mat_<int>& w_max, const cv::Range& w_range) {
		SH_max = cv::Mat_<float>(img.cols, 1, 0.0f);
		w_max = cv::Mat_<float>(img.cols, 1, 0.0f);

		printf("computing");
		for (int c = 0; c < img.cols; ++c) {
			printf("\rcomputing c = %d/%d  ", c, img.cols);

			computeSH(img, c, SH_max(c), w_max(c), w_range);
		}
		printf("\n");
	}

	void computeSH(const cv::Mat& img, int c, float& SH_max, int& w_max, const cv::Range& w_range) {
		SH_max = 0;
		w_max = 0;

		for (int w = w_range.start; w <= w_range.end; ++w) {
			if (c - w < 0 || c + w >= img.cols) continue;

			cv::Mat R1 = img(cv::Rect(c, 0, w, img.rows));
			cv::Mat R2 = img(cv::Rect(c - w, 0, w, img.rows));
			float SH = MI(R1, R2);

			if (SH > SH_max) {
				SH_max = SH;
				w_max = w;
			}
		}
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
	void computeVerAndHor2(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float alpha) {
		cv::Mat grayImg;
		cvutils::grayScale(img, grayImg);

		// smoothing
		//cv::GaussianBlur(grayImg, grayImg, cv::Size(5, 5), 5, 5);

		// compute gradient magnitude
		cv::Mat sobelx;
		cv::Sobel(grayImg, sobelx, CV_32F, 1, 0);
		sobelx = cv::abs(sobelx);
		cv::Mat sobely;
		cv::Sobel(grayImg, sobely, CV_32F, 0, 1);
		sobely = cv::abs(sobely);

		// sum up the gradient magnitude horizontally and vertically
		cv::Mat sobelx_hor, sobelx_ver;
		cv::reduce(sobelx, sobelx_hor, 0, CV_REDUCE_SUM);
		cv::reduce(sobelx, sobelx_ver, 1, CV_REDUCE_SUM);
		cv::Mat sobely_hor, sobely_ver;
		cv::reduce(sobely, sobely_hor, 0, CV_REDUCE_SUM);
		cv::reduce(sobely, sobely_ver, 1, CV_REDUCE_SUM);

		// compute Ver and Hor
		Ver = sobelx_ver - sobely_ver * alpha;
		Hor = sobely_hor - sobelx_hor * alpha;

		// compute min/max
		double min_Ver, max_Ver;
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		double min_Hor, max_Hor;
		cv::minMaxLoc(Ver, &min_Hor, &max_Hor);

		// normalize Ver and Hor
		Ver = (Ver - min_Ver) / (max_Ver - min_Ver);
		Hor = (Hor - min_Hor) / (max_Hor - min_Hor);

		Hor = Hor.t();
	}

	/**
	* 与えられた関数の極小値を使ってsplit lineを決定する。
	*/
	void getSplitLines(const cv::Mat_<float>& val, float threshold, std::vector<float>& split_positions) {
		cv::Mat_<float> mat = val.clone();
		if (mat.rows == 1) {
			mat = mat.t();
		}

		double max_value, min_value;
		cv::minMaxLoc(mat, &min_value, &max_value);
		threshold *= (max_value - min_value);

		for (int r = 0; r < mat.rows; ++r) {
			if (isLocalMinimum(mat, r, threshold)) {
				split_positions.push_back(r);
			}
		}

		// remove the consecutive ones
		for (int i = 0; i < (int)split_positions.size() - 1;) {
			if (split_positions[i + 1] == split_positions[i] + 1) {
				split_positions.erase(split_positions.begin() + i + 1);
			}
			else {
				++i;
			}
		}

		// remove the both end
		if (split_positions.size() > 0) {
			if (split_positions[0] <= 1) {
				split_positions.erase(split_positions.begin());
			}
			if (split_positions.back() >= mat.rows - 2) {
				split_positions.pop_back();
			}
		}

		/*
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

		if (split_positions.back() < mat.rows - 1) {
			if (split_positions.back() >= mat.rows - 5) {
				split_positions.back() = mat.rows - 1;
			}
			else {
				split_positions.push_back(mat.rows - 1);
			}
		}
		*/
	}

	// 間隔が狭すぎる場合は、分割して隣接領域にマージする
	void refineSplitLines(std::vector<float>& split_positions, float threshold) {
		// 最大の間隔を計算する（ただし、１階は除く）
		float max_interval = 0;
		for (int i = 0; i < split_positions.size() - 2; ++i) {
			float interval = split_positions[i + 1] - split_positions[i];
			if (interval > max_interval) max_interval = interval;
		}

		while (true) {
			bool updated = false;
			for (int i = 0; i < split_positions.size() - 1;) {
				if (split_positions[i + 1] - split_positions[i] < max_interval * threshold) {
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
	void distributeSplitLines(std::vector<float>& split_positions, float threshold) {
		std::vector<int> flags(split_positions.size() - 2, 0);

		float min_stddev = std::numeric_limits<float>::max();
		std::vector<int> min_flags;

		while (true) {
			// count 1s
			int cnt_ones = 0;
			for (int i = 0; i < flags.size(); ++i) {
				if (flags[i] == 1) cnt_ones++;
			}

			// valid only if 1s are more than 50%
			if ((float)(cnt_ones + 2) / split_positions.size() > threshold) {
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

	cv::Scalar getDominantColor(const cv::Mat& img, std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, int clusterCount) {
		cv::Mat lab_img;
		cv::cvtColor(img, lab_img, cv::COLOR_BGR2Lab);

		// k-means
		std::vector<std::vector<float>> raw_samples;
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				if (win_rects[i][j].valid != fs::WindowPos::VALID) continue;

				for (int v = 0; v < y_splits[i + 1] - y_splits[i]; ++v) {
					for (int u = 0; u < x_splits[j + 1] - x_splits[j]; ++u) {
						if (v >= win_rects[i][j].top && v <= win_rects[i][j].bottom && u >= win_rects[i][j].left && u <= win_rects[i][j].right) continue;

						cv::Vec3b col = lab_img.at<cv::Vec3b>(y_splits[i] + v, x_splits[j] + u);

						std::vector<float> v(3);
						for (int k = 0; k < 3; ++k) {
							v[k] = col[k];
						}
						raw_samples.push_back(v);
					}
				}
			}
		}
		cv::Mat samples(raw_samples.size(), 3, CV_32F);
		for (int i = 0; i < raw_samples.size(); ++i) {
			for (int k = 0; k < 3; ++k) {
				samples.at<float>(i, k) = raw_samples[i][k];
			}
		}

		cv::Mat labels;
		int attempts = 5;
		cv::Mat centers;
		cv::kmeans(samples, clusterCount, labels, cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, cv::KMEANS_PP_CENTERS, centers);
		std::vector<int> hist(centers.rows, 0);
		for (int i = 0; i < labels.rows; ++i) {
			int l = labels.at<int>(i, 0);
			hist[l]++;
		}

		int max_count = 0;
		int max_idx = -1;
		for (int i = 0; i < hist.size(); ++i) {
			if (hist[i] > max_count) {
				max_count = hist[i];
				max_idx = i;
			}
		}

		cv::Scalar max_color(0, 0, 0);
		for (int k = 0; k < 3; ++k) {
			max_color[k] = centers.at<float>(max_idx, k);
		}

		cv::Mat temp(1, 1, CV_8UC3, max_color);
		cv::cvtColor(temp, temp, cv::COLOR_Lab2BGR);
		cv::Vec3b rgb_col = temp.at<cv::Vec3b>(0, 0);

		return cv::Scalar(rgb_col[2], rgb_col[1], rgb_col[0]);
	}

	bool isLocalMinimum(const cv::Mat& mat, int index, float threshold) {
		float origin_value = mat.at<float>(index);

		// check upward
		bool local_max_found = false;
		for (int r = index - 1; r >= 0; --r) {
			if (mat.at<float>(r) < origin_value) return false;
			if (fabs(mat.at<float>(r) -origin_value) > threshold) {
				local_max_found = true;
				break;
			}
		}

		if (!local_max_found) return false;

		// check downward
		for (int r = index + 1; r < mat.rows; ++r) {
			if (mat.at<float>(r) < origin_value) return false;
			if (fabs(mat.at<float>(r) - origin_value) > threshold) return true;
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// visualization

	void generateWindowImage(std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, cv::Size window_img_size, cv::Mat& window_img) {
		int img_width = x_splits.back() + 1;
		int img_height = y_splits.back() + 1;

		// resize the window coordinates
		for (int i = 0; i < win_rects.size(); ++i) {
			for (int j = 0; j < win_rects[i].size(); ++j) {
				win_rects[i][j].left = win_rects[i][j].left * (float)window_img_size.width / img_width;
				win_rects[i][j].right = win_rects[i][j].right * (float)window_img_size.width / img_width;
				win_rects[i][j].top = win_rects[i][j].top * (float)window_img_size.height / img_height;
				win_rects[i][j].bottom = win_rects[i][j].bottom * (float)window_img_size.height / img_height;
			}
		}
		for (int i = 0; i < x_splits.size(); ++i) {
			x_splits[i] = x_splits[i] * (float)window_img_size.width / img_width;
		}
		for (int i = 0; i < y_splits.size(); ++i) {
			y_splits[i] = y_splits[i] * (float)window_img_size.height / img_height;
		}

		// generate window image with specified size
		window_img = cv::Mat(window_img_size, CV_8UC3, cv::Scalar(255, 255, 255));
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				if (win_rects[i][j].valid == fs::WindowPos::VALID) {
					int x = std::round(x_splits[j] + win_rects[i][j].left);
					int y = std::round(y_splits[i] + win_rects[i][j].top);
					int w = std::round(win_rects[i][j].right - win_rects[i][j].left + 1);
					int h = std::round(win_rects[i][j].bottom - win_rects[i][j].top + 1);
					cv::rectangle(window_img, cv::Rect(x, y, w, h), cv::Scalar(0, 0, 0), 1);
				}
			}
		}
	}

	void outputFacadeStructure(cv::Mat img, const std::vector<float>& y_splits, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result;
		if (img.channels() == 1) {
			cv::cvtColor(img, result, cv::COLOR_GRAY2BGR);
		}
		else {
			result = img.clone();
		}

		for (int i = 0; i < y_splits.size(); ++i) {
			if (i < y_splits.size() - 1) {
				cv::line(result, cv::Point(0, y_splits[i]), cv::Point(img.cols, y_splits[i]), lineColor, lineWidth);
			}
			else {
				// For the last line, we need to move the line upward by 1px to make it inside the image.
				cv::line(result, cv::Point(0, y_splits[i] - 1), cv::Point(img.cols, y_splits[i] - 1), lineColor, lineWidth);
			}
		}
		for (int i = 0; i < x_splits.size(); ++i) {
			if (i < x_splits.size() - 1) {
				cv::line(result, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], img.rows), lineColor, lineWidth);
			}
			else {
				// For the last line, we need to move the line upward by 1px to make it inside the image.
				cv::line(result, cv::Point(x_splits[i] - 1, 0), cv::Point(x_splits[i] - 1, img.rows), lineColor, lineWidth);
			}
		}
		cv::imwrite(filename, result);
	}

	void outputFacadeStructure(cv::Mat img, const cv::Mat_<float>& SV_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const cv::Mat_<float>& SH_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		if (img.channels() == 1) {
			cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
		}

		double min_SV, max_SV;
		double min_Ver, max_Ver;
		double min_h, max_h;
		double min_SH, max_SH;
		double min_Hor, max_Hor;
		double min_w, max_w;
		cv::minMaxLoc(SV_max, &min_SV, &max_SV);
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		cv::minMaxLoc(h_max, &min_h, &max_h);
		cv::minMaxLoc(SH_max, &min_SH, &max_SH);
		cv::minMaxLoc(Hor, &min_Hor, &max_Hor);
		cv::minMaxLoc(h_max, &min_w, &max_w);

		int graphSizeV = img.rows * 0.25;
		int marginV = graphSizeV * 0.2;
		int graphSizeH = std::max(80.0, img.rows * 0.25);
		int marginH = graphSizeH * 0.2;
		cv::Mat result(img.rows + graphSizeH * 2 + max_w + marginH * 4, img.cols + graphSizeV * 2 + max_h + marginV * 4, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw SV_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + marginV + (SV_max(r) - min_SV) / (max_SV - min_SV) * graphSizeV;
			int x2 = img.cols + marginV + (SV_max(r + 1) - min_SV) / (max_SV - min_SV) * graphSizeV;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Ver
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSizeV + marginV * 2 + (Ver(r) - min_Ver) / (max_Ver - min_Ver) * graphSizeV;
			int x2 = img.cols + graphSizeV + marginV * 2 + (Ver(r + 1) - min_Ver) / (max_Ver - min_Ver) * graphSizeV;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSizeV * 2 + marginV * 3 + h_max(r);
			int x2 = img.cols + graphSizeV * 2 + marginV * 3 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw y splits
		for (int i = 0; i < y_splits.size(); ++i) {
			cv::line(result, cv::Point(0, y_splits[i]), cv::Point(img.cols, y_splits[i]), cv::Scalar(0, 255, 255), lineWidth);
		}

		// draw SH_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + marginH + (SH_max(c) - min_SH) / (max_SH - min_SH) * graphSizeH;
			int y2 = img.rows + marginH + (SH_max(c + 1) - min_SH) / (max_SH - min_SH) * graphSizeH;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Hor
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSizeH + marginH * 2 + (Hor(c) - min_Hor) / (max_Hor - min_Hor) * graphSizeH;
			int y2 = img.rows + graphSizeH + marginH * 2 + (Hor(c + 1) - min_Hor) / (max_Hor - min_Hor) * graphSizeH;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSizeH * 2 + marginH * 3 + w_max(c);
			int y2 = img.rows + graphSizeH * 2 + marginH * 3 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw x splits
		for (int i = 0; i < x_splits.size(); ++i) {
			cv::line(result, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], img.rows), cv::Scalar(0, 255, 255), lineWidth);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeAndWindows(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result = img.clone();
#if 0
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
#endif
		for (int i = 0; i < y_split.size() - 1; ++i) {
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					int x = x_split[j] + winpos[i][j].left;
					int y = y_split[i] + winpos[i][j].top;
					int w = winpos[i][j].right - winpos[i][j].left + 1;
					int h = winpos[i][j].bottom - winpos[i][j].top + 1;
					cv::rectangle(result, cv::Rect(x, y, w, h), lineColor, lineWidth);
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

	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const std::vector<float>& ys, const cv::Mat_<float>& hor, const std::vector<float>& xs, const std::string& filename, int lineWidth) {
		int graphSize = std::max(10.0, std::max(img.rows, img.cols) * 0.3);

		cv::Mat result;
		cv::Scalar graph_color;
		cv::Scalar peak_color;

		result = cv::Mat(img.rows + graphSize + 3, img.cols + graphSize + 3, CV_8UC3, cv::Scalar(255, 255, 255));
		graph_color = cv::Scalar(0, 0, 0);
		peak_color = cv::Scalar(0, 255, 255);

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
			int x1 = img.cols + (ver(r) - min_ver) / (max_ver - min_ver) * graphSize;
			int x2 = img.cols + (ver(r + 1) - min_ver) / (max_ver - min_ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), graph_color, 1, cv::LINE_8);
		}
		for (int i = 0; i < ys.size(); ++i) {
			cv::line(result, cv::Point(0, ys[i]), cv::Point(img.cols - 1, ys[i]), peak_color, lineWidth);
		}

		// draw horizontal graph
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + (hor(c) - min_hor) / (max_hor - min_hor) * graphSize;
			int y2 = img.rows + (hor(c + 1) - min_hor) / (max_hor - min_hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), graph_color, 1, cv::LINE_8);
		}
		for (int i = 0; i < xs.size(); ++i) {
			cv::line(result, cv::Point(xs[i], 0), cv::Point(xs[i], img.rows - 1), peak_color, lineWidth);
		}

		cv::imwrite(filename, result);
	}

	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const cv::Mat_<float>& hor, const std::string& filename) {
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
			int x1 = img.cols + (ver(r) - min_ver) / (max_ver - min_ver) * graphSize;
			int x2 = img.cols + (ver(r + 1) - min_ver) / (max_ver - min_ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), graph_color, 1, cv::LINE_8);
		}

		// draw horizontal graph
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + (hor(c) - min_hor) / (max_hor - min_hor) * graphSize;
			int y2 = img.rows + (hor(c + 1) - min_hor) / (max_hor - min_hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), graph_color, 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	/**
	* Facade画像と合わせて、S_max(y)とh_max(y)を画像として保存する。
	* 論文 Fig 5に相当する画像。
	*
	* @param img		Facade画像
	* @param S_max		S_max
	* @param h_max		h_max
	* @param filename	output file name
	*/
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& h_max, const std::string& filename) {
		float max_S = cvutils::max(S_max);
		float min_S = cvutils::min(S_max);
		float max_h = cvutils::max(h_max);
		float min_h = cvutils::min(h_max);

		int graphSize = img.rows * 0.25;
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows, img.cols + graphSize + max_h + margin * 3, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + margin + (S_max(r) - min_S) / (max_S - min_S) * graphSize;
			int x2 = img.cols + margin + (S_max(r + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize + margin * 2 + h_max(r);
			int x2 = img.cols + graphSize + margin * 2 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::string& filename) {
		double min_S, max_S;
		double min_Ver, max_Ver;
		double min_h, max_h;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		cv::minMaxLoc(h_max, &min_h, &max_h);
		
		int graphSize = img.rows * 0.25;
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows, img.cols + graphSize * 2 + max_h + margin * 4, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + margin + (S_max(r) - min_S) / (max_S - min_S) * graphSize;
			int x2 = img.cols + margin + (S_max(r + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Ver
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize + margin * 2 + (Ver(r) - min_Ver) / (max_Ver - min_Ver) * graphSize;
			int x2 = img.cols + graphSize + margin * 2 + (Ver(r + 1) - min_Ver) / (max_Ver - min_Ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize * 2 + margin * 3 + h_max(r);
			int x2 = img.cols + graphSize * 2 + margin * 3 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const std::string& filename) {
		double min_S, max_S;
		double min_Ver, max_Ver;
		double min_h, max_h;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		cv::minMaxLoc(h_max, &min_h, &max_h);

		int graphSize = img.rows * 0.25;
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows, img.cols + graphSize * 2 + max_h + margin * 4, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + margin + (S_max(r) - min_S) / (max_S - min_S) * graphSize;
			int x2 = img.cols + margin + (S_max(r + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Ver
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize + margin * 2 + (Ver(r) - min_Ver) / (max_Ver - min_Ver) * graphSize;
			int x2 = img.cols + graphSize + margin * 2 + (Ver(r + 1) - min_Ver) / (max_Ver - min_Ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize * 2 + margin * 3 + h_max(r);
			int x2 = img.cols + graphSize * 2 + margin * 3 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw y splits
		for (int i = 0; i < y_splits.size(); ++i) {
			cv::line(result, cv::Point(0, y_splits[i]), cv::Point(img.cols, y_splits[i]), cv::Scalar(0, 255, 255), 3);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& w_max, const std::string& filename) {
		float max_S = cvutils::max(S_max);
		float min_S = cvutils::min(S_max);
		float max_w = cvutils::max(w_max);
		float min_w = cvutils::min(w_max);

		int graphSize = std::max(80.0, img.rows * 0.25);
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows + graphSize + max_w + margin * 3, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + margin + (S_max(c) - min_S) / (max_S - min_S) * graphSize;
			int y2 = img.rows + margin + (S_max(c + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize + margin * 2 + w_max(c);
			int y2 = img.rows + graphSize + margin * 2 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::string& filename) {
		double min_S, max_S;
		double min_Hor, max_Hor;
		double min_w, max_w;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Hor, &min_Hor, &max_Hor);
		cv::minMaxLoc(w_max, &min_w, &max_w);

		int graphSize = std::max(80.0, img.rows * 0.25);
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows + graphSize * 2 + max_w + margin * 4, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + margin + (S_max(c) - min_S) / (max_S - min_S) * graphSize;
			int y2 = img.rows + margin + (S_max(c + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Hor
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize + margin * 2 + (Hor(c) - min_Hor) / (max_Hor - min_Hor) * graphSize;
			int y2 = img.rows + graphSize + margin * 2 + (Hor(c + 1) - min_Hor) / (max_Hor - min_Hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize * 2 + margin * 3 + w_max(c);
			int y2 = img.rows + graphSize * 2 + margin * 3 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename) {
		double min_S, max_S;
		double min_Hor, max_Hor;
		double min_w, max_w;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Hor, &min_Hor, &max_Hor);
		cv::minMaxLoc(w_max, &min_w, &max_w);

		int graphSize = std::max(80.0, img.rows * 0.25);
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows + graphSize * 2 + max_w + margin * 4, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + margin + (S_max(c) - min_S) / (max_S - min_S) * graphSize;
			int y2 = img.rows + margin + (S_max(c + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Hor
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize + margin * 2 + (Hor(c) - min_Hor) / (max_Hor - min_Hor) * graphSize;
			int y2 = img.rows + graphSize + margin * 2 + (Hor(c + 1) - min_Hor) / (max_Hor - min_Hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize * 2 + margin * 3 + w_max(c);
			int y2 = img.rows + graphSize * 2 + margin * 3 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw x splits
		for (int i = 0; i < x_splits.size(); ++i) {
			cv::line(result, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], img.rows), cv::Scalar(0, 255, 255), 3);
		}

		cv::imwrite(filename, result);
	}

}