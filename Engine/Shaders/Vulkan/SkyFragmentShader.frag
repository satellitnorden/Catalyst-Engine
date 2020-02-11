//Version declaration.
#version 450

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
layout (set = 1, binding = 0) uniform samplerCube sky_texture;

//Out parameters
layout (location = 0) out vec4 fragment;

void main()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //Write the fragment.
   fragment = texture(sky_texture, view_direction);
}