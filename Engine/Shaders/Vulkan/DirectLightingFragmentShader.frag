//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "..\..\Include\Rendering\Native\Shader\CatalystLighting.h"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 3, binding = 0) uniform sampler2D sceneFeatures1Texture;
layout (set = 3, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 3, binding = 2) uniform sampler2D sceneFeatures3Texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Retrieve the scene features.
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, fragmentTextureCoordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, fragmentTextureCoordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, fragmentTextureCoordinate);

	//Retrieve all properties.
	Material material = GLOBAL_MATERIALS[int(sceneFeatures1.w * 255.0f)];
	vec3 albedo = sceneFeatures1.rgb;
	float depth = sceneFeatures2.w;
	vec3 shadingNormal = sceneFeatures2.xyz;
	float roughness = sceneFeatures3.x;
	float metallic = sceneFeatures3.y;
	float ambientOcclusion = sceneFeatures3.z;

	//Calculate the hit position.
	vec3 world_position = CalculateWorldPosition(fragmentTextureCoordinate, depth);

	//Generate the view direction.
	vec3 view_direction = normalize(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the direct lighting.
	vec3 direct_lighting = vec3(0.0f);

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
													shadingNormal,
													roughness,
													metallic,
													ambientOcclusion,
													material.thickness,
													light.position_or_direction,
													light.color * light.intensity);

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
															shadingNormal,
															roughness,
															metallic,
															ambientOcclusion,
															material.thickness,
															light_direction,
															light.color * light.intensity) * attenuation;
				}

				break;
			}
		}
	}

	//Write the fragment.
	fragment = vec4(direct_lighting, 1.0f);
}