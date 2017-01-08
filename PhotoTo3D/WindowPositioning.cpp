#include "WindowPositioning.h"
#include "Utils.h"

namespace wp {
	
	obj_function::obj_function(cv::Mat dist_map, float initial_x1, float initial_y1, float initial_x2, float initial_y2) {
		this->dist_map = dist_map;
		this->initial_x1 = initial_x1;
		this->initial_y1 = initial_y1;
		this->initial_x2 = initial_x2;
		this->initial_y2 = initial_y2;
	}

	double obj_function::operator() (const column_vector& arg) const {
		/*
		float x1 = arg(0, 0) * 227;
		float y1 = arg(1, 0) * 227;
		float x2 = arg(2, 0) * 227;
		float y2 = arg(3, 0) * 227;
		if (x1 < 0) x1 = 0;
		if (x1 >= 227) x1 = 226;
		if (y1 < 0) y1 = 0;
		if (y1 >= 227) y1 = 226;
		if (x2 < 0) x2 = 0;
		if (x2 >= 227) x2 = 226;
		if (y2 < 0) y2 = 0;
		if (y2 >= 227) y2 = 226;
		*/
		return distance(dist_map, initial_x1, initial_y1, initial_x2, initial_y2, arg(0, 0), arg(1, 0), arg(2, 0), arg(3, 0));
	}



	std::vector<float> parameterEstimation(boost::shared_ptr<Regression> regression, const cv::Mat& tile, bool refine, int maxIters) {
		// prediction
		std::vector<float> init_params = regression->Predict(tile);
		
		std::vector<float> params = init_params;
		
		if (refine) {
			// edges
			cv::Mat edges;
			cv::Canny(tile, edges, 30, 150);

			// invert
			cv::bitwise_not(edges, edges);

			// distance transform
			cv::Mat dist;
			cv::distanceTransform(edges, dist, cv::DIST_L2, 3);

			// BOBYQA
			column_vector starting_point(params.size());
			column_vector lower_bound(params.size());
			column_vector upper_bound(params.size());
			for (int k = 0; k < params.size(); ++k) {
				starting_point(k, 0) = params[k];
				lower_bound(k, 0) = params[k] - 0.1;
				upper_bound(k, 0) = params[k] + 0.1;
			}
			
			try {
				find_min_bobyqa(obj_function(dist, params[0], params[1], params[2], params[3]),
					starting_point,
					10,
					lower_bound,
					upper_bound,
					0.03,
					0.002,
					maxIters
					);
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			
			for (int k = 0; k < params.size(); ++k) {
				params[k] = starting_point(k, 0);
			}
		}

		// check the range
		for (int i = 0; i < params.size(); ++i) {
			if (params[i] < 0) params[i] = 0;
			if (params[i] > 1) params[i] = 1;
		}

		// check the order
		if (params[0] > params[2]) std::swap(params[0], params[2]);
		if (params[1] > params[3]) std::swap(params[1], params[3]);

		return params;
	}

	float distance(const cv::Mat& dist_map, float initial_x1, float initial_y1, float initial_x2, float initial_y2, float x1, float y1, float x2, float y2) {
		float diff = 0;
		int cnt = 0;

		initial_x1 *= dist_map.cols;
		initial_y1 *= dist_map.rows;
		initial_x2 *= dist_map.cols;
		initial_y2 *= dist_map.rows;
		x1 *= dist_map.cols;
		y1 *= dist_map.rows;
		x2 *= dist_map.cols;
		y2 *= dist_map.rows;

		if (initial_x1 < 0) initial_x1 = 0;
		if (initial_y1 < 0) initial_y1 = 0;
		if (initial_x2 >= dist_map.cols) initial_x2 = dist_map.cols - 1;
		if (initial_y2 >= dist_map.rows) initial_y2 = dist_map.rows - 1;
		if (x1 < 0) x1 = 0;
		if (x1 >= dist_map.cols) x1 = dist_map.cols - 1;
		if (y1 < 0) y1 = 0;
		if (y1 >= dist_map.rows) y1 = dist_map.rows - 1;
		if (x2 < 0) x2 = 0;
		if (x2 >= dist_map.cols) x2 = dist_map.cols - 1;
		if (y2 < 0) y2 = 0;
		if (y2 >= dist_map.rows) y2 = dist_map.rows - 1;

		if (x1 > x2) std::swap(x1, x2);
		if (y1 > y2) std::swap(y1, y2);

		for (int r = y1; r < y2; ++r) {
			diff += dist_map.at<float>(r, x1);
			diff += dist_map.at<float>(r, x2);
			cnt += 2;
		}
		for (int c = x1; c < x2; ++c) {
			diff += dist_map.at<float>(y1, c);
			diff += dist_map.at<float>(y2, c);
			cnt += 2;
		}

		//std::cout << "dist: " << diff / cnt << std::endl;


		return diff / cnt + (abs(x1 - initial_x1) + abs(y1 - initial_y1) + abs(x2 - initial_x2) + abs(y2 - initial_y2)) * 0.2;
	}
}