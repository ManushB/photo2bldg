#include "VanishingPoint.h"
#include <glm/gtx/string_cast.hpp>

namespace vp {

	/**
	 * Compute all three vanishing points, left of the horizontal ones, right of the horizontal ones, and a vertical one.
	 */
	void computeVanishingPoints(const std::vector<VanishingLine>& lines, std::vector<glm::dvec2>& vps) {
		vps.resize(3);

		// compute vp of horizontal left
		std::vector<VanishingLine> lines_horizontal_left;
		for (auto line : lines) {
			if (line.type == VanishingLine::TYPE_HORIZONTAL_LEFT) {
				lines_horizontal_left.push_back(line);
			}
		}
		vps[0] = computeVanishingPoint(lines_horizontal_left);

		// compute vp of horizontal right
		std::vector<VanishingLine> lines_horizontal_right;
		for (auto line : lines) {
			if (line.type == VanishingLine::TYPE_HORIZONTAL_RIGHT) {
				lines_horizontal_right.push_back(line);
			}
		}
		vps[1] = computeVanishingPoint(lines_horizontal_right);

		// compute vp of vertical
		std::vector<VanishingLine> lines_vertical;
		for (auto line : lines) {
			if (line.type == VanishingLine::TYPE_VERTICAL) {
				lines_vertical.push_back(line);
			}
		}
		vps[2] = computeVanishingPoint(lines_vertical);
	}

	/**
	 * Compute a vanishing point in a least squared manner.
	 * Each line is represented by a_1 x + a_2 y = b.
	 * Then, the entire system can be formulated as A[x,y]^T = b,
	 * where A is a N x 2 matrix and b is a N x 1 matrix.
	 * Finally, [x, y] can be computed as A^-1 * b.
	 *
	 * TODO: use RANSAC to remove outliers and get more reliable vanishign point.
	 */
	glm::dvec2 computeVanishingPoint(const std::vector<VanishingLine>& lines) {
		cv::Mat_<double> A(lines.size(), 2);
		cv::Mat_<double> b(lines.size(), 1);
		for (int i = 0; i < lines.size(); ++i) {
			A(i, 0) = lines[i].end.y - lines[i].start.y;
			A(i, 1) = lines[i].start.x - lines[i].end.x;
			b(i, 0) = (lines[i].end.y - lines[i].start.y) * lines[i].start.x + (lines[i].start.x - lines[i].end.x) * lines[i].start.y;
		}

		cv::Mat_<double> invA;
		cv::invert(A, invA, cv::DECOMP_SVD);
		cv::Mat X = invA * b;

		glm::dvec2 result(X.at<double>(0, 0), X.at<double>(1, 0));
		return result;
	}

	void extractCameraParameters(const std::vector<glm::dvec2>& vps, double& f, double& xrot, double& yrot, double& zrot, glm::vec2& center) {
		// compute K and R matrices
		glm::dmat3 K, R;
		vp::extractCameraMatrixByGenApproach(vps, K, R);
		f = K[0][0];
		center = glm::vec2(K[2][0], K[2][1]);

		vp::decomposeRotation(R, xrot, yrot, zrot);
	}

	void extractCameraMatrix(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R) {
		// assume the center of the image be the orthographic projection of the camera center
		glm::dvec2 Oi(0, 0);

		// compute the projection of Oi on the horizontal vanishing line
		glm::dvec2 Vi = vp::nearestPointOnLine(vps[0], vps[1], glm::dvec2(0, 0));

		// compute the length between Vi and the camera center
		double OcVi = sqrt(glm::length(vps[0] - Vi) * glm::length(vps[1] - Vi));

		// compute the folca length by Eq (4) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		double f = sqrt(OcVi * OcVi - glm::dot(Vi - Oi, Vi - Oi));

		// recover the intrinsic matrix by Eq (3) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		K = glm::dmat3();
		K[0][0] = f;
		K[1][1] = f;
		K[2][2] = 1;

		// recover the rotation matrix by Eq (6) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		R = glm::dmat3();
		glm::dvec3 Rx(-vps[0].x, -vps[0].y, f);
		glm::dvec3 Ry(vps[2].x, vps[2].y, -f);
		glm::dvec3 Rz(-vps[1].x, -vps[1].y, f);
		Rx = glm::normalize(Rx);
		Ry = glm::normalize(Ry);
		Rz = glm::normalize(Rz);
		R[0] = Rx;
		R[1] = Ry;
		R[2] = Rz;
		R[1] = glm::cross(R[2], R[0]);
	}

	void extractCameraMatrixT(double f, const glm::dvec2& origin, double camera_ditance, glm::dvec3& T) {
		// define O_c P_1' in Eq (11)
		glm::dvec3 OP1(origin.x, origin.y, -f);

		// compute T
		T = OP1 * (camera_ditance / f);
	}

	void extractCameraMatrixByThreeVPs(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R) {
		// compute the focal length by Eq (18)
		// Note: Eq (18) is incorrect.
		//       The correct form is f = sqrt(| (V_1 - O_i) \dot (V_2 - O_i) |)
		double f = sqrt(abs(glm::dot(vps[0], vps[1])));

		// recover the intrinsic matrix by Eq (12)
		K = glm::dmat3();
		K[0][0] = f;
		K[1][1] = f;
		K[2][2] = -1;

		// recover the rotation matrix by Eq (15)
		cv::Mat_<double> A(5, 2);
		for (int i = 0; i < 2; ++i) {
			A(0, i) = vps[i].x * vps[i].x - vps[2].x * vps[2].x;
			A(1, i) = vps[i].y * vps[i].y - vps[2].y * vps[2].y;
			A(2, i) = vps[i].x * vps[i].y - vps[2].x * vps[2].y;
			A(3, i) = vps[i].x - vps[2].x;
			A(4, i) = vps[i].y - vps[2].y;
		}
		cv::Mat_<double> b(5, 1);
		b(0, 0) = f * f - vps[2].x * vps[2].x;
		b(1, 0) = f * f - vps[2].y * vps[2].y;
		b(2, 0) = -vps[2].x * vps[2].y;
		b(3, 0) = -vps[2].x;
		b(4, 0) = -vps[2].y;

		cv::Mat x = A.inv(cv::DECOMP_SVD) * b;
		//std::cout << x << std::endl;

		float lmdb[3];
		lmdb[0] = sqrt(x.at<double>(0, 0));
		lmdb[1] = sqrt(x.at<double>(1, 0));
		lmdb[2] = sqrt(1 - x.at<double>(0, 0) - x.at<double>(1, 0));

		glm::dmat3 KR;
		KR[0][0] = -vps[0].x * lmdb[0];
		KR[0][1] = -vps[0].y * lmdb[0];
		KR[0][2] = -lmdb[0];
		KR[2][0] = -vps[1].x * lmdb[1];
		KR[2][1] = -vps[1].y * lmdb[1];
		KR[2][2] = -lmdb[1];
		KR[1][0] = vps[2].x * lmdb[1];
		KR[1][1] = vps[2].y * lmdb[1];
		KR[1][2] = lmdb[2];

		R = glm::inverse(K) * KR;
		/*
		for (int i = 0; i < 3; ++i) {
			R[i][2] = -R[i][2];
		}
		*/
	}

	void extractCameraMatrixByGenApproach(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R) {
		cv::Mat_<double> A(2, 2);
		A(0, 0) = vps[0].x - vps[2].x;
		A(0, 1) = vps[0].y - vps[2].y;
		A(1, 0) = vps[0].x - vps[1].x;
		A(1, 1) = vps[0].y - vps[1].y;
		cv::Mat_<double> B(2, 1);
		B(0, 0) = glm::dot(vps[0], vps[1]) - glm::dot(vps[1], vps[2]);
		B(1, 0) = glm::dot(vps[0], vps[2]) - glm::dot(vps[1], vps[2]);

		cv::Mat O = A.inv(cv::DECOMP_SVD) * B;
		glm::dvec2 o(O.at<double>(0, 0), O.at<double>(1, 0));

		double f = sqrt(abs(glm::dot(vps[0] - o, vps[1] - o)));

		// recover the intrinsic matrix by Eq (12)
		K = glm::dmat3();
		K[0][0] = f;
		K[1][1] = f;
		K[2][0] = o.x;
		K[2][1] = o.y;
		K[2][2] = -1;

		// recover the rotation matrix by Eq (6) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		R = glm::dmat3();
		glm::dvec3 Rx(-(vps[0].x - o.x), -(vps[0].y - o.y), f);
		glm::dvec3 Ry(vps[2].x - o.x, vps[2].y - o.y, -f);
		glm::dvec3 Rz(-(vps[1].x - o.x), -(vps[1].y - o.y), f);
		Rx = glm::normalize(Rx);
		Ry = glm::normalize(Ry);
		Rz = glm::normalize(Rz);
		R[0] = Rx;
		R[1] = Ry;
		R[2] = Rz;
		R[1] = glm::cross(R[2], R[0]);
	}

	/**
	 * Return the projected point of the specified 3d point onto the image plane.
	 * Note that the coordinates of the projected point is normalized to [-1, 1].
	 */
	glm::dvec2 projectPoint(const glm::dmat4& mvpMatrix, const glm::dvec3& p) {
		glm::dvec4 pp = mvpMatrix * glm::dvec4(p, 1);
		return glm::dvec2(pp.x / pp.w, pp.y / pp.w);
	}

	/**
	 * Create a rotation matrix.
	 * Note: glm mat4 contains element values in column major. For instance,
	 *       X[1][2] means a element at 3rd row and 2nd column.
	 *       This is different from a typical notation, so be careful!
	 *
	 * @param xrot		rotation around X axis in radian
	 * @param yrot		rotation around Y axis in radian
	 * @param zrot		rotation around Z axis in radian
	 * @return			rotation matrix
	 */
	glm::dmat3 composeRotation(double xrot, double yrot, double zrot) {
		glm::dmat3 X;
		X[1][1] = cos(xrot);
		X[2][1] = -sin(xrot);
		X[1][2] = sin(xrot);
		X[2][2] = cos(xrot);

		glm::dmat3 Y;
		Y[0][0] = cos(yrot);
		Y[2][0] = sin(yrot);
		Y[0][2] = -sin(yrot);
		Y[2][2] = cos(yrot);

		glm::dmat3 Z;
		Z[0][0] = cos(zrot);
		Z[1][0] = -sin(zrot);
		Z[0][1] = sin(zrot);
		Z[1][1] = cos(zrot);

		return X * Y * Z;
	}

	/**
	 * Convert the rotation matrix to three rotation angles around axes.
	 * ToDO: Degenerate cases are not supported yet.
	 *       See the below to support them.
	 *       http://www.staff.city.ac.uk/~sbbh653/publications/euler.pdf
	 *
	 * @param R		rotation matrix
	 * @param xrot	rotation around X axis in radian
	 * @param yrot	rotation around Y axis in radian
	 * @param zrot	rotation around Z axis in radian
	 */
	void decomposeRotation(const glm::dmat3& R, double& xrot, double& yrot, double& zrot) {
		xrot = atan2(-R[2][1], R[2][2]);
		yrot = atan2(R[2][0], std::hypot(R[2][1], R[2][2]));
		zrot = atan2(-R[1][0], R[0][0]);
	}

	glm::dvec2 nearestPointOnLine(const glm::dvec2& p1, const glm::dvec2& p2, const glm::dvec2& p) {
		glm::dvec2 dir = p2 - p1;
		return p1 + dir * glm::dot(p - p1, dir) / glm::dot(dir, dir);
	}

	double deg2rad(double deg) {
		return deg / 180.0 * M_PI;
	}

	double rad2deg(double rad) {
		return rad / M_PI * 180.0;
	}

}