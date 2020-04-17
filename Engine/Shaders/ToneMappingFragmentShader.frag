//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "..\..\Include\Rendering\Native\Shader\CatalystToneMapping.h"

//Constants.
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS (16.0f)
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS - 1.0f)
#define TONE_MAPPING_COLOR_GRADING_WIDTH (256.0f)
#define TONE_MAPPING_COLOR_GRADING_HEIGHT (16.0f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint color_grading_texture_index;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*   Applies color grading.
*/
vec3 ApplyColorGrading(vec3 fragment)
{
    //Calculate the cell that should be sampled based on the blue channel.
    float cell = fragment.b * TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE;

    //Calculate the current cell and the next cell.
    float current_cell = floor(cell);
    float next_cell = ceil(cell);

    //Calculate the red and green offsets.
    float red_offset = (0.5f / TONE_MAPPING_COLOR_GRADING_WIDTH) + fragment.r / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);
    float green_offset = (0.5f / TONE_MAPPING_COLOR_GRADING_HEIGHT) + fragment.g * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);

    //Sample the current and next color graded values.
    vec3 current_color_graded_value = texture(sampler2D(GLOBAL_TEXTURES[color_grading_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(current_cell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + red_offset, green_offset)).rgb;
    vec3 next_colorgraded_value = texture(sampler2D(GLOBAL_TEXTURES[color_grading_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(next_cell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + red_offset, green_offset)).rgb;

    //The color graded value is a linearly interpolated value of the current and the next cells!
    return mix(current_color_graded_value, next_colorgraded_value, fract(cell));
}

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureColor = texture(scene_texture, fragment_texture_coordinate).rgb;

    //Apply tone mapping.
    sceneTextureColor = ApplyToneMapping(sceneTextureColor);

    //Apply color grading.
    //sceneTextureColor = ApplyColorGrading(sceneTextureColor);

    //Write the fragment
    if (isnan(sceneTextureColor.r) || isnan(sceneTextureColor.g) || isnan(sceneTextureColor.b)) fragment = vec4(vec3(1.0f, 0.0f, 0.0f), 1.0f);
    else fragment = vec4(sceneTextureColor, 1.0f);
}