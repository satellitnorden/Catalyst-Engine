//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec3 fragmentNormal;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Write the albedo.
    albedo = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    //Write the normal/depth.
    normalDepth = vec4(fragmentNormal, gl_FragCoord.z);

    //Write the roughness.
    materialProperties.r = 1.0f;

    //Write the metallic.
    materialProperties.g = 0.0f;

    //Write the ambient occlusion.
    materialProperties.b = 1.0f;

    //Write the thickness.
    materialProperties.a = 0.9f;
}