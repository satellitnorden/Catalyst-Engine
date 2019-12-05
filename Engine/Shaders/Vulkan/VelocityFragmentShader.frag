//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters
layout (location = 0) out vec4 scene_features_2;

/*
*	Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenViewMatrix, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenViewMatrix * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

void main()
{
   //Calculate the world position.
   vec3 worldPosition = PERCEIVER_WORLD_POSITION + CalculateRayDirection(fragmentTextureCoordinate) * VIEW_DISTANCE;

    //Write the fragments.
    scene_features_2 = vec4(0.0f, CalculateScreenCoordinate(viewMatrix, worldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, worldPosition), VIEW_DISTANCE);
}