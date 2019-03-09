#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec2 outUV;

layout(location = 0)out vec4 def_AO;

uniform sampler2D tex0;//color
uniform sampler2D tex1;//normals
uniform sampler2D tex2;//orig pos

uniform sampler2D noiseTex;//noise

uniform sampler2D depthTex;

uniform vec2 pixelSize;//in texture space

//uniform mat4 uProjectionMatrix; // current projection matrix, for linearized depth
//uniform mat4 uInvProjectionMatrix;
uniform mat4 pMatrix;
uniform mat4 mvpMatrix;

float LinearizeDepth(float z){
		const float zNear = 5.0; // camera z near
		const float zFar = 10000.0; // camera z far

		return (2.0 * zNear) / (zFar + zNear - z * (zFar - zNear));
}//

/*----------------------------------------------------------------------------*/
//	ssao uniforms:
const int MAX_KERNEL_SIZE = 128;
uniform int uKernelSize=16;//16
uniform vec3 uKernelOffsets[MAX_KERNEL_SIZE];
uniform float uRadius = 20.0;//1.5
uniform float uPower = 1.0;//2.0

float linearizeDepth(in float depth, in mat4 projMatrix) {
	return projMatrix[3][2] / (depth - projMatrix[2][2]);
}

float ssao(in mat3 kernelBasis, in vec3 originPos, in float radius) {
	float occlusion = 0.0;
	
	vec4 originPosProj = mvpMatrix * vec4(originPos, 1.0);//for rangecheck
	originPosProj.xyz /= originPosProj.w; 
	originPosProj.xyz = originPosProj.xyz * 0.5 + 0.5; // scale/bias to texcoords
	float originDepth = texture(depthTex, originPosProj.xy).r;
	//originDepth = linearizeDepth(originDepth, pMatrix);

	for (int i = 0; i < uKernelSize; ++i) {
		//	get sample position:
		vec3 samplePos = kernelBasis * uKernelOffsets[i];
		samplePos = samplePos * radius + originPos;
		
		//samplePos = originPos + uKernelOffsets[i];
		//	project sample position:
		vec4 offset;
		//if (false) {
		//	offset = pMatrix * vec4(samplePos, 1.0);
		//} else {
			offset = mvpMatrix * vec4(samplePos, 1.0);
		//}
		//vec4 offset = uProjectionMatrix * vec4(samplePos, 1.0);
		
		offset.xyz /= offset.w; // only need xy
		offset.xyz = offset.xyz * 0.5 + 0.5; // scale/bias to texcoords

		//	get sample depth:
		float sampleDepth = texture(depthTex, offset.xy).r;
		//sampleDepth = linearizeDepth(sampleDepth, pMatrix);

		//float rangeCheck = smoothstep(0.0, 1.0, radius / abs(originPos.z - sampleDepth));
		//occlusion += rangeCheck * step(sampleDepth, samplePos.z);
		//occlusion += (sampleDepth <= samplePos.z ? 1.0 : 0.0);// *rangeCheck;

		float offsetDepth = offset.z;

		float rangeCheck = 1.0f;// abs(offsetDepth - sampleDepth) < radius ? 1.0 : 0.0;
		//float rangeCheck = smoothstep(0.0, 1.0, radius / abs(originPosProj.z - sampleDepth));


		
		//offsetDepth = linearizeDepth(offsetDepth, pMatrix);

		

		occlusion += (sampleDepth <= offsetDepth ? 1.0 : 0.0)*rangeCheck;// *rangeCheck;
	}

	occlusion = 1.0 - (occlusion / float(uKernelSize));
	return pow(occlusion, uPower);
}//

void main(){
	/////////////////
	// SECOND PASS

	vec2 coord = outUV.xy;
	
	vec3 normal = texture(tex1, coord).rgb;
	if (length(normal) < 0.1) {
		def_AO.rgb = vec3(1.0);//sky
		return;
	}
	normal = normalize(normal);
	float originDepth = texture(depthTex, coord).r;

	originDepth = linearizeDepth(originDepth, pMatrix);
	vec3 originPos = texture(tex2, coord).rgb;// texture(tex2, coord).rgb;
	
	const float uNoiseScale = 4.0;
	vec3 rvec = texture(noiseTex, coord*uNoiseScale).rgb;// *2 - 1;
	rvec = normalize(rvec);

	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 kernelBasis = mat3(tangent, bitangent, normal);

	float ssaoVal = ssao(kernelBasis, originPos, uRadius);
	def_AO.rgb = vec3(ssaoVal);
	return;

	if (false) {
		//////////////////////////////////
		// BOTTOM RIGHT
		bool coverAll = true;
		if ((outUV.x > 0.5&&outUV.y < 0.5) || coverAll) {
			vec2 coord = outUV.xy * 2 + vec2(-1, 0);//bottom right
			if (coverAll == true) {
				coord = outUV.xy;
			}
			// pos
			/*{
			vec3 pos = texture(tex2, coord).rgb;
			def_AO.rgb = pos;
			return;
			}*/
			// light direction
			vec3 color = texture(tex0, coord).rgb;

			/*
			{
			const vec3 lightDir = normalize(vec3(0.4, 0.1, -0.8));


			if (length(normal)>0.1)
			def_AO.rgb = color.rgb*(vec3(0.05) + vec3(0.95)*max(0.0, dot(-lightDir, normal)));
			else
			def_AO.rgb = color.rgb;
			return;
			}*/

			vec3 normal = texture(tex1, coord).rgb;
			if (length(normal) < 0.1) {
				def_AO.rgb = color.rgb;
				return;
			}
			//normal = normal * 2 - 1;
			//normal = normalize(normal * 2 - 1);
			normal = normalize(normal);
			float originDepth = texture(depthTex, coord).r;

			//originDepth = LinearizeDepth(originDepth);
			originDepth = linearizeDepth(originDepth, pMatrix);
			if (false) {//just depth
				def_AO = vec4(originDepth, originDepth, originDepth, 1);
				return;
			}

			//vec3 originPos = ray*originDepth;
			//vec4 vProjectedPos = vec4(coord.x*2-1, coord.y*2-1, originDepth, 1.0f);
			//vec4 vPositionVS = vProjectedPos*uInvProjectionMatrix;
			//vec3 originPos = vPositionVS.xyz / vPositionVS.w;

			vec3 originPos = texture(tex2, coord).rgb;// texture(tex2, coord).rgb;
			if (false) {//just poss
				def_AO = vec4(abs(originPos / 600), 1);// div 600 to render better
				//def_AO = vec4(originPos, 1);
				return;
			}

			//def_AO = vec4(normal, 1.0);
			//def_AO = vec4(texture(tex1, coord).rgb, 1.0);

			//return;

			//vec3 rvec = texture(noiseTex, coord).rgb * 2.0 - 1.0;
			const float uNoiseScale = 4.0;
			vec3 rvec = texture(noiseTex, coord*uNoiseScale).rgb;// *2 - 1;
			rvec = normalize(rvec);

			if (false) {//noise vec
				def_AO = vec4(abs(rvec), 1);
				return;
			}
			//vec3 rvec = vec3(0.1);
			vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
			if (false) {//noisy tangent
				def_AO = vec4(abs(tangent), 1);
				return;
			}
			vec3 bitangent = cross(tangent, normal);
			mat3 kernelBasis = mat3(tangent, bitangent, normal);

			if (false) {
				def_AO = vec4(vec3(ssao(kernelBasis, originPos, uRadius)), 1.0);
				return;
			}
			float ssaoVal = ssao(kernelBasis, originPos, uRadius);
			def_AO.rgb = color.rgb*ssaoVal;
			return;
		}//

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////
		// TOP LEFY
		// top left: Color
		if (outUV.x<0.5&&outUV.y>0.5) {
			vec2 coord = outUV.xy * 2 + vec2(0.0, -1.0);
			def_AO = vec4(texture(tex0, coord).rgb, 1.0);
			return;
		}
		//////////////////////////////////
		// TOP RIGHT
		// top right: Normals
		if (outUV.x > 0.5&&outUV.y > 0.5) {
			vec2 coord = outUV.xy * 2 + vec2(-1, -1);
			def_AO.rgb = normalize(texture(tex1, coord).rgb);
			def_AO.rgb = (def_AO.rgb*0.5) + 0.5;//move from -1-1 to 0-1
			return;
		}

		//////////////////////////////////
		// BOTTOM LEFT
		// bot left: edge
		// laplace edge
		//   -1 -1 -1
		//   -1  8 -1
		//   -1 -1 -1
		/*if(outUV.x<0.5&&outUV.y<0.5){
			vec2 coord=outUV.xy*2;
			vec3 edge=8*texture( tex0, coord ).rgb;
			edge-=texture( tex0, coord+vec2(-pixelSize.x,-pixelSize.y) ).rgb;
			edge-=texture( tex0, coord+vec2(           0,-pixelSize.y) ).rgb;
			edge-=texture( tex0, coord+vec2(+pixelSize.x,-pixelSize.y) ).rgb;
			edge-=texture( tex0, coord+vec2(+pixelSize.x,           0) ).rgb;
			edge-=texture( tex0, coord+vec2(+pixelSize.x,+pixelSize.y) ).rgb;
			edge-=texture( tex0, coord+vec2(           0,+pixelSize.y) ).rgb;
			edge-=texture( tex0, coord+vec2(-pixelSize.x,+pixelSize.y) ).rgb;
			edge-=texture( tex0, coord+vec2(-pixelSize.x,           0) ).rgb;
			float wedge = min(1.0,(edge.x+edge.y+edge.z)*1000.0);
			wedge=max(0.0,wedge);
			def_AO =vec4(edge,1.0);
			return;
			}*/
		// wscg.zcu.cz/wscg2003/Papers_2003/D73.pdf
		//   A  B  C
		//   D  X  E
		//   F  G  H
		if (outUV.x < 0.5&&outUV.y < 0.5) {
			vec2 coord = outUV.xy * 2;
			if (false) {
				def_AO = vec4(texture(tex2, coord).rgb, 1.0);
				def_AO = abs(def_AO / 1000.0f);
				return;
			}
			if (true) {
				float originDepth = texture(depthTex, coord).r;
				//depth=pow(2,depth);
				originDepth = LinearizeDepth(originDepth);
				//originDepth = linearizeDepth(originDepth, uProjectionMatrix);
				def_AO = vec4(originDepth, originDepth, originDepth, 1);
				return;


			}
			// Discontinuity in Normal Space
			// IN=1/2((expanded A dot expand H)+(expand C dot expand F))
			vec3 A = texture(tex1, coord + vec2(-pixelSize.x, +pixelSize.y)).rgb;
			vec3 H = texture(tex1, coord + vec2(+pixelSize.x, -pixelSize.y)).rgb;
			vec3 C = texture(tex1, coord + vec2(+pixelSize.x, +pixelSize.y)).rgb;
			vec3 F = texture(tex1, coord + vec2(-pixelSize.x, -pixelSize.y)).rgb;
			float In = 0.5*((dot(A, H)) + (dot(C, F)));//normal discontinuity
			if (A == H&&C == F&&A == C)
				In = 1;
			//def_AO =vec4(In,In,In,1.0);

			// Discontinuity in depth space
			/*float zA=LinearizeDepth(texture( depthTex, coord+vec2(-pixelSize.x,+pixelSize.y) ).r);
			float zH=LinearizeDepth(texture( depthTex, coord+vec2(+pixelSize.x,-pixelSize.y) ).r);
			float zC=LinearizeDepth(texture( depthTex, coord+vec2(+pixelSize.x,+pixelSize.y) ).r);
			float zF=LinearizeDepth(texture( depthTex, coord+vec2(-pixelSize.x,-pixelSize.y) ).r);
			float Iz=pow((1-0.5*(zA-zH)),2)*pow((1-0.5*(zC-zF)),2);
			def_AO =vec4(Iz*0.5,Iz*0.5,Iz*0.5,1.0);*/
			float edge = 8 * LinearizeDepth(texture(depthTex, coord).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(-pixelSize.x, -pixelSize.y)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(0, -pixelSize.y)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(+pixelSize.x, -pixelSize.y)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(+pixelSize.x, 0)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(+pixelSize.x, +pixelSize.y)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(0, +pixelSize.y)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(-pixelSize.x, +pixelSize.y)).r);
			edge -= LinearizeDepth(texture(depthTex, coord + vec2(-pixelSize.x, 0)).r);
			//def_AO =vec4(edge*10,edge*10,edge*10,1.0);
			float edgeD = (In)*(1 - edge * 10);
			def_AO = vec4(edgeD, edgeD, edgeD, 1.0);
			return;
		}
		/*//////////////////////////////////
		// BOTTOM RIGHT
		if(outUV.x>0.5&&outUV.y<0.5){
		vec2 coord=outUV.xy*2+vec2(-1,0);
		def_AO =vec4(texture( tex0,coord  ).rgb,1.0);

		float depth=texture( depthTex,coord  ).r;
		//depth=pow(2,depth);
		depth=LinearizeDepth(depth);
		def_AO =vec4(depth,depth,depth,1);
		return;
		}*/

		//def_AO = vec4(0.5,0.5,0.5,1);

	}//if(false)
	
}//

