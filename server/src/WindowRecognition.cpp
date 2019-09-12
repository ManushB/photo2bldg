#include "WindowRecognition.h"
#include <iostream>
#include "facadeA.h"
#include "facadeB.h"
#include "facadeC.h"
#include "facadeD.h"
#include "facadeE.h"
#include "facadeF.h"
#include "facadeG.h"
#include "facadeH.h"

namespace winrec {

	std::vector<int> recognition(const cv::Mat& img, int facade_id, const std::vector<float>& y_splits, const std::vector<float>& x_splits, std::vector<std::vector<fs::WindowPos>>& win_rects, boost::shared_ptr<Classifier> classifier) {
		// cluster the tiles based on the grammar
        int num_window_types = 0;

        if (facade_id == 0) {
			num_window_types = FacadeA::clusterWindowTypes(win_rects);
		}
		else if (facade_id == 1) {
            num_window_types = FacadeB::clusterWindowTypes(win_rects);
        }
		else if (facade_id == 2) {
			num_window_types = FacadeC::clusterWindowTypes(win_rects);
		}
		else if (facade_id == 3) {
			num_window_types = FacadeD::clusterWindowTypes(win_rects);
		}
		else if (facade_id == 4) {
			num_window_types = FacadeE::clusterWindowTypes(win_rects);
		}
		else if (facade_id == 5) {
			num_window_types = FacadeF::clusterWindowTypes(win_rects);
		}
		else if (facade_id == 6) {
			num_window_types = FacadeG::clusterWindowTypes(win_rects);
		}
		else if (facade_id == 7) {
			num_window_types = FacadeH::clusterWindowTypes(win_rects);
		}

        //std::cout << "window:" << std::endl;
		std::map<int, std::vector<int>> win_type_votes;
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {

                //if (j > 0) std::cout << ", ";
				if (win_rects[i][j].valid == fs::WindowPos::VALID) {
					int x = x_splits[j];
					int w = x_splits[j + 1] - x_splits[j];
					int y = y_splits[i];
					int h = y_splits[i + 1] - y_splits[i];

					cv::Mat tile_img(img, cv::Rect(x, y, w, h));

					cv::Mat tile_img227;
					cv::resize(tile_img, tile_img227, cv::Size(227, 227));

					//////////////////////////////////////////////////////////////////
					// DEBUG
					//char filename[256];
					//sprintf(filename, "results/tile_%d_%d.png", i, j);
					//cv::imwrite(filename, tile_img227);
					//////////////////////////////////////////////////////////////////


                    std::vector<Prediction> win_predictions = classifier->Classify(tile_img227, 13);
					int win_id = win_predictions[0].first;
					//std::cout << win_id + 1 << "(" << win_rects[i][j].type << ")";

					win_type_votes[win_rects[i][j].type].push_back(win_id);
				}
				else {
					//std::cout << " " << "(" << win_rects[i][j].type << ")";
				}
			}
			//std::cout << std::endl;
		}
		//std::cout << std::endl;

        // find the maximum vote for each window group
		std::vector<int> selected_win_types;
		for (int i = 0; i < num_window_types; ++i) {
			std::map<int, int> votes;	// mapping from window type to #votes
			int max_votes = 0;
			int selected_win_type = 0;

			for (int k = 0; k < win_type_votes[i].size(); ++k) {
				votes[win_type_votes[i][k]]++;
				if (votes[win_type_votes[i][k]] > max_votes) {
					max_votes = votes[win_type_votes[i][k]];
					selected_win_type = win_type_votes[i][k];
				}
			}

			selected_win_types.push_back(selected_win_type);
			std::cout << "window group=" << i << ": type=" << selected_win_type + 1 << std::endl;
		}

		return selected_win_types;
	}

}
