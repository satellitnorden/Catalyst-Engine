//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 12) int albedoTextureIndex;
    layout (offset = 16) int normalMapTextureIndex;
    layout (offset = 20) int materialPropertiesIndex;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 3) in vec3 fragmentPreviousWorldPosition;
layout (location = 4) in vec3 fragmentCurrentWorldPosition;
layout (location = 5) in vec2 fragmentTextureCoordinate;

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
  //Sample the albedo.
  vec3 albedo = texture(sampler2D(globalTextures[albedoTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).rgb;

  //Sample the normal map.
  vec3 normalMap = texture(sampler2D(globalTextures[normalMapTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).xyz;

  //Sample the material properties.
  vec4 materialProperties = texture(sampler2D(globalTextures[materialPropertiesIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate);

  //Calculate the shading normal.
  vec3 shadingNormal = normalMap * 2.0f - 1.0f;
  shadingNormal = fragmentTangentSpaceMatrix * shadingNormal;
  shadingNormal *= gl_FrontFacing ? 1.0f : -1.0f;
  shadingNormal = normalize(shadingNormal);

  //Write the fragments.
  sceneFeatures1 = vec4(pow(albedo, vec3(2.2f)), 0.0f);
  sceneFeatures2 = vec4(PackNormal(fragmentTangentSpaceMatrix[2] * (gl_FrontFacing ? 1.0f : -1.0f)), 0.0f, 0.0f, length(fragmentCurrentWorldPosition - perceiverWorldPosition));
  sceneFeatures3 = vec4(PackNormal(shadingNormal), 0.0f, 0.0f, 0);
  sceneFeatures4 = materialProperties;
  velocity = vec4(CalculateScreenCoordinate(viewMatrix, fragmentCurrentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentPreviousWorldPosition), 0.0f, 0.0f);
}