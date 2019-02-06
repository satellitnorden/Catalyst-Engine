//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystFogUtilities.glsl"

//Preprocessor defines.
#define EXPONENTIAL_HEIGHT_FOG_DISTANCE (65536.0f)
#define EXPONENTIAL_HEIGHT_FOG_DISTANCE_SQUARED (EXPONENTIAL_HEIGHT_FOG_DISTANCE * EXPONENTIAL_HEIGHT_FOG_DISTANCE)
#define EXPONENTIAL_HEIGHT_FOG_INVERSE_DISTANCE_SQUARED (1.0f / EXPONENTIAL_HEIGHT_FOG_DISTANCE_SQUARED)
#define EXPONENTIAL_HEIGHT_FOG_HEIGHT (25000.0f)
#define EXPONENTIAL_HEIGHT_FOG_INVERSE_HEIGHT (1.0f / EXPONENTIAL_HEIGHT_FOG_HEIGHT)

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneNormalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Calculate the world position.
    vec3 worldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w);

    //Calculate the direction.
    vec3 direction = normalize(worldPosition - cameraWorldPosition);

    //Calculate the distance.
    float distance = LengthSquared3(worldPosition - cameraWorldPosition);

    //Calculate the distance weight.
    float distanceWeight = min(distance * EXPONENTIAL_HEIGHT_FOG_INVERSE_DISTANCE_SQUARED, 1.0f);
    distanceWeight *= distanceWeight;

    //Calculate the height weight.
    float heightWeight = 1.0f - min(worldPosition.y * EXPONENTIAL_HEIGHT_FOG_INVERSE_HEIGHT, 1.0f);
    heightWeight *= heightWeight;

    //Write the fragment
    fragment = vec4(CalculateFogColor(direction), distanceWeight * heightWeight);
}