//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 sceneFeatures4;
layout (location = 4) out vec4 velocity;

void main()
{
    //Write the fragments.
    sceneFeatures1 = vec4(vec3(1.0f, 0.0f, 0.0f), 0.0f);
    sceneFeatures2 = vec4(vec3(0.0f, 1.0f, 0.0f), 0.0f);
    sceneFeatures3 = vec4(vec3(0.0f, 1.0f, 0.0f), 0);
    sceneFeatures4 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
    velocity = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}