//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 20) int normalTextureIndex;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 sceneFeatures4;
layout (location = 4) out vec4 velocity;

/*
* Returns the screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(mat4 givenViewMatrix, vec3 worldPosition)
{
  vec4 viewSpacePosition = givenViewMatrix * vec4(worldPosition, 1.0f);
  viewSpacePosition.xy /= viewSpacePosition.w;

  return viewSpacePosition.xy * 0.5f + 0.5f;
}

void main()
{
    //Retrieve the terrain normal.
    vec3 terrainNormal = normalize(texture(sampler2D(globalTextures[normalTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f);

    //calculate the material texture coordinate.
    vec2 materialTextureCoordinate = fragmentWorldPosition.xz;

    //Retrieve the albedo.
    //vec3 albedo = texture(sampler2D(globalTextures[204], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate).rgb;
    vec3 albedo = dot(terrainNormal, vec3(0.0f, 1.0f, 0.0f)) > 0.75f ? texture(sampler2D(globalTextures[204], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), materialTextureCoordinate).rgb : vec3(0.1f, 0.1f, 0.1f);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(albedo, vec3(2.2f)), 0.0f);
    sceneFeatures2 = vec4(terrainNormal, length(fragmentWorldPosition - perceiverWorldPosition));
    sceneFeatures3 = vec4(terrainNormal, 0.0f);
    sceneFeatures4 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
    velocity = vec4(CalculateScreenCoordinate(viewMatrix, fragmentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentWorldPosition), 0.0f, 0.0f);
}