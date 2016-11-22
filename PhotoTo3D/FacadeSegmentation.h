#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace fs {

	class WindowPos {
	public:
		static enum { INVALID = 0, VALID, UNCERTAIN };
	public:
		int left;
		int right;
		int top;
		int bottom;
		int valid;

	public:
		WindowPos() : valid(INVALID), left(0), top(0), right(0), bottom(0) {}
		WindowPos(int left, int top, int right, int bottom) : left(left), top(top), right(right), bottom(bottom), valid(VALID) {}
	};

	void subdivideFacade(const cv::Mat& img, bool align_windows, std::vector<float>& y_split, std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& win_rects);
	float MI(const cv::Mat& R1, const cv::Mat& R2);
	void computeSV(const cv::Mat& img, cv::Mat_<float>& SV_max, cv::Mat_<float>& h_max, const std::pair<int, int>& h_range);
	void computeSH(const cv::Mat& img, cv::Mat_<float>& SH_max, cv::Mat_<float>& w_max, const std::pair<int, int>& w_range);
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor);
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float sigma);
	void computeVerAndHor2(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor);
	bool subdivideTile(const cv::Mat& tile, const cv::Mat& edges, int min_size, int tile_margin, WindowPos& winpos);
	bool subdivideTile2(const cv::Mat& tile, cv::Mat Ver, cv::Mat Hor, int min_size, int tile_margin, WindowPos& winpos);
	void findBestHorizontalSplitLines(const cv::Mat& img, const cv::Mat_<float>& Ver, float min_interval, float max_interval, std::vector<int>& y_split);
	void findBestVerticalSplitLines(const cv::Mat& img, const cv::Mat_<float>& Hor, float min_interval, float max_interval, std::vector<int>& x_split);
	void getSplitLines(const cv::Mat_<float>& mat, float threshold, std::vector<float>& split_positions);
	void refineSplitLines(std::vector<float>& split_positions, float threshold);
	void distributeSplitLines(std::vector<float>& split_positions);
	void refine(std::vector<float>& y_split, std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& winpos, float threshold);
	void align(const cv::Mat& edge_img, const std::vector<float>& y_split, const std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& winpos, int max_iter);
	bool isLocalMinimum(const cv::Mat& mat, int index, float threshold);

	// visualization
	void outputFacadeStructure(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputFacadeAndWindows(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputWindows(const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat& ver, const std::vector<float>& ys, const cv::Mat& hor, const std::vector<float>& xs, const std::string& filename, int lineWidth);
	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat& ver, const cv::Mat& hor, const std::string& filename);

}