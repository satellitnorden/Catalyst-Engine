//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShadowUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) int maskTextureIndex;
};

//In parameters.
layout (location = 0) in vec3 fragmentNormal;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Discard this fragment according to the mask texture.
    if (texture(sampler2D(globalTextures[maskTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index]), fragmentTextureCoordinate).r < 0.5f)
    {
        discard;
    }

    else
    {
    	//Just write the depth.
    	fragment = vec4(gl_FragCoord.z + CalculateDirectionalShadowBias(fragmentNormal), 0.0f, 0.0f, 0.0f);
    }
}