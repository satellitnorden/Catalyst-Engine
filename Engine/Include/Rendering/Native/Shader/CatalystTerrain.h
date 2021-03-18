#if !defined(CATALYST_TERRAIN)
#define CATALYST_TERRAIN

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
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

	/*
	*	For use in terrain generation compute shaders. Calculate the current absolute world position from the global invocation ID.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_FOUR_ARGUMENTS(vec3, CalculateTerrainAbsoluteWorldPosition, uvec3 global_invocation_id, uvec3 number_of_work_groups, vec2 minimum_bounds, vec2 maximum_bounds)
	{
		float X = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(minimum_bounds.x, maximum_bounds.x, float(global_invocation_id.x) / float(number_of_work_groups.x - 1));
		float Z = CATALYST_SHADER_FUNCTION_LINEAR_INTERPOLATION(minimum_bounds.y, maximum_bounds.y, float(global_invocation_id.y) / float(number_of_work_groups.y - 1));


		return vec3(X, 0.0f, Z);
	}

CATALYST_SHADER_NAMESPACE_END()

#endif