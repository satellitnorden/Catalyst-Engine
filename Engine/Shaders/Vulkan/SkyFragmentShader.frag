//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 sky_light_view_direction;
    layout (offset = 16) vec3 sky_light_luminance;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 fragment;

void main()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //Calculate the sky color.
   vec3 sky_color = SkyColor(view_direction);

   //Calculate the sun weight.
   float sun_weight = pow(max(dot(view_direction, -sky_light_view_direction), 0.0f), 4096.0f);

   //Write the fragment.
   fragment = vec4(mix(sky_color, sky_light_luminance, sun_weight), 1.0f);
}