//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define POST_PROCESSING_CONTRAST (1.025f) //0.025f step.
#define POST_PROCESSING_FILM_GRAIN_STRENGTH (0.0125f) //0.0025f step.

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D source_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Applies chromatic aberration.
*/
vec3 ApplyChromaticAberration(vec3 fragment, float edge_factor)
{
	//Determine the offset weight.
	float offset_weight = 1.0f - edge_factor;

	//Calculate the chromatic aberration.
	return vec3(texture(source_texture, fragment_texture_coordinate - vec2(chromaticAberrationIntensity, chromaticAberrationIntensity) * offset_weight).r, texture(source_texture, fragment_texture_coordinate + vec2(chromaticAberrationIntensity, chromaticAberrationIntensity) * offset_weight).gb);
}

/*
*	Applies contrast.
*/
vec3 ApplyContrast(vec3 fragment)
{
	//return mix(fragment, vec3(SmoothStep(fragment.r), SmoothStep(fragment.g), SmoothStep(fragment.b)), 1.0f - POST_PROCESSING_CONTRAST);
	//return mix(vec3(0.5f), fragment, POST_PROCESSING_CONTRAST);
    return fragment * POST_PROCESSING_CONTRAST + (0.5f - POST_PROCESSING_CONTRAST * 0.5f);
}

/*
*	Applies film grain.
*/
vec3 ApplyFilmGrain(vec3 fragment)
{
	return mix(fragment, vec3(RandomFloat(fragment_texture_coordinate, globalRandomSeed1)), POST_PROCESSING_FILM_GRAIN_STRENGTH);
}

/*
*	Applies vignette.
*/
vec3 ApplyVignette(vec3 fragment, float edge_factor)
{
	return mix(vec3(0.0f), fragment, edge_factor);
}

/*
*	Applies borders.
*/
vec3 ApplyBorders(vec3 fragment)
{
	return fragment * float(fragment_texture_coordinate.y >= 0.0f && fragment_texture_coordinate.y <= 1.0f);
}

void main()
{
	//Sample the source texture.
	vec3 post_processed_fragment = texture(source_texture, fragment_texture_coordinate).rgb;

	//Determine the direction of the current fragment.
	vec3 fragment_direction = CalculateRayDirection(fragment_texture_coordinate);

	//Calculate the edge factor.
	float edge_factor = max(dot(perceiverForwardVector, fragment_direction), 0.0f);

	//Apply chromatic aberration.
	post_processed_fragment = ApplyChromaticAberration(post_processed_fragment, edge_factor);

	//Apply contrast.
	post_processed_fragment = ApplyContrast(post_processed_fragment);

	//Apply film grain.
	post_processed_fragment = ApplyFilmGrain(post_processed_fragment);

	//Apply vignette.
	post_processed_fragment = ApplyVignette(post_processed_fragment, edge_factor);

	//Apply borders.
	post_processed_fragment = ApplyBorders(post_processed_fragment);

    //Write the fragment.
    fragment = vec4(post_processed_fragment, 1.0f);
}