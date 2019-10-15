#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <string>
#include <vector>
//#include "gl_core_3_3.h"
#include <GL/glew.h>

GLuint compileShader(GLenum type, std::string filename, std::string prepend = "");
GLuint linkProgram(std::vector<GLuint> shaders);

#endif
