//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int maskTextureIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;
layout (location = 1) in float fragmentOpacity;

void main()
{
    //Discard this fragment according to the mask texture.
    if (fragmentOpacity < RandomFloat(gl_FragCoord.xyz)
    	|| texture(sampler2D(globalTextures[maskTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate).r < 0.5f)
    {
        discard;
    }
}