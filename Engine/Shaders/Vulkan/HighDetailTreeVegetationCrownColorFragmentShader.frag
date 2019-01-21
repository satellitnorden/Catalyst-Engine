//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int albedoTextureIndex;
    layout (offset = 4) int normalMapTextureIndex;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 3) in vec2 fragmentTextureCoordinate;

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
    normal *= gl_FrontFacing ? 1.0f : -1.0f;
    normalDepth = vec4(normal, gl_FragCoord.z);

    //Write the material properties.
    materialProperties = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}