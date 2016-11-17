#pragma once

#include <QString>
#include <vector>

class Shader
{
public:
	Shader();
	~Shader();

	uint createProgram(const std::string& vertex_file, const std::string& fragment_file);
	uint createProgram(const std::string& vertex_file, const std::string& fragment_file, const std::vector<QString>& fragDataNamesP1);
	void cleanShaders();

private:
	void loadTextFile(const std::string& filename, std::string& str);
	GLuint compileShader(const std::string& source, GLuint mode);
	
private:
	std::vector<GLuint> programs;
	std::vector<GLuint> vertex_shaders;
	std::vector<GLuint> fragment_shaders;
};

