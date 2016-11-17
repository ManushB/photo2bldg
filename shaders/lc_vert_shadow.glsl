#version 420

layout(location = 0)in vec3 vertex;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec4 color;
layout(location = 3)in vec2 uv;

out vec4 outColor;
out vec2 outUV;
out vec3 origVertex;// L

out vec3 varyingNormal;

uniform mat4 light_mvpMatrix;

void main(){
	outColor=color;
	outUV=uv;
	origVertex=vertex;

	varyingNormal=normal;

	gl_Position = light_mvpMatrix * vec4(origVertex, 1.0);

}