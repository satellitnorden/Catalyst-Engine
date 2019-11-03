//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 fragment;

void main()
{
   //Calculate the view direction
   vec3 view_direction = CalculateRayDirection(fragment_texture_coordinate);

   //Find the intersection point on the clouds plane.
   float intersection_distance;
   LinePlaneIntersection(perceiverWorldPosition, view_direction, vec3(0.0f, 10000.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);

   vec3 intersection_point = perceiverWorldPosition + view_direction * intersection_distance;

   //Calculate the cell coordinates for the intersection point.
   vec2 cell_coordinate = (intersection_point.xz - vec2(totalTime * 256.0f)) * 0.0001f;
   cell_coordinate.x = cell_coordinate.x - fract(cell_coordinate.x);
   cell_coordinate.y = cell_coordinate.y - fract(cell_coordinate.y);

   //Calculate the cloud weight.
   float cloud_weight = float(RandomFloat(cell_coordinate, 0.0f) > 0.9f);

   //Write the fragment.
   fragment = vec4(vec3(0.8f, 0.9f, 1.0f) * CalculateAmbientIlluminationIntensity(), cloud_weight);
}