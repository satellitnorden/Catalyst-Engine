//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define MINIMUM_SHADOW_BIAS (0.002f)
#define MAXIMUM_SHADOW_BIAS (0.004f)

//In parameters.
layout (location = 0) in vec3 fragmentNormal;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Just write the depth.
    fragment = vec4(gl_FragCoord.z + mix(MAXIMUM_SHADOW_BIAS, MINIMUM_SHADOW_BIAS, max(dot(fragmentNormal, -directionalLightDirection), 0.0f)), 0.0f, 0.0f, 0.0f);
}