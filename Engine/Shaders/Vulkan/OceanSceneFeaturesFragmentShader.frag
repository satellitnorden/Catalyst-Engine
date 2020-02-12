//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystOceanCore.glsl"

//Constants.
#define FOAM_POSITION_SCALE (0.25f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 world_position;
    layout (offset = 8) int borders;
    layout (offset = 12) float patch_size;
    layout (offset = 16) int ocean_texture_index;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_features_3_texture;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;

/*
*   Calculates the normal.
*/
vec3 CalculateNormal(vec3 point)
{
#define OFFSET (0.1f)

    float left = SampleOceanHeight(point + vec3(-OFFSET, 0.0f, 0.0f), ocean_texture_index);
    float right = SampleOceanHeight(point + vec3(OFFSET, 0.0f, 0.0f), ocean_texture_index);
    float down = SampleOceanHeight(point + vec3(0.0f, 0.0f, -OFFSET), ocean_texture_index);
    float up = SampleOceanHeight(point + vec3(0.0f, 0.0f, OFFSET), ocean_texture_index);

    return normalize(vec3(left - right, 2.0f, down - up));
}

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenViewMatrix, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenViewMatrix * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

/*
*	Calculates the foam.
*/
float CalculateFoam(vec3 point)
{
	//Sample all levels.
	vec3 sample_point;
    float height_sample;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float total = 0.0f;
    float height = 0.0f;

    sample_point = ((point * FOAM_POSITION_SCALE) * frequency);
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).x;
    height += (height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point * FOAM_POSITION_SCALE) * frequency );
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).y;
    height += (height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point * FOAM_POSITION_SCALE) * frequency);
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).z;
    height += (height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point * FOAM_POSITION_SCALE) * frequency);
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).w;
    height += (height_sample) * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    //Normalize the height.
    height /= total;

    //Return the foam.
    return height;
}

void main()
{
	//Calculate the normal.
	vec3 normal = CalculateNormal(fragment_world_position);

	//Calculate the screen coordinate.
	vec2 screen_coordinate = (gl_FragCoord.xy + 0.5f) * inverseScaledResolution;

	//Calculate the original world position.
	vec3 original_world_position = CalculateWorldPosition(screen_coordinate, texture(scene_features_2_texture, screen_coordinate).w);

	//Calculate the foam.
	float foam = CalculateFoam(fragment_world_position);

	//Calculate the distance to the bottom.
	float distance_to_bottom = length(original_world_position - fragment_world_position);

	//Calculate the foam weight.
	float foam_weight = max((1.0f - min(distance_to_bottom * 0.1f, 1.0f)) * foam, fragment_world_position.y / OCEAN_WAVE_HEIGHT);
	//float foam_weight = fragment_world_position.y / OCEAN_WAVE_HEIGHT;

	//Calculate the displacement weight.
	float displacement_weight = distance_to_bottom * 0.1f;

	//Calculate the albedo.
	vec3 original_albedo = texture(scene_features_1_texture, screen_coordinate + normal.xz * displacement_weight).rgb;
	float original_albedo_weight = 1.0f - min(distance_to_bottom * 0.01f, 1.0f);

	vec3 albedo = mix(mix(vec3(0.0f, 1.0f * 0.25f, 1.0f * 0.5f), original_albedo * 0.5f, original_albedo_weight), vec3(foam), foam_weight);

    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragment_world_position) - CalculateScreenCoordinate(viewMatrixMinusOne, fragment_world_position);

    //Write the fragments.
    scene_features_1 = vec4(albedo, 0.0f);
    scene_features_2 = vec4(PackNormal(normal), velocity, gl_FragCoord.z);
    scene_features_3 = mix(vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(1.0f, 0.0f, 1.0f, 0.0f), foam_weight);
}