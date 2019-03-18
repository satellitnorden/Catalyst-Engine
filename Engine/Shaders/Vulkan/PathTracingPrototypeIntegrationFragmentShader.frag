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
    layout (offset = 0) int accumulations;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D currentFrameTexture;
layout (set = 1, binding = 1) uniform sampler2D previousFramesTexture;

//Out parameters.
layout (location = 0) out vec4 currentFrame;
layout (location = 1) out vec4 previousFrames;

void main()
{
    vec4 currentFrameTextureSampler = texture(currentFrameTexture, fragmentTextureCoordinate);
    vec4 previousFramesTextureSampler = texture(previousFramesTexture, fragmentTextureCoordinate);

    if (accumulations == 0)
    {
        currentFrame = previousFrames = currentFrameTextureSampler;
    }

    else
    {
        currentFrame = (previousFramesTextureSampler + currentFrame) / (accumulations + 1);
        previousFrames = previousFramesTextureSampler + currentFrameTextureSampler;
    }
}