#version 460

out vec4 outCol;	// Final pixel color
uniform sampler2D tex_unit;	// Texture unit number.
smooth in vec2 vsout_tex_uv;	// Interpolated model-space normal

void main() {
	// built-in function `texture` reads from the given texture unit
	//   and uses the uv coordinate to lookup a texture value.
	// Note that `texture` always returns a vec4, no matter what the texture format is.
	//   (see the 3rd argument of glTexImage2D as a comparison)
	vec4 texture_color = texture(tex_unit, vsout_tex_uv);
	outCol = texture_color;
}
