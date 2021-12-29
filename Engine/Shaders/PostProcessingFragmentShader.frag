//Includes.
#include "CatalystRayTracingCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float CHROMATIC_ABERRATION_INTENSITY;
	layout (offset = 4) float BRIGHTNESS;
	layout (offset = 8) float CONTRAST;
	layout (offset = 12) float FILM_GRAIN_INTENSITY;
	layout (offset = 16) float HORIZONTAL_BORDER;
	layout (offset = 20) float SATURATION;
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
*	Samples with chromatic aberration.
*/
vec3 SampleWithChromaticAberration(float edge_factor, vec2 coordinate)
{
	//Determine the offset weight.
	vec2 offset_weight = vec2(1.0f - edge_factor);

	//Account for the aspect ratio.
	offset_weight.y *= ASPECT_RATIO;

	//Calculate the chromatic aberration.
	return vec3(texture(source_texture, coordinate - vec2(CHROMATIC_ABERRATION_INTENSITY, CHROMATIC_ABERRATION_INTENSITY) * offset_weight.x).r, texture(source_texture, coordinate + vec2(CHROMATIC_ABERRATION_INTENSITY, CHROMATIC_ABERRATION_INTENSITY) * offset_weight.y).gb);
}

/*
*	Samples with sharpen.
*/
vec3 SampleWithSharpen(float edge_factor)
{
	vec3 original_sample = SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate);

	vec3 sharpened_sample = original_sample * 7.0f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(-1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(0.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(0.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(-1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -0.5f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(-1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -0.5f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -0.5f
							+ SampleWithChromaticAberration(edge_factor, fragment_texture_coordinate + vec2(1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -0.5f;

	return sharpened_sample;
}

/*
*	Applies brightness.
*/
vec3 ApplyBrightness(vec3 fragment)
{
	return fragment + vec3(BRIGHTNESS - 1.0f);
}

/*
*	Applies contrast.
*/
vec3 ApplyContrast(vec3 fragment)
{
	return ((fragment - 0.5f) * CONTRAST) + 0.5f;
}

/*
*	Applies saturation.
*/
vec3 ApplySaturation(vec3 fragment)
{
	return mix(vec3(CalculateAverage(fragment)), fragment, SATURATION);
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
*	Apply dithering.
*/
vec3 ApplyDithering(vec3 fragment)
{
	vec4 blue_noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

	return clamp(fragment + ((blue_noise_texture_sample.rgb * 2.0f - 1.0f) * 0.125f * 0.125f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
}

/*
*	Applies borders.
*/
vec3 ApplyBorders(vec3 fragment)
{
	return fragment * float(fragment_texture_coordinate.y >= HORIZONTAL_BORDER && fragment_texture_coordinate.y <= (1.0f - HORIZONTAL_BORDER));
}

void CatalystShaderMain()
{
	//Determine the direction of the current fragment.
	vec3 fragment_direction = CalculateRayDirection(fragment_texture_coordinate);

	//Calculate the edge factor.
	float edge_factor = max(dot(PERCEIVER_FORWARD_VECTOR, fragment_direction), 0.0f);

	//Sample with chromatic aberration.
	vec3 post_processed_fragment = SampleWithSharpen(edge_factor);

	//Apply brightness.
	post_processed_fragment = ApplyBrightness(post_processed_fragment);

	//Apply contrast.
	post_processed_fragment = ApplyContrast(post_processed_fragment);

	//Apply saturation.
	post_processed_fragment = ApplySaturation(post_processed_fragment);

	//Apply film grain.
	post_processed_fragment = ApplyFilmGrain(post_processed_fragment);

	//Apply vignette.
	post_processed_fragment = ApplyVignette(post_processed_fragment, edge_factor);

	//Apply dithering.
	post_processed_fragment = ApplyDithering(post_processed_fragment);

	//Apply borders.
	post_processed_fragment = ApplyBorders(post_processed_fragment);

    //Write the fragment.
    fragment = vec4(post_processed_fragment, 1.0f);
}