#if !defined(CATALYST_TERRAIN)
#define CATALYST_TERRAIN

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystTerrain)

	/*
	*	Calculates the terrain tangent space matrix.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_THREE_ARGUMENTS(mat3, CalculateTerrainTangentSpaceMatrix, float center_height, float right_height, float up_height)
	{
		vec3 tangent = CATALYST_SHADER_FUNCTION_NORMALIZE(vec3(1.0f, right_height, 0.0f) - vec3(0.0f, center_height, 0.0f));
		vec3 bitangent = CATALYST_SHADER_FUNCTION_NORMALIZE(vec3(0.0f, up_height, 1.0f) - vec3(0.0f, center_height, 0.0f));
		vec3 normal = CATALYST_SHADER_FUNCTION_NORMALIZE(CATALYST_SHADER_FUNCTION_CROSS_PRODUCT(tangent, bitangent));

		normal = CATALYST_SHADER_FUNCTION_DOT_PRODUCT(normal, vec3(0.0f, 1.0f, 0.0f)) >= 0.0f ? normal : -normal;

		return mat3(tangent, bitangent, normal);
	}

CATALYST_SHADER_NAMESPACE_END()

#endif