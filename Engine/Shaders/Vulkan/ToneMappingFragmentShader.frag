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
*   Applies tone mapping.
*/
vec3 ApplyToneMapping(vec3 fragment)
{
    vec3 x = max(vec3(0.0f), fragment - 0.004f);

    return (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
}

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureColor = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply tone mapping.
    sceneTextureColor = ApplyToneMapping(sceneTextureColor);

    //Write the fragment
    fragment = vec4(sceneTextureColor, 1.0f);
}