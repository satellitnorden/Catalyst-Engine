#if !defined(CATALYST_VALUE_NOISE)
#define CATALYST_VALUE_NOISE

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include <Rendering/Native/Shader/CatalystShaderCore.h>
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

/*
*	Generates a value in the range [0.0f, 1.0f] at the given 2D coordinates.
*/
CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(float, ValueNoiseGenerate2D, vec2 coordinate)
{
    //Determine the cell indices.
    vec2 center = CATALYST_SHADER_FUNCTION_FLOOR(coordinate);
    vec2 right = center + vec2(1.0f, 0.0f);
    vec2 upper = center + vec2(0.0f, 1.0f);
    vec2 upper_right = center + vec2(1.0f, 1.0f);

    //Determine the random value in each cell.
    float center_value = CatalystShaderRandomFloat(center);
    float right_value = CatalystShaderRandomFloat(right);
    float upper_value = CatalystShaderRandomFloat(upper);
    float upper_right_value = CatalystShaderRandomFloat(upper_right);

    //Determine the horizontal and vertical alpha.
    float horizontal_alpha = CatalystShaderSmoothStep(CATALYST_SHADER_FUNCTION_FRACTIONAL(coordinate.x));
    float vertical_alpha = CatalystShaderSmoothStep(CATALYST_SHADER_FUNCTION_FRACTIONAL(coordinate.y));

    //Perform the blend.
    return CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(center_value, right_value, horizontal_alpha), CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(upper_value, upper_right_value, horizontal_alpha), vertical_alpha);
}

/*
*	Generates an octaved value in the range [0.0f, 1.0f] at the given 2D coordinates.
*/
CATALYST_SHADER_FUNCTION_RETURN_TWO_ARGUMENTS(float, ValueNoiseGenerateOctaved2D, vec2 coordinate, uint octaves)
{
    float final_noise = 0.0f;

    float total_noise = 0.0f;

    float current_amplitude = 1.0f;
    float current_frequency = 1.0f;

    for (uint i = 0; i < octaves; ++i)
    {
        final_noise += ValueNoiseGenerate2D(coordinate * current_frequency) * current_amplitude;

        total_noise += current_amplitude;

        current_amplitude *= 0.5f;
        current_frequency *= 2.0f;
    }

    final_noise /= total_noise;

    return final_noise;
}

#endif