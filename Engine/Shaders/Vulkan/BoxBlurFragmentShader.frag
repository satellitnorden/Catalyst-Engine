//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D inputTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Push constant
layout (push_constant) uniform BlurData
{
    float radius;
};

/*
*   Calculates the blur.
*/
vec4 CalculateBlur()
{
    vec4 blurredFragment = vec4(0.0f);

    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(-radius, -radius)) * 0.0625f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(-radius, 0.0f)) * 0.125f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(-radius, radius)) * 0.0625f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(0.0f, -radius)) * 0.125f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(0.0f, 0.0f)) * 0.25f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(0.0f, radius)) * 0.125f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(radius, -radius)) * 0.0625f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(radius, 0.0f)) * 0.125f;
    blurredFragment += texture(inputTexture, fragmentTextureCoordinate + vec2(radius, radius)) * 0.0625f;

    return blurredFragment;
}

void main()
{
    //Write the fragment.
    fragment = CalculateBlur();
}