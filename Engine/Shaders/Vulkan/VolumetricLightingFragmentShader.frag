//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Constants.
#define CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR (vec3(0.6f, 0.8f, 1.0f))
#define CATALYST_VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER (0.125f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 3, binding = 0) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Load the scene features.
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, fragmentTextureCoordinate);

	//Retrieve all properties.
	vec3 world_position = CalculateWorldPosition(fragmentTextureCoordinate, sceneFeatures2.w);
	float hitDistance = length(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the ray direction.
	vec3 rayDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Calculate the volumetric lighting.
	vec3 volumetric_lighting = vec3(0.0f);

	//Add the ambient lighting.
	volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(-1.0f, 0.0f, 0.0f)).rgb * 0.167;
	volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(1.0f, 0.0f, 0.0f)).rgb * 0.167;
	volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, -1.0f, 0.0f)).rgb * 0.167;
	volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, 1.0f, 0.0f)).rgb * 0.167;
	volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, 0.0f, -1.0f)).rgb * 0.167;
	volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], vec3(0.0f, 0.0f, 1.0f)).rgb * 0.167;

	//Sample the noise vector.
	vec4 noise_vector = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

	//Calculate the volumetric lighting for all volumetric lights.
	for (int i = 0; i < NUMBER_OF_LIGHTS; ++i)
	{
		//Unpack the light.
		Light light = UnpackLight(i);

		switch (light.light_type)
		{
			case LIGHT_TYPE_DIRECTIONAL:
			{
				for (int j = 0; j < 4; ++j)
				{
					//Calculate the volumetric particle hit distance.
					float volumetric_particle_hit_distance = hitDistance * noise_vector[j];

					//Calculate the volumetric particle hit position.
					vec3 volumetric_particle_hit_position = PERCEIVER_WORLD_POSITION + rayDirection * volumetric_particle_hit_distance;

					//Add the volumetric lighting.
					volumetric_lighting += CATALYST_VOLUMETRIC_LIGHTING_BASE_COLOR * light.color * light.intensity * CATALYST_VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER * 0.25f;
				}

				break;
			}

			case LIGHT_TYPE_POINT:
			{
				//TODO

				break;
			}
		}
	}

	//Write the fragment.
	fragment = vec4(volumetric_lighting, 1.0f);
}