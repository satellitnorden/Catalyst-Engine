//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec4 fragmentLayerWeights;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalTexture;

//Out parameters.
layout (location = 0) out vec4 normalDepth;
layout (location = 1) out vec4 layerWeights;

void main()
{
    //Write the normal/depth.
    normalDepth = vec4(texture(normalTexture, fragmentTextureCoordinate).xyz * 2.0f - 1.0f, gl_FragCoord.z);

    //Write the layer weights.
    layerWeights = fragmentLayerWeights;
}