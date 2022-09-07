//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Constants.
#define DEPTH_WEIGHT (4.0f)
#define DITHER_STRENGTH (0.1f)
#define INDIRECT_LIGHTING_QUALITY_LOW (0)
#define INDIRECT_LIGHTING_QUALITY_HIGH (1)
#define INDIRECT_LIGHTING_DIFFUSE_WEIGHT (0.750f) //0.025f step.

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint INDIRECT_LIGHTING_ENABLED;
	layout (offset = 4) uint SPECULAR_BIAS_LOOKUP_TEXTURE_INDEX;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_1_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SCENE_FEATURES_2_TEXTURE;
layout (set = 1, binding = 2) uniform sampler2D SCENE_FEATURES_3_TEXTURE;
layout (set = 1, binding = 3) uniform sampler2D DEPTH_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the indirect lighting at the given coordinate.
*/
vec4 SampleIndirectLighting(vec2 coordinate)
{
	if (INDIRECT_LIGHTING_ENABLED != 0)
	{
		return texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), coordinate);
	}
	
	else
	{
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

void CatalystShaderMain()
{
	//Sample the indirect lighting.
	vec4 indirect_lighting = SampleIndirectLighting(fragment_texture_coordinate);

	//No negative values. \o/
	indirect_lighting = vec4(max(indirect_lighting.rgb, vec3(0.0f, 0.0f, 0.0f)), clamp(indirect_lighting.a, 0.0f, 1.0f));

	//Blend in the sky a bit to account for misses.
	vec4 scene_features_1 = texture(SCENE_FEATURES_1_TEXTURE, fragment_texture_coordinate);
	vec4 scene_features_2 = texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate);
	vec4 scene_features_3 = texture(SCENE_FEATURES_3_TEXTURE, fragment_texture_coordinate);

	//Calculate the view direction.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w);
	
	//Calculate the view direction.
	vec3 view_direction = normalize(CAMERA_WORLD_POSITION - world_position);

	//Calculate the reflection direction.
	vec3 reflection_direction = reflect(view_direction, scene_features_2.xyz);

	//Retrieve the specular bias.
	vec2 specular_bias = texture(sampler2D(GLOBAL_TEXTURES[SPECULAR_BIAS_LOOKUP_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(max(dot(scene_features_2.xyz, view_direction), 0.0f), scene_features_3[0])).xy;

	//Calculate the lighting.
	vec3 calculated_lighting = CalculateIndirectLighting(	view_direction,
															scene_features_1.rgb,
															scene_features_2.xyz,
															scene_features_3[0],
															scene_features_3[1],
															scene_features_3[2],
															mix(SampleSky(scene_features_2.xyz, MAX_SKY_TEXTURE_MIPMAP_LEVEL), indirect_lighting.rgb, indirect_lighting.a),
															mix(SampleSky(reflection_direction, MAX_SKY_TEXTURE_MIPMAP_LEVEL * scene_features_3[0]), indirect_lighting.rgb, indirect_lighting.a),
															specular_bias);

	//Write the fragment.
	scene = vec4(calculated_lighting, 1.0f);
}