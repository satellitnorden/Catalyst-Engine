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
#define CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR (vec3(0.8f, 0.9f, 1.0f))

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
	volumetricLighting += CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR * vec3(ambientIlluminationIntensity);

	//Calculate the volumetric lighting for all lights.
	for (int i = 0; i < numberOfLights; ++i)
	{
		//Unpack the light.
		Light light = UnpackLight(i);

		//Sample the random vector.
		vec4 randomVector = texture(sampler2D(globalTextures[(activeNoiseTextureIndex + i) & 63], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

		for (int j = 0; j < 4; ++j)
		{
			//Calculate the volumetric particle hit distance.
			float volumetricParticleHitDistance = hitDistance * randomVector[j];

			//Calculate the hit position.
			vec3 hitPosition = perceiverWorldPosition + rayDirection * volumetricParticleHitDistance;

			float lengthToLight = length(light.position - hitPosition);
			vec3 lightDirection = vec3(light.position - hitPosition) / lengthToLight;

			//Calculate the attenuation.
			float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

			//Add the direct lighting.
			volumetricLighting += CATALYST_RAY_TRACING_VOLUMETRIC_LIGHTING_BASE_COLOR * light.color * light.strength * float(numberOfLights) * attenuation * 0.25f;
		}
	}

	//Write the fragment.
	fragment = vec4(volumetricLighting, 1.0f);
}