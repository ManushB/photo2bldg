#pragma once

#include <opencv/cv.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <boost/shared_ptr.hpp>
#include "Classifier.h"
#include "Regression.h"

namespace fe {

	void adjustFacadeImage(cv::Mat& img);
	void adjustFacadeImage(cv::Mat& image, double hori, double vert);
	void parametereEstimation(const cv::Mat& img);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Image based facade reconstruction by Muller et al 2007
	void subdivideFacade(const cv::Mat& img, const std::string& filename_prefix);
	//void clusterFloors(const cv::Mat& img, const vector<float>& y_split, int max_cluster, std::vector<cv::Mat>& floors, std::vector<int>& labels, std::vector<cv::Mat>& centroids);

}