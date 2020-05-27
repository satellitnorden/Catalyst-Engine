//Defines.
#define DEPTH_WEIGHT (2.0f)
#define DITHER_STRENGTH (0.1f)
#define BIAS_AMBIENT_OCCLUSION(X) (X * X * X)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_3;

/*
*	Samples the ambient occlusion at the given coordinate.
*/
float SampleAmbientOcclusion(vec2 coordinate)
{
	//Sample the current depth.
	float current_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate).w);

	//Sample the four neighbor samples along with their depth.
	float sample_1_color = texture(sampler2D(RENDER_TARGETS[AMBIENT_OCCLUSION_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f)).r;
	float sample_1_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, 0.0f)).w);
	float sample_2_color = texture(sampler2D(RENDER_TARGETS[AMBIENT_OCCLUSION_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).r;
	float sample_2_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(0.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).w);
	float sample_3_color = texture(sampler2D(RENDER_TARGETS[AMBIENT_OCCLUSION_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).r;
	float sample_3_depth = LinearizeDepth(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, 0.0f)).w);
	float sample_4_color = texture(sampler2D(RENDER_TARGETS[AMBIENT_OCCLUSION_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate + vec2(INVERSE_SCALED_RESOLUTION.x * 2.0f, INVERSE_SCALED_RESOLUTION.y * 2.0f)).r;
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
	float final_blend = sample_1_color * first_weight
						+ sample_2_color * second_weight
						+ sample_3_color * third_weight
						+ sample_4_color * fourth_weight;

	//Return the final blend.
	return final_blend;
}

void CatalystShaderMain()
{
	//Sample the ambient occlusion while sharpening it a bit.
	float ambient_occlusion = 	SampleAmbientOcclusion(fragment_texture_coordinate) * 9.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(-1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(-1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(-1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(0.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(0.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(1.0f, -1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(1.0f, 0.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f
								+ SampleAmbientOcclusion(fragment_texture_coordinate + vec2(1.0f, 1.0f) * INVERSE_SCALED_RESOLUTION) * -1.0f;

	//Sample the scene features 3 texture.
	vec4 scene_features_3_texture_sampler = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Write the fragment.
	scene_features_3 = vec4(scene_features_3_texture_sampler.x,
							scene_features_3_texture_sampler.y,
							scene_features_3_texture_sampler.z * BIAS_AMBIENT_OCCLUSION(ambient_occlusion),
							scene_features_3_texture_sampler.w);
}