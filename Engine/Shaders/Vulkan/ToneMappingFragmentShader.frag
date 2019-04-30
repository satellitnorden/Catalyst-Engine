//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   Applies HDR .
*/
vec3 ApplyHDR(vec3 fragment)
{
    return fragment / (fragment + vec3(1.0f));
}

/*
*   Applies gamma correction.
*/
vec3 ApplyGammaCorrection(vec3 fragment)
{
    return pow(fragment, vec3(1.0f / 2.2f));
}

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureColor = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply HDR.
    //sceneTextureColor = ApplyHDR(sceneTextureColor);

    //Apply gamma correction.
    sceneTextureColor = ApplyGammaCorrection(sceneTextureColor);

    //Write the fragment
    fragment = vec4(sceneTextureColor, 1.0f);
}