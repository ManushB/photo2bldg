#pragma once

#undef ERROR
#undef STRICT

#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace caffe;  // NOLINT(build/namespaces)
using std::string;

/* Pair (label, confidence) representing a prediction. */
//typedef std::pair<string, float> Prediction;
typedef std::pair<int, float> Prediction;

class Regression {
public:
	Regression(const string& model_file, const string& trained_file);

	std::vector<Prediction> Classify(const cv::Mat& img, int N = 5);
	std::vector<float> Predict(const cv::Mat& img);
	void clear();

private:
	void SetMean(const string& mean_file);

	void WrapInputLayer(std::vector<cv::Mat>* input_channels);

	void Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);

private:
	boost::shared_ptr<Net<float> > net_;
	cv::Size input_geometry_;
	int num_channels_;
};

