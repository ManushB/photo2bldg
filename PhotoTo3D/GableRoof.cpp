#include "GableRoof.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/create_straight_skeleton_2.h>
#include "GLUtils.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "CGA.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2 KPoint;
typedef CGAL::Polygon_2<K> Polygon_2 ;
typedef CGAL::Straight_skeleton_2<K> Ss ;
typedef boost::shared_ptr<Ss> SsPtr ;

namespace cga {

GableRoof::GableRoof(const std::string& name, const std::string& grammar_type, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float angle, const glm::vec3& color) {
	this->_active = true;
	this->_axiom = false;
	this->_name = name;
	this->_grammar_type = grammar_type;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_angle = angle;
	this->_color = color;
}

boost::shared_ptr<Shape> GableRoof::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new GableRoof(*this));
	copy->_name = name;
	return copy;
}

// To be fixed!!!
void GableRoof::comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes) {
	// hack for square
	if (_points.size() == 4 && fabs(glm::length(_points[0] - _points[1]) - glm::length(_points[1] - _points[2])) < 0.01 && glm::dot(_points[2] - _points[1], _points[1] - _points[0]) < 0.01) {
		float width = glm::length(_points[0] - _points[1]);
		float depth = glm::length(_points[3] - _points[0]);
		float height = tanf(_angle / 180.0f * M_PI) * depth * 0.5f;

		float rot_x = atan2f(height, depth * 0.5f);
		glm::mat4 mat = glm::rotate(_modelMat, rot_x, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, width, sqrt(depth * 0.5 * depth * 0.5 + height * height), _color)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(width, depth, 0)), M_PI, glm::vec3(0, 0, 1)), rot_x, glm::vec3(1, 0, 0));
		shapes.push_back(boost::shared_ptr<Shape>(new Rectangle(name_map.at("top"), _grammar_type, _pivot, mat, width, sqrt(depth * 0.5 * depth * 0.5 + height * height), _color)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(0, depth, 0)), -M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));
		std::vector<glm::vec2> pts;
		pts.push_back(glm::vec2(0, 0));
		pts.push_back(glm::vec2(depth, 0));
		pts.push_back(glm::vec2(depth * 0.5, height));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, pts, _color, _texture)));

		mat = glm::rotate(glm::rotate(glm::translate(_modelMat, glm::vec3(width, 0, 0)), M_PI * 0.5f, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));
		pts.clear();
		pts.push_back(glm::vec2(0, 0));
		pts.push_back(glm::vec2(depth, 0));
		pts.push_back(glm::vec2(depth * 0.5, height));
		shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, pts, _color, _texture)));

		return;
	}

	Polygon_2 poly;
	for (int i = 0; i < _points.size(); ++i) {
		poly.push_back(KPoint(_points[i].x, _points[i].y));
	}

	// You can pass the polygon via an iterator pair
	SsPtr iss = CGAL::create_interior_straight_skeleton_2(poly);

	std::map<int, glm::vec2> pts_conv;

	// compute the points conversion for the side face (this is only for the triangle)
	for (auto face = iss->faces_begin(); face != iss->faces_end(); ++face) {
		// count num of edges
		int num_edges= 0;
		{
			auto edge0 = face->halfedge();
			auto edge = edge0;
			do {
				num_edges++;
			} while ((edge = edge->next()) != edge0);
		}

		if (num_edges == 3) {
			auto edge0 = face->halfedge();
			auto edge = edge0;
			glm::vec2 p0, p1, p2;

			do {
				auto head = edge->vertex();
				auto tail = edge->opposite()->vertex();

				if (edge->is_bisector()) {
					if (edge->is_inner_bisector()) { // 外側に接続されていない分割線
					} else { // 外側と接続されている分割線
						p2 = glm::vec2(head->point().x(), head->point().y());
						pts_conv[head->id()] = (p0 + p1) * 0.5f; // to be fixed
					}
				} else { // 一番外側のボーダー
					p0 = glm::vec2(tail->point().x(), tail->point().y());
					p1 = glm::vec2(head->point().x(), head->point().y());
				}
			} while ((edge = edge->next()) != edge0);
		}
	}

	// create a face for eacy polygon
	for (auto face = iss->faces_begin(); face != iss->faces_end(); ++face) {
		auto edge0 = face->halfedge();
		auto edge = edge0;

		// 最初のエッジを保存する
		glm::vec2 p0, p1;
		bool is_first_edge = true;

		std::vector<glm::vec3> pts3d;

		do {
			auto head = edge->vertex();
			auto tail = edge->opposite()->vertex();

			if (is_first_edge) {
				p0 = glm::vec2(tail->point().x(), tail->point().y());
				p1 = glm::vec2(head->point().x(), head->point().y());
				pts3d.push_back(glm::vec3(p0, 0));
				pts3d.push_back(glm::vec3(p1, 0));

				is_first_edge = false;
			} else {
				glm::vec2 p2 = glm::vec2(head->point().x(), head->point().y());

				if (p2 != p0) {
					// p2の高さを計算
					float z = glutils::distance(p0, p1, p2) * tanf(_angle * M_PI / 180.0f);

					if (pts_conv.find(head->id()) != pts_conv.end()) {
						p2 = pts_conv[head->id()];
					}

					pts3d.push_back(glm::vec3(p2, z));
				}
			}
		} while ((edge = edge->next()) != edge0);

		glm::vec3 normal = glm::normalize(glm::cross(pts3d[1] - pts3d[0], pts3d[2] - pts3d[0]));

		if (glm::dot(glm::vec3(0, 0, 1), normal) < 0.01f) {	// vertical face
			if (name_map.find("side") != name_map.end() && name_map.at("side") != "NIL") {
				float rot_z = atan2f(pts3d[1].y - pts3d[0].y, pts3d[1].x - pts3d[0].x);
				glm::mat4 convMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(), pts3d[0]), rot_z, glm::vec3(0, 0, 1)), M_PI * 0.5f, glm::vec3(1, 0, 0));
				glm::mat4 invMat = glm::inverse(convMat);
				glm::mat4 mat = _modelMat * convMat;

				std::vector<glm::vec2> pts2d;
				for (int i = 0; i < pts3d.size(); ++i) {
					pts2d.push_back(glm::vec2(invMat * glm::vec4(pts3d[i], 1)));
				}

				shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("side"), _grammar_type, _pivot, mat, pts2d, _color, _texture)));
			}
		}
		else {	// top face
			if (name_map.find("top") != name_map.end() && name_map.at("top") != "NIL") {
				float rot_x = atan2f(sqrt(normal.x * normal.x + normal.y * normal.y), normal.z);
				float rot_z = atan2f(pts3d[1].y - pts3d[0].y, pts3d[1].x - pts3d[0].x);
				glm::mat4 convMat = glm::rotate(glm::rotate(glm::translate(glm::mat4(), pts3d[0]), rot_z, glm::vec3(0, 0, 1)), rot_x, glm::vec3(1, 0, 0));
				glm::mat4 invMat = glm::inverse(convMat);
				glm::mat4 mat = _modelMat * convMat;

				std::vector<glm::vec2> pts2d;
				for (int i = 0; i < pts3d.size(); ++i) {
					pts2d.push_back(glm::vec2(invMat * glm::vec4(pts3d[i], 1)));
				}

				shapes.push_back(boost::shared_ptr<Shape>(new Polygon(name_map.at("top"), _grammar_type, _pivot, mat, pts2d, _color, _texture)));
			}
		}
	}
}

void GableRoof::generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const {
	std::vector<Vertex> vertices;

	Polygon_2 poly;
	for (int i = 0; i < _points.size(); ++i) {
		poly.push_back(KPoint(_points[i].x, _points[i].y));
	}

	// You can pass the polygon via an iterator pair
	SsPtr iss = CGAL::create_interior_straight_skeleton_2(poly);

	std::map<int, glm::vec2> new_pts;

	for (auto face = iss->faces_begin(); face != iss->faces_end(); ++face) {
		int numEdges = 0;
		{
			auto edge0 = face->halfedge();
			auto edge = edge0;
			do {
				numEdges++;
			} while ((edge = edge->next()) != edge0);
		}

		if (numEdges == 3) {
			auto edge0 = face->halfedge();
			auto edge = edge0;
			glm::vec2 p0, p1, p2;

			do {
				auto head = edge->vertex();
				auto tail = edge->opposite()->vertex();

				if (edge->is_bisector()) {
					if (edge->is_inner_bisector()) { // 外側に接続されていない分割線
					} else { // 外側と接続されている分割線
						p2 = glm::vec2(head->point().x(), head->point().y());
						new_pts[head->id()] = (p0 + p1) * 0.5f; // to be fixed
					}
				} else { // 一番外側のボーダー
					p0 = glm::vec2(tail->point().x(), tail->point().y());
					p1 = glm::vec2(head->point().x(), head->point().y());
				}
			} while ((edge = edge->next()) != edge0);
		}
	}

	for (auto face = iss->faces_begin(); face != iss->faces_end(); ++face) {
		// 各faceについて、ポリゴンを生成する
		auto edge0 = face->halfedge();
		auto edge = edge0;

		// 最初のエッジを保存する
		glm::vec2 p0, p1;
		bool first = true;

		glm::vec3 prev_p;

		do {
			auto head = edge->vertex();
			auto tail = edge->opposite()->vertex();

			if (first) {
				p0 = glm::vec2(tail->point().x(), tail->point().y());
				p1 = glm::vec2(head->point().x(), head->point().y());
				first = false;

				prev_p = glm::vec3(p1, 0);
			} else {
				glm::vec2 p2 = glm::vec2(head->point().x(), head->point().y());

				if (p2 != p0) {
					// p2の高さを計算
					float z = glutils::distance(p0, p1, p2) * tanf(_angle * M_PI / 180.0f);

					if (new_pts.find(head->id()) != new_pts.end()) {
						p2 = new_pts[head->id()];
					}

					// 三角形を作成
					glm::vec3 v0 = glm::vec3(_pivot * _modelMat * glm::vec4(p0, 0, 1));
					glm::vec3 v1 = glm::vec3(_pivot * _modelMat * glm::vec4(prev_p, 1));
					glm::vec3 v2 = glm::vec3(_pivot * _modelMat * glm::vec4(p2, z, 1));

					glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

					vertices.push_back(Vertex(v0, normal, glm::vec4(_color, opacity)));
					vertices.push_back(Vertex(v1, normal, glm::vec4(_color, opacity)));
					vertices.push_back(Vertex(v2, normal, glm::vec4(_color, opacity)));
	
					prev_p = glm::vec3(p2, z);
				}
			}
		} while ((edge = edge->next()) != edge0);
	}

	faces.push_back(boost::shared_ptr<glutils::Face>(new glutils::Face(_name, _grammar_type, vertices)));
}

}
