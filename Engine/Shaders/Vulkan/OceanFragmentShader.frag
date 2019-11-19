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
#define OCEAN_BASE_COLOR (vec3(0.0f, 0.125f * 0.125f, 0.125f))
#define OCEAN_WAVE_HEIGHT (1.25f)
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
    height_sample = texture(sampler2D(globalTextures[ocean_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).x;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + (offset * HALF_PI)) * frequency * HALF_PI) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(globalTextures[ocean_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).y;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + (offset * PHI)) * frequency * PHI) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(globalTextures[ocean_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).z;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    sample_point = ((point + (offset * EULERS_NUMBER)) * frequency * EULERS_NUMBER) * OCEAN_POSITION_SCALE;
    height_sample = texture(sampler2D(globalTextures[ocean_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), sample_point.xz).w;
    height += height_sample * amplitude;
    total += amplitude;
    amplitude *= OCEAN_PERSISTENCE;
    frequency *= OCEAN_LACUNARITY;

    //Normalize the height.
    height /= total;

    return height * OCEAN_WAVE_HEIGHT;
}

/*
*   Samples the density at the given point.
*/
float SampleDensity(vec3 point)
{
    return 1.0f - (clamp(point.y + SampleHeight(point), 0.0f, OCEAN_WAVE_HEIGHT) / OCEAN_WAVE_HEIGHT);
}

/*
*   Calculates the normal at the given point.
*/
vec3 CalculateNormal(vec3 point)
{
    float center = SampleHeight(point + vec3(0.0f, 0.0f, 0.0f));
    float up = SampleHeight(point + vec3(0.0f, 0.0f, 1.0f));
    float right = SampleHeight(point + vec3(1.0f, 0.0f, 0.0f));

    return normalize(vec3(center - right, 1.0f, center - up));
}

void main()
{
	//Sample the scene features textures.
    vec4 scene_features_1_sampler = texture(scene_features_1_texture, fragment_texture_coordinate);
    vec4 scene_features_2_sampler = texture(scene_features_2_texture, fragment_texture_coordinate);
    vec4 scene_features_3_sampler = texture(scene_features_3_texture, fragment_texture_coordinate);

	//Calculate the world position.
    vec3 ray_direction = CalculateRayDirection(fragment_texture_coordinate);
	vec3 world_position = perceiverWorldPosition + ray_direction * scene_features_2_sampler.w;

    //Is the world position under water?
    if (world_position.y <= OCEAN_WAVE_HEIGHT)
    {
        //Do a line-plane intersection againt the ocean plane to determine the start position.
        vec3 start;

        float intersection_distance;

        LinePlaneIntersection(perceiverWorldPosition, ray_direction, vec3(0.0f, OCEAN_WAVE_HEIGHT, 0.0f), vec3(0.0f, 1.0f, 0.0f), intersection_distance);
        start = perceiverWorldPosition + ray_direction * intersection_distance;

        //Remember the hit distance.
        float hit_distance = intersection_distance;

        //The end is just the actual world_position. (:
        vec3 end = world_position;

        //Sample the noise texture.
        vec4 noise_texture = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

        //Sample the density.
        float density = 0.0f;

        for (int i = 0; i < 4; ++i)
        {
            density = min(density + SampleDensity(mix(start, end, noise_texture[i])), 1.0f);

            if (density == 1.0f)
            {
                break;
            }
        }

        //Sample the ocean texture.
        vec4 ocean_texture_sampler = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), start.xz);

        //Write the fragments.
        scene_features_1 = mix(scene_features_1_sampler, vec4(OCEAN_BASE_COLOR, 0.0f), density);
        scene_features_2 = mix(scene_features_2_sampler, vec4(PackNormal(CalculateNormal(start)), 0.0f, 0.0f, hit_distance), density);
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