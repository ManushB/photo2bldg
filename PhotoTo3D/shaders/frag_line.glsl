#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec2 outUV;

layout(location = 0)out vec4 outputF;

uniform sampler2D tex0;//color
uniform sampler2D tex1;//normals
uniform sampler2D tex2;//orig pos
uniform sampler2D tex3;//light intensity

uniform sampler2D depthTex;
uniform sampler3D hatchingTexture;

uniform vec2 pixelSize;//in texture space
uniform mat4 pMatrix;

uniform int useHatching;	// 1 -- use hatching / 0 -- use white color

float linearizeDepth(float depth, mat4 pMatrix) {
	return pMatrix[3][2] / (depth + pMatrix[2][2]);
}

void main(){
	vec2 coord = outUV.xy;
	
	vec3 normal = texture(tex1, coord).rgb;
	normal = normalize(normal);

	float depth = texture(depthTex, coord).r;
	float orig_depth = linearizeDepth(depth, pMatrix);

	vec3 originPos = texture(tex2, coord).rgb;

	/////// DEBUG //////
	/*
	//outputF = vec4(orig_depth / 300, orig_depth / 300, orig_depth / 300, 1);
	outputF = vec4(originPos.x / 10, originPos.y / 10, originPos.z / 10, 1);
	return;
	*/
	/////// DEBUG //////


	// background
	if (depth == 1) {
		outputF = vec4(1, 1, 1, 1);
		return;
	}



	// check the normal and depth in the surrounding pixels
	int range = 1;
	float diff = 0;

	for (int xx = -range; xx <= range; ++xx) {
		for (int yy = -range; yy <= range; ++yy) {
			if (xx == 0 && yy == 0) continue;

			vec3 nn = normalize(texture(tex1, vec2(coord.x + xx * pixelSize.x, coord.y + yy * pixelSize.y)).xyz);
			float dd = texture(depthTex, vec2(coord.x + xx * pixelSize.x, coord.y + yy * pixelSize.y)).x;
			dd = linearizeDepth(dd, pMatrix);
			vec3 pp = texture(tex2, vec2(coord.x + xx * pixelSize.x, coord.y + yy * pixelSize.y)).xyz;

			if (length(pp - vec3(0.95, 0.95, 0.95)) > 0.1 && abs(dot(normalize(pp - originPos), normal)) < 0.1) continue;

			diff = max(diff, length(normal - nn));
			diff = max(diff, length(orig_depth - dd));
		}
	}

	if (diff > 0.00001) {
		outputF = vec4(0, 0, 0, 1);	// line
	}
	else {
		if (useHatching == 1) {
			float lightIntensity = texture(tex3, coord).r;

			////////////////////// DEBUG ///////////////////////
			/*
			outputF = vec4(lightIntensity, lightIntensity, lightIntensity, 1);
			return;
			*/
			////////////////////// DEBUG ///////////////////////

			ivec3 sizeOfTex = textureSize(hatchingTexture, 0);

			// sample 3D texture to get hatching intensity
			//outputF.rgb = texture(hatchingTexture, vec3(coord.x / pixelSize.x / sizeOfTex.x, coord.y / pixelSize.y / sizeOfTex.y, lightIntensity)).rgb;
			outputF.rgb = texture(hatchingTexture, vec3((originPos.x + originPos.z) * 0.5, (originPos.x * 0.5 + originPos.y + originPos.z) * 0.5, lightIntensity)).rgb;
			outputF.a = 1;
		}
		else {
			outputF = vec4(1, 1, 1, 1);
		}
	}
}

