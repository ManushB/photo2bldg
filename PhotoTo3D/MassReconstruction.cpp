#include "MassReconstruction.h"
#include "RenderManager.h"
#include "GLWidget3D.h"
#include "Grammar.h"
#include "Utils.h"

namespace massrec {

	int NUM_GRAMMARS = 10;

	obj_function::obj_function(GLWidget3D* glWidget, cga::Grammar* grammar, cv::Mat silhouette_dist_map, float xrotMax, float xrotMin, float yrotMax, float yrotMin, float zrotMax, float zrotMin, float fovMax, float fovMin, float oxMax, float oxMin, float oyMax, float oyMin, float xMax, float xMin, float yMax, float yMin) {
		this->glWidget = glWidget;
		this->grammar = grammar;
		this->silhouette_dist_map = silhouette_dist_map;
		this->xrotMin = xrotMin;
		this->xrotMax = xrotMax;
		this->yrotMin = yrotMin;
		this->yrotMax = yrotMax;
		this->zrotMin = zrotMin;
		this->zrotMax = zrotMax;
		this->fovMin = fovMin;
		this->fovMax = fovMax;
		this->oxMin = oxMin;
		this->oxMax = oxMax;
		this->oyMin = oyMin;
		this->oyMax = oyMax;
		this->xMin = xMin;
		this->xMax = xMax;
		this->yMin = yMin;
		this->yMax = yMax;
	}

	double obj_function::operator() (const column_vector& arg) const {
		//static int cnt = 0;

		std::vector<float> params;
		for (int k = 0; k < arg.size(); ++k) {
			params.push_back(arg(k, 0));
		}

		glWidget->setupCamera(params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

		cv::Mat rendered_image;
		glWidget->renderImage(grammar, &std::vector<float>(params.begin() + 8, params.end()), rendered_image, false, false);

		/*
		QString filename = QString("results/rendered_%1.png").arg(cnt);
		cv::imwrite(filename.toUtf8().constData(), rendered_image);
		cnt++;
		*/

		double diff = glWidget->distanceMap(rendered_image, silhouette_dist_map);

		return diff;
	}

	int recognition(boost::shared_ptr<Classifier> classifier, int image_size, int screen_width, int screen_height, std::vector<vp::VanishingLine> silhouette) {
		glm::vec2 scale((float)image_size / screen_width, (float)image_size / screen_height);
		cv::Mat input = cv::Mat(image_size, image_size, CV_8UC3, cv::Scalar(255, 255, 255));
		for (auto stroke : silhouette) {
			cv::Point p1(stroke.start.x * scale.x, stroke.start.y * scale.y);
			cv::Point p2(stroke.end.x * scale.x, stroke.end.y * scale.y);

			cv::line(input, p1, p2, cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
		}


		std::vector<Prediction> predictions = classifier->Classify(input, NUM_GRAMMARS);
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// DEBUG
		std::cout << "--- mass recognition ---------------------------" << std::endl;
		for (int i = 0; i < predictions.size(); ++i) {
			std::cout << predictions[i].first << " (" << predictions[i].second << ")" << std::endl;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////


		return predictions[0].first;

	}

	std::vector<float> parameterEstimation(GLWidget3D* glWidget, boost::shared_ptr<Regression> regression, cga::Grammar* grammar, const std::vector<vp::VanishingLine>& silhouette, int image_size, float cameraDistanceBase, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float zrotMin, float zrotMax, float fovMin, float fovMax, float oxMin, float oxMax, float oyMin, float oyMax, float xMin, float xMax, float yMin, float yMax, int silhouette_line_type, bool refinement, int maxIters, int refinement_method) {
		time_t start = clock();

		std::cout << "-----------------------------------------------------" << std::endl;

		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;
		glWidget->camera.distanceBase = cameraDistanceBase;

		/*
		// adjust the original background image such that the ratio of width to height is equal to the ratio of the window
		float imageScale = std::min((float)glWidget->width() / imageOrig.width(), (float)height() / imageOrig.height());
		resizeImageCanvasSize(imageOrig, width() / imageScale, height() / imageScale);

		// grammar id
		glWidget->grammar_ids["mass"] = gramamr_id;
		*/

		// create image of silhouette
		cv::Mat silhouette_image(glWidget->height(), glWidget->width(), CV_8UC1, cv::Scalar(255));
		for (auto line : silhouette) {
			cv::line(silhouette_image, cv::Point(line.start.x, line.start.y), cv::Point(line.end.x, line.end.y), cv::Scalar(0), 1);
		}

		// create distance map of silhouette
		cv::Mat silhouette_dist_map;
		cv::threshold(silhouette_image, silhouette_image, 254, 255, CV_THRESH_BINARY);
		cv::distanceTransform(silhouette_image, silhouette_dist_map, CV_DIST_L2, 3);
		silhouette_dist_map.convertTo(silhouette_dist_map, CV_64F);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// parameter estimation by CNN
		std::vector<float> best_params;
		double diff_min = std::numeric_limits<double>::max();

		// create input image to CNN
		glm::vec2 scale((float)image_size / glWidget->width(), (float)image_size / glWidget->height());
		cv::Mat input = cv::Mat(image_size, image_size, CV_8UC3, cv::Scalar(255, 255, 255));
		for (auto stroke : silhouette) {
			cv::Point p1(stroke.start.x * scale.x, stroke.start.y * scale.y);
			cv::Point p2(stroke.end.x * scale.x, stroke.end.y * scale.y);

			if (silhouette_line_type == 0) {
				cv::line(input, p1, p2, cv::Scalar(0, 0, 0), 1, cv::LINE_8);
			}
			else {
				cv::line(input, p1, p2, cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
			}
		}

		// estimate paramter values by CNN
		std::vector<float> params = regression->Predict(input);
		//utils::output_vector(params);

		// 固定の場合には、ダミーでパラメータを入れちゃう
		if (xrotMin == xrotMax) {
			params.insert(params.begin() + 0, 0.5);
		}
		if (yrotMin == yrotMax) {
			params.insert(params.begin() + 1, 0.5);
		}
		if (zrotMin == zrotMax) {
			params.insert(params.begin() + 2, 0.5);
		}
		if (fovMin == fovMax) {
			params.insert(params.begin() + 3, 0.5);
		}
		if (oxMin == oxMax) {
			params.insert(params.begin() + 4, 0.5);
		}
		if (oyMin == oyMax) {
			params.insert(params.begin() + 5, 0.5);
		}
		if (xMin == xMax) {
			params.insert(params.begin() + 6, 0.5);
		}
		if (yMin == yMax) {
			params.insert(params.begin() + 7, 0.5);
		}

		// setup the camera
		glWidget->setupCamera(params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

		cv::Mat rendered_image;
		glWidget->renderImage(grammar, &std::vector<float>(params.begin() + 8, params.end()), rendered_image);

		double diff = glWidget->distanceMap(rendered_image, silhouette_dist_map);
		if (diff < diff_min) {
			diff_min = diff;
			best_params = params;
		}

		std::vector<float> init_params = best_params;

		std::cout << "Initial estimate:" << std::endl;
		utils::output_vector(best_params);
		std::cout << "Dist = " << diff_min << std::endl;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// refine the parameter estimation
		if (refinement) {
			if (refinement_method == 0) {	// BOBYQA
				column_vector starting_point(best_params.size());
				column_vector lower_bound(best_params.size());
				column_vector upper_bound(best_params.size());
				for (int k = 0; k < best_params.size(); ++k) {
					starting_point(k, 0) = best_params[k];
					lower_bound(k, 0) = best_params[k] - 0.1;
					upper_bound(k, 0) = best_params[k] + 0.1;
				}

				try {
					find_min_bobyqa(obj_function(glWidget, grammar, silhouette_dist_map, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin),
						starting_point,
						23,
						lower_bound,
						upper_bound,
						0.003,
						0.00001,
						maxIters
						);
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}

				for (int k = 0; k < best_params.size(); ++k) {
					best_params[k] = starting_point(k, 0);
				}
			}
			else {	// Random Walk
				double diff_min = std::numeric_limits<double>::max();
				printf("find good values by random sampling: ");
				for (int iter = 0; iter < maxIters; ++iter) {
					printf("\rfind good initial values by random sampling: %d", iter + 1);

					// randomly pick the initial values around the initial estimate by CNN
					std::vector<float> cur_params = init_params;
					if (iter > 0) {
						for (int k = 0; k < cur_params.size(); ++k) {
							cur_params[k] += utils::genRand(-0.02, +0.02);
						}
					}

					// setup the camera
					glWidget->setupCamera(cur_params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

					cv::Mat rendered_image;
					glWidget->renderImage(grammar, &std::vector<float>(cur_params.begin() + 8, cur_params.end()), rendered_image);

					// compute the difference
					double diff = glWidget->distanceMap(rendered_image, silhouette_dist_map);

					// coordinate descent
					float delta = 0.002;
					for (int iter2 = 0; iter2 < 10; ++iter2) {
						for (int k = 0; k < cur_params.size(); ++k) {
							// option 1
							std::vector<float> next_params1 = cur_params;
							next_params1[k] -= delta;
							glWidget->setupCamera(next_params1, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);
							cv::Mat rendered_image1;
							double diff1 = std::numeric_limits<double>::max();
							if (glWidget->renderImage(grammar, &std::vector<float>(next_params1.begin() + 8, next_params1.end()), rendered_image1, true, true)) {
								diff1 = glWidget->distanceMap(rendered_image1, silhouette_dist_map);
							}

							// option 2
							std::vector<float> next_params2 = cur_params;
							next_params2[k] += delta;
							glWidget->setupCamera(next_params2, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);
							cv::Mat rendered_image2;
							double diff2 = std::numeric_limits<double>::max();
							if (glWidget->renderImage(grammar, &std::vector<float>(next_params2.begin() + 8, next_params2.end()), rendered_image2, true, true)) {
								diff2 = glWidget->distanceMap(rendered_image2, silhouette_dist_map);
							}

							if (diff1 < diff2 && diff1 < diff) {
								diff = diff1;
								cur_params = next_params1;
							}
							else if (diff2 < diff1 && diff2 < diff) {
								diff = diff2;
								cur_params = next_params2;
							}
						}

						delta *= 0.8;
					}

					if (diff < diff_min) {
						diff_min = diff;
						best_params = cur_params;
					}
				}
				printf("\n");
			}

			// set the camera
			glWidget->setupCamera(best_params, xrotMax, xrotMin, yrotMax, yrotMin, zrotMax, zrotMin, fovMax, fovMin, oxMax, oxMin, oyMax, oyMin, xMax, xMin, yMax, yMin);

			cv::Mat rendered_image;
			glWidget->renderImage(grammar, &std::vector<float>(best_params.begin() + 8, best_params.end()), rendered_image, false, false);
			diff_min = glWidget->distanceMap(rendered_image, silhouette_dist_map);

			std::cout << "Refined estimate:" << std::endl;
			utils::output_vector(best_params);
			std::cout << "Dist = " << diff_min << std::endl;
		}
		
		time_t end = clock();
		std::cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << "sec." << std::endl;

		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;

		return best_params;
	}

}