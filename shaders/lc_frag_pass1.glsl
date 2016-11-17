#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec4 outColor;
in vec2 outUV;
in vec3 origVertex;
in vec3 varyingNormal;

layout(location = 0)out vec3 def_diffuse;
layout(location = 1)out vec3 def_normal;
layout(location = 2)out vec3 def_originPos;
layout(location = 3)out vec3 def_intensity;

uniform sampler2D tex0;
uniform sampler2DArray tex_3D;

uniform int useShadow;
uniform int softShadow;
uniform int lighting;
uniform mat4 light_mvpMatrix;
uniform vec3 lightDir;
uniform sampler2D shadowMap;
uniform int textureEnabled;

vec2 poissonDisk4[4] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760)
);

vec2 poissonDisk16[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed, i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float shadowCoef(int softShadow){
	vec4 shadow_coord2 = light_mvpMatrix * vec4(origVertex, 1.0);
	vec3 ProjCoords = shadow_coord2.xyz / shadow_coord2.w;
	vec2 UVCoords;
	UVCoords.x = 0.5 * ProjCoords.x + 0.5;
	UVCoords.y = 0.5 * ProjCoords.y + 0.5;
	float z = 0.5 * ProjCoords.z + 0.5;

	float visibility = 1.0f;

	if (softShadow == 1) {
		for (int i = 0; i<4; i++){
			int index = int(4.0*random(origVertex.xyz, i)) % 4;
			if (texture2D(shadowMap, UVCoords + poissonDisk4[index] / 3500.0).z  <  z - 0.001){
				visibility -= 0.2;
			}
		}
	}
	else {
		if (texture2D(shadowMap, UVCoords).z  <  z) {
			visibility = 0;
		}
	}

	return visibility;
}

void main(){
	def_diffuse = outColor.xyz;
	def_normal = varyingNormal;
	def_originPos = origVertex;

	if (textureEnabled == 1) {
		def_diffuse = def_diffuse * texture(tex0, outUV.rg).xyz;
	}

	float visibility = 1.0;
	if (useShadow == 1) {
		visibility = shadowCoef(softShadow);
	}

	float ambient = 0.6;
	float diffuse = 0.5;
	float intensity;

	// lighting
	if (lighting == 1) {
		intensity = ambient + (visibility * 0.95 + 0.05) * diffuse * max(0.0, dot(-lightDir, varyingNormal));
	}
	else {
		intensity = ambient + (visibility * 0.95 + 0.05) * diffuse;
	}

	def_intensity = vec3(intensity, intensity, intensity);
	def_diffuse = def_diffuse * intensity;
}

