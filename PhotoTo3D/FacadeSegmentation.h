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
		int type;

	public:
		WindowPos() : valid(INVALID), left(0), top(0), right(0), bottom(0) {}
		WindowPos(int left, int top, int right, int bottom) : left(left), top(top), right(right), bottom(bottom), valid(VALID) {}
	};

	void subdivideFacade(cv::Mat img, float floor_height, float column_width, bool align_windows, std::vector<float>& y_split, std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& win_rects);
	std::vector<float> findBoundaries(const cv::Mat& img, cv::Range range1, cv::Range range2, int num_splits, const cv::Mat_<float>& Ver);
	bool sortBySecondValue(const std::pair<float, float>& a, const std::pair<float, float>& b);
	void sortByS(std::vector<float>& splits, std::map<int, float>& S_max);
	void extractWindows(cv::Mat gray_img, const std::vector<float>& y_splits, const std::vector<float>& x_splits, std::vector<std::vector<WindowPos>>& win_rects);
	float MI(const cv::Mat& R1, const cv::Mat& R2);
	void computeSV(const cv::Mat& img, cv::Mat_<float>& SV_max, cv::Mat_<int>& h_max, const cv::Range& h_range);
	void computeSV(const cv::Mat& img, int r, float& SV_max, int& h_max, const cv::Range& h_range);
	void computeSH(const cv::Mat& img, cv::Mat_<float>& SH_max, cv::Mat_<int>& w_max, const cv::Range& w_range);
	void computeSH(const cv::Mat& img, int c, float& SH_max, int& w_max, const cv::Range& w_range);
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float sigma);
	void computeVerAndHor2(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float alpha);
	void getSplitLines(const cv::Mat_<float>& mat, float threshold, std::vector<float>& split_positions);
	void refineSplitLines(std::vector<float>& split_positions, float threshold);
	void distributeSplitLines(std::vector<float>& split_positions, float threshold);
	void align(const cv::Mat& edge_img, const std::vector<float>& y_split, const std::vector<float>& x_split, std::vector<std::vector<WindowPos>>& winpos, int max_iter);
	bool isLocalMinimum(const cv::Mat& mat, int index, float threshold);

	// visualization
	void generateWindowImage(std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, cv::Size window_img_size, cv::Mat& window_img);
	void outputFacadeStructure(cv::Mat img, const std::vector<float>& y_splits, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputFacadeStructure(cv::Mat img, const cv::Mat_<float>& SV_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const cv::Mat_<float>& SH_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputFacadeAndWindows(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputWindows(const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const std::vector<float>& ys, const cv::Mat_<float>& hor, const std::vector<float>& xs, const std::string& filename, int lineWidth);
	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const cv::Mat_<float>& hor, const std::string& filename);
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& h_max, const std::string& filename);
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::string& filename);
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const std::string& filename);
	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& w_max, const std::string& filename);
	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::string& filename);
	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename);

}