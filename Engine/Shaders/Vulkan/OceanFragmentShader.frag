//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define OCEAN_BASE_COLOR (vec3(0.0f, 0.125f * 0.125f * 0.5f, 0.125f * 0.5f))
#define OCEAN_WAVE_HEIGHT (1.0f)
#define OCEAN_POSITION_SCALE (0.0025f)
#define OCEAN_PERSISTENCE (0.5f)
#define OCEAN_LACUNARITY (2.0f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int ocean_texture_index;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_1_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_features_3_texture;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;

/*
*   Samples the height at the given point.
*/
float SampleHeight(vec3 point)
{
    //Set up all variables.
    vec3 offset = -vec3(totalTime, 0.0f, totalTime) * 0.1f;

    vec3 sample_point;
    float height_sample;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float total = 0.0f;
    float height = 0.0f;

    //Sample all levels.
    sample_point = ((point + (offset * SQUARE_ROOT_OF_TWO)) * frequency * SQUARE_ROOT_OF_TWO) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).x;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + (offset * HALF_PI)) * frequency * HALF_PI) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).y;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + (offset * PHI)) * frequency * PHI) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).z;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + (offset * EULERS_NUMBER)) * frequency * EULERS_NUMBER) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(GLOBAL_TEXTURES[ocean_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).w;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    //Normalize the height.
    height /= total;

    return height * OCEAN_WAVE_HEIGHT;
}

/*
*   Calculates the normal.
*/
vec3 CalculateNormal(vec3 point)
{
#define OFFSET (1.0f)

    float left = SampleHeight(point + vec3(-OFFSET, 0.0f, 0.0f));
    float right = SampleHeight(point + vec3(OFFSET, 0.0f, 0.0f));
    float down = SampleHeight(point + vec3(0.0f, 0.0f, -OFFSET));
    float up = SampleHeight(point + vec3(0.0f, 0.0f, OFFSET));

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

void main()
{
	//Sample the scene features textures.
    vec4 scene_features_1_sampler = texture(scene_features_1_texture, fragment_texture_coordinate);
    vec4 scene_features_2_sampler = texture(scene_features_2_texture, fragment_texture_coordinate);
    vec4 scene_features_3_sampler = texture(scene_features_3_texture, fragment_texture_coordinate);

	//Calculate the world position.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2_sampler.w);
    float distance_to_world_position = length(world_position - PERCEIVER_WORLD_POSITION);
    vec3 view_direction = (world_position - PERCEIVER_WORLD_POSITION) / distance_to_world_position;

    //Is the world position (possibly) under water?
    if (world_position.y <= OCEAN_WAVE_HEIGHT)
    {
        //Do a line-plane intersection againt the ocean plane to determine the hit position.
        float start_intersection_distance;
        LinePlaneIntersection(PERCEIVER_WORLD_POSITION, view_direction, vec3(0.0f, OCEAN_WAVE_HEIGHT, 0.0f), vec3(0.0f, 1.0f, 0.0f), start_intersection_distance);
        vec3 start = PERCEIVER_WORLD_POSITION + view_direction * start_intersection_distance;

        float end_intersection_distance;
        LinePlaneIntersection(PERCEIVER_WORLD_POSITION, view_direction, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), end_intersection_distance);
        end_intersection_distance = min(end_intersection_distance, distance_to_world_position);
        vec3 end = PERCEIVER_WORLD_POSITION + view_direction * end_intersection_distance;

        //Calculate the noise texture coordinate.
        vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);

        //Retrieve the random sample.
        vec4 random_sample = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate);

        //Determine the hit position and density.
        vec3 hit_position = end;
        float density = float(world_position.y <= 0.0f);
        float step = (end_intersection_distance - start_intersection_distance) * 0.25f;

        for (int i = 0; i < 4; ++i)
        {
            float sample_step = step * float(i + 1) + step * random_sample[i];
            vec3 sample_point = start + view_direction * sample_step;
            float sample_height = SampleHeight(sample_point);

            if (sample_point.y <= sample_height)
            {
                hit_position = sample_point;
                density = 1.0f;

                break;
            }
        }

        //Calculate the albedo.
        float distance_travelled_underwater = length(world_position - hit_position);
        float distance_travelled_underwater_weight = min(distance_travelled_underwater / 10.0f, 1.0f);
        vec3 underwater_color = mix(OCEAN_BASE_COLOR, OCEAN_BASE_COLOR * 0.5f, distance_travelled_underwater_weight);
        vec3 albedo = mix(scene_features_1_sampler.rgb * 0.5f, underwater_color, distance_travelled_underwater_weight);

        //Calculate the velocity.
        vec2 velocity = CalculateScreenCoordinate(viewMatrix, hit_position) - CalculateScreenCoordinate(viewMatrixMinusOne, hit_position);

        //Calculate the depth.
        vec4 view_space_position = viewMatrix * vec4(hit_position, 1.0f);
        float depth = view_space_position.z / view_space_position.w;

        //Write the fragments.
        scene_features_1 = mix(scene_features_1_sampler, vec4(albedo, 0.0f), density);
        scene_features_2 = mix(scene_features_2_sampler, vec4(PackNormal(CalculateNormal(hit_position)), velocity, depth), density);
        scene_features_3 = mix(scene_features_3_sampler, vec4(0.0f, 0.0f, 1.0f, 0.0f), density);
    }

    //Or is it over water?
    else
    {
        //Write the fragments.
        scene_features_1 = scene_features_1_sampler;
        scene_features_2 = scene_features_2_sampler;
        scene_features_3 = scene_features_3_sampler;
    }
}