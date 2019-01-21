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
layout (location = 0) in vec3 fragmentNormal;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Write the albedo.
    albedo = texture(sampler2D(globalTextures[albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);

    //Write the normal/depth.
    normalDepth = vec4(fragmentNormal, gl_FragCoord.z);

    //Write the material properties.
    materialProperties = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}