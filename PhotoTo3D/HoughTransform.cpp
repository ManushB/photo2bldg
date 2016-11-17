#include "HoughTransform.h"
#include <iostream>

namespace ht {

	const double M_PI = 3.1415926535897932384626433832795;

	void warpImageByDominantOrientation(cv::Mat& image) {
		double hori, vert;
		ht::getDominantOrientation(image, cv::Size(5, 5), 15, 0.6, hori, vert);

		std::vector<cv::Point2f> srcTri(3);
		std::vector<cv::Point2f> dstTri(3);
		srcTri[0] = cv::Point2f(image.cols / 2.0, image.rows / 2.0);
		srcTri[1] = cv::Point2f(image.cols, image.rows / 2.0 + image.cols / 2.0 * tan(hori / 180.0 * M_PI));
		srcTri[2] = cv::Point2f(image.cols / 2.0 + image.rows / 2.0 / tan(vert / 180.0 * M_PI), image.rows);
		dstTri[0] = cv::Point2f(image.cols / 2.0, image.rows / 2.0);
		dstTri[1] = cv::Point2f(image.cols, image.rows / 2.0);
		dstTri[2] = cv::Point2f(image.cols / 2.0, image.rows);

		cv::Mat warpMat = cv::getAffineTransform(srcTri, dstTri);
		cv::warpAffine(image, image, warpMat, image.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
	}

	void houghTransform(const cv::Mat& image, const cv::Size& kernel, cv::Mat& accum) {
		// convert the image to grayscale
		cv::Mat grayImg;
		if (image.channels() == 1) {
			image.copyTo(grayImg);
		}
		else if (image.channels() == 3) {
			cv::cvtColor(image, grayImg, CV_BGR2GRAY);
		}
		else if (image.channels() == 4) {
			cv::cvtColor(image, grayImg, CV_BGRA2GRAY);
		}

		// blur the image
		//cv::blur(grayImg, grayImg, cv::Size(5, 5));

		cv::Mat edgeImg;
		//cv::Canny(grayImg, edgeImg, 50, 160, 3);
		//cv::Canny(grayImg, edgeImg, 30, 100, 3);
		autoCanny2(grayImg, edgeImg);

		cv::imwrite("edge.png", edgeImg);
		
		// compute the accumulation
		double hough_h = ((sqrt(2.0) * (double)(edgeImg.rows > edgeImg.cols ? edgeImg.rows : edgeImg.cols)) / 2.0);

		accum = cv::Mat(hough_h * 2, 180, CV_32F, cv::Scalar(0.0f));
		double center_x = edgeImg.cols / 2.0;
		double center_y = edgeImg.rows / 2.0;
		for (int y = 0; y < edgeImg.rows; ++y) {
			for (int x = 0; x < edgeImg.cols; ++x) {
				if (edgeImg.at<uchar>(y, x) < 250) continue;

				for (int t = 0; t < 180; ++t) {
					double r = ((double)x - center_x) * cos((double)t / 180.0 * M_PI) + ((double)y - center_y) * sin((double)t / 180.0 * M_PI);
					accum.at<float>(r + hough_h, t) += 1.0f;
				}
			}
		}

		//saveImage(accum, "accum.png");
	}

	/**
	 * 指定された画像のエッジを抽出し、水平方向および垂直方向のdominantな角度を返却する。
	 */
	void getDominantOrientation(const cv::Mat& image, const cv::Size& kernel, int max_degree, float threshold_ratio, double& hori, double& vert) {
#if 1
		cv::Mat accum;
		houghTransform(image, kernel, accum);

		visualizeAccum(image, accum, max_degree, threshold_ratio, "image_edge.png");

		vert = getVerticalOrientation(accum, max_degree, threshold_ratio);
		hori = getHorizontalOrientation(accum, max_degree, threshold_ratio);
#endif

#if 0
		cv::Mat grayImg;
		cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);

		cv::Mat sobelx;
		cv::Sobel(grayImg, sobelx, CV_32F, 1, 0, 5);
		cv::Mat sobely;
		cv::Sobel(grayImg, sobely, CV_32F, 0, 1, 5);

		cv::imwrite("sobelx.png", abs(sobelx));
		cv::imwrite("sobely.png", abs(sobely));

		std::vector<float> histogram(180, 0.0f);
		for (int r = 0; r < image.rows; ++r) {
			for (int c = 0; c < image.cols; ++c) {
				float theta = atan2(sobely.at<float>(r, c), sobelx.at<float>(r, c)) + M_PI * 0.5f;
				float magnitude = sqrtf(sobelx.at<float>(r, c) * sobelx.at<float>(r, c) + sobely.at<float>(r, c) * sobely.at<float>(r, c));
				if (theta < 0) {
					theta += M_PI;
				}
				else if (theta >= M_PI) {
					theta -= M_PI;
				}

				if (theta <= max_degree / 180.0f * M_PI || theta >= (180.0f - max_degree) / 180.0f * M_PI) {
					histogram[(int)(theta / M_PI * 180.0f)] += magnitude;
				}
				else if (theta >= (90.0f - max_degree) / 180.0f * M_PI && theta <= (90.0f + max_degree) / 180.0f * M_PI) {
					histogram[(int)(theta / M_PI * 180.0f)] += magnitude;
				}
			}
		}

		int vert_max = 0;
		int hori_max = 0;
		for (int i = 0; i < 180; ++i) {
			if (i <= max_degree || i >= 180 - max_degree) {
				if (histogram[i] > hori_max) {
					hori_max = histogram[i];
					if (i <= max_degree) {
						hori = i;
					}
					else {
						hori = i - 180;
					}
				}
			}
			else if (i >= 90 - max_degree && i <= 90 + max_degree) {
				if (histogram[i] > vert_max) {
					vert_max = histogram[i];
					vert = i;
				}
			}
		}
#endif
	}

	float getVerticalOrientation(const cv::Mat& accum, int max_degree, float threshold_ratio) {
		// find teh maximum value
		float v_max = getVerticalAccumMax(accum, max_degree);

		// change the value below the threshold to 0
		cv::Mat accumV;
		cv::threshold(accum, accumV, v_max * threshold_ratio, 255, cv::THRESH_TOZERO);
		for (int r = 0; r < accumV.rows; ++r) {
			for (int c = 0; c < accumV.cols; ++c) {
				if (c > max_degree && c < 180 - max_degree) accumV.at<float>(r, c) = 0;
			}
		}
		//saveImage(accumV, "accumV_th.png");

		// clean up the accum by only keeping the local maximum
		cv::Mat accumV_refined(accumV.size(), CV_32F, cv::Scalar(0.0f));
		for (int r = 0; r < accumV.rows; ++r) {
			for (int t = 0; t < 180; ++t) {
				if (t > max_degree && t < 180 - max_degree) continue;
				if (accumV.at<float>(r, t) == 0) continue;
				if (!isLocalMaximum(accumV, r, t, 4)) continue;
				
				accumV_refined.at<float>(r, t) = accumV.at<float>(r, t);
				//accumV_refined.at<float>(r, t) = 1.0f;
			}
		}
		//saveImage(accumV_refined, "accumV_refined.png");
		
		// find the theta that has the maximum value
		float vert = 90.0f;
		cv::reduce(accumV_refined, accumV_refined, 0, CV_REDUCE_SUM);
		//saveHistogram(accumV_refined, "accumV_hist.png");
		v_max = 0.0f;
		for (int t = 0; t <= max_degree; ++t) {
			if (accumV_refined.at<float>(0, t) > v_max) {
				v_max = accumV_refined.at<float>(0, t);
				vert = t + 90;
			}
		}
		for (int t = 180 - max_degree; t < 180; ++t) {
			if (accumV_refined.at<float>(0, t) > v_max) {
				v_max = accumV_refined.at<float>(0, t);
				vert = t - 180 + 90;
			}
		}

		return vert;
	}

	float getHorizontalOrientation(const cv::Mat& accum, int max_degree, float threshold_ratio) {
		// find teh maximum value
		float h_max = getHorizontalAccumMax(accum, max_degree);

		// change the value below the threshold to 0
		cv::Mat accumH;
		cv::threshold(accum, accumH, h_max * threshold_ratio, 255, cv::THRESH_TOZERO);
		for (int r = 0; r < accumH.rows; ++r) {
			for (int c = 0; c < accumH.cols; ++c) {
				if (c < 90 - max_degree || c > 90 + max_degree) accumH.at<float>(r, c) = 0;
			}
		}
		//saveImage(accumH, "accumH_th.png");

		// clean up the accum by only keeping the local maximum
		cv::Mat accumH_refined(accumH.size(), CV_32F, cv::Scalar(0.0f));
		for (int r = 0; r < accumH.rows; ++r) {
			for (int t = 90 - max_degree; t <= 90 + max_degree; ++t) {
				if (accumH.at<float>(r, t) == 0) continue;
				if (!isLocalMaximum(accumH, r, t, 4)) continue;

				accumH_refined.at<float>(r, t) = accumH.at<float>(r, t);
				//accumH_refined.at<float>(r, t) = 1.0f;
			}
		}
		//saveImage(accumH_refined, "accumH_refined.png");

		// find the theta that has the maximum value
		float hori = 0.0f;
		cv::reduce(accumH_refined, accumH_refined, 0, CV_REDUCE_SUM);
		saveHistogram(accumH_refined, "accumH_hist.png");
		h_max = 0.0f;
		for (int t = 90 - max_degree; t <= 90 + max_degree; ++t) {
			if (accumH_refined.at<float>(0, t) > h_max) {
				h_max = accumH_refined.at<float>(0, t);
				hori = t - 90;
			}
		}

		return hori;
	}

	float getVerticalAccumMax(const cv::Mat& accum, float max_degree) {
		cv::Mat accum_max;
		cv::reduce(accum, accum_max, 0, CV_REDUCE_MAX);

		// find the maximum value
		float v_max = 0.0f;
		for (int t = 0; t < 180; ++t) {
			if (t > max_degree && t < 180 - max_degree) continue;

			if (accum_max.at<float>(0, t) > v_max) {
				v_max = accum_max.at<float>(0, t);
			}
		}

		return v_max;
	}

	float getHorizontalAccumMax(const cv::Mat& accum, float max_degree) {
		cv::Mat accum_max;
		cv::reduce(accum, accum_max, 0, CV_REDUCE_MAX);

		// find the maximum value
		float h_max = 0.0f;
		for (int t = 90 - max_degree; t <= 90 + max_degree; ++t) {
			if (accum_max.at<float>(0, t) > h_max) {
				h_max = accum_max.at<float>(0, t);
			}
		}

		return h_max;
	}

	bool isLocalMaximum(const cv::Mat& mat, int r, int c, int size = 4) {
		float value = mat.at<float>(r, c);

		for (int ly = -size; ly <= size; ly++) {
			for (int lx = -size; lx <= size; lx++) {
				if (lx == 0 && ly == 0) continue;
				if ((ly + r >= 0 && ly + r < mat.rows) && (lx + c >= 0 && lx + c < mat.cols)) {
					if (mat.at<float>(r + ly, c + lx) > value) {
						return false;
					}
				}
			}
		}
		
		return true;
	}

	void autoCanny(const cv::Mat& grayImg, cv::Mat& edgeImg, float sigma) {
		float v = median(grayImg);

		int lower = std::max(0.0f, (1.0f - sigma) * v);
		int upper = std::max(255.0f, (1.0f + sigma) * v);
		std::cout << "mean: " << v << ", threshold1: " << lower << ", threshold2: " << upper << std::endl;
		cv::Canny(grayImg, edgeImg, lower, upper);
	}

	void autoCanny2(const cv::Mat& grayImg, cv::Mat& edgeImg, float sigma) {
		float v = median(grayImg);

		int lower = std::max(0.0f, (1.0f - sigma) * v);
		int upper = std::max(255.0f, (1.0f + sigma) * v);
		//std::cout << "mean: " << v << ", threshold1: " << lower << ", threshold2: " << upper << std::endl;
		cv::Canny(grayImg, edgeImg, v * 0.5, v);
	}

	double median(const cv::Mat& grayImg) {
		double m = (grayImg.rows * grayImg.cols) / 2;
		int bin = 0;
		double med = -1.0;

		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		cv::Mat hist;
		cv::calcHist(&grayImg, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

		for (int i = 0; i < histSize && med < 0.0; ++i) {
			bin += cvRound(hist.at< float >(i));
			if (bin > m && med < 0.0)
				med = i;
		}

		return med;
	}

	void saveImage(const cv::Mat& image, const std::string& filename) {
		double minVal;
		double maxVal;
		cv::Point minIdx;
		cv::Point maxIdx;
		cv::minMaxLoc(image, &minVal, &maxVal, &minIdx, &maxIdx);

		cv::imwrite(filename.c_str(), (image - minVal) / (maxVal - minVal) * 255);
		//cv::imwrite(filename.c_str(), image / maxVal * 255);
	}

	void saveHistogram(const cv::Mat& mat, const std::string& filename) {
		cv::Mat mat2 = mat.clone();

		cv::Mat mat_max;
		cv::reduce(mat, mat_max, 0, CV_REDUCE_MAX);
		cv::reduce(mat_max, mat_max, 1, CV_REDUCE_MAX);

		//std::cout << "Max: " << mat_max.at<float>(0, 0) << std::endl;

		mat2 *= (100.0 / mat_max.at<float>(0, 0));

		if (mat2.cols == 1) {
			mat2 = mat2.t();
		}

		cv::Mat hist(100, mat2.cols, CV_8U, cv::Scalar(255));
		for (int c = 0; c < mat2.cols; ++c) {
			if (mat2.at<float>(0, c) > 0) {
				cv::line(hist, cv::Point(c, hist.rows - 1), cv::Point(c, hist.rows - 1 - mat2.at<float>(0, c)), cv::Scalar(0), 1);
			}
		}

		cv::imwrite(filename.c_str(), hist);
	}

	void visualizeAccum(const cv::Mat& image, const cv::Mat& accum, int max_degree, float threshold_ratio, const std::string& filename) {
		float v_max = getVerticalAccumMax(accum, max_degree);
		float h_max = getHorizontalAccumMax(accum, max_degree);

		// apply threshold
		cv::Mat accum_threshold = accum.clone();
		for (int r = 0; r < accum_threshold.rows; ++r) {
			for (int t = 0; t < 180; ++t) {
				if (t <= max_degree || t >= 180 - max_degree) {
					if (accum_threshold.at<float>(r, t) < v_max * threshold_ratio) {
						accum_threshold.at<float>(r, t) = 0;
					}
				}
				else if (t >= 90 - max_degree && t <= 90 + max_degree) {
					if (accum_threshold.at<float>(r, t) < h_max * threshold_ratio) {
						accum_threshold.at<float>(r, t) = 0;
					}
				}
				else {
					accum_threshold.at<float>(r, t) = 0;
				}
			}
		}

		// visualize the result
		cv::Mat result = image.clone();
		for (int r = 0; r < accum_threshold.rows; ++r) {
			for (int t = 0; t < accum_threshold.cols; ++t) {
				if ((t > max_degree && t < 90 - max_degree) || (t > 90 + max_degree && t < 180 - max_degree)) continue;

				if (accum_threshold.at<float>(r, t) > 0) {
					if (!isLocalMaximum(accum_threshold, r, t, 4)) continue;

					int x1, y1, x2, y2;
					x1 = y1 = x2 = y2 = 0;

					if (t >= 45 && t <= 135) {
						x1 = 0;
						y1 = ((double)(r - (accum_threshold.rows / 2)) - ((x1 - (image.cols / 2)) * cos(t / 180.0 * M_PI))) / sin(t / 180.0 * M_PI) + (image.rows / 2);
						x2 = image.cols;
						y2 = ((double)(r - (accum_threshold.rows / 2)) - ((x2 - (image.cols / 2)) * cos(t / 180.0 * M_PI))) / sin(t / 180.0 * M_PI) + (image.rows / 2);
					}
					else {
						y1 = 0;
						x1 = ((double)(r - (accum_threshold.rows / 2)) - ((y1 - (image.rows / 2)) * sin(t / 180.0 * M_PI))) / cos(t / 180.0 * M_PI) + (image.cols / 2);
						y2 = image.rows;
						x2 = ((double)(r - (accum_threshold.rows / 2)) - ((y2 - (image.rows / 2)) * sin(t / 180.0 * M_PI))) / cos(t / 180.0 * M_PI) + (image.cols / 2);
					}
					cv::line(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 1);
				}
			}
		}

		cv::imwrite(filename.c_str(), result);
	}
}
