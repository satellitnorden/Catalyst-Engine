//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;
layout (set = 1, binding = 1) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 scene_features_2;
layout (location = 1) out vec4 scene;

void main()
{
	//Sample the textures.
	vec4 scene_features_2_sampler = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec4 scene_sampler = texture(scene_texture, fragment_texture_coordinate);

	//Calculate the world position.
	vec3 world_position = perceiverWorldPosition + CalculateRayDirection(fragment_texture_coordinate) * scene_features_2_sampler.w;

    //Write the fragments.
    if (world_position.y < 1.0f)
    {
    	if (world_position.y < 0.0f)
	    {
	    	scene_features_2 = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	    	scene = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	    }
    	
    	else
    	{
    		scene_features_2 = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    		scene = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    	}
    }
    
    else
    {
    	scene_features_2 = scene_features_2_sampler;
    	scene = scene_sampler;
    }
}