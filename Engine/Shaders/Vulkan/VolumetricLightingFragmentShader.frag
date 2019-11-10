//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystModelData.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Constants.
#define CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR (vec3(0.6f, 0.8f, 1.0f))

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
	float hitDistance = sceneFeatures2.w;

	//Calculate the ray direction.
	vec3 rayDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Calculate the volumetric lighting.
	vec3 volumetricLighting = vec3(0.0f);

	//Add the ambient lighting.
	volumetricLighting += CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR * CalculateAmbientIlluminationIntensity();

	//Calculate the volumetric lighting for all lights.
	for (int i = 0; i < numberOfLights; ++i)
	{
		//Unpack the light.
		Light light = UnpackLight(i);

		switch (light.type)
		{
			case LIGHT_TYPE_DIRECTIONAL:
			{
				//Calculate the density multiplier.
				float density_multiplier = exp(-(1.0f + volumetricLightingIntensity));

				//Add the volumetric lighting lighting.
				volumetricLighting += CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR * light.luminance * density_multiplier;

				break;
			}

			case LIGHT_TYPE_POINT:
			{
				//Sample the random vector.
				vec4 randomVector = texture(sampler2D(globalTextures[(activeNoiseTextureIndex + i) & 63], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

				for (int j = 0; j < 4; ++j)
				{
					//Calculate the density multiplier.
					float density_multiplier = exp(-(1.0f + volumetricLightingIntensity));

					//Calculate the volumetric particle hit distance.
					float volumetricParticleHitDistance = hitDistance * randomVector[j];

					//Calculate the hit position.
					vec3 hitPosition = perceiverWorldPosition + rayDirection * volumetricParticleHitDistance;

					float lengthToLight = length(light.position_or_direction - hitPosition);
					vec3 lightDirection = vec3(light.position_or_direction - hitPosition) / lengthToLight;

					//Calculate the attenuation.
					float attenuation = pow(1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight)), 1.5f);

					//Add the direct lighting.
					volumetricLighting += CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR * light.luminance * attenuation * density_multiplier * 0.25f;
				}

				break;
			}
		}
	}

	//Write the fragment.
	fragment = vec4(volumetricLighting, 1.0f);
}