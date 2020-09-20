//Includes.
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //Write the fragment.
   fragment = vec4(SampleSky(view_direction, 0.0f), 1.0f);
}