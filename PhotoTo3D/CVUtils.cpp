#include "CVUtils.h"
#include "Utils.h"

namespace cvutils {

	/**
	 * 画像と4つの頂点を与えられた時に、rectifyして四角形の画像を作成する。
	 * ただし、4つの頂点は半時計周りに並んでいること。
	 */
	cv::Mat rectify_image(cv::Mat& image, const std::vector<cv::Point2f>& pts, cv::Mat& transformMat, float width_to_height_ratio) {
		cv::Point2f tl = pts[3];
		cv::Point2f tr = pts[2];
		cv::Point2f br = pts[1];
		cv::Point2f bl = pts[0];

		// compute the width of the new image, which will be the
		// maximum distance between bottom - right and bottom - left
		// x - coordiates or the top - right and top - left x - coordinates
		float widthA = cv::norm(br - bl);
		float widthB = cv::norm(tr - tl);
		int width = std::max(int(widthA), int(widthB));

		// compute the height of the new image, which will be the
		// maximum distance between the top - right and bottom - right
		// y - coordinates or the top - left and bottom - left y - coordinates
		float heightA = cv::norm(tr - br);
		float heightB = cv::norm(tl - bl);
		int height = std::max(int(heightA), int(heightB));

		if (width_to_height_ratio > 0) {
			if (width > height * width_to_height_ratio) {
				height = (float)width / width_to_height_ratio;
			}
			else {
				width = (float)height * width_to_height_ratio;
			}
		}

		// now that we have the dimensions of the new image, construct
		// the set of destination points to obtain a "birds eye view",
		// (i.e. top-down view) of the image, again specifying points
		// in the top - left, top - right, bottom - right, and bottom - left
		// order
		std::vector<cv::Point2f> dst(4);
		dst[0] = cv::Point2f(0, height - 1);
		dst[1] = cv::Point2f(width - 1, height - 1);
		dst[2] = cv::Point2f(width - 1, 0);
		dst[3] = cv::Point2f(0, 0);

		// compute the perspective transform matrix and then apply it
		transformMat = cv::getPerspectiveTransform(pts, dst);
		cv::Mat warped;
		cv::warpPerspective(image, warped, transformMat, cv::Size(width, height));

		// return the warped image
		return warped;
	}

	cv::Mat adjust_contrast(const cv::Mat& image) {
		// convert the image to HSV format
		cv::Mat src;
		if (image.channels() == 1) {
			cv::cvtColor(image, src, cv::COLOR_GRAY2BGR);
			cv::cvtColor(src, src, cv::COLOR_BGR2HSV);
		}
		else if (image.channels() == 3) {
			cv::cvtColor(image, src, cv::COLOR_BGR2HSV);
		}
		else if (image.channels() == 4) {
			cv::cvtColor(image, src, cv::COLOR_BGRA2BGR);
			cv::cvtColor(src, src, cv::COLOR_BGR2HSV);
		}

		// split the image into 3 channels
		std::vector<cv::Mat> splittedChannels(3);
		cv::split(src, splittedChannels);
		
		// adjusst the contrast and brightness
		/*
		// equalize the histogram
		cv::equalizeHist(splittedChannels[2], splittedChannels[2]);
		*/

		cv::Mat adjusted;
		splittedChannels[2].convertTo(adjusted, CV_64F, 1.0 / 255.0);
		cv::Scalar mean, stddev;
		cv::meanStdDev(adjusted, mean, stddev);
		adjusted = adjusted - (mean - stddev * 2);// *stddev;// / 4.0;
		cv::Mat diviser(adjusted.size(), adjusted.type(), stddev * 4);
		cv::divide(adjusted, diviser, adjusted);
		adjusted.convertTo(splittedChannels[2], CV_8U, 255.0);

		// merge the channels back to a single image
		cv::merge(splittedChannels, src);

		// convert the HSV to BGR format
		cv::Mat result;
		cv::cvtColor(src, result, cv::COLOR_HSV2BGR);

		return result;
	}

	cv::Mat adjust_contrast2(const cv::Mat& image, float threshold) {
		cv::Mat hsvImg;
		if (image.channels() == 1) {
			cv::cvtColor(image, hsvImg, cv::COLOR_GRAY2BGR);
			cv::cvtColor(hsvImg, hsvImg, cv::COLOR_BGR2HSV);
		}
		else if (image.channels() == 3) {
			cv::cvtColor(image, hsvImg, cv::COLOR_BGR2HSV);
		}
		else if (image.channels() == 4) {
			cv::cvtColor(image, hsvImg, cv::COLOR_BGRA2BGR);
			cv::cvtColor(hsvImg, hsvImg, cv::COLOR_BGR2HSV);
		}

		std::vector<float> histogram(256, 0.0f);
		for (int r = 0; r < hsvImg.rows; ++r) {
			for (int c = 0; c < hsvImg.cols; ++c) {
				int intensity = hsvImg.at<cv::Vec3b>(r, c)[2];
				histogram[intensity]++;
			}
		}

		int min_val;
		float total = 0.0f;
		for (int i = 0; i < histogram.size(); ++i) {
			total += histogram[i];
			if (total > hsvImg.rows * hsvImg.cols * threshold) {
				min_val = i;
				break;
			}
		}

		int max_val;
		total = 0.0f;
		for (int i = histogram.size() - 1; i >= 0; --i) {
			total += histogram[i];
			if (total > hsvImg.rows * hsvImg.cols * threshold) {
				max_val = i;
				break;
			}
		}

		//std::cout << "Max val: " << max_val << ", Min val: " << min_val << std::endl;

		for (int r = 0; r < hsvImg.rows; ++r) {
			for (int c = 0; c < hsvImg.cols; ++c) {
				int intensity = hsvImg.at<cv::Vec3b>(r, c)[2];

				int new_intensity = std::max(0, std::min(255, (int)((float)(intensity - min_val) / (max_val - min_val) * 255.0)));
				cv::Vec3b new_val(hsvImg.at<cv::Vec3b>(r, c)[0], hsvImg.at<cv::Vec3b>(r, c)[1], new_intensity);
				hsvImg.at<cv::Vec3b>(r, c) = new_val;
			}
		}

		cv::Mat result;
		cv::cvtColor(hsvImg, result, cv::COLOR_HSV2BGR);

		return result;
	}

	void grayScale(const cv::Mat& img, cv::Mat& grayImg) {
		if (img.channels() == 1) {
			grayImg = img.clone();
		}
		else if (img.channels() >= 3) {
			std::vector<cv::Mat> img_channels;
			cv::split(img, img_channels);
			grayImg = img_channels[0] * 0.114 + img_channels[1] * 0.587 + img_channels[2] * 0.299;
		}
	}







	/**
	* Find the maximum element value of the matrix and return it.
	*
	* @param mat	Matrix
	* @return		Maximum element value
	*/
	double max(const cv::Mat& mat) {
		cv::Mat result = mat.clone();
		result = result.reshape(1, result.cols * result.rows * result.channels());
		cv::reduce(result, result, 0, CV_REDUCE_MAX);
		result.convertTo(result, CV_64F);

		return result.at<double>(0, 0);
	}

	/**
	* Find the minimum element value of the matrix and return it.
	*
	* @param mat	Matrix
	* @return		Minimum element value
	*/
	double min(const cv::Mat& mat) {
		cv::Mat result = mat.clone();
		result = result.reshape(1, result.cols * result.rows * result.channels());
		cv::reduce(result, result, 0, CV_REDUCE_MIN);
		result.convertTo(result, CV_64F);

		return result.at<double>(0, 0);
	}

	std::vector<double> mean(const cv::Mat& mat) {
		cv::Mat result = mat.clone();
		result = result.reshape(1, result.cols * result.rows);
		cv::reduce(result, result, 0, CV_REDUCE_AVG);
		result.convertTo(result, CV_64F);

		std::vector<double> ret(mat.channels());
		for (int i = 0; i < ret.size(); ++i) {
			ret[i] = result.at<double>(0, i);
		}
		return ret;
	}

	/**
	* Return the mean squared differences between two images.
	*
	* @param img1		Image 1 (1-channel image)
	* @param img2		Image 2 (1-channel image)
	* @return			Mean squared differences
	*/
	double msd(const cv::Mat& img1, const cv::Mat& img2) {
		cv::Mat diff_mat;
		cv::absdiff(img1, img2, diff_mat);
		diff_mat = diff_mat.reshape(1, diff_mat.rows * diff_mat.cols * diff_mat.channels());

		cv::Mat sqr_mat;
		cv::multiply(diff_mat, diff_mat, sqr_mat, 1, CV_64F);

		cv::reduce(sqr_mat, sqr_mat, 0, CV_REDUCE_SUM);
		return sqr_mat.at<double>(0, 0) / img1.rows / img1.cols;
	}

	double corr(const cv::Mat& img1, const cv::Mat& img2) {
		cv::Mat a;
		img1.convertTo(a, CV_64F);
		cv::Mat b;
		img2.convertTo(b, CV_64F);

		cv::Mat m1, m2, m3;
		cv::multiply(a, b, m1);
		cv::multiply(a, a, m2);
		cv::multiply(b, b, m3);

		m1 = m1.reshape(1, m1.rows * m1.cols * m1.channels());
		m2 = m2.reshape(1, m2.rows * m2.cols * m2.channels());
		m3 = m3.reshape(1, m3.rows * m3.cols * m3.channels());
		m1.convertTo(m1, CV_64F);
		m2.convertTo(m2, CV_64F);
		m3.convertTo(m3, CV_64F);
		cv::reduce(m1, m1, 0, CV_REDUCE_SUM, CV_64F);
		cv::reduce(m2, m2, 0, CV_REDUCE_SUM, CV_64F);
		cv::reduce(m3, m3, 0, CV_REDUCE_SUM, CV_64F);

		return m1.at<double>(0, 0) / sqrt(m2.at<double>(0, 0) * m3.at<double>(0, 0));
	}

	double get(const cv::Mat& mat, int r, int c) {
		if (mat.type() == CV_8U) {
			return mat.at<unsigned char>(r, c);
		}
		else if (mat.type() == CV_32F) {
			return mat.at<float>(r, c);
		}
		else if (mat.type() == CV_64F) {
			return mat.at<double>(r, c);
		}
		else {
			return 0.0;
		}
	}

	bool isLocalMinimum(const cv::Mat& mat, int index, int num) {
		bool localMinimum = true;

		if (mat.rows == 1) {	// row vector
			if (index == 0) {
				localMinimum = false;
			}
			else {
				for (int c = index; c > std::max(0, index - num); --c) {
					if (get(mat, 0, c) >= get(mat, 0, c - 1)) {
						localMinimum = false;
						break;
					}
				}
			}

			if (localMinimum) {
				if (index == mat.cols - 1) {
					localMinimum = false;
				}
				else {
					for (int c = index; c < std::min(mat.cols - 1, index + num); ++c) {
						if (get(mat, 0, c) >= get(mat, 0, c + 1)) {
							localMinimum = false;
							break;
						}
					}
				}
			}
		}
		else if (mat.cols == 1) {	// column vector
			if (index == 0) {
				localMinimum = false;
			}
			else {
				for (int r = index; r > std::max(0, index - num); --r) {
					if (get(mat, r, 0) >= get(mat, r - 1, 0)) {
						localMinimum = false;
						break;
					}
				}
			}

			if (localMinimum) {
				if (index == mat.rows - 1) {
					localMinimum = false;
				}
				else {
					for (int r = index; r < std::min(mat.rows - 1, index + num); ++r) {
						if (get(mat, r, 0) >= get(mat, r + 1, 0)) {
							localMinimum = false;
							break;
						}
					}
				}
			}
		}

		return localMinimum;
	}

	bool isLocalMaximum(const cv::Mat& mat, int index, int num) {
		bool localMaximum = true;

		if (mat.rows == 1) {	// row vector
			if (index == 0) {
				localMaximum = false;
			}
			else {
				for (int c = index; c > std::max(0, index - num); --c) {
					if (get(mat, 0, c) <= get(mat, 0, c - 1)) {
						localMaximum = false;
						break;
					}
				}
			}

			if (localMaximum) {
				if (index == mat.cols - 1) {
					localMaximum = false;
				}
				else {
					for (int c = index; c < std::min(mat.cols - 1, index + num); ++c) {
						if (get(mat, 0, c) <= get(mat, 0, c + 1)) {
							localMaximum = false;
							break;
						}
					}
				}
			}
		}
		else if (mat.cols == 1) {	// column vector
			if (index == 0) {
				localMaximum = false;
			}
			else {
				for (int r = index; r > std::max(0, index - num); --r) {
					if (get(mat, r, 0) <= get(mat, r - 1, 0)) {
						localMaximum = false;
						break;
					}
				}
			}

			if (localMaximum) {
				if (index == mat.rows - 1) {
					localMaximum = false;
				}
				else {
					for (int r = index; r < std::min(mat.rows - 1, index + num); ++r) {
						if (get(mat, r, 0) <= get(mat, r + 1, 0)) {
							localMaximum = false;
							break;
						}
					}
				}
			}
		}

		return localMaximum;
	}

	/**
	* 極値となるインデックスのリストを返却する。
	*
	* @param mat				データ
	* @param smooth			スムースするか
	* @param sigma				スムースする際のsigma
	* @param flag
	* @param continuous_num	極値周辺で、チェックする幅
	* @return					極値となるインデックスのリスト
	*/
	std::vector<int> getPeak(const cv::Mat& mat, bool smooth, int sigma, int flag, int continuous_num) {
		std::vector<int> peaks;

		// sigma has to be odd.
		if (sigma % 2 == 0) sigma++;

		cv::Mat mat_copy;
		if (smooth) {
			cv::GaussianBlur(mat, mat_copy, cv::Size(sigma, sigma), 0, 0);
		}
		else {
			mat_copy = mat.clone();
		}

		if (mat.cols == 1) {
			for (int r = 0; r < mat.rows; ++r) {
				if (flag == LOCAL_MINIMUM) {
					if (cvutils::isLocalMinimum(mat, r, continuous_num)) {
						peaks.push_back(r);
					}
				}
				else if (flag == LOCAL_MAXIMUM) {
					if (cvutils::isLocalMaximum(mat, r, continuous_num)) {
						peaks.push_back(r);
					}

				}
			}
		}
		else if (mat.rows == 1) {
			for (int c = 0; c < mat.cols; ++c) {
				if (flag == LOCAL_MINIMUM) {
					if (cvutils::isLocalMinimum(mat, c, continuous_num)) {
						peaks.push_back(c);
					}
				}
				else if (flag == LOCAL_MAXIMUM) {
					if (cvutils::isLocalMaximum(mat, c, continuous_num)) {
						peaks.push_back(c);
					}
				}
			}
		}

		return peaks;
	}

	/**
	* matの最も頻度の多い値を返却する。
	*/
	float getMostPopularValue(const cv::Mat& h_max, float sigma, float min_value) {
		std::vector<float> histogram((int)cvutils::max(h_max), 0.0f);

		for (int r = 0; r < h_max.rows; ++r) {
			for (int c = 0; c < h_max.cols; ++c) {
				if (get(h_max, r, c) < min_value) continue;

				for (int i = 0; i < histogram.size(); ++i) {
					histogram[i] += utils::gause(get(h_max, r, c) - i, sigma);
				}
			}
		}

		float max_value = 0.0f;
		int max_index = -1;
		for (int i = 0; i < histogram.size(); ++i) {
			if (histogram[i] > max_value) {
				max_value = histogram[i];
				max_index = i;
			}
		}

		return max_index;
	}

	/**
	* 指定されたindexから周辺の値を調べ、極大値を返却する。
	*/
	float findNextMax(cv::Mat mat, int index, int& max_index) {
		if (mat.rows == 1) {
			mat = mat.t();
		}

		bool foundLocalMin = false;
		float val = mat.at<float>(index, 0);
		for (int r = index - 1; r >= 0; --r) {
			if (!foundLocalMin) {
				if (mat.at<float>(r, 0) > val) {
					foundLocalMin = true;
				}
				val = mat.at<float>(r, 0);
			}
			else {
				if (mat.at<float>(r, 0) > val) {
					val = mat.at<float>(r, 0);
				}
				else {
					break;
				}
			}
		}
		float max_val = val;

		foundLocalMin = false;
		val = mat.at<float>(index, 0);
		for (int r = index + 1; r < mat.rows; ++r) {
			if (!foundLocalMin) {
				if (mat.at<float>(r, 0) > val) {
					foundLocalMin = true;
				}
				val = mat.at<float>(r, 0);
			}
			else {
				if (mat.at<float>(r, 0) > val) {
					val = mat.at<float>(r, 0);
				}
				else {
					break;
				}
			}
		}

		if (val > max_val) max_val = val;

		return max_val;
	}

	/**
	* 指定されたindexから周辺の値を調べ、極大値を返却する。
	*/
	bool findNextMax(cv::Mat mat, int index, int dir, int& max_index, float& max_value) {
		if (mat.rows == 1) {
			mat = mat.t();
		}

		bool foundLocalMin = false;
		float val = mat.at<float>(index, 0);
		if (dir == -1) {
			for (int r = index - 1; r >= 0; --r) {
				if (!foundLocalMin) {
					if (mat.at<float>(r, 0) > val) {
						foundLocalMin = true;
					}
					val = mat.at<float>(r, 0);
				}
				else {
					if (mat.at<float>(r, 0) > val) {
						val = mat.at<float>(r, 0);
					}
					else {
						max_index = r + 1;
						max_value = val;
						return true;
					}
				}
			}
			return false;
		}
		else {
			for (int r = index + 1; r < mat.rows; ++r) {
				if (!foundLocalMin) {
					if (mat.at<float>(r, 0) > val) {
						foundLocalMin = true;
					}
					val = mat.at<float>(r, 0);
				}
				else {
					if (mat.at<float>(r, 0) > val) {
						val = mat.at<float>(r, 0);
					}
					else {
						max_index = r - 1;
						max_value = val;
						return true;
					}
				}
			}
			return false;
		}
	}


}