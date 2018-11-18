//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;
layout (location = 1) in flat int fragmentNormalTextureIndex;
layout (location = 2) in flat int fragmentLayerWeightsTextureIndex;

//Out parameters.
layout (location = 0) out vec4 normalDepth;
layout (location = 1) out vec4 layerWeights;

void main()
{
    //Write the normal/depth.
    normalDepth = vec4(texture(sampler2D(globalTextures[fragmentNormalTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f, gl_FragCoord.z);

    //Write the layer weights.
    layerWeights = texture(sampler2D(globalTextures[fragmentLayerWeightsTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), fragmentTextureCoordinate);
}