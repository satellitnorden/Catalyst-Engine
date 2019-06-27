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
layout (set = 1, binding = 1) uniform sampler2D depthOfFieldTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the hit distance.
	float hitDistance = texture(sceneFeatures2Texture, fragmentTextureCoordinate).w;

	//Sample the depth of field texture.
	vec3 depthOfField = Upsample(depthOfFieldTexture, fragmentTextureCoordinate).rgb;

	//Determine the direction of the current fragment.
	vec3 fragmentDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Calculate the edge factor.
	float edgeFactor = max(dot(perceiverForwardVector, fragmentDirection), 0.0f);

	//Calculate the depth of field weight.
	float distanceWeight = pow(1.0f - min(hitDistance / CATALYST_RAY_TRACING_T_MAXIMUM, 1.0f), 2.0f);
	float depthOfFieldWeight = 1.0f - (distanceWeight * edgeFactor);

    //Write the fragment.
    fragment = vec4(depthOfField, depthOfFieldWeight);
}