//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SHADOW_BIAS 0.0025f

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float fogLengthSquared;
    layout (offset = 4) float fogMinimumHeight;
    layout (offset = 8) float fogMaximumHeight;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneNormalDepthTexture;
layout (set = 1, binding = 1) uniform sampler2D directionalShadowMap;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   Calculates the world position given a texture coordinate and depth.
*/
vec3 CalculateWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec3 fragmentScreenSpacePosition = vec3(nearPlaneCoordinate, depth);
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(fragmentScreenSpacePosition, 1.0f);
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main()
{
	 //Sample the depth of the scene at this point.
    float sceneDepth = texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w;

    //Calculate the scene world position.
    vec3 sceneWorldPosition = CalculateWorldPosition(fragmentTextureCoordinate, sceneDepth);

    //Calculate the distance to the scene world position.
    float distanceToSceneWorldPosition = LengthSquared3(sceneWorldPosition - cameraWorldPosition);

    //Calculate the fog weight.
    float distanceWeight = min(distanceToSceneWorldPosition / fogLengthSquared, 1.0f);

    float heightWeight = 1.0f - clamp((sceneWorldPosition.y - fogMinimumHeight) / (fogMaximumHeight - fogMinimumHeight), 0.0f, 1.0f);

    float fogWeight = distanceWeight * heightWeight;

    //Calculate the fog color.
    vec3 fogColor = vec3(1.0f, 1.0f, 1.0f) * 0.01f + directionalLightColor * (directionalLightIntensity * 0.25f);

    //Write the fragment.
    fragment = vec4(fogColor, fogWeight);

    /*
    //Calculate the ray direction, distance and step.
    vec3 rayDirection = normalize(sceneWorldPosition - cameraWorldPosition);
    float distanceToScenePositionSquared = LengthSquared3(sceneWorldPosition - cameraWorldPosition);
    float rayDistance = min(rayDistanceSquared, distanceToScenePositionSquared);
    vec3 rayStep = rayDirection * (sqrt(rayDistance) / numberOfRaySteps);

    //Calculate the ray properties.
    float densityPerStep = density / numberOfRaySteps;
    vec3 currentPosition = cameraWorldPosition;

    //Perform the ray.
    vec3 accumulatedFog = vec3(0.0f);

    for (int i = 0; i < numberOfRaySteps; ++i)
    {
    	vec4 directionalLightShadowMapCoordinate = directionalLightViewMatrix * vec4(currentPosition, 1.0f);
	    directionalLightShadowMapCoordinate.xy = directionalLightShadowMapCoordinate.xy * 0.5f + 0.5f;

	    float directionalDepth = texture(directionalShadowMap, directionalLightShadowMapCoordinate.xy).r;
	    float compare = directionalLightShadowMapCoordinate.z - SHADOW_BIAS;

        bool isNotInShadow = compare >= 1.0f || compare < directionalDepth;

	    accumulatedFog += isNotInShadow ? directionalLightColor * directionalLightIntensity * densityPerStep : vec3(0.0f);

	    currentPosition += rayStep;
    }

    //Write the fragment
    fragment = vec4(accumulatedFog, 1.0f);
    */
}