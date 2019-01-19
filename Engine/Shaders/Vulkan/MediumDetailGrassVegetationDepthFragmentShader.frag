//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D maskTexture;

void main()
{
    //Discard this fragment according to the mask texture.
    if (texture(maskTexture, fragmentTextureCoordinate).r < 0.5f)
    {
        discard;
    }
}