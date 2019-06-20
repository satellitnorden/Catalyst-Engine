//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define BLOOM_INTENSITY (1.0f)
#define BLOOM_THRESHOLD (1.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the scene texture.
	vec3 scene = texture(sceneTexture, fragmentTextureCoordinate).rgb;

	//Calculate the luminance of this fragment.
	float luminance = CalculateAverage(scene);

	//Apply the threshold.
	luminance = max(luminance - BLOOM_THRESHOLD, 0.0f);

    //Write the fragment.
    fragment = vec4(scene * luminance * BLOOM_INTENSITY, 1.0f);
}