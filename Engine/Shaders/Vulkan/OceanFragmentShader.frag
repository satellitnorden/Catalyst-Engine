//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D ocean_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 2) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 scene_features_2;
layout (location = 1) out vec4 scene;

void main()
{
	//Sample the textures.
	vec4 scene_features_2_sampler = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec4 scene_sampler = texture(scene_texture, fragment_texture_coordinate);

	//Calculate the world position.
    vec3 ray_direction = CalculateRayDirection(fragment_texture_coordinate);
	vec3 world_position = perceiverWorldPosition + ray_direction * scene_features_2_sampler.w;

    //Is the world position underwater?
    if (world_position.y < 0.0f)
    {
        //Calculate the intersection point for the ocean plane.
        float intersection_distance = 0.0f;

        LinePlaneIntersection(perceiverWorldPosition, ray_direction, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), intersection_distance);

        vec3 intersection_point = perceiverWorldPosition + ray_direction * intersection_distance;

        //Sample the ocean texture.
        vec4 ocean_texture_sampler = texture(ocean_texture, intersection_point.xz);

        //Write the fragments.
    	scene_features_2 = vec4(0.0f, 1.0f, 0.0f, intersection_distance);
        scene = vec4(vec3(ocean_texture_sampler.w), 1.0f);
    }
    
    else
    {
        //Write the fragments.
    	scene_features_2 = scene_features_2_sampler;
    	scene = scene_sampler;
    }
}