//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 16) int textureIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Write the fragment.
    fragment = texture(globalTextures[textureIndex], fragmentTextureCoordinate);
}