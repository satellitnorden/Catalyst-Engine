//Includes.
#include "CatalystTransparency.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int material_index;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;
layout (location = 1) in float fragment_opacity;

void CatalystShaderMain()
{
    //Retrieve the material.
    Material material = GLOBAL_MATERIALS[material_index];

    //Discard conditionally.
    float final_opacity = texture(sampler2D(GLOBAL_TEXTURES[material.optional_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).r * fragment_opacity;
    
    if (ShouldClip(uint(gl_FragCoord.x), uint(gl_FragCoord.y), final_opacity))
    {
        discard;
    }
}