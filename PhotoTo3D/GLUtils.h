#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Vertex.h"
#include <vector>

namespace glutils {

const double M_PI = 3.1415926535897932384626433832795028841971693993751;

class BoundingBox {
public:
	glm::vec3 minPt;
	glm::vec3 maxPt;

public:
	BoundingBox();
	BoundingBox(const std::vector<glm::vec2>& points);
	BoundingBox(const std::vector<glm::vec3>& points);
	BoundingBox(const std::vector<std::vector<glm::vec3> >& points);
	void addPoint(const glm::vec2& point);
	void addPoint(const glm::vec3& point);
	float sx() { return maxPt.x - minPt.x; }
	float sy() { return maxPt.y - minPt.y; }
	float sz() { return maxPt.z - minPt.z; }
	glm::vec3 center() { return (maxPt + minPt) * 0.5f; }
	bool contains(const glm::vec2& point, float threshold);
	bool contains(const glm::vec3& point, float threshold);
};

class Face {
public:
	std::string name;
	std::vector<Vertex> vertices;
	std::string texture;
	glm::vec4 backupColor;
	BoundingBox bbox;
	std::string grammar_type;

public:
	Face() {}
	Face(const std::string& name, const std::string& grammar_type, const std::vector<Vertex>& vertices);
	Face(const std::string& name, const std::string& grammar_type, const std::vector<Vertex>& vertices, const std::string& texture);

	void select();
	void unselect();

	Face rotate(float rad, const glm::vec3& axis);
};

// geometry computation
bool isWithinPolygon(const glm::vec2& p, const std::vector<glm::vec2>& points);
float area(const std::vector<glm::vec2>& points);
void offsetPolygon(const std::vector<glm::vec2>& points, float offsetDistance, std::vector<glm::vec2>& offset_points);
float distance(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, bool segmentOnly = false);
float distance(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
glm::vec3 lineLineIntersection(const glm::vec3& p1, const glm::vec3& v1, const glm::vec3& p2, const glm::vec3& v2, float weight1 = 0.5f, float weight2 = 0.5f);
glm::vec3 rayPlaneIntersection(const glm::vec3& a, const glm::vec3& v, const glm::vec3& p, const glm::vec3& n);
bool rayTriangleIntersection(const glm::vec3& a, const glm::vec3& v, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, glm::vec3& intPt);
glm::vec2 barycentricCoordinates(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p);

// mesh generation
void drawCircle(float r1, float r2, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices = 12);
void drawCircle(float r1, float r2, float texWidth, float texHeight, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices = 12);
void drawQuad(float w, float h, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawQuad(float w, float h, const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, const glm::vec2& t4, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawPolygon(const std::vector<glm::vec3>& points, const glm::vec4& color, const std::vector<glm::vec2>& texCoords, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawPolygon(const std::vector<glm::vec3>& points, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, const std::vector<glm::vec2>& texCoords, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawConcavePolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawConcavePolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, const std::vector<glm::vec2>& texCoords, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawGrid(float width, float height, float cell_size, const glm::vec4& lineColor, const glm::vec4& backgroundColor, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawBox(float length_x, float length_y, float length_z, glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawSphere(float radius, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawEllipsoid(float r1, float r2, float r3, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawCylinderX(float radius1, float radius2, float h, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices = 12);
void drawCylinderY(float radius1, float radius2, float h, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices = 12);
void drawCylinderZ(float radius1, float radius2, float radius3, float radius4, float h, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices = 12);
void drawArrow(float radius, float length, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawAxes(float radius, float length, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawTube(std::vector<glm::vec3>& points, float radius, const glm::vec4& color, std::vector<Vertex>& vertices, int slices = 12);
void drawCurvilinearMesh(int numX, int numY, std::vector<glm::vec3>& points, const glm::vec4& color, const glm::mat4& mat, std::vector<Vertex>& vertices);

float deg2rad(float degree);

}
