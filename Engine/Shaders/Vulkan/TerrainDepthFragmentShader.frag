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
	layout (offset = 24) int layerWeightsTextureIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 normalDepth;
layout (location = 1) out vec4 layerWeights;

void main()
{
    //Write the normal/depth.
    normalDepth = vec4(texture(sampler2D(globalTextures[normalTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f, gl_FragCoord.z);

    //Write the layer weights.
    layerWeights = texture(sampler2D(globalTextures[layerWeightsTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);
}