//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define INVERSE_RESOLUTION (1.0f / 1920.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   Calculates blur.
*/
vec4 CalculateBlur()
{
    vec4 color = vec4(0.0f);

    vec2 offset1 = vec2(1.411764705882353f, 0.0f) * INVERSE_RESOLUTION;
    vec2 offset2 = vec2(3.2941176470588234f, 0.0f) * INVERSE_RESOLUTION;
    vec2 offset3 = vec2(5.176470588235294f, 0.0f) * INVERSE_RESOLUTION;

    color += texture(sceneTexture, fragmentTextureCoordinate) * 0.1964825501511404f;
    color += texture(sceneTexture, fragmentTextureCoordinate + offset1) * 0.2969069646728344f;
    color += texture(sceneTexture, fragmentTextureCoordinate - offset1) * 0.2969069646728344f;
    color += texture(sceneTexture, fragmentTextureCoordinate + offset2) * 0.09447039785044732f;
    color += texture(sceneTexture, fragmentTextureCoordinate - offset2) * 0.09447039785044732f;
    color += texture(sceneTexture, fragmentTextureCoordinate + offset3) * 0.010381362401148057f;
    color += texture(sceneTexture, fragmentTextureCoordinate - offset3) * 0.010381362401148057f;

    return color;
}

void main()
{
    //Write the fragment.
    fragment = CalculateBlur();
}