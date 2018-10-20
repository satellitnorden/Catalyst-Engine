//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec3 fragmentNormal;
layout (location = 1) in vec4 fragmentLayerWeights;

//Out parameters.
layout (location = 0) out vec4 normalDepth;
layout (location = 1) out vec4 layerWeights;

void main()
{
    //Write the normal/depth.
    normalDepth = vec4(fragmentNormal, gl_FragCoord.z);

    //Write the layer weights.
    layerWeights = fragmentLayerWeights;
}