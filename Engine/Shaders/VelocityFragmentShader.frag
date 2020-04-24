//Includes.
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters
layout (location = 0) out vec4 scene_features_4;

/*
*	Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenWORLD_TO_CLIP_MATRIX, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenWORLD_TO_CLIP_MATRIX * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

void CatalystShaderMain()
{
   //Calculate the world position.
   vec3 worldPosition = CalculateWorldPosition(fragmentTextureCoordinate, 0.0f);

    //Write the fragments.
    scene_features_4 = vec4(CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, worldPosition) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, worldPosition), 0.0f, 0.0f);
}