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
   layout (offset = 0) vec3 sky_light_direction;
   layout (offset = 16) vec3 sky_light_luminance;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 fragment;

//Global variables.
float sample_offsets[CLOUD_NUMBER_OF_SAMPLES];

void main()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //If the player isn't looking up, there's no clouds. (:
   if (dot(view_direction, vec3(0.0f, 1.0f, 0.0f)) <= 0.0f)
   {
      fragment = vec4(vec3(0.0f), 0.0f);
   }

   else
   {
      //Calculate the start and end points using the two cloud planes.
      float intersection_distance;

      LinePlaneIntersection(PERCEIVER_WORLD_POSITION, view_direction, vec3(0.0f, PERCEIVER_WORLD_POSITION.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
      vec3 start = PERCEIVER_WORLD_POSITION + view_direction * intersection_distance;

      //Calculate the hit distance multiplier.
      float hit_distance_multiplier = CalculateCloudDensityMultipluer(intersection_distance);

      //Return early. (:
      if (hit_distance_multiplier == 0.0f)
      {
         fragment = vec4(vec3(0.0f), 0.0f);

         return;
      }

      LinePlaneIntersection(PERCEIVER_WORLD_POSITION, view_direction, vec3(0.0f, PERCEIVER_WORLD_POSITION.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
      vec3 end = PERCEIVER_WORLD_POSITION + view_direction * intersection_distance;

      //Sample all the sample offsets.
      for (int i = 0; i < CLOUD_NUMBER_OF_NOISE_TEXTURES; ++i)
      {
         vec4 noise_texture = texture(sampler2D(GLOBAL_TEXTURES[(activeNoiseTextureIndex + i) & 63], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

         for (int j = 0; j < 4; ++j)
         {
            int sample_offset_index = (i * 4) + j;

            sample_offsets[sample_offset_index] = (float(sample_offset_index) / CLOUD_NUMBER_OF_SAMPLES) + (((noise_texture[j] * 2.0f - 1.0f) / CLOUD_NUMBER_OF_SAMPLES) * 0.5f);
         }
      }

      //Calculate the density and cloud color.
      float density = 0.0f;
      vec3 cloud_color = vec3(0.0f);

      //Start off with ambient lighting.
      cloud_color += CLOUD_BASE_COLOR * CalculateAmbientIlluminationIntensity() * 0.25f;

      for (int i = 0; i < CLOUD_NUMBER_OF_SAMPLES; ++i)
      {
         //Calculate the sample point.
         vec3 sample_point = mix(start, end, sample_offsets[i]);

         //Get the ensity at this point.
         float new_density = SampleCloudDensity(sample_point, 3);
         new_density = min(new_density, 1.0f - density);
         density += new_density;

         //Add to the cloud color.
         if (new_density > 0.0f)
         {
            float cloud_density_in_direction = 1.0f - SampleCloudDensityInDirection(sample_point, -sky_light_direction, 3);
            cloud_density_in_direction *= cloud_density_in_direction;
            cloud_color += (CLOUD_BASE_COLOR * sky_light_luminance * cloud_density_in_direction) * new_density;
         }

         if (density >= 0.99f)
         {
            break;
         }
      }

      //Calculate the transmittance.
      float transmittance = density * hit_distance_multiplier;

      //Write the fragment.
      fragment = vec4(cloud_color, transmittance);
   }
}