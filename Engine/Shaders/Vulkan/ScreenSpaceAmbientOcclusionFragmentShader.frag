//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRandomUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (16)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_ORIGIN_BIAS (2.0f) //0.25f step.

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the scene features.
	vec4 scene_features = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec3 normal = UnpackNormal(scene_features.x);
	float hit_distance = scene_features.w;

	//Calculate the world position at this fragment.
	vec3 world_position = PERCEIVER_WORLD_POSITION + CalculateRayDirection(fragment_texture_coordinate) * hit_distance;

	//Calculate the noise texture coordinate.
	vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);

	//Calculate the bias.
	float bias = hit_distance * 0.0325f; //0.0025f step.

	//Calculate the occlusion.
	float occlusion = 0.0f;

	for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
	{
		//Calculate the sample position.
		vec4 random_sample = texture(sampler2D(GLOBAL_TEXTURES[(activeNoiseTextureIndex + i) & 63], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate);

		vec3 random_direction = normalize(random_sample.xyz * 2.0f - 1.0f);
		random_direction = dot(random_direction, normal) >= 0.0f ? random_direction : random_direction * -1.0f;

		float random_length = pow(random_sample.w, SCREEN_SPACE_AMBIENT_OCCLUSION_ORIGIN_BIAS);

		vec3 sample_position = world_position + random_direction * random_length;

		//Calculate the expected hit distance.
		float expected_hit_distance = length(sample_position - PERCEIVER_WORLD_POSITION);

		//Calculate the sample screen coordinate.
		vec4 sample_view_space_position = viewMatrix * vec4(sample_position, 1.0f);
		vec2 sample_screen_coordinate = 0.5f * (sample_view_space_position.xy / sample_view_space_position.w) + 0.5f;

		//Sample the samplescene features.
		vec4 sample_scene_features = texture(scene_features_2_texture, sample_screen_coordinate);
		float sample_hit_distance = sample_scene_features.w + bias;

		//Calculate the distance falloff.
		float distance_falloff = SmoothStep(1.0f - min(abs(expected_hit_distance - sample_hit_distance), 1.0f));

		//If the expected hit distance is greater then the sample hit distance, there is occlusion.
		occlusion += float(expected_hit_distance > sample_hit_distance) * distance_falloff;
	}

	//Normalize the ambient occlusion.
	occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Write the fragment
    fragment = vec4(vec3(occlusion), 1.0f);
}