//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float intensity;
};

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

    //Write the fragment.
    fragment = vec4(scene * luminance * intensity, 1.0f);
}