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

    vec2 offset1 = vec2(1.411764705882353f) * blurDirection;
    vec2 offset2 = vec2(3.2941176470588234f) * blurDirection;
    vec2 offset3 = vec2(5.176470588235294f) * blurDirection;

    color += texture(blurTexture, fragmentTextureCoordinate) * 0.1964825501511404f;
    color += texture(blurTexture, fragmentTextureCoordinate + (offset1 * inverseResolution)) * 0.2969069646728344f;
    color += texture(blurTexture, fragmentTextureCoordinate - (offset1 * inverseResolution)) * 0.2969069646728344f;
    color += texture(blurTexture, fragmentTextureCoordinate + (offset2 * inverseResolution)) * 0.09447039785044732f;
    color += texture(blurTexture, fragmentTextureCoordinate - (offset2 * inverseResolution)) * 0.09447039785044732f;
    color += texture(blurTexture, fragmentTextureCoordinate + (offset3 * inverseResolution)) * 0.010381362401148057f;
    color += texture(blurTexture, fragmentTextureCoordinate - (offset3 * inverseResolution)) * 0.010381362401148057f;

    return color;
}

void main()
{
    //Set the fragment color.
    fragment = CalculateBlur();
}