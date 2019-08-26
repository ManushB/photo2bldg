#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

namespace cvutils {

	enum { LOCAL_MINIMUM = 1, LOCAL_MAXIMUM = 2 };

	cv::Mat rectify_image(cv::Mat& image, const std::vector<cv::Point2f>& pts, cv::Mat& transformMat, float width_to_height_ratio = 0);
	cv::Mat adjust_contrast(const cv::Mat& image);
	cv::Mat adjust_contrast2(const cv::Mat& image, float threshold = 0.01);
	void grayScale(const cv::Mat& img, cv::Mat& grayImg);

	double max(const cv::Mat& mat);
	double min(const cv::Mat& mat);
	std::vector<double> mean(const cv::Mat& mat);
	double msd(const cv::Mat& img1, const cv::Mat& img2);
	double corr(const cv::Mat& img1, const cv::Mat& img2);
	double get(const cv::Mat& mat, int r, int c);
	bool isLocalMinimum(const cv::Mat& mat, int index, int num);
	bool isLocalMaximum(const cv::Mat& mat, int index, int num);
	std::vector<int> getPeak(const cv::Mat& mat, bool smooth, int sigma, int flag = 1, int width = 1);
	float getMostPopularValue(const cv::Mat& h_max, float sigma, float min_value);
	float findNextMax(cv::Mat mat, int index, int& max_index);
	bool findNextMax(cv::Mat mat, int index, int dir, int& max_index, float& max_value);

}


