#version 460

layout(location = 0) in vec3 pos;		// Model-space position
layout(location = 1) in vec2 tex_uv;		// Model-space normal

smooth out vec2 vsout_tex_uv;	// Model-space interpolated normal

uniform mat4 xform;			// Model-to-clip space transform

void main() {
	// Transform vertex position
	gl_Position = xform * vec4(pos, 1.0);

	// Interpolate normals
	vsout_tex_uv = tex_uv;
}
