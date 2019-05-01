#include "RenderManager.h"
#include <iostream>
#include "Shader.h"
#include <QImage>
#include <QGLWidget>
#include <sstream>

GeometryObject::GeometryObject() {
	vaoCreated = false;
	vaoOutdated = true;
}

GeometryObject::GeometryObject(const std::vector<Vertex>& vertices, bool lighting) {
	this->vertices = vertices;
	this->lighting = lighting;
	vaoCreated = false;
	vaoOutdated = true;
}

void GeometryObject::addVertices(const std::vector<Vertex>& vertices) {
	this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());
	vaoOutdated = true;
}

/**
 * Create VAO according to the vertices.
 */
void GeometryObject::createVAO() {
	// VAOが作成済みで、最新なら、何もしないで終了
	if (vaoCreated && !vaoOutdated) return;

	if (!vaoCreated) {
		// create vao and bind it
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// create VBO and tranfer the vertices data to GPU buffer
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		vaoCreated = true;
	} else {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// configure the attributes in the vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, drawEdge));
		
	// unbind the vao
	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vaoOutdated = false;
}

RenderManager::RenderManager() {
	//ssao
	uKernelSize = 64;// 16;
	uRadius = 1;// 17.0f;
	uPower = 2.0f;
}

RenderManager::~RenderManager() {
	shader.cleanShaders();

	//delete
	glDeleteVertexArrays(1,&secondPassVBO);
	glDeleteVertexArrays(1,&secondPassVAO);
}

void RenderManager::init(const std::string& vertex_file, const std::string& geometry_file, const std::string& fragment_file, bool useShadow, int shadowMapSize) {
	this->useShadow = useShadow;
	this->softShadow = true;
	renderingMode = RENDERING_MODE_BASIC;

	// init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	// init program shader
	// PASS 1
	fragDataNamesP1.push_back("def_diffuse");
	fragDataNamesP1.push_back("def_normal");
	fragDataNamesP1.push_back("def_originPos");
	fragDataNamesP1.push_back("def_intensity");

	programs["pass1"] = shader.createProgram("shaders/vert_pass1.glsl", "shaders/frag_pass1.glsl", fragDataNamesP1);
	// PASS 2
	std::vector<QString> fragDataNamesP2;
	fragDataNamesP2.push_back("def_AO");
	programs["ssao"] = shader.createProgram("shaders/vert_ssao.glsl", "shaders/frag_ssao.glsl", fragDataNamesP2);
	// PASS 3
	programs["blur"] = shader.createProgram("shaders/vert_blur.glsl", "shaders/frag_blur.glsl");

	// Line rendering
	programs["line"] = shader.createProgram("shaders/vert_line.glsl", "shaders/frag_line.glsl");

	// Contour rendering
	programs["contour"] = shader.createProgram("shaders/vert_contour.glsl", "shaders/frag_contour.glsl");

	// Shadow mapping
	programs["shadow"] = shader.createProgram("shaders/vert_shadow.glsl", "shaders/frag_shadow.glsl");

	glUseProgram(programs["pass1"]);


	//////////////////////////////////////////////
	// INIT SECOND PASS
	// Quad
	std::vector<Vertex> vert(4);
	vert[0]=Vertex(glm::vec3(-1,-1,0),glm::vec3(0,0,0),glm::vec4(0,0,0,1), glm::vec2(0,0));
	vert[1]=Vertex(glm::vec3(1,-1,0),glm::vec3(0,0,0),glm::vec4(0,0,0,1), glm::vec2(1,0));
	vert[2]=Vertex(glm::vec3(1,1,0),glm::vec3(0,0,0),glm::vec4(0,0,0,1), glm::vec2(1,1));
	vert[3]=Vertex(glm::vec3(-1,1,0),glm::vec3(0,0,0),glm::vec4(0,0,0,1), glm::vec2(0,1));

	glGenVertexArrays(1,&secondPassVAO);
	glBindVertexArray(secondPassVAO);
	glGenBuffers(1, &secondPassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, secondPassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vert.size(), vert.data(), GL_STATIC_DRAW);
	
	// Configure the attributes in the VAO.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindVertexArray(0);
	// fragm
	fragDataFB=INT_MAX;
	
	///// load 3d texture for hatching
	std::vector<QString> hatchingTextureFiles;
	hatchingTextureFiles.push_back("hatching/hatching0.png");
	hatchingTextureFiles.push_back("hatching/hatching1.png");
	hatchingTextureFiles.push_back("hatching/hatching2.png");
	hatchingTextureFiles.push_back("hatching/hatching3.png");
	hatchingTextureFiles.push_back("hatching/hatching4.png");
	hatchingTextureFiles.push_back("hatching/hatching5.png");
	hatchingTextureFiles.push_back("hatching/hatching6.png");
	hatchingTextureFiles.push_back("hatching/hatching7.png");
	hatchingTextureFiles.push_back("hatching/hatching8.png");
	hatchingTextures = load3DTexture(hatchingTextureFiles);
	
	shadow.init(programs["shadow"], shadowMapSize, shadowMapSize);
}

void RenderManager::resize(int winWidth, int winHeight){
		
	if(fragDataTex.size()>0){
		glDeleteTextures(fragDataTex.size(),&fragDataTex[0]);
		glDeleteTextures(1,&fragDepthTex);
		glDeleteTextures(1, &fragNoiseTex);
		fragDataTex.clear();
		glDeleteFramebuffers(1,&fragDataFB);
	}


	// The texture we're going to render to
	fragDataTex.resize(fragDataNamesP1.size());

	// TEX1: Diffuse Texture
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fragDataTex[0]);
	glBindTexture(GL_TEXTURE_2D, fragDataTex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);//NULL means reserve texture memory, but texels are undefined

	// TEX2: Normal Texture
	glActiveTexture (GL_TEXTURE2);
	glEnable( GL_TEXTURE_2D );
	glGenTextures(1, &fragDataTex[1]);
	glBindTexture(GL_TEXTURE_2D, fragDataTex[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// TEX3: Original Position Texture
	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fragDataTex[2]);
	glBindTexture(GL_TEXTURE_2D, fragDataTex[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


	// TEX4: AO Texture
	glActiveTexture(GL_TEXTURE4);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fragAOTex);
	glBindTexture(GL_TEXTURE_2D, fragAOTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


	// TEX5: Light intensity
	glActiveTexture(GL_TEXTURE5);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fragDataTex[3]);
	glBindTexture(GL_TEXTURE_2D, fragDataTex[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);// GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


	/////////////////////////////////////////

	// DEPTH
	glActiveTexture (GL_TEXTURE8);
	glEnable( GL_TEXTURE_2D );
	glGenTextures(1, &fragDepthTex);
	glBindTexture(GL_TEXTURE_2D, fragDepthTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, winWidth, winHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, winWidth, winHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
	glActiveTexture(GL_TEXTURE9);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fragDepthTex_AO);
	glBindTexture(GL_TEXTURE_2D, fragDepthTex_AO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, winWidth, winHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/////////////////////////////////////////

	// FRAME BUFFER
	fragDataFB = 0;
	glGenFramebuffers(1, &fragDataFB);
		
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fragDataTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fragDataTex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, fragDataTex[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, fragDataTex[3], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fragDepthTex, 0);
	// Set the list of draw buffers.
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers); // "3" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		printf("+1ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
		exit(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// FRAME BUFFER AO
	fragDataFB_AO = 0;
	glGenFramebuffers(1, &fragDataFB_AO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fragAOTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fragDepthTex, 0);
	// Set the list of draw buffers.
	GLenum DrawBuffers_AO[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers_AO); // "3" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("+2ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
		exit(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//////////////////////////////////////////////////////////////////////////////////
	// Noise
	fragNoiseTex = 0;
	glActiveTexture(GL_TEXTURE7);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &fragNoiseTex);
	glBindTexture(GL_TEXTURE_2D, fragNoiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
	qsrand((uint)5648943215);
		
	int sizeX = winWidth;
	int sizeZ = winHeight;

	std::vector<GLfloat> data(sizeX*sizeZ*3);
	for (int c = 0; c < sizeX*sizeZ*3; c++) {
		if (c % 3 == 0 || c % 3 == 1)
			data[c] = (float(qrand()) / RAND_MAX)*2.0f - 1.0f;
		else
			data[c] = 0.0f; //0 in component z
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, sizeX, sizeZ, 0, GL_RGB, GL_FLOAT, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
		
	resizeSsaoKernel();
}//

template<typename T>
T lerp(T v0, T v1, T t) {
	return (1 - t)*v0 + t*v1;
}

void RenderManager::resizeSsaoKernel() {
	uKernelOffsets.resize(uKernelSize*3);
	qsrand(123456);
	for (int i = 0; i < uKernelSize; ++i) {
		glm::vec3 kernel = glm::normalize(glm::vec3((float(qrand()) / RAND_MAX)*2.0f - 1.0f, (float(qrand()) / RAND_MAX)*2.0f - 1.0f, (float(qrand()) / RAND_MAX)));
		float scale = float(i) / uKernelSize;
		kernel *= lerp<float>(0.1f, 1.0f, scale*scale);
		//printf("[%d] %f %f %f\n",i,kernel.x,kernel.y,kernel.z);
		uKernelOffsets[i * 3 + 0] = kernel.x;
		uKernelOffsets[i * 3 + 1] = kernel.y;
		uKernelOffsets[i * 3 + 2] = kernel.z;
	}
}//


void RenderManager::addFaces(const std::vector<boost::shared_ptr<glutils::Face> >& faces, bool lighting) {
	for (int i = 0; i < faces.size(); ++i) {
		addObject(faces[i]->name.c_str(), faces[i]->texture.c_str(), faces[i]->vertices, lighting);
	}
}

void RenderManager::addObject(const QString& object_name, const QString& texture_file, const std::vector<Vertex>& vertices, bool lighting) {
	GLuint texId;
	
	if (texture_file.length() > 0) {
		// テクスチャファイルがまだ読み込まれていない場合は、ロードする
		if (!textures.contains(texture_file)) {
			texId = loadTexture(texture_file);
			textures[texture_file] = texId;
		} else {
			texId = textures[texture_file];
		}
	} else {
		texId = 0;
	}

	if (objects.contains(object_name)) {
		if (objects[object_name].contains(texId)) {
			objects[object_name][texId].addVertices(vertices);
		} else {
			objects[object_name][texId] = GeometryObject(vertices, lighting);
		}
	} else {
		objects[object_name][texId] = GeometryObject(vertices, lighting);
	}
}

void RenderManager::removeObjects() {
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		removeObject(it.key());
	}
	objects.clear();
}

void RenderManager::removeObject(const QString& object_name) {
	for (auto it = objects[object_name].begin(); it != objects[object_name].end(); ++it) {
		glDeleteBuffers(1, &it->vbo);
		glDeleteVertexArrays(1, &it->vao);
	}

	objects[object_name].clear();
}

glutils::BoundingBox RenderManager::computeBoundingBox() {
	glutils::BoundingBox bbox;

	for (auto it = objects.begin(); it != objects.end(); ++it) {
		for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
			for (int k = 0; k < it2->vertices.size(); ++k) {
				bbox.addPoint(it2->vertices[k].position);
			}
		}
	}

	return bbox;
}

void RenderManager::centerObjects() {
	glm::vec3 minPt((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	glm::vec3 maxPt = -minPt;

	// もとのサイズを計算
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
			for (int k = 0; k < it2->vertices.size(); ++k) {
				minPt.x = (std::min)(minPt.x, it2->vertices[k].position.x);
				minPt.y = (std::min)(minPt.y, it2->vertices[k].position.y);
				minPt.z = (std::min)(minPt.z, it2->vertices[k].position.z);
				maxPt.x = (std::max)(maxPt.x, it2->vertices[k].position.x);
				maxPt.y = (std::max)(maxPt.y, it2->vertices[k].position.y);
				maxPt.z = (std::max)(maxPt.z, it2->vertices[k].position.z);
			}
		}
	}

	glm::vec3 center = (maxPt + minPt) * 0.5f;

	float size = (std::max)(maxPt.x - minPt.x, (std::max)(maxPt.y - minPt.y, maxPt.z - minPt.z));
	float scale = 1.0f / size;

	// 単位立方体に入るよう、縮尺・移動
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
			for (int k = 0; k < it2->vertices.size(); ++k) {
				it2->vertices[k].position = (it2->vertices[k].position - center) * scale;
			}
		}
	}
}

void RenderManager::renderAll() {
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		render(it.key());
	}
}

void RenderManager::renderAllExcept(const QString& object_name) {
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if (it.key() == object_name) continue;

		render(it.key());
	}
}

void RenderManager::render(const QString& object_name) {
	for (auto it = objects[object_name].begin(); it != objects[object_name].end(); ++it) {
		GLuint texId = it.key();
		
		// vaoを作成
		it->createVAO();

		if (texId > 0) {
			// テクスチャなら、バインドする
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texId);
			glUniform1i(glGetUniformLocation(programs["pass1"], "textureEnabled"), 1);
			glUniform1i(glGetUniformLocation(programs["pass1"], "tex0"), 0);
		} else {
			glUniform1i(glGetUniformLocation(programs["pass1"], "textureEnabled"), 0);
		}

		if (it->lighting) {
			glUniform1i(glGetUniformLocation(programs["pass1"], "lighting"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(programs["pass1"], "lighting"), 0);
		}

		if (useShadow) {
			glUniform1i(glGetUniformLocation(programs["pass1"], "useShadow"), 1);
			if (softShadow) {
				glUniform1i(glGetUniformLocation(programs["pass1"], "softShadow"), 1);
			}
			else {
				glUniform1i(glGetUniformLocation(programs["pass1"], "softShadow"), 0);
			}
		} else {
			glUniform1i(glGetUniformLocation(programs["pass1"], "useShadow"), 0);
		}

		// 描画
		glBindVertexArray(it->vao);
		glDrawArrays(GL_TRIANGLES, 0, it->vertices.size());

		glBindVertexArray(0);
	}
}

void RenderManager::updateShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_dir, const glm::mat4& light_mvpMatrix) {
	if (useShadow) {
		shadow.update(glWidget3D, light_dir, light_mvpMatrix);
	}
}

GLuint RenderManager::loadTexture(const QString& filename) {
	QImage img;
	if (!img.load(filename)) {
		std::stringstream ss;
		ss << "load texture failed : " << filename.toUtf8().constData();
		std::cout << ss.str() << std::endl;
		throw ss.str();
	}

	QImage GL_formatted_image;
	GL_formatted_image = QGLWidget::convertToGLFormat(img);
	if (GL_formatted_image.isNull()) {
		std::stringstream ss;
		ss << "Failed to convert to gl format : " << filename.toUtf8().constData();
		std::cout << ss.str() << std::endl;
		throw ss.str();
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GL_formatted_image.width(), GL_formatted_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

GLuint RenderManager::load3DTexture(const std::vector<QString> & pathes) {
	GLsizei width, height, depth = (GLsizei)pathes.size();

	std::vector<QImage> formatedImages(pathes.size());

	// load and format each image
	{
		for (uint i = 0; i < pathes.size(); ++i) {
			QImage imageToConvert;
			if (!imageToConvert.load(pathes[i])) {
				std::stringstream ss;
				ss << "Failed to load 3D texture : " << pathes[i].toUtf8().constData();
				std::cout << ss.str() << std::endl;
				throw ss.str();
			}

			QImage GL_formatted_image = QGLWidget::convertToGLFormat(imageToConvert);
			if (GL_formatted_image.isNull()) {
				std::stringstream ss;
				ss << "Failed to convert to gl format : " << pathes[i].toUtf8().constData();
				std::cout << ss.str() << std::endl;
				throw ss.str();
			}

			formatedImages[i] = GL_formatted_image;

			if (i == 0) {
				width = formatedImages[i].width();
				height = formatedImages[i].height();
			}
			else {
				if (width != formatedImages[i].width() || height != formatedImages[i].height())	{
					std::stringstream ss;
					ss << "Texture3D::load() failed : different dimensions of images. " << pathes[0].toUtf8().constData() << " " << pathes[i].toUtf8().constData();
					std::cout << ss.str() << std::endl;
					throw ss.str();
				}
			}
		}
	}

	GLuint texture;

	// create empty 3D texture
	{
	   glGenTextures(1, &texture);
	   glBindTexture(GL_TEXTURE_3D, texture);
	   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	   // allocate memory for 3D texture
	   glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}

	// copy image data to each layer of 3D texture
	{
		for (uint i = 0; i < pathes.size(); ++i) {
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, (GLint)i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, formatedImages[i].bits());
		}
	}

	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);

	return texture;
}
