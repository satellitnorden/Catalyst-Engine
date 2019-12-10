//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int material_index;
    layout (offset = 12) float cutoff_distance_squared;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;
layout (location = 1) in float fragment_distance_squared;

void main()
{
    //Retrieve the material.
    Material material = GLOBAL_MATERIALS[material_index];

    //Discard conditionally based on the mask texture.
    if (texture(sampler2D(GLOBAL_TEXTURES[material.optional_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).r < 0.5f)
    {
        discard;
    }

    //Calculate the opacity.
    float opacity = CalculateDistanceBasedOpacity(fragment_distance_squared, cutoff_distance_squared);

    //Retrieve the random opacity.
    if (opacity < 1.0f)
    {
        //Calculate the noise texture coordinate.
        vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);

        //Retrieve the random opacity.
        float random_opacity = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate).x;

        //Discard conditionally based on the opacity.
        if (opacity < random_opacity)
        {
            discard;
        }
    }
}