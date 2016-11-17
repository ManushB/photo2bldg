#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec4 outColor;
in vec2 outUV;
in vec3 origVertex;
in vec3 varyingNormal;

// output color
out vec4 outputF;

uniform mat4 light_mvpMatrix;

void main(){
	outputF = vec4(outColor.xyz, 1);
}

