//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;
layout (location = 1) in float fragmentLengthFactor;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D maskTexture;

void main()
{
    //Discard this fragment according to the mask texture.
    if (fragmentLengthFactor == 0.0f
        || fragmentLengthFactor < RandomFloat(gl_FragCoord.xyz)
        || texture(maskTexture, fragmentTextureCoordinate).r == 0.0f)
    {
        discard;
    }
}