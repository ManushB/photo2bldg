#include "FacadeEstimation.h"
#include <iostream>
#include <QString>
#include <time.h>
#include "HoughTransform.h"
#include "Utils.h"
#include "CVUtils.h"
#include "FacadeSegmentation.h"

namespace fe {

	const double M_PI = 3.1415926535897932384626433832795;

	void adjustFacadeImage(cv::Mat& img) {
		ht::warpImageByDominantOrientation(img);
	}

	void adjustFacadeImage(cv::Mat& image, double hori, double vert) {
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
	
	void parametereEstimation(const cv::Mat& img) {
		Classifier classifier("models/facade/deploy.prototxt", "models/facade/train_iter_20000.caffemodel", "models/facade/mean.binaryproto");
		std::vector<boost::shared_ptr<Regression>> regressions(4);
		regressions[0] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_01.prototxt", "models/facade/train_01_iter_80000.caffemodel"));
		regressions[1] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_02.prototxt", "models/facade/train_02_iter_80000.caffemodel"));
		regressions[2] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_03.prototxt", "models/facade/train_03_iter_80000.caffemodel"));
		regressions[3] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_04.prototxt", "models/facade/train_04_iter_80000.caffemodel"));

		// convert the image to grayscale
		cv::Mat grayImg;
		utils::grayScale(img, grayImg);

		//cv::resize(grayImg, grayIm)
		// ...
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Image based facade reconstruction by Muller et al 2007
	void subdivideFacade(const cv::Mat& img, const std::string& filename_prefix) {
		// convert the image to grayscale
		cv::Mat grayImg;
		utils::grayScale(img, grayImg);

		// vertical split
		cv::Mat_<float> SV_max;
		cv::Mat_<float> h_max;
		fs::computeSV(grayImg, SV_max, h_max, make_pair(10, 40));

		// estimate the floor height
		float floor_height = cvutils::getMostPopularValue(h_max, 3, 3);

		// horizontal split
		cv::Mat_<float> SH_max;
		cv::Mat_<float> w_max;
		fs::computeSH(grayImg, SH_max, w_max, make_pair(10, 40));

		float tile_width = cvutils::getMostPopularValue(w_max, 3, 3);

		// compute Ver(y) and Hor(x)
		cv::Mat_<float> Ver;
		cv::Mat_<float> Hor;
		fs::computeVerAndHor(img, Ver, Hor, floor_height * 0.1);

		// Facadeのsplit linesを求める
		vector<float> y_split;
		fs::getSplitLines(Ver, y_split);
		fs::refineSplitLines(y_split);
		vector<float> x_split;
		fs::getSplitLines(Hor, x_split);
		fs::refineSplitLines(x_split);
		fs::outputFacadeStructure(img, y_split, x_split, filename_prefix + std::string("_subdivision.png"), 1);

		cv::Mat detected_edges;
		cv::Canny(img, detected_edges, 30, 100, 3);
		cv::imwrite("detected_edges.png", detected_edges);
		cv::Mat detected_edges_inverted;
		cv::bitwise_not(detected_edges, detected_edges_inverted);
		cv::imwrite("detected_edges_inverted.png", detected_edges_inverted);

		// facadeの端のエッジを削除する
		int margin = 8;
		for (int r = 0; r < detected_edges.rows; ++r) {
			for (int c = 0; c < detected_edges.cols; ++c) {
				if (r < margin || r >= detected_edges.rows - margin || c < margin || c >= detected_edges.cols - margin) {
					detected_edges.at<unsigned char>(r, c) = 0;
				}
			}
		}
		
		// 各tileの窓の位置を求める
		std::vector<std::vector<fs::WindowPos>> window_rects;
		int window_count = 0;
		window_rects.resize(y_split.size() - 1);
		for (int i = 0; i < y_split.size() - 1; ++i) {
			window_rects[i].resize(x_split.size() - 1);
			for (int j = 0; j < x_split.size() - 1; ++j) {
				cv::Mat tile(img, cv::Rect(x_split[j], y_split[i], x_split[j + 1] - x_split[j], y_split[i + 1] - y_split[i]));
				cv::Mat tile_edges(detected_edges, cv::Rect(x_split[j], y_split[i], x_split[j + 1] - x_split[j], y_split[i + 1] - y_split[i]));
				if (subdivideTile(tile, tile_edges, 10, 1, window_rects[i][j])) {
					window_count++;
				}
				else {
					window_rects[i][j] = fs::WindowPos();
				}
			}
		}
		std::cout << "Window count: " << window_count << std::endl;
		fs::outputFacadeAndWindows(img, y_split, x_split, window_rects, filename_prefix + std::string("_windows.png"));

		/*
		// 窓の位置をalignする
		std::vector<std::vector<fs::WindowPos>> refined_window_rects;
		fs::refine(y_split, x_split, window_rects);
		fs::align(y_split, x_split, window_rects);
		//outputFacadeAndWindows(img, y_split, x_split, window_rects, "facade_windows_refined.png");

		// 各floorのsimilarityを計算する
		vector<cv::Mat> floors;
		vector<cv::Mat> floor_centroids;
		vector<int> floor_labels;
		clusterFloors(img, y_split, 4, floors, floor_labels, floor_centroids);
		//outputFacadeSegmentation(img, y_split, floor_labels, "facade_labeled.png");

		// 最も類似するfacade grammarを探す
		floor_labels = fs::findBestFacadeGrammar(floor_labels);
		y_split = fs::estimateFacadeParams(img, y_split, floor_labels);
		//outputFacadeSegmentation(img, y_split, floor_labels, "facade_labeled_refined.png");
		*/


	}

	/*
	void clusterFloors(const cv::Mat& img, const vector<float>& y_split, int max_cluster, vector<cv::Mat>& floors, vector<int>& labels, vector<cv::Mat>& centroids) {
		floors.resize(y_split.size() - 1);
		for (int i = 0; i < y_split.size() - 1; ++i) {
			int height = y_split[y_split.size() - i - 1] - y_split[y_split.size() - i - 2];
			floors[i] = cv::Mat(img, cv::Rect(0, y_split[y_split.size() - i - 2], img.cols, height));
		}

		cvutils::clusterImages(floors, labels, centroids, max_cluster);

		std::cout << "Facade segmentation:" << std::endl;
		for (int i = 0; i < labels.size(); ++i) {
			std::cout << "class: " << labels[i] << std::endl;
		}
	}
	*/
}
