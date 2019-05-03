//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define POST_PROCESSING_CHROMATIC_ABERRATION_SCALE (0.00125f) //0.00025f step.
#define POST_PROCESSING_VIGNETTE_STRENGTH (2.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sourceTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Applies chromatic aberration.
*/
vec3 ApplyChromaticAberration(vec3 fragment, float edgeFactor)
{
	//Determine the offset direction.
	vec2 offsetDirection = normalize(fragmentTextureCoordinate - vec2(0.5f, 0.5f)) * POST_PROCESSING_CHROMATIC_ABERRATION_SCALE;

	//Determine the offset weight.
	float offsetWeight = 1.0f - edgeFactor;

	//Calculate the chromatic aberration.
	return vec3(texture(sourceTexture, fragmentTextureCoordinate + offsetDirection * offsetWeight).r, texture(sourceTexture, fragmentTextureCoordinate - offsetDirection * offsetWeight).gb);
}

/*
*	Applies vignette.
*/
vec3 ApplyVignette(vec3 fragment, float edgeFactor)
{
	return mix(vec3(0.0f), fragment, pow(edgeFactor, POST_PROCESSING_VIGNETTE_STRENGTH));
}

void main()
{
	//Sample the source texture.
	vec3 postProcessedFragment = texture(sourceTexture, fragmentTextureCoordinate).rgb;

	//Determine the direction of the current fragment.
	vec3 fragmentDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Calculate the edge factor.
	float edgeFactor = max(dot(perceiverForwardVector, fragmentDirection), 0.0f);

	//Apply chromatic aberration.
	postProcessedFragment = ApplyChromaticAberration(postProcessedFragment, edgeFactor);

	//Apply vignette.
	postProcessedFragment = ApplyVignette(postProcessedFragment, edgeFactor);

    //Write the fragment.
    fragment = vec4(postProcessedFragment, 1.0f);
}