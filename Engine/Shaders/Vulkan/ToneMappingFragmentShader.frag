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

#define TONE_MAPPING_ALGORITHM TONE_MAPPING_ALGORITHM_UNCHARTED_2

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

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
    return pow(Uncharted2ToneMap(max(fragment * EXPOSURE_BIAS, vec3(0.0000001f))) * WHITE_SCALE, 1.0f / vec3(2.2f));
}

#endif

void main()
{
    //Sample the scene texture.
    vec3 sceneTextureColor = texture(sceneTexture, fragmentTextureCoordinate).rgb;

    //Apply tone mapping.
    sceneTextureColor = ApplyToneMapping(sceneTextureColor);

    //Write the fragment
    fragment = vec4(sceneTextureColor, 1.0f);
}