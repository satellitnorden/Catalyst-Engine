//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int maskIndex;
};

//In parameters.
layout (location = 0) in vec3 fragmentCurrentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;

void main()
{
    //Discard conditionally.
    if (texture(sampler2D(globalTextures[maskIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).r < 0.5f)
    {
        discard;
    }
}