//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	vec3 normal;
	vec3 view_direction;
	vec3 indirect_lighting;
	float roughness;
	float metallic;
	float ambientOcclusion;
};

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_features_3_texture;
layout (set = 1, binding = 3) uniform sampler2D ambient_occlusion_texture;
layout (set = 1, binding = 4) uniform sampler2D indirect_lighting_texture;

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
	vec4 ambient_occlusion = ambientOcclusionMode == AMBIENT_OCCLUSION_MODE_NONE ? vec4(1.0f) : Upsample(ambient_occlusion_texture, coordinate);
	vec4 indirect_lighting = texture(indirect_lighting_texture, coordinate);

	SceneFeatures features;

	features.albedo = scene_features_1.rgb;
	features.normal = UnpackNormal(scene_features_2.x);
	vec3 world_position = CalculateWorldPosition(coordinate, scene_features_2.w);
	features.view_direction = normalize(world_position - PERCEIVER_WORLD_POSITION);
	features.indirect_lighting = indirect_lighting.rgb;
	features.roughness = scene_features_3.x;
	features.metallic = scene_features_3.y;
	features.ambientOcclusion = pow(scene_features_3.z * pow(ambient_occlusion.x, AMBIENT_OCCLUSION_POWER), AMBIENT_OCCLUSION_POWER);

	return features;
}

void main()
{
	//Sample the current features.
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Calculate the indirect lighting.
	vec3 indirect_lighting = CalculateIndirectLighting(	current_features.view_direction,
														current_features.albedo,
														current_features.normal,
														current_features.roughness,
														current_features.metallic,
														current_features.ambientOcclusion,
														current_features.indirect_lighting);

	//Write the fragment.
	scene = vec4(indirect_lighting, 1.0f);
}