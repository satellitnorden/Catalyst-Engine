//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float CHROMATIC_ABERRATION_INTENSITY;
	layout (offset = 4) float CONTRAST;
	layout (offset = 8) float FILM_GRAIN_INTENSITY;
	layout (offset = 12) float HORIZONTAL_BORDER;
};

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
	vec2 offset_weight = vec2(1.0f - edge_factor);

	//Account for the aspect ratio.
	offset_weight.y *= ASPECT_RATIO;

	//Calculate the chromatic aberration.
	return vec3(texture(source_texture, fragment_texture_coordinate - vec2(CHROMATIC_ABERRATION_INTENSITY, CHROMATIC_ABERRATION_INTENSITY) * offset_weight.x).r, texture(source_texture, fragment_texture_coordinate + vec2(CHROMATIC_ABERRATION_INTENSITY, CHROMATIC_ABERRATION_INTENSITY) * offset_weight.y).gb);
}

/*
*	Applies film grain.
*/
vec3 ApplyFilmGrain(vec3 fragment)
{
	return mix(fragment, vec3(RandomFloat(fragment_texture_coordinate, globalRandomSeed1)), FILM_GRAIN_INTENSITY);
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
	return fragment * float(fragment_texture_coordinate.y >= HORIZONTAL_BORDER && fragment_texture_coordinate.y <= (1.0f - HORIZONTAL_BORDER));
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
	post_processed_fragment = ApplyContrast(post_processed_fragment, CONTRAST);

	//Apply film grain.
	post_processed_fragment = ApplyFilmGrain(post_processed_fragment);

	//Apply vignette.
	post_processed_fragment = ApplyVignette(post_processed_fragment, edge_factor);

	//Apply borders.
	post_processed_fragment = ApplyBorders(post_processed_fragment);

    //Write the fragment.
    fragment = vec4(post_processed_fragment, 1.0f);
}