#pragma once

#include "glew.h"
#include <vector>
#include <QMap>
#include "Vertex.h"
#include "ShadowMapping.h"
#include "GLUtils.h"
#include <boost/shared_ptr.hpp>
#include "Shader.h"
#include <map>

class GeometryObject {
public:
	GLuint vao;
	GLuint vbo;
	std::vector<Vertex> vertices;
	bool lighting;
	bool vaoCreated;
	bool vaoOutdated;

public:
	GeometryObject();
	GeometryObject(const std::vector<Vertex>& vertices, bool lighting = true);
	void addVertices(const std::vector<Vertex>& vertices);
	void createVAO();
};

class RenderManager {
public:
	static enum { RENDERING_MODE_BASIC = 0, RENDERING_MODE_SSAO, RENDERING_MODE_LINE, RENDERING_MODE_HATCHING, RENDERING_MODE_CONTOUR };

public:
	Shader shader;
	std::map<std::string, GLuint> programs;

	QMap<QString, QMap<GLuint, GeometryObject> > objects;
	QMap<QString, GLuint> textures;

	bool useShadow;
	bool softShadow;
	ShadowMapping shadow;
	GLuint hatchingTextures;

	int renderingMode;

	// SSAO
	std::vector<QString> fragDataNamesP1;//Multi target fragmebuffer names P1
	std::vector<GLuint> fragDataTex;
	GLuint fragNoiseTex;
	GLuint fragDepthTex;
	GLuint fragDataFB;
	GLuint fragAOTex;
	GLuint fragDataFB_AO;
	GLuint fragDepthTex_AO;
	
	// second pass
	GLuint secondPassVBO;
	GLuint secondPassVAO;
	//void renderSecondPass();
	// ssao
	float uRadius;
	float uPower;
	float uKernelSize;
	std::vector<float> uKernelOffsets;


public:
	RenderManager();
	~RenderManager();

	void init(const std::string& vertex_file, const std::string& geometry_file, const std::string& fragment_file, bool useShadow, int shadowMapSize = 4096);
	
	// ssao
	void resize(int width,int height);
	void resizeSsaoKernel();

	void addFaces(const std::vector<boost::shared_ptr<glutils::Face> >& faces, bool lighting);
	void addObject(const QString& object_name, const QString& texture_file, const std::vector<Vertex>& vertices, bool lighting);
	void removeObjects();
	void removeObject(const QString& object_name);
	void centerObjects();
	void renderAll();
	void renderAllExcept(const QString& object_name);
	void render(const QString& object_name);
	void updateShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_dir, const glm::mat4& light_mvpMatrix);
	

private:
	GLuint loadTexture(const QString& filename);
	GLuint load3DTexture(const std::vector<QString> & pathes);
};

