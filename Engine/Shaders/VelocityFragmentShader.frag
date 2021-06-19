//Includes.
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters
layout (location = 0) out vec4 scene_features_4;

void CatalystShaderMain()
{
   //Calculate the world position.
   vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, 0.0f);

    //Write the fragments.
    scene_features_4 = vec4(CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, world_position), 0.0f, 0.0f);
}