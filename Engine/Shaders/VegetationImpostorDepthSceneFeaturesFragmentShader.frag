//Includes.
#include "CatalystTransparency.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int material_index;
    layout (offset = 12) float cutoff_distance_squared;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;
layout (location = 1) in float fragment_distance_squared;

void CatalystShaderMain()
{
    //Retrieve the material.
    Material material = GLOBAL_MATERIALS[material_index];

    //Discard conditionally based on the mask texture.
    if (texture(sampler2D(GLOBAL_TEXTURES[material._Opacity], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).r < 0.5f)
    {
        discard;
    }

    //Calculate the opacity.
    float opacity = CalculateDistanceBasedOpacity(fragment_distance_squared, cutoff_distance_squared);

    //Retrieve the random opacity.
    if (opacity < 1.0f)
    {
         //Discard conditionally based on the opacity.
        if (ShouldClip(uint(gl_FragCoord.x), uint(gl_FragCoord.y), opacity))
        {
            discard;
        }
    }
}