//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D blurTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Push constant
layout (push_constant) uniform ModelData
{
    vec2 blurDirection;
    vec2 inverseResolution;
};

/*
*   Calculates blur.
*/
vec4 CalculateBlur()
{
    vec4 color = vec4(0.0f);

    vec2 offset1 = vec2(1.3333333333333333f) * blurDirection;

    color += texture(blurTexture, fragmentTextureCoordinate) * 0.29411764705882354f;
    color += texture(blurTexture, fragmentTextureCoordinate + (offset1 * inverseResolution)) * 0.35294117647058826f;
    color += texture(blurTexture, fragmentTextureCoordinate - (offset1 * inverseResolution)) * 0.35294117647058826f;

    return color;
}

void main()
{
    //Set the fragment color.
    fragment = CalculateBlur();
}