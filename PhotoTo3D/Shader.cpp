#include <glew.h>
#include "Shader.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <QFile>
#include <QTextStream>

using namespace std;

Shader::Shader() {

}

Shader::~Shader() {
	cleanShaders();
}

/**
 * 指定されたvertex shader、fragment shaderを読み込んでコンパイルし、
 * プログラムにリンクする。
 *
 * @param vertex_file		vertex shader file
 * @param fragment_file		frament shader file
 * @return					program id
 */
uint Shader::createProgram(const string& vertex_file, const string& fragment_file) {
	return createProgram(vertex_file, fragment_file, std::vector<QString>());
}

uint Shader::createProgram(const string& vertex_file, const string& fragment_file, const std::vector<QString>& fragDataNamesP1) {
	std::cout << "Compiling " << vertex_file << std::endl;

	std::string source;
	loadTextFile(vertex_file, source);
	GLuint vertex_shader = compileShader(source, GL_VERTEX_SHADER);

	std::cout << "Compiling " << fragment_file << std::endl;

	loadTextFile(fragment_file, source);
	GLuint fragment_shader = compileShader(source, GL_FRAGMENT_SHADER);

	// create program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	if (fragDataNamesP1.size() == 0) {
		glBindFragDataLocation(program, 0, "outputF");
	} else {
		for (int i = 0; i < fragDataNamesP1.size(); ++i) {
			glBindFragDataLocation(program, i, fragDataNamesP1[i].toUtf8().constData());
		}
	}
	glLinkProgram(program);
	
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		char* logText = new char[logLength];
		glGetProgramInfoLog(program, logLength, NULL, logText);

		stringstream ss;
		ss << "Error linking program:" << endl << logText << endl;
		delete [] logText;

		glDeleteProgram(program);
		throw runtime_error(ss.str());
	}
	
	programs.push_back(program);
	vertex_shaders.push_back(vertex_shader);
	fragment_shaders.push_back(fragment_shader);

	return program;
}

void Shader::cleanShaders() {
	for (int pN = 0; pN<programs.size(); pN++){
		glDetachShader(programs[pN], vertex_shaders[pN]);
		glDetachShader(programs[pN], fragment_shaders[pN]);
		glDeleteShader(vertex_shaders[pN]);
		glDeleteShader(fragment_shaders[pN]);
		glDeleteProgram(programs[pN]);
	}
	programs.clear();
	vertex_shaders.clear();
	fragment_shaders.clear();
}

/**
 * Load a text from a file.
 *
 * @param filename		file name
 * @param str [OUT]		the text in the file
 */
void Shader::loadTextFile(const string& filename, string& str) {
	/*
	ifstream file(filename);
	if (file.fail()) {
		stringstream ss;
		ss << "Could not open file: " << filename;
		cout << ss << endl;
		throw ss.str();
	}

	QString text;
	char line[1024];
	while (!file.eof()) {
		char line[1024];
		file.getline(line, 1024);
		text += line;
	}
	str = std::string(text.toAscii().constData());
	*/
	
	QFile file(filename.c_str());
	if (!file.open(QIODevice::ReadOnly)) {
		stringstream ss;
		ss << "Could not open file: " << filename;
		throw ss.str();
	}
	QTextStream in(&file);
	QString text;
	while (!in.atEnd()) {
		QString line = in.readLine();    
		text += line + "\n"; 
	}
	file.close();
	str = std::string(text.toUtf8().constData());
}

/**
 * compile a shader.
 *
 * @param source	shader text
 * @param mode		GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 * @return			shader id
 */
GLuint Shader::compileShader(const string& source, GLuint mode) {
	GLenum err;
	uint shader = glCreateShader(mode);
	const char* csource = source.c_str();
	GLint length = source.length();
	glShaderSource(shader, 1, &csource, &length);//NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		char* logText = new char[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, logText);

		if (mode == GL_VERTEX_SHADER) {
			cout << "Vertex shader compilation error:" << endl;
		} else if (mode == GL_GEOMETRY_SHADER) {
			cout << "Geometry shader compilation error:" << endl;
		} else {
			cout << "Fragment shader compilation error:" << endl;
		}
		cout << logText << endl;
		delete [] logText;

		glDeleteShader(shader);
		throw logText;
	}

	return shader;
}

