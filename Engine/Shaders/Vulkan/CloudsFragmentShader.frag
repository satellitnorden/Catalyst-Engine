//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER (100.0f)
#define CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER (1000.0f)
#define NUMBER_OF_SAMPLES (16) //Needs to be a multiple of 4.
#define NUMBER_OF_NOISE_TEXTURES (NUMBER_OF_SAMPLES / 4)
#define CLOUD_POSITION_SCALE (0.000125f) //0.000025f step.
#define CLOUD_PERSISTENCE (0.5f) //0.025f step.
#define CLOUD_LACUNARITY (2.0f) //0.25f step.
#define CLOUD_BASE_COLOR (vec3(0.8f, 0.9f, 1.0f))
#define CLOUD_DENSITY_MULTIPLIER (2.0f) //0.25f step.
      
//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
   layout (offset = 0) vec3 sky_light_direction;
   layout (offset = 16) vec3 sky_light_luminance;
   layout (offset = 32) float cloud_density;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler3D cloud_texture;

//Out parameters
layout (location = 0) out vec4 fragment;

//Global variables.
float sample_offsets[NUMBER_OF_SAMPLES];

/*
*  Calculates the inverse exponential of a number.
*/
float InverseExponential(float number, int iterations)
{
   float result = 1.0f - number;

   for (int i = 0; i < iterations; ++i)
   {
      result *= result;
   }

   result = 1.0f - result;

   return result;
}

/*
*  Samples the density at the given point.
*/
float SampleDensity(vec3 point)
{
   vec3 cloud_offset = -vec3(totalTime, 0.0f, totalTime) * 16.0f;

   vec3 sample_point;
   float density_sample;
   float amplitude = 1.0f;
   float frequency = 1.0f;
   float total = 0.0f;
   float density = 0.0f;

   sample_point = ((point + (cloud_offset * SQUARE_ROOT_OF_TWO)) * frequency * SQUARE_ROOT_OF_TWO) * CLOUD_POSITION_SCALE;
   density_sample = texture(cloud_texture, sample_point).x;
   density += density_sample * amplitude;
   total += amplitude;
   amplitude *= CLOUD_PERSISTENCE;
   frequency *= CLOUD_LACUNARITY;

   sample_point = ((point + (cloud_offset * HALF_PI)) * frequency * HALF_PI) * CLOUD_POSITION_SCALE;
   density_sample = texture(cloud_texture, sample_point).y;
   density += density_sample * amplitude;
   total += amplitude;
   amplitude *= CLOUD_PERSISTENCE;
   frequency *= CLOUD_LACUNARITY;

   sample_point = ((point + (cloud_offset * PHI)) * frequency * PHI) * CLOUD_POSITION_SCALE;
   density_sample = texture(cloud_texture, sample_point).z;
   density += density_sample * amplitude;
   total += amplitude;
   amplitude *= CLOUD_PERSISTENCE;
   frequency *= CLOUD_LACUNARITY;

   sample_point = ((point + (cloud_offset * EULERS_NUMBER)) * frequency * EULERS_NUMBER) * CLOUD_POSITION_SCALE;
   density_sample = texture(cloud_texture, sample_point).w;
   density += density_sample * amplitude;
   total += amplitude;
   amplitude *= CLOUD_PERSISTENCE;
   frequency *= CLOUD_LACUNARITY;

   density /= total;

   density = max(density - (1.0f - cloud_density), 0.0f) * CLOUD_DENSITY_MULTIPLIER;

   return density;
}

/*
*  Returns the density in the direction.
*/
float SampleDensityInDirection(vec3 point, vec3 direction)
{
   //Calculate the start and end points.
   vec3 start = point;
   vec3 end;

   //Is the direction pointing up or down?
   if (dot(direction, vec3(0.0f, 1.0f, 0.0f)) > 0.0f)
   {
      float intersection_distance;

      LinePlaneIntersection(perceiverWorldPosition, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
      end = start + direction * intersection_distance;
   }

   else
   {
      float intersection_distance;

      LinePlaneIntersection(perceiverWorldPosition, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, 1.0f, 0.0f), intersection_distance);
      end = start + direction * intersection_distance;
   }

   //Calculate the density.
   float density = 0.0f;

   for (int i = 0; i < NUMBER_OF_SAMPLES; i += 2)
   {
      //Calculate the sample point.
      vec3 sample_point = mix(start, end, sample_offsets[i]);

      //Get the ensity at this point.
      density = min(density + SampleDensity(sample_point), 1.0f);

      if (density == 1.0f)
      {
         break;
      }
   }

   //Return the total density.
   return density;
}

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

      LinePlaneIntersection(perceiverWorldPosition, view_direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
      vec3 start = perceiverWorldPosition + view_direction * intersection_distance;

      //Calculate the hit distance multiplier.
      float hit_distance_multiplier = (1.0f - clamp((intersection_distance - 1000.0f) * 0.001f, 0.0f, 1.0f));

      //Return early. (:
      if (hit_distance_multiplier == 0.0f)
      {
         fragment = vec4(vec3(0.0f), 0.0f);

         return;
      }

      LinePlaneIntersection(perceiverWorldPosition, view_direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
      vec3 end = perceiverWorldPosition + view_direction * intersection_distance;

      //Sample all the sample offsets.
      for (int i = 0; i < NUMBER_OF_NOISE_TEXTURES; ++i)
      {
         vec4 noise_texture = texture(sampler2D(globalTextures[(activeNoiseTextureIndex + i) & 63], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

         for (int j = 0; j < 4; ++j)
         {
            int sample_offset_index = (i * 4) + j;

            sample_offsets[sample_offset_index] = (float(sample_offset_index) / NUMBER_OF_SAMPLES) + (((noise_texture[j] * 2.0f - 1.0f) / NUMBER_OF_SAMPLES) * 0.5f);
         }
      }

      //Calculate the density and cloud color.
      float density = 0.0f;
      vec3 cloud_color = vec3(0.0f);

      //Start off with ambient lighting.
      cloud_color += CLOUD_BASE_COLOR * CalculateAmbientIlluminationIntensity();

      for (int i = 0; i < NUMBER_OF_SAMPLES; ++i)
      {
         //Calculate the sample point.
         vec3 sample_point = mix(start, end, sample_offsets[i]);

         //Get the ensity at this point.
         float new_density = SampleDensity(sample_point);
         new_density = min(new_density, 1.0f - density);
         density += new_density;

         //Add to the cloud color.
         if (new_density > 0.0f)
         {
            cloud_color += (CLOUD_BASE_COLOR * sky_light_luminance * (1.0f - SampleDensityInDirection(sample_point, -sky_light_direction))) * new_density;
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