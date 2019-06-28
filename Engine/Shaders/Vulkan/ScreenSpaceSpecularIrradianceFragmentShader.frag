//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define SCREEN_SPACE_SPECULAR_IRRADIANCE_MAXIMUM_SAMPLES (64)
#define SCREEN_SPACE_SPECULAR_IRRADIANCE_STEP_LENGTH (0.1f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Calculate the world position at this fragment the current frame.
	vec3 worldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * texture(sceneFeatures2Texture, fragmentTextureCoordinate).w;

	//Calculate the reflection direction.
	vec3 reflectionDirection = reflect(normalize(worldPosition - perceiverWorldPosition), texture(sceneFeatures3Texture, fragmentTextureCoordinate).xyz);

	//Sample the active noise texture.
	vec4 activeNoiseTexture = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

	//Modify the original world position a bit.
	worldPosition += reflectionDirection * SCREEN_SPACE_SPECULAR_IRRADIANCE_STEP_LENGTH * activeNoiseTexture.x;

	//Calculate the screen space specular irradiance.
	vec3 screenSpaceSpecularIrradiance = vec3(0.0f);

	for (int i = 0; i < SCREEN_SPACE_SPECULAR_IRRADIANCE_MAXIMUM_SAMPLES; ++i)
	{
		//Calculate the expected sample world position.
		vec3 expectedSampleWorldPosition = worldPosition + reflectionDirection * SCREEN_SPACE_SPECULAR_IRRADIANCE_STEP_LENGTH * (i + 1);

		//Calculate the sample screen coordinate.
		vec4 sampleViewSpacePosition = viewMatrix * vec4(expectedSampleWorldPosition, 1.0f);
		vec2 sampleScreenCoordinate = 0.5f * (sampleViewSpacePosition.xy / sampleViewSpacePosition.w) + 0.5f;

		//If the sample screen coordinate is outside the screen, just quit, won't find anything after this point.
		if (!ValidCoordinate(sampleScreenCoordinate))
		{
			break;
		}

		//Sample the scene features at the sample coordinate.
		vec4 sampleSceneFeatures = texture(sceneFeatures2Texture, sampleScreenCoordinate);

		//Retrieve the sample geometry normal/hit distance.
		vec3 sampleGeometryNormal = sampleSceneFeatures.xyz;
		float sampleHitDistance = sampleSceneFeatures.w;

		//Calculate the expected sample hit distance.
		float expectedSampleHitDistance = length(expectedSampleWorldPosition - perceiverWorldPosition);

		//If the sample hit distance is lower than the expected sample hit distance, it's a hit. (:
		if (sampleHitDistance < expectedSampleHitDistance)
		{
			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. Are the normals aligned?
			*/
			float sampleWeight = 1.0f;

			sampleWeight = float(dot(reflectionDirection, sampleGeometryNormal) < 0.0f);

			//The specular irradiance at this point is the direct lighting texture at the sample screen coordinate. (:
			screenSpaceSpecularIrradiance = texture(sceneTexture, sampleScreenCoordinate).rgb * sampleWeight;

			break;
		}
	}

    //Write the fragment
    fragment = vec4(screenSpaceSpecularIrradiance, 1.0f);
}