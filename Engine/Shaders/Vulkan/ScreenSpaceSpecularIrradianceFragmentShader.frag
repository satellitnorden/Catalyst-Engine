//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define SCREEN_SPACE_SPECULAR_IRRADIANCE_MAXIMUM_SAMPLES (32)
#define SCREEN_SPACE_SPECULAR_IRRADIANCE_STEP_LENGTH (0.1f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 2) uniform sampler2D directLightingTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Calculate the world position at this fragment the current frame.
	vec3 worldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * texture(sceneFeatures2Texture, fragmentTextureCoordinate).w;

	//Calculate the reflection direction.
	vec3 reflectionDirection = reflect(normalize(worldPosition - perceiverWorldPosition), texture(sceneFeatures3Texture, fragmentTextureCoordinate).xyz);

	//Calculate the screen coordinate delta.
	vec4 nextViewSpacePosition = viewMatrix * vec4(worldPosition + reflectionDirection * SCREEN_SPACE_SPECULAR_IRRADIANCE_STEP_LENGTH, 1.0f);
	vec2 nextScreenCoordinate = 0.5f * (nextViewSpacePosition.xy / nextViewSpacePosition.w) + 0.5f;

	vec2 screenCoordinateDelta = nextScreenCoordinate - fragmentTextureCoordinate;

	//Calculate the screen space specular irradiance.
	vec3 screenSpaceSpecularIrradiance = vec3(0.0f);

	for (int i = 0; i < SCREEN_SPACE_SPECULAR_IRRADIANCE_MAXIMUM_SAMPLES; ++i)
	{
		//Calculate the sample coordinate.
		vec2 sampleCoordinate = fragmentTextureCoordinate + screenCoordinateDelta * (i + 1);

		//Break if the sample coordinate isn't valid.
		if (!ValidCoordinate(sampleCoordinate))
		{
			break;
		}

		//Sample the scene features at the sample coordinate.
		vec4 sampleSceneFeatures = texture(sceneFeatures2Texture, sampleCoordinate);

		//Retrieve the sample geometry normal/hit distance.
		vec3 sampleGeometryNormal = sampleSceneFeatures.xyz;
		float sampleHitDistance = sampleSceneFeatures.w;

		//Calculate the expected sample world position.
		vec3 expectedSampleWorldPosition = worldPosition + reflectionDirection * SCREEN_SPACE_SPECULAR_IRRADIANCE_STEP_LENGTH * (i + 1);

		//Calculate the expected sample hit distance.
		float expectedSampleHitDistance = length(expectedSampleWorldPosition - perceiverWorldPosition);

		//If the sample hit distance is lower than the expected sample hit distance, it's a hit. (:
		if (sampleHitDistance < expectedSampleHitDistance)
		{
			//Test the normal.
			if (dot(reflectionDirection, sampleGeometryNormal) < 0.0f)
			{
				//Calculate the edge fade factor.
				float edgeFadeFactor = (1.0f - (abs(0.5f - sampleCoordinate.x) * 2.0f)) * (1.0f - (abs(0.5f - sampleCoordinate.y) * 2.0f));
				edgeFadeFactor = 1.0f - pow(1.0f - edgeFadeFactor, 4.0f);

				//The specular irradiance at this point is the direct lighting texture at the sample coordinate. (:
				screenSpaceSpecularIrradiance = texture(directLightingTexture, sampleCoordinate).rgb * edgeFadeFactor;
			}

			break;
		}
	}

    //Write the fragment
    fragment = vec4(screenSpaceSpecularIrradiance, 1.0f);
}