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
    layout (offset = 8) float delta;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sourceTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Calculate the offset.
    vec4 offset = texelSize.xyxy * vec2(-delta, delta).xxyy;

    //Write the fragment.
    fragment =  (texture(sourceTexture, fragmentTextureCoordinate + offset.xy)
                + texture(sourceTexture, fragmentTextureCoordinate + offset.zy)
                + texture(sourceTexture, fragmentTextureCoordinate + offset.xw)
                + texture(sourceTexture, fragmentTextureCoordinate + offset.zw))
                * 0.25f;
}