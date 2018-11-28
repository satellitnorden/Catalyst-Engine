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
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in mat3 fragmentTangentSpaceMatrix;
layout (location = 4) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Set the albedo color.
    albedoColor = texture(sampler2D(globalTextures[albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);

    //Set the normal.
    vec3 normalDirection = texture(sampler2D(globalTextures[normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normalDirection = fragmentTangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, gl_FragCoord.z);

    //Sample the material properties.
    vec4 materialPropertiesSampler = texture(sampler2D(globalTextures[materialPropertiesIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);

    //Set the roughness.
    materialProperties.r = materialPropertiesSampler.r;

    //Set the metallic.
    materialProperties.g = materialPropertiesSampler.g;

    //Set the ambient occlusion.
    materialProperties.b = materialPropertiesSampler.b;

    //Write the thickness.
    materialProperties.a = 1.0f;
}