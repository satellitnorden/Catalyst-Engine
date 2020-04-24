//Includes.
#include "CatalystLightingData.glsl"

//Constants.
#define CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR (vec3(0.6f, 0.8f, 1.0f))
#define CATALYST_VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER (0.125f)

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

//Texture samplers.
layout (set = 3, binding = 0) uniform sampler2D scene_features_2_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Load the scene features.
	vec4 scene_features_2 = texture(scene_features_2_texture, fragment_texture_coordinate);

	//Retrieve all properties.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w);
	float hit_distance = length(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the ray direction.
	vec3 ray_direction = (world_position - PERCEIVER_WORLD_POSITION) / hit_distance;

	//Calculate the ambient lighting.
	vec3 ambient_lighting = vec3(0.0f);

	ambient_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(-1.0f, 0.0f, 0.0f)).rgb * 0.167;
	ambient_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(1.0f, 0.0f, 0.0f)).rgb * 0.167;
	ambient_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, -1.0f, 0.0f)).rgb * 0.167;
	ambient_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, 1.0f, 0.0f)).rgb * 0.167;
	ambient_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, 0.0f, -1.0f)).rgb * 0.167;
	ambient_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, 0.0f, 1.0f)).rgb * 0.167;

	//Calculate the sky light lighting.
	vec3 sky_light_lighting = CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * SKY_LIGHT_LUMINANCE * CATALYST_VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER;

	//Different logic depending on if the sky light is on the screen or not.
	if (ValidCoordinate(SKY_LIGHT_SCREEN_SPACE_POSITION))
	{
		//Sample the noise.
		vec4 noise_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

		//Accumulate occlusion.
		float occlusion = 0.0f;

		for (uint i = 0; i < 4; ++i)
		{
			//Calculate the sample point.
			vec2 sample_point = mix(fragment_texture_coordinate, SKY_LIGHT_SCREEN_SPACE_POSITION, noise_sample[i]);

			//Accumulate occlusion.
			occlusion += float(texture(scene_features_2_texture, sample_point).w > 0.0f) * 0.25f;
		}

		//Calculate the disocclusion.
		float disocclusion = 1.0f - min(occlusion, 1.0f);

		//Write the fragment.
		fragment = vec4(mix(ambient_lighting, sky_light_lighting, disocclusion), disocclusion);
	}

	else
	{
		//Write the fragment.
		fragment = vec4(mix(ambient_lighting, sky_light_lighting, 0.5f), 0.5f);
	}
}