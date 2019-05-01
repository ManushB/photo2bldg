#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "FacadeSegmentation.h"
#include "Classifier.h"
#include <boost/shared_ptr.hpp>

namespace winrec {

	std::vector<int> recognition(const cv::Mat& img, int facade_id, const std::vector<float>& y_splits, const std::vector<float>& x_splits, std::vector<std::vector<fs::WindowPos>>& win_rects, boost::shared_ptr<Classifier> classifier);

}
