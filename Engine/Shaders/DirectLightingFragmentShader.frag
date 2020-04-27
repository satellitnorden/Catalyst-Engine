//Includes.
#include "CatalystLightingData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 3, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 3, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 3, binding = 2) uniform sampler2D scene_features_3_texture;
layout (set = 3, binding = 3) uniform sampler2D shadows_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Retrieve the scene features.
	vec4 scene_features_1 = texture(scene_features_1_texture, fragment_texture_coordinate);
	vec4 scene_features_2 = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec4 scene_features_3 = texture(scene_features_3_texture, fragment_texture_coordinate);
	vec4 shadows = texture(shadows_texture, fragment_texture_coordinate);

	//Retrieve all properties.
	Material material = GLOBAL_MATERIALS[int(scene_features_1.w * 255.0f)];
	vec3 albedo = scene_features_1.rgb;
	float depth = scene_features_2.w;
	vec3 shading_normal = scene_features_2.xyz;
	float roughness = scene_features_3.x;
	float metallic = scene_features_3.y;
	float ambient_occlusion = scene_features_3.z;

	//Calculate the hit position.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, depth);

	//Generate the view direction.
	vec3 view_direction = normalize(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the direct lighting.
	vec3 direct_lighting = vec3(0.0f);
	uint current_shadow_index = 0;

	//Calculate all lights.
	for (int i = 0; i < NUMBER_OF_LIGHTS; ++i)
	{
		Light light = UnpackLight(i);

		switch (light.light_type)
		{
			case LIGHT_TYPE_DIRECTIONAL:
			{
				direct_lighting += CalculateLighting(-view_direction,
													albedo,
													shading_normal,
													roughness,
													metallic,
													ambient_occlusion,
													material.thickness,
													light.position_or_direction,
													light.color * light.intensity) * shadows[current_shadow_index++];

				break;
			}

			case LIGHT_TYPE_POINT:
			{
				//Calculate the light direction.
				vec3 light_direction = world_position - light.position_or_direction;

				//Determine the distance to the light.
				float distance_to_light = LengthSquared3(light_direction);

				//Only calculate lighting if the the world position is within the light's radius.
				if (distance_to_light < light.radius * light.radius)
				{
					//Normalize.
					distance_to_light = sqrt(distance_to_light);
					light_direction /= distance_to_light;

					//Calculate the attenuation.
					float attenuation = CalculateAttenuation(distance_to_light, light.radius);

					direct_lighting += CalculateLighting(	-view_direction,
															albedo,
															shading_normal,
															roughness,
															metallic,
															ambient_occlusion,
															material.thickness,
															light_direction,
															light.color * light.intensity) * attenuation * shadows[current_shadow_index++];
				}

				break;
			}
		}
	}

	//Write the fragment.
	fragment = vec4(direct_lighting, 1.0f);
}