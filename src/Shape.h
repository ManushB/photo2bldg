#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Asset.h"
#include "GLUtils.h"

class RenderManager;

namespace cga {

class Shape {
public:
	std::string _name;
	bool _active;
	bool _axiom;
	glm::mat4 _modelMat;
	glm::vec3 _color;
	bool _textureEnabled;
	std::string _texture;
	std::vector<glm::vec2> _texCoords;
	glm::vec3 _scope;
	glm::vec3 _prev_scope;
	glm::mat4 _pivot;
	std::string _grammar_type;

	static std::map<std::string, Asset> assets;

public:
	void center(int axesSelector);
	virtual boost::shared_ptr<Shape> clone(const std::string& name) const;
	virtual void comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes);
	virtual boost::shared_ptr<Shape> cornerCut(const std::string& name, int type, float length);
	virtual boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	virtual boost::shared_ptr<Shape> hemisphere(const std::string& name);
	virtual void innerArch(const std::string& name, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes);
	virtual boost::shared_ptr<Shape> innerCircle(const std::string& name);
	virtual boost::shared_ptr<Shape> innerSemiCircle(const std::string& name);
	boost::shared_ptr<Shape> insert(const std::string& name, const std::string& geometryPath);
	virtual void offset(const std::string& name, float offsetDistance, const std::string& inside, const std::string& border, std::vector<boost::shared_ptr<Shape> >& shapes);
	virtual boost::shared_ptr<Shape> pyramid(const std::string& name, float height);
	virtual boost::shared_ptr<Shape> roofGable(const std::string& name, float angle);
	virtual boost::shared_ptr<Shape> roofHip(const std::string& name, float angle);
	void rotate(const std::string& name, float xAngle, float yAngle, float zAngle);
	virtual void setupProjection(int axesSelector, float texWidth, float texHeight);
	virtual boost::shared_ptr<Shape> shapeL(const std::string& name, float frontWidth, float leftWidth);
	virtual boost::shared_ptr<Shape> shapeU(const std::string& name, float frontWidth, float backDepth);
	virtual void size(float xSize, float ySize, float zSize, bool centered);
	virtual void split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects);
	virtual boost::shared_ptr<Shape> taper(const std::string& name, float height, float slope);
	void texture(const std::string& tex);
	void translate(int mode, int coordSystem, float x, float y, float z);
	virtual void generateGeometry(std::vector<boost::shared_ptr<glutils::Face> >& faces, float opacity) const;

protected:
	//void drawAxes(RenderManager* renderManager, const glm::mat4& modelMat) const;
	static Asset getAsset(const std::string& filename);
};

}
