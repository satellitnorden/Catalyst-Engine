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
    layout (offset = 0) int albedoTextureIndex;
    layout (offset = 4) int normalMapTextureIndex;
    layout (offset = 8) int materialPropertiesIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;
layout (location = 1) in mat3 fragmentTangentSpaceMatrix;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Write the albedo.
    albedo = texture(sampler2D(globalTextures[albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);

    //Write the normal/depth.
    vec3 normal = texture(sampler2D(globalTextures[normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normal = fragmentTangentSpaceMatrix * normal;
    normalDepth = vec4(normal, gl_FragCoord.z);

    //Sample the material properties.
    vec4 materialPropertiesSampler = texture(sampler2D(globalTextures[materialPropertiesIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);

    //Write the roughness.
    materialProperties.r = materialPropertiesSampler.r;

    //Write the metallic.
    materialProperties.g = materialPropertiesSampler.g;

    //Write the ambient occlusion.
    materialProperties.b = materialPropertiesSampler.b;

    //Write the thickness.
    materialProperties.a = 1.0f;
}