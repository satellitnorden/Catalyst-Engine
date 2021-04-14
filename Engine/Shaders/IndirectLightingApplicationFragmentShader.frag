//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Constants.
#define DEPTH_WEIGHT (2.0f)
#define DITHER_STRENGTH (0.1f)
#define INDIRECT_LIGHTING_QUALITY_LOW (0)
#define INDIRECT_LIGHTING_QUALITY_HIGH (1)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint INDIRECT_LIGHTING_ENABLED;
	layout (offset = 4) uint INDIRECT_LIGHTING_QUALITY;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the indirect lighting at the given coordinate.
*/
vec4 SampleIndirectLighting(vec2 coordinate)
{
	if (INDIRECT_LIGHTING_ENABLED != 0)
	{
		//If the indirect lighting quality is set to high (rendering at full resolution), then no upsampling needs to happen.
		if (INDIRECT_LIGHTING_QUALITY == INDIRECT_LIGHTING_QUALITY_HIGH)
		{
			//Sample the final blend.
			vec4 final_blend = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

			//Return the final blend.
			return final_blend;
		}

		else
		{
			//Sample the current depth.
			float current_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate).w);

			//Sample the four neighbor samples along with their depth.
			vec4 sample_1_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f));
			float sample_1_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f)).w);
			vec4 sample_2_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f));
			float sample_2_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).w);
			vec4 sample_3_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f));
			float sample_3_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).w);
			vec4 sample_4_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f));
			float sample_4_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).w);

			//Calculate the horizontal and vertical weights.
			float horizontal_weight = fract(coordinate.x * (SCALED_RESOLUTION.x * 0.5f));
			float vertical_weight = fract(coordinate.y * (SCALED_RESOLUTION.y * 0.5f));

			//Calculate all the weights.
			float first_weight = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
			float second_weight = (1.0f - horizontal_weight) * vertical_weight;
			float third_weight = horizontal_weight * (1.0f - vertical_weight);
			float fourth_weight = horizontal_weight * vertical_weight;

			//Alter the weights depending on how closely aligned they are with the current depth.
			first_weight *= pow(1.0f - abs(current_depth - sample_1_depth), DEPTH_WEIGHT);
			second_weight *= pow(1.0f - abs(current_depth - sample_2_depth), DEPTH_WEIGHT);
			third_weight *= pow(1.0f - abs(current_depth - sample_3_depth), DEPTH_WEIGHT);
			fourth_weight *= pow(1.0f - abs(current_depth - sample_4_depth), DEPTH_WEIGHT);

			//Renormalize the weights.
			float total_weight_reciprocal = 1.0f / (first_weight + second_weight + third_weight + fourth_weight);

			first_weight *= total_weight_reciprocal;
			second_weight *= total_weight_reciprocal;
			third_weight *= total_weight_reciprocal;
			fourth_weight *= total_weight_reciprocal;

			//Perform the final blend.
			vec4 final_blend = 	sample_1_color * first_weight
								+ sample_2_color * second_weight
								+ sample_3_color * third_weight
								+ sample_4_color * fourth_weight;

			//Return the final blend.
			return final_blend;
		}
	}
	
	else
	{
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

void CatalystShaderMain()
{
	//Sample the indirect lighting while sharpening it a bit.
#if 0 //Apply sharpening.
	vec4 indirect_lighting = 	SampleIndirectLighting(fragment_texture_coordinate) * 9.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(-1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(-1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(-1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(0.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(0.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f;
#else
	vec4 indirect_lighting = SampleIndirectLighting(fragment_texture_coordinate);
#endif

	//No negative values. \o/
	indirect_lighting = vec4(max(indirect_lighting.rgb, vec3(0.0f, 0.0f, 0.0f)), clamp(indirect_lighting.a, 0.0f, 1.0f));

	//Blend in the sky a bit to account for misses.
	vec4 scene_features_1 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);
	vec4 scene_features_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);
	vec4 scene_features_3 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	vec3 view_direction = normalize(CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w) - PERCEIVER_WORLD_POSITION);
	vec3 specular_direction = reflect(view_direction, scene_features_2.xyz);
	vec3 diffuse_direction = scene_features_2.xyz;

	float diffuse_weight = scene_features_3[0] * (1.0f - scene_features_3[1]);

	vec3 indirect_lighting_direction = normalize(mix(specular_direction, diffuse_direction, diffuse_weight));

	vec3 sky_lighting = CalculateLighting(	-view_direction,
											scene_features_1.rgb,
											scene_features_2.xyz,
											scene_features_3[0],
											scene_features_3[1],
											scene_features_3[2],
											1.0f,
											-indirect_lighting_direction,
											SampleSky(indirect_lighting_direction, MAX_SKY_TEXTURE_MIPMAP_LEVEL * diffuse_weight));

	indirect_lighting.rgb = mix(sky_lighting, indirect_lighting.rgb, indirect_lighting.a * (1.0f - diffuse_weight));

	//Write the fragment.
	scene = vec4(indirect_lighting.rgb, 1.0f);
}