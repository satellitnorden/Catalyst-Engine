//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 12) int material_index;
    layout (offset = 16) float cutoffDistanceSquared;
};

//In parameters.
layout (location = 0) in vec3 fragmentCurrentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;
layout (location = 2) in float fragmentDistanceSquared;

void main()
{
    //Retrieve the material.
    Material material = GLOBAL_MATERIALS[material_index];

	//Retrieve the random length.
	float randomLength = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f).x * cutoffDistanceSquared;

    //Discard conditionally.
    if (texture(sampler2D(GLOBAL_TEXTURES[material.optional_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).r < 0.5f
    	|| randomLength < fragmentDistanceSquared)
    {
        discard;
    }
}