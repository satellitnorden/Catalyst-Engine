//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define INVERSE_RESOLUTION (1.0f / 1080.0f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) float inverseDepthOfFieldDistanceSquared;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D depthOfFieldTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   Calculates blur.
*/
vec4 CalculateBlur()
{
    vec4 color = vec4(0.0f);

    vec2 offset1 = vec2(0.0f, 1.411764705882353f) * INVERSE_RESOLUTION;
    vec2 offset2 = vec2(0.0f, 3.2941176470588234f) * INVERSE_RESOLUTION;
    vec2 offset3 = vec2(0.0f, 5.176470588235294f) * INVERSE_RESOLUTION;

    color += texture(depthOfFieldTexture, fragmentTextureCoordinate) * 0.1964825501511404f;
    color += texture(depthOfFieldTexture, fragmentTextureCoordinate + offset1) * 0.2969069646728344f;
    color += texture(depthOfFieldTexture, fragmentTextureCoordinate - offset1) * 0.2969069646728344f;
    color += texture(depthOfFieldTexture, fragmentTextureCoordinate + offset2) * 0.09447039785044732f;
    color += texture(depthOfFieldTexture, fragmentTextureCoordinate - offset2) * 0.09447039785044732f;
    color += texture(depthOfFieldTexture, fragmentTextureCoordinate + offset3) * 0.010381362401148057f;
    color += texture(depthOfFieldTexture, fragmentTextureCoordinate - offset3) * 0.010381362401148057f;

    return color;
}

void main()
{
    //Calculate the blur.
    vec4 blur = CalculateBlur();

    //Sample the depth.
    float depth = texture(normalDepthTexture, fragmentTextureCoordinate).w;

    //Calculate the fragment world position.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, depth == 0.0f ? 1.0f : depth);

    //Calculate the distance to the fragment world position squared.
    float distanceSquared = LengthSquared3(cameraWorldPosition - fragmentWorldPosition);

    //Calculate the depth of field weight.
    float depthOfFieldWeight = min(distanceSquared * inverseDepthOfFieldDistanceSquared, 1.0f);

    //Write the fragment.
    fragment = vec4(blur.rgb, depthOfFieldWeight);
}