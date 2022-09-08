//Includes.
#include "CatalystIndirectLightingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 COORDINATE_STEP;
};

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_2_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_3_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D SCENE_FEATURES_4_TEXTURE;
layout (set = 1, binding = 3) uniform sampler2D SCREEN_SPACE_INDIRECT_LIGHTING_TEXTURE;
layout (set = 1, binding = 4) uniform sampler2D SCENE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 indirect_lighting;
layout (location = 1) out vec4 temporal_reprojection_buffer;

void CatalystShaderMain()
{
	//Sample the scene features.
	vec4 scene_features_2 = texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate);
	vec4 scene_features_3 = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate);

	//Cache values.
	vec3 normal = scene_features_2.xyz;
	float depth = scene_features_2.w;
	float roughness = scene_features_3.x;
	float metallic = scene_features_3.y;
	float ambient_occlusion = scene_features_3.z;

	//Calculate the world position at this fragment.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, depth);

	//Calculate the view direction.
	vec3 view_direction = normalize(CAMERA_WORLD_POSITION - world_position);

	//Accumulate the radiance.
	vec3 radiance = vec3(0.0f);
	vec3 hit_position = vec3(0.0f);
	float total_weight = 0.0f;

	for (int Y = -1; Y <= 1; ++Y)
	{
		for (int X = -1; X <= 1; ++X)
		{
			//Calculate the sample coordinate.
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(X) * COORDINATE_STEP.x, float(Y) * COORDINATE_STEP.y);

			if (!ValidCoordinate(sample_coordinate))
			{
				continue;
			}

			//Retrieve the input data.
			vec4 input_data = texture(SCREEN_SPACE_INDIRECT_LIGHTING_TEXTURE, sample_coordinate);

			//Unpack the screen space indirect lighting data.
			ScreenSpaceIndirectLightingData data = PackScreenSpaceIndirectLightingData(input_data);

			if (data._HitPercentage == 0.0f)
			{
				continue;
			}

			//Calculate the screen coordinate.
			vec2 screen_coordinate = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, data._HitPosition);

			//Calculate the irradiance direction.
			vec3 irradiance_direction = normalize(world_position - data._HitPosition);

			//Retrieve the velocity.
			vec2 velocity = texture(SCENE_FEATURES_4_TEXTURE, screen_coordinate).xy;

			//Retrieve the sample radiance.
			vec3 sample_radiance = texture(SCENE_TEXTURE, screen_coordinate - velocity).rgb;

			//Sample the BRDF.
			float weight = SampleBidirectionalReflectanceDistribution
			(
				view_direction,
				vec3(0.0f),
				normal,
				max(roughness, FLOAT32_EPSILON),
				metallic,
				ambient_occlusion,
				1.0f,
				irradiance_direction,
				vec3(0.0f)
			) * data._HitPercentage;

			//Accumulate.
			radiance += sample_radiance * weight;
			hit_position += data._HitPosition * weight;
			total_weight += weight;
		}
	}

	//Normalize the radiance.
	radiance = total_weight != 0.0f ? radiance / total_weight : vec3(0.0f);

	//Normalize the hit position.
	hit_position = total_weight != 0.0f ? hit_position / total_weight : vec3(0.0f);

    //Write the fragment
    indirect_lighting = vec4(radiance, float(total_weight > 0.0f));
    temporal_reprojection_buffer = vec4(CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, hit_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, hit_position), 0.0f, 0.0f);
}