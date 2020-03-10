//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "..\..\Include\Rendering\Native\Shader\CatalystLighting.h"

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
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_features_3_texture;
layout (set = 1, binding = 3) uniform sampler2D indirect_lighting_texture;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 scene_features_1 = texture(scene_features_1_texture, coordinate);
	vec4 scene_features_2 = texture(scene_features_2_texture, coordinate);
	vec4 scene_features_3 = texture(scene_features_3_texture, coordinate);

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

/*
*	Samples the sky diffuse.
*/
vec3 SampleSkyDiffuse(vec3 normal)
{
	return texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], normal).rgb;
}

/*
*	Samples the sky specular.
*/
vec3 SampleSkySpecular(vec3 view_direction, vec3 normal, float roughness)
{
	//Calculate the reflection vector.
	vec3 reflection_vector = reflect(view_direction, normal);

	//Calculate the indices for the sky textures.
	float float_index = roughness * float(NUMBER_OF_SKY_TEXTURES - 1);

	uint first_index = uint(float_index);
	uint second_index = first_index + 1;

	float alpha = fract(float_index);

	return mix(texture(SKY_TEXTURES[first_index], reflection_vector).rgb, texture(SKY_TEXTURES[second_index], reflection_vector).rgb, alpha);
}

void main()
{
	//Sample the current features.
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample the indirect lighting.
	vec4 indirect_lighting_sample = INDIRECT_LIGHTING_ENABLED ? texture(indirect_lighting_texture, fragment_texture_coordinate) : vec4(0.0f, 0.0f, 0.0f, 0.0f);

	//Sample the sky.
	vec3 sky_diffuse_sample = SampleSkyDiffuse(current_features.normal);
	vec3 sky_specular_sample = SampleSkySpecular(current_features.view_direction, current_features.normal, current_features.roughness);

	//Calculate the blended specular irradiance
	vec3 blended_specular_irradiance = mix(sky_specular_sample, indirect_lighting_sample.rgb, indirect_lighting_sample.a);

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
															vec2(1.0f, 0.0f));

	//Write the fragment.
	scene = vec4(max(indirect_lighting, vec3(0.0f)), 1.0f);
	//scene = vec4(vec3(current_features.ambientOcclusion), 1.0f);
	//scene = vec4(indirect_lighting_sample.rgb, 1.0f);
}