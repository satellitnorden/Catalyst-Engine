#if !defined(CATALYST_VOLUMETRIC_LIGHTING)
#define CATALYST_VOLUMETRIC_LIGHTING

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystVolumetricLighting)

	/*
	*	Calculates the volumetric lighting opacity given the arguments.
	*/
	CATALYST_SHADER_FUNCTION_RETURN_SIX_ARGUMENTS(float, CalculateVolumetricLightingOpacity, float hit_distance, float volumetric_lighting_distance, float hit_height, float volumetric_lighting_height, float volumetric_lighting_thickness, float perceiving_height)
	{
		//Calculate the distance weight.
		float volumetric_lighting_distance_weight = CATALYST_SHADER_FUNCTION_MINIMUM(hit_distance / volumetric_lighting_distance, 1.0f);

		//Calculate the height weight.
		float volumetric_lighting_height_weight = CATALYST_SHADER_FUNCTION_SQUARE(CATALYST_SHADER_FUNCTION_SQUARE(1.0f - CATALYST_SHADER_FUNCTION_CLAMP(hit_height / volumetric_lighting_height, 0.0f, 1.0f)));

		//Calculate the perceiving height weight.
		float perceiving_height_weight = CATALYST_SHADER_FUNCTION_SQUARE(CATALYST_SHADER_FUNCTION_SQUARE(CATALYST_SHADER_FUNCTION_SQUARE(1.0f - CATALYST_SHADER_FUNCTION_CLAMP(perceiving_height / volumetric_lighting_height, 0.0f, 1.0f))));

		//Calculate the volumetric lighting opacity.
		float volumetric_lighting_opacity = volumetric_lighting_distance_weight * CATALYST_SHADER_FUNCTION_MAXIMUM(volumetric_lighting_height_weight, perceiving_height_weight);

		//Apply the thickness.
		volumetric_lighting_opacity = 1.0f - pow(1.0f - volumetric_lighting_opacity, volumetric_lighting_thickness);

		//Return the volumetric lighting opacity.
		return volumetric_lighting_opacity;
	}

CATALYST_SHADER_NAMESPACE_END()

#endif