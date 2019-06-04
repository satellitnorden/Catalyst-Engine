//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 64) int albedoTextureIndex;
    layout (offset = 68) int normalMapTextureIndex;
    layout (offset = 72) int materialPropertiesIndex;
    layout (offset = 76) int materialPropertyFlags;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 4) in vec3 fragmentWorldPosition;
layout (location = 5) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 sceneFeatures4;

void main()
{
    //Sample the albedo.
   vec3  albedo = texture(globalTextures[albedoTextureIndex], fragmentTextureCoordinate).rgb;

   //Sample the normal map.
   vec3 normalMap = texture(globalTextures[normalMapTextureIndex], fragmentTextureCoordinate).xyz;

   //Sample the material properties.
   vec4 materialProperties = texture(globalTextures[materialPropertiesIndex], fragmentTextureCoordinate);

   //Calculate the shading normal.

    //Set the normal.
    vec3 shadingNormal = normalMap * 2.0f - 1.0f;
    shadingNormal = fragmentTangentSpaceMatrix * shadingNormal;
    shadingNormal = normalize(shadingNormal);

    //Write the fragments.
    sceneFeatures1 = vec4(albedo, 1.0f);
    sceneFeatures2 = vec4(fragmentTangentSpaceMatrix[2], length(fragmentWorldPosition - perceiverWorldPosition));
    sceneFeatures3 = vec4(shadingNormal, materialPropertyFlags);
    sceneFeatures4 = materialProperties;
}