#version 420

layout(location = 0)in vec3 vertex;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec4 color;
layout(location = 3)in vec2 uv;

out vec2 outUV;

uniform mat4 pMatrix;

void main(){
	outUV=uv;
	
	gl_Position = vec4(vertex.xy,0,1.0);

}