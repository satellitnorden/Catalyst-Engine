//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystFogUtilities.glsl"

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

    //Write the fragment
    fragment = vec4(CalculateFogColor(vec3(0.0f, 1.0f, 0.0f)), 1.0f);
}