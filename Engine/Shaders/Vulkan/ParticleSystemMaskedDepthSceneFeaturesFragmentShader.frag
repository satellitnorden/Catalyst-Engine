//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int material_index;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

void main()
{
    //Retrieve the material.
    Material material = GLOBAL_MATERIALS[material_index];

    //Discard conditionally.
    if (texture(sampler2D(globalTextures[material.optional_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).r < 0.5f)
    {
        discard;
    }
}