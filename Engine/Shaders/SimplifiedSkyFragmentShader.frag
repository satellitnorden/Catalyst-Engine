//Includes.
#include "CatalystRayTracingCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystToneMapping.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
   layout (offset = 0) vec3 SKY_LIGHT_LUMINANCE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //Calculate the world position.
   vec3 world_position = PERCEIVER_WORLD_POSITION + view_direction * VIEW_DISTANCE;

   //Calculate the final sky.
   vec3 final_sky = SampleSky(view_direction, 0.0f);

   //Add volumetric lighting.
   {
      //Calculate the volumetric lighting.
      vec3 volumetric_lighting = vec3(0.0f);

      //Add the volumetric ambient lighting.
      volumetric_lighting += CalculateVolumetricAmbientLighting();

      //Add the sky lighting.
      volumetric_lighting += VOLUMETRIC_LIGHTING_BASE_COLOR * SKY_LIGHT_RADIANCE.rgb * SKY_LIGHT_RADIANCE.a * VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER;

      //Calculate the volumetric lighting opacity.
      float volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(VIEW_DISTANCE, VOLUMETRIC_LIGHTING_DISTANCE, world_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, PERCEIVER_WORLD_POSITION.y);

      //Blend the volumetric lighting with the final sky.
      final_sky = mix(final_sky, volumetric_lighting, volumetric_lighting_opacity);
   }

   //Apply tone mapping.
   final_sky = ApplyToneMapping(final_sky);

   //Write the fragment.
   fragment = vec4(final_sky, 1.0f);
}