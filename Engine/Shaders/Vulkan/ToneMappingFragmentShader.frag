//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define TONE_MAPPING_ALGORITHM_BURGESS (0)
#define TONE_MAPPING_ALGORITHM_REINHARDT (1)
#define TONE_MAPPING_ALGORITHM_UNCHARTED_2 (2)
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS (16.0f)
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS - 1.0f)
#define TONE_MAPPING_COLOR_GRADING_WIDTH (256.0f)
#define TONE_MAPPING_COLOR_GRADING_HEIGHT (16.0f)

#define TONE_MAPPING_ALGORITHM TONE_MAPPING_ALGORITHM_UNCHARTED_2

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;
layout (set = 1, binding = 1) uniform sampler2D colorGradingTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

#if TONE_MAPPING_ALGORITHM == TONE_MAPPING_ALGORITHM_BURGESS

/*
*   Applies tone mapping.
*/
vec3 ApplyToneMapping(vec3 fragment)
{
    vec3 x = max(vec3(0.0f), fragment - 0.004f);

    return (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
}

#elif TONE_MAPPING_ALGORITHM == TONE_MAPPING_ALGORITHM_REINHARDT

/*
*   Applies tone mapping.
*/
vec3 ApplyToneMapping(vec3 fragment)
{
    return pow(fragment / (1.0f + fragment), 1.0f / vec3(2.2f));
}

#elif TONE_MAPPING_ALGORITHM == TONE_MAPPING_ALGORITHM_UNCHARTED_2

#define EXPOSURE_BIAS (2.0f)
#define WHITE_SCALE (1.0f / Uncharted2ToneMap(vec3(W)))
#define A (0.15f)
#define B (0.50f)
#define C (0.10f)
#define D (0.20f)
#define E (0.02f)
#define F (0.30f)
#define W (11.2f)

/*
*   Applies the Uncharted 2 tone map.
*/
vec3 Uncharted2ToneMap(vec3 x)
{
   return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

/*
*   Applies tone mapping.
*/
vec3 ApplyToneMapping(vec3 fragment)
{
    return min(pow(Uncharted2ToneMap(max(fragment * EXPOSURE_BIAS, vec3(0.0000001f))) * WHITE_SCALE, 1.0f / vec3(2.2f)), vec3(1.0f));
}

#endif

/*
*   Applies color grading.
*/
vec3 ApplyColorGrading(vec3 fragment)
{
    //Calculate the cell that should be sampled based on the blue channel.
    float cell = fragment.b * TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE;

    //Calculate the current cell and the next cell.
    float currentCell = floor(cell);
    float nextCell = ceil(cell);

    //Calculate the red and green offsets.
    float redOffset = (0.5f / TONE_MAPPING_COLOR_GRADING_WIDTH) + fragment.r / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);
    float greenOffset = (0.5f / TONE_MAPPING_COLOR_GRADING_HEIGHT) + fragment.g * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);

    //Sample the current and next color graded values.
    vec3 currentColorGradedValue = texture(colorGradingTexture, vec2(currentCell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + redOffset, greenOffset)).rgb;
    vec3 nextColorGradedValue = texture(colorGradingTexture, vec2(nextCell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + redOffset, greenOffset)).rgb;

    //The color graded value is a linearly interpolated value of the current and the next cells!
    return mix(currentColorGradedValue, nextColorGradedValue, fract(cell));
}

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureColor = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply tone mapping.
    sceneTextureColor = ApplyToneMapping(sceneTextureColor);

    //Apply color grading.
    sceneTextureColor = ApplyColorGrading(sceneTextureColor);

    //Write the fragment
    fragment = vec4(sceneTextureColor, 1.0f);
}