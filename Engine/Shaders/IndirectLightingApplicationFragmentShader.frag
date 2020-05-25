//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
	vec3 world_position;
	vec3 view_direction;
	float roughness;
	float metallic;
	float ambientOcclusion;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) bool INDIRECT_LIGHTING_ENABLED;
	layout (offset = 4) uint SPECULAR_BIAS_LOOKUP_TEXTURE_INDEX;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene;

void CatalystShaderMain()
{
	//Sample the indirect lighting.
	vec3 indirect_lighting_sample = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).rgb;

	//Write the fragment.
	scene = vec4(indirect_lighting_sample, 1.0f);
}