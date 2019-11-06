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
#define NUMBER_OF_STEPS (4)
#define CLOUD_LAYER_0_POSITION_SCALE (0.001f) //0.00025f step.
#define CLOUD_LAYER_1_POSITION_SCALE (0.01f) //0.00025f step.
#define CLOUD_LAYER_2_POSITION_SCALE (0.1f) //0.00025f step.
#define CLOUD_LAYER_3_POSITION_SCALE (1.0f) //0.00025f step.

#define CLOUD_DENSITY (0.5f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler3D cloud_texture;

//Out parameters
layout (location = 0) out vec4 fragment;

/*
*  Calculates the inverse exponential of a number.
*/
float InverseExponential(float number)
{
   float result = 1.0f - number;
   result *= result;
   result = 1.0f - result;

   return result;
}

/*
*  Samples the density at the given point.
*/
float SampleDensity(vec3 point)
{
   float density = 0.0f;

   point = point * CLOUD_LAYER_0_POSITION_SCALE;
   density += texture(cloud_texture, point).x;

   point = point * CLOUD_LAYER_1_POSITION_SCALE;
   density += texture(cloud_texture, point).x * 0.5f;

   point = point * CLOUD_LAYER_2_POSITION_SCALE;
   density += texture(cloud_texture, point).x * 0.25f;

   point = point * CLOUD_LAYER_3_POSITION_SCALE;
   density += texture(cloud_texture, point).x * 0.125f;

   density /= 1.875f;

   density = max(density - (1.0f - CLOUD_DENSITY), 0.0f) * 2.0f;

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

      LinePlaneIntersection(perceiverWorldPosition, view_direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
      vec3 end = perceiverWorldPosition + view_direction * intersection_distance;

      //Sample the noise texture.
      vec4 noise_texture_1 = texture(sampler2D(globalTextures[activeNoiseTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY));

      //Calculate the density.
      float density = 0.0f;

      for (int i = 0; i < NUMBER_OF_STEPS; ++i)
      {
         //Get the ensity at this point.
         density += SampleDensity(mix(start, end, noise_texture_1[i]));
      }

      //Normalize the density.
      density /= NUMBER_OF_STEPS;

      //Calculate the transmittance.
      float transmittance = exp(-density);

      //Write the fragment.
      fragment = vec4(vec3(1.0f), density);
   }
}