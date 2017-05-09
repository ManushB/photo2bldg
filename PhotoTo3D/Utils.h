#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

namespace utils {
	
	bool isNumber(const std::string& str);
	void computeMean(const std::vector<std::vector<float>>& values, std::vector<float>& mean);
	void computeVariance(const std::vector<std::vector<float>>& values, const std::vector<float>& mean, std::vector<float>& var);
	float median(std::vector<float> list);
	float stddev(std::vector<float> list);
	float mean(std::vector<float> list);

	double genRand();
	double genRand(double v);
	double genRand(double a, double b);
	int genIntRand(int v);
	int genIntRand(int a, int b);
	double genNormal(double mu, double sigma);
	double gause(double u, double sigma);
	bool segmentSegmentIntersect(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, float *tab, float *tcd, bool segmentOnly, glm::vec2& intPoint);

	glm::vec2 projectPoint(int screen_width, int screen_height, const glm::vec3& p, const glm::mat4& mvpMatrix);

	float computeArea(std::vector<cv::Point2f> pts);

	void extractEdges(const cv::Mat& img, std::vector<std::pair<glm::vec2, glm::vec2>>& edges);
	void cleanEdges(std::vector<std::pair<glm::vec2, glm::vec2>>& edges, float maxLineGap, float theta);
	void cleanContours(std::vector<std::pair<glm::vec2, glm::vec2>>& edges, float maxLineGap, float theta);
	void grayScale(const cv::Mat& img, cv::Mat& grayImg);
	void scaleToFit(const cv::Mat& src, cv::Mat& dst, const cv::Size& size);
	void distanceMap(const cv::Mat& img, cv::Mat& distMap);

	void output_vector(const std::vector<float>& values);
}
