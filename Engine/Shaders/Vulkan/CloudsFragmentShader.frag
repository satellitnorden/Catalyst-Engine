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
#define NUMBER_OF_STEPS (16)
#define CLOUD_POSITION_SCALE (0.0001f)

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
   return   InverseExponential(max( ((texture(cloud_texture, point).x
                                    + texture(cloud_texture, point * 2.0f).x * 0.5f
                                    + texture(cloud_texture, point * 4.0f).x * 0.25f) / 1.75f) - (1.0f - CLOUD_DENSITY), 0.0f));
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

      //Calculate the step.
      vec3 step = (end - start) / NUMBER_OF_STEPS;

      //Calculate the density.
      float density = 0.0f;

      vec3 current = start;

      for (int i = 0; i < NUMBER_OF_STEPS; ++i)
      {
         //Get the ensity at this point.
         density += SampleDensity((current - (vec3(totalTime, 0.0f, totalTime) * 16.0f)) * CLOUD_POSITION_SCALE);

         //Advance the current position.
         current += step;
      }

      //Normalize the density.
      density /= NUMBER_OF_STEPS;

      //Calculate the transmittance.
      float transmittance = density;

      //Write the fragment.
      fragment = vec4(vec3(0.8f, 0.9f, 1.0f), transmittance);
   }
}