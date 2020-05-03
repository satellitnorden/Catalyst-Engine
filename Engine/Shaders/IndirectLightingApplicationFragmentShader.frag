//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
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

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 scene_features_1 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);
	vec4 scene_features_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);
	vec4 scene_features_3 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);

	SceneFeatures features;

	features.albedo = scene_features_1.rgb;
	features.normal = scene_features_2.xyz;
	vec3 world_position = CalculateWorldPosition(coordinate, scene_features_2.w);
	features.view_direction = normalize(world_position - PERCEIVER_WORLD_POSITION);
	features.roughness = scene_features_3.x;
	features.metallic = scene_features_3.y;
	features.ambientOcclusion = scene_features_3.z;

	return features;
}

void CatalystShaderMain()
{
	//Sample the current features.
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample the indirect lighting.
	vec4 indirect_lighting_sample = INDIRECT_LIGHTING_ENABLED ? texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate) : vec4(0.0f, 0.0f, 0.0f, 0.0f);

	//Sample the sky.
	vec3 sky_diffuse_sample = SampleSkyDiffuse(current_features.normal);
	vec3 sky_specular_sample = SampleSkySpecular(current_features.view_direction, current_features.normal, current_features.roughness, current_features.metallic);

	//Calculate the blended specular irradiance
	vec3 blended_specular_irradiance = mix(sky_specular_sample, indirect_lighting_sample.rgb, indirect_lighting_sample.a);

	//Calculate the specular bias.
	vec2 specular_bias_texture_coordinate = vec2(max(dot(current_features.normal, -current_features.view_direction), 0.0f), current_features.roughness * (1.0f - current_features.metallic));
	vec2 specular_bias = texture(sampler2D(GLOBAL_TEXTURES[SPECULAR_BIAS_LOOKUP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), specular_bias_texture_coordinate).xy;	

	//Calculate the indirect lighting.
	vec3 indirect_lighting = CalculatePrecomputedLighting(	-current_features.view_direction,
															current_features.albedo,
															current_features.normal,
															current_features.roughness,
															current_features.metallic,
															current_features.ambientOcclusion,
															1.0f,
															sky_diffuse_sample,
															blended_specular_irradiance,
															specular_bias);

	//Write the fragment.
	scene = vec4(max(indirect_lighting, vec3(0.0f)), 1.0f);
}