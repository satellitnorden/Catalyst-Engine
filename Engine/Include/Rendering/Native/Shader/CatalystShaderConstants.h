#if !defined(CATALYST_SHADER_CONSTANTS)
#define CATALYST_SHADER_CONSTANTS

#if defined(CATALYST_SHADER_LANGUAGE_CXX)
	#include "CatalystShaderCore.h"
#endif

#if defined(CATALYST_SHADER_LANGUAGE_GLSL)
	#include "..\Include\Rendering\Native\Shader\CatalystShaderCore.h"
#endif

CATALYST_SHADER_NAMESPACE_BEGIN(CatalystShaderConstants)
	//Core.
	CATALYST_SHADER_CONSTANT(uint, MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, 4096)
	CATALYST_SHADER_CONSTANT(uint, MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS, 512)
	CATALYST_SHADER_CONSTANT(uint, NUMBER_OF_BLUE_NOISE_TEXTURES, 32)
	CATALYST_SHADER_CONSTANT(uint, BLUE_NOISE_TEXTURE_RESOLUTION, 32)

	//Material.
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_TYPE_MASKED, 1 << 0)
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_TYPE_OPAQUE, 1 << 1)
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_TYPE_TRANSLUCENT, 1 << 2)
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE, 1 << 3)
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE, 1 << 4)
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE, 1 << 5)
	CATALYST_SHADER_CONSTANT(uint, MATERIAL_PROPERTY_OPACITY_TEXTURE, 1 << 6)

	//Ambient occlusion.
	CATALYST_SHADER_CONSTANT(float, AMBIENT_OCCLUSION_RADIUS, 2.0f)

	//Lighting.
	CATALYST_SHADER_CONSTANT(uint, LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT, 1)
	CATALYST_SHADER_CONSTANT(uint, LIGHT_PROPERTY_VOLUMETRIC_BIT, 2)
	CATALYST_SHADER_CONSTANT(uint, LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT, 4)
	CATALYST_SHADER_CONSTANT(vec3, VOLUMETRIC_LIGHTING_BASE_COLOR, vec3(0.8f, 0.9f, 1.0f))
	CATALYST_SHADER_CONSTANT(float, VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER, 0.125f)

CATALYST_SHADER_NAMESPACE_END()

#endif