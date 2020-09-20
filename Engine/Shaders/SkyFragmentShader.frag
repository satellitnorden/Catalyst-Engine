//Includes.
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 SKY_LIGHT_DIRECTION;
	layout (offset = 16) vec3 SKY_LIGHT_RADIANCE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //Sample the sky.
   vec3 sky_sample = SampleSky(view_direction, 0.0f);

   //Calculate the sky light influence.
   float sky_light_influence = pow(max(dot(view_direction, SKY_LIGHT_DIRECTION), 0.0f), 256.0f);

   //Write the fragment.
   fragment = vec4(mix(sky_sample, SKY_LIGHT_RADIANCE, sky_light_influence), 1.0f);
}