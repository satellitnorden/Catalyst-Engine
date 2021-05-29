//Includes.
#define CATALYST_LIGHTING_DATA_SET_INDEX (2)
#include "CatalystLightingData.glsl"

//Constants.
#define CATALYST_VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER (0.1225f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 SKY_LIGHT_LUMINANCE;
	layout (offset = 16) vec2 SKY_LIGHT_SCREEN_SPACE_POSITION;
};

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Load the scene features.
	vec4 scene_features_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Retrieve all properties.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w);
	float hit_distance = length(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the ray direction.
	vec3 ray_direction = (world_position - PERCEIVER_WORLD_POSITION) / hit_distance;

	//Calculate the ambient lighting.
	vec3 ambient_lighting = CalculateVolumetricAmbientLighting();

	//Calculate the sky light lighting.
	vec3 sky_light_lighting = VOLUMETRIC_LIGHTING_BASE_COLOR * SKY_LIGHT_LUMINANCE * CATALYST_VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER;

	//Calculate the sky light influence.
	float sky_light_influence = min(abs(0.5f - SKY_LIGHT_SCREEN_SPACE_POSITION.x) * 2.0f * abs(0.5f - SKY_LIGHT_SCREEN_SPACE_POSITION.y) * 2.0f, 1.0f);
	sky_light_influence *= sky_light_influence;
	sky_light_influence = 1.0f - sky_light_influence;

	//Sample the noise.
	vec4 noise_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

	//Accumulate occlusion.
	float occlusion = 0.0f;

	for (uint i = 0; i < 4; ++i)
	{
		//Calculate the sample point.
		vec2 sample_point = mix(fragment_texture_coordinate, SKY_LIGHT_SCREEN_SPACE_POSITION, noise_sample[i]);

		//Accumulate occlusion.
		occlusion += float(texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), sample_point).w > 0.0f) * 0.25f;
	}

	//Calculate the disocclusion.
	float disocclusion = 1.0f - min(occlusion, 1.0f);

	//Write the fragment.
	fragment = vec4(mix(ambient_lighting, ambient_lighting + sky_light_lighting, mix(1.0f, disocclusion * disocclusion, sky_light_influence)), 1.0f);
}