//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 texelSize;
    layout (offset = 8) float threshold;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sourceTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   The sample function.
*/
vec4 Sample(vec2 coordinate)
{
    vec4 bloom = texture(sourceTexture, coordinate);

    return bloom * float(CalculateAverage(bloom.rgb) >= threshold);
}

void main()
{
    //Calculate the offset.
    vec4 offset = texelSize.xyxy * vec2(-1.0f, 1.0f).xxyy;

    //Write the fragment.
    fragment =  (Sample(fragmentTextureCoordinate + offset.xy)
                + Sample(fragmentTextureCoordinate + offset.zy)
                + Sample(fragmentTextureCoordinate + offset.xw)
                + Sample(fragmentTextureCoordinate + offset.zw))
                * 0.25f;
}