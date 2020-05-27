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
	layout (offset = 0) uint INDIRECT_LIGHTING_QUALITY;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the indirect lighting at the given coordinate.
*/
vec3 SampleIndirectLighting(vec2 coordinate)
{
	//If the indirect lighting quality is set to high (rendering at full resolution), then no upsampling needs to happen.
	if (INDIRECT_LIGHTING_QUALITY == INDIRECT_LIGHTING_QUALITY_HIGH)
	{
		//Sample the final blend.
		vec3 final_blend = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate).rgb;

		//Return the final blend.
		return final_blend;
	}

	else
	{
		//Sample the current depth.
		float current_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate).w);

		//Sample the four neighbor samples along with their depth.
		vec3 sample_1_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f)).rgb;
		float sample_1_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f)).w);
		vec3 sample_2_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).rgb;
		float sample_2_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).w);
		vec3 sample_3_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).rgb;
		float sample_3_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).w);
		vec3 sample_4_color = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).rgb;
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
		vec3 final_blend = 	sample_1_color * first_weight
							+ sample_2_color * second_weight
							+ sample_3_color * third_weight
							+ sample_4_color * fourth_weight;

		//Return the final blend.
		return final_blend;
	}
}

void CatalystShaderMain()
{
	//Sample the indirect lighting while sharpening it a bit.
	vec3 indirect_lighting = 	SampleIndirectLighting(fragment_texture_coordinate) * 9.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(-1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(-1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(-1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(0.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(0.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleIndirectLighting(fragment_texture_coordinate + vec2(1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f;

	//No negative values. \o/
	indirect_lighting = max(indirect_lighting, vec3(0.0f, 0.0f, 0.0f));

	//Write the fragment.
	scene = vec4(indirect_lighting, 1.0f);
}