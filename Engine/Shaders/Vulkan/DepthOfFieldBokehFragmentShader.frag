//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DEPTH_OF_FIELD_BOKEH_MAXIMUM_OFFSET (0.0025f)
#define DEPTH_OF_FIELD_BOKEH_SAMPLES (16)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 1) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the hit distance.
	float hitDistance = texture(sceneFeatures2Texture, fragmentTextureCoordinate).w;

	//Determine the direction of the current fragment.
	vec3 fragmentDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Calculate the edge factor.
	float edgeFactor = max(dot(perceiverForwardVector, fragmentDirection), 0.0f);

	//Calculate the depth of field weight.
	float distanceWeight = 1.0f - min(abs(hitDistance - depthOfFieldFocusDistance) / viewDistance, 1.0f);
	float depthOfFieldWeight = 1.0f - (distanceWeight * edgeFactor);

	//Calculate the aspect ratio.
	float aspectRatio = scaledResolution.x / scaledResolution.y;

	//Calculate the offset weight.
	float offsetWeightScalar = DEPTH_OF_FIELD_BOKEH_MAXIMUM_OFFSET * depthOfFieldWeight;
	vec2 offsetWeight = vec2(offsetWeightScalar, offsetWeightScalar * aspectRatio);

	//Calculate the depth of field.
	vec3 depthOfField = vec3(0.0f);

	for (int i = 0; i < DEPTH_OF_FIELD_BOKEH_SAMPLES; ++i)
	{
		//Calculate the offset.
		vec4 randomVector = texture(sampler2D(globalTextures[i], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f);
		vec2 randomOffset = normalize(randomVector.xy * 2.0f - 1.0f) * randomVector.z * offsetWeight;

		//Add to the depth of field.
		depthOfField += texture(sceneTexture, fragmentTextureCoordinate + randomOffset).rgb;
	}

	//Normalize the depth of field.
	depthOfField /= DEPTH_OF_FIELD_BOKEH_SAMPLES;

    //Write the fragment.
    fragment = vec4(depthOfField, depthOfFieldWeight);
}