//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the scene features.
	vec4 sceneFeatures = texture(sceneFeatures2Texture, fragmentTextureCoordinate);
	vec3 geometryNormal = sceneFeatures.xyz;
	float hitDistance = sceneFeatures.w;

	//Calculate the world position at this fragment the current frame.
	vec3 worldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * hitDistance;

	//Sample the active noise texture.
	vec4 activeNoiseTexture = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

	//Calculate the occlusion.
	float occlusion = 0.0f;

	for (int i = 0; i < 4; ++i)
	{
		//Calculate the sample position.
		vec3 randomDirection;
		float randomLength;

		switch(i)
		{
			case 0:
			{
				randomDirection = normalize(activeNoiseTexture.xyz * 2.0f - 1.0f);
				randomLength = activeNoiseTexture.w;

				break;
			}

			case 1:
			{
				randomDirection = normalize(activeNoiseTexture.xyw * 2.0f - 1.0f);
				randomLength = activeNoiseTexture.z;

				break;
			}

			case 2:
			{
				randomDirection = normalize(activeNoiseTexture.xzw * 2.0f - 1.0f);
				randomLength = activeNoiseTexture.y;

				break;
			}

			case 3:
			{
				randomDirection = normalize(activeNoiseTexture.yzw * 2.0f - 1.0f);
				randomLength = activeNoiseTexture.x;

				break;
			}
		}

		randomDirection = dot(randomDirection, geometryNormal) >= 0.0f ? randomDirection : randomDirection * -1.0f;
		vec3 samplePosition = worldPosition + randomDirection * randomLength;

		//Calculate the expected hit distance.
		float expectedHitDistance = length(samplePosition - perceiverWorldPosition);

		//Calculate the sample screen coordinate.
		vec4 sampleViewSpacePosition = viewMatrix * vec4(samplePosition, 1.0f);
		vec2 sampleScreenCoordinate = 0.5f * (sampleViewSpacePosition.xy / sampleViewSpacePosition.w) + 0.5f;

		//Sample the samplescene features.
		vec4 sampleSceneFeatures = texture(sceneFeatures2Texture, sampleScreenCoordinate);
		float sampleHitDistance = sampleSceneFeatures.w;

		//Calculate the distance falloff.
		float distanceFalloff = SmoothStep(1.0f - min(abs(hitDistance - sampleHitDistance), 1.0f));

		//If the expected hit distance is greater then the sample hit distance, there is occlusion.
		occlusion += float(expectedHitDistance > sampleHitDistance) * distanceFalloff;
	}

	//Normalize the ambient occlusion.
	occlusion = 1.0f - (occlusion * 0.25f);

    //Write the fragment
    fragment = vec4(vec3(occlusion), 1.0f);
}