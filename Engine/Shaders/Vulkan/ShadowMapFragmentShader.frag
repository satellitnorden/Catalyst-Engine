//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShadowUtilities.glsl"

//In parameters.
layout (location = 0) in vec3 fragmentNormal;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Just write the depth.
    fragment = vec4(gl_FragCoord.z + CalculateDirectionalShadowBias(fragmentNormal), 0.0f, 0.0f, 0.0f);
}