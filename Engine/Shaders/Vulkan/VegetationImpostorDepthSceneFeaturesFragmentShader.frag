//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int mask_index;
    layout (offset = 12) float cutoff_distance_squared;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;
layout (location = 1) in float fragment_distance_squared;

void main()
{
    //Retrieve the random length.
    float random_length = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f).x * cutoff_distance_squared;

    //Discard conditionally.
    if (texture(sampler2D(globalTextures[mask_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).r < 0.5f
        || random_length < fragment_distance_squared)
    {
        discard;
    }
}