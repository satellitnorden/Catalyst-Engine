#if !defined(CATALYST_SIMPLEX_NOISE)
#define CATALYST_SIMPLEX_NOISE

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include <Rendering/Native/Shader/CatalystShaderCore.h>
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystSimplexNoise)

	/*
	*	Generates a value in the range [-1.0f, 1.0f] at the given 2D coordinates.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_ONE_ARGUMENT(float, Generate2D, vec2 coordinate)
	{
		return 4.0f;
	}

CATALYST_SHADER_NAMESPACE_END()

#endif