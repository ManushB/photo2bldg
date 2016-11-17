#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <opencv2/opencv.hpp>

namespace vp {

	const double M_PI = 3.1415926535897932384626433832795028841971693993751;

	class VanishingLine {
	public:
		static enum { TYPE_HORIZONTAL_LEFT = 0, TYPE_HORIZONTAL_RIGHT = 1, TYPE_VERTICAL = 2 };
	public:
		glm::dvec2 start;
		glm::dvec2 end;
		int type;

	public:
		VanishingLine() : start(0, 0), end(0, 0), type(0) {}
		VanishingLine(double x1, double y1, double x2, double y2, int type) : start(x1, y1), end(x2, y2), type(type) {}
	};

	class VanishingPoint
	{
	public:
		VanishingPoint() {}
	};

	void computeVanishingPoints(const std::vector<VanishingLine>& lines, std::vector<glm::dvec2>& vps);
	glm::dvec2 computeVanishingPoint(const std::vector<VanishingLine>& lines);
	void extractCameraParameters(const std::vector<glm::dvec2>& vps, double& f, double& xrot, double& yrot, double& zrot, glm::vec2& center);
	void extractCameraMatrix(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R);
	void extractCameraMatrixT(double f, const glm::dvec2& origin, double camera_ditance, glm::dvec3& T);
	void extractCameraMatrixByThreeVPs(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R);
	void extractCameraMatrixByGenApproach(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R);
	glm::dvec2 projectPoint(const glm::dmat4& mvpMatrix, const glm::dvec3& p);
	glm::dmat3 composeRotation(double xrot, double yrot, double zrot);
	void decomposeRotation(const glm::dmat3& R, double& xrot, double& yrot, double& zrot);
	glm::dvec2 nearestPointOnLine(const glm::dvec2& p1, const glm::dvec2& p2, const glm::dvec2& p);
	double deg2rad(double deg);
	double rad2deg(double rad);
}