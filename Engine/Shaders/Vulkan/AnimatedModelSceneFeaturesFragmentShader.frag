//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 128) int albedoTextureIndex;
    layout (offset = 132) int normalMapTextureIndex;
    layout (offset = 136) int materialPropertiesIndex;
    layout (offset = 140) int materialPropertyFlags;
    layout (offset = 144) float luminanceMultiplier;
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
   vec3 albedo = texture(sampler2D(GLOBAL_TEXTURES[albedoTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).rgb;

   //Sample the normal map.
   vec3 normalMap = texture(sampler2D(GLOBAL_TEXTURES[normalMapTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).xyz;

   //Sample the material properties.
   vec4 materialProperties = texture(sampler2D(GLOBAL_TEXTURES[materialPropertiesIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate);

   //Calculate the shading normal.
    vec3 shadingNormal = normalMap * 2.0f - 1.0f;
    shadingNormal = fragmentTangentSpaceMatrix * shadingNormal;
    shadingNormal = normalize(shadingNormal);


    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(viewMatrix, fragmentCurrentWorldPosition) - CalculateScreenCoordinate(viewMatrixMinusOne, fragmentPreviousWorldPosition);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(albedo, vec3(2.2f)), intBitsToFloat(materialPropertyFlags));
    sceneFeatures2 = vec4(PackNormal(shadingNormal), velocity, gl_FragCoord.z);
    sceneFeatures3 = materialProperties;
}