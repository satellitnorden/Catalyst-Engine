#if !defined(CATALYST_TONE_MAPPING)
#define CATALYST_TONE_MAPPING

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystToneMapping)

	/*
	*	Applies tone mapping.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(vec3, ApplyToneMapping, vec3 color)
	{
		//Define constants.
		CATALYST_SHADER_CONSTANT(float, GAMMA, 1.0f / 2.2f);
		CATALYST_SHADER_CONSTANT(float, A, 2.51f);
		CATALYST_SHADER_CONSTANT(float, B, 0.03f);
		CATALYST_SHADER_CONSTANT(float, C, 2.43f);
		CATALYST_SHADER_CONSTANT(float, D, 0.59f);
		CATALYST_SHADER_CONSTANT(float, E, 0.14f);

		//Apply color mapping.
		color *= 0.6f;

		//Calculate the tone mapped color.
		vec3 tone_mapped = (color * (A * color + B)) / (color * (C * color + D) + E);

		tone_mapped[0] = pow(tone_mapped[0], GAMMA);
		tone_mapped[1] = pow(tone_mapped[1], GAMMA);
		tone_mapped[2] = pow(tone_mapped[2], GAMMA);

		return tone_mapped;
	}

CATALYST_SHADER_NAMESPACE_END()

#endif