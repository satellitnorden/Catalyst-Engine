//Includes.
#include "CatalystRayTracingCore.glsl"
#define CLOUD_TEXTURE_SET_INDEX (1)
#include "CatalystCloudsCore.glsl"

//Constants.
#define SKY_LIGHT_ANCHOR (0.98075f) //0.0025f step.
#define SKY_LIGHT_SIZE (0.0075f) //0.0025f step.

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

   //Sample the sky.
   vec3 sky_sample = SampleSky(view_direction, 0.0f);

   //Calculate the sky light influence.
   float sky_light_influence = smoothstep(SKY_LIGHT_ANCHOR - SKY_LIGHT_SIZE, SKY_LIGHT_ANCHOR + SKY_LIGHT_SIZE, dot(view_direction, -SKY_LIGHT_DIRECTION));

   //Calculate the bare sky color.
   vec3 bare_sky_color = mix(sky_sample, max((SKY_LIGHT_RADIANCE.rgb * SKY_LIGHT_RADIANCE.a) * 4.0f, sky_sample), sky_light_influence);

   //Calculate the cloud color/alpha.
   //vec4 cloud_color = GetCloudColorInDirection(PERCEIVER_WORLD_POSITION, view_direction, uvec2(gl_FragCoord.xy), -SKY_LIGHT_DIRECTION, SKY_LIGHT_RADIANCE.rgb * SKY_LIGHT_RADIANCE.a);
   vec4 cloud_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

   //Write the fragment.
   fragment = vec4(mix(bare_sky_color, cloud_color.rgb, cloud_color.a), 1.0f);
}