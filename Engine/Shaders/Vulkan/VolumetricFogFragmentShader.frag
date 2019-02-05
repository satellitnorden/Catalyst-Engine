//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystFogUtilities.glsl"

//Preprocessor defines.
#define VOLUMETRIC_FOG_DISTANCE (10.0f)
#define VOLUMETRIC_FOG_RAY_STEPS (32)
#define VOLUMETRIC_FOG_DENSITY_PER_STEP (1.0f / VOLUMETRIC_FOG_RAY_STEPS)

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float density;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneNormalDepthTexture;
layout (set = 1, binding = 1) uniform sampler2D directionalShadowMap;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Calculate the scene world position.
    vec3 sceneWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w);

    //Calculate the ray direction.
    vec3 rayDirection = normalize(sceneWorldPosition - cameraWorldPosition);

    //Calculate the ray distance.
    float rayDistance = min(length(sceneWorldPosition - cameraWorldPosition), VOLUMETRIC_FOG_DISTANCE);

    //Calculate the ray step.
    vec3 rayStep = rayDirection * (rayDistance / VOLUMETRIC_FOG_RAY_STEPS);

    //Start off at the camera position.
    vec3 currentPosition = cameraWorldPosition;

    //Perform the ray.
    float accumulatedFog = 0.0f;

    for (int i = 0; i < VOLUMETRIC_FOG_RAY_STEPS; ++i)
    {
        //Calculate the directional shadow map coordinate.
    	vec4 directionalLightShadowMapCoordinate = directionalLightViewMatrix * vec4(currentPosition, 1.0f);
	    directionalLightShadowMapCoordinate.xy = directionalLightShadowMapCoordinate.xy * 0.5f + 0.5f;

        //Sample the depth.
	    float directionalDepth = texture(directionalShadowMap, directionalLightShadowMapCoordinate.xy).r;

        bool isInShadow = directionalLightShadowMapCoordinate.z < 1.0f && directionalLightShadowMapCoordinate.z > directionalDepth;

	    accumulatedFog += isInShadow ? 0.0f : VOLUMETRIC_FOG_DENSITY_PER_STEP;

	    currentPosition += rayStep;
    }

    //Write the fragment
    fragment = vec4(CalculateFogColor(rayDirection), accumulatedFog * density);
}