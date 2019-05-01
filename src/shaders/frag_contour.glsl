#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec2 outUV;

layout(location = 0)out vec4 outputF;

uniform sampler2D depthTex;

uniform vec2 pixelSize;//in texture space

void main(){
	float normalSensitivity = 1.0;
	float depthSensitivity = 10.0;

	vec2 coord = outUV.xy;
	
	float depth = texture(depthTex, coord).r;

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
	float depth_diff = 0;

	for (int xx = -range; xx <= range; ++xx) {
		for (int yy = -range; yy <= range; ++yy) {
			if (xx == 0 && yy == 0) continue;

			float dd = texture(depthTex, vec2(coord.x + xx * pixelSize.x, coord.y + yy * pixelSize.y)).x;

			if (dd == 1) {
				outputF = vec4(0, 0, 0, 1);
				return;
			}
		}
	}

	outputF = vec4(1, 1, 1, 1);
	return;
}

