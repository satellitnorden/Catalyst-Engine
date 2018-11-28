//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;
layout (location = 1) in flat int patchIndex;

//Out parameters.
layout (location = 0) out vec4 terrainInformation;

void main()
{
    //Write the terrain information.
    terrainInformation = vec4(float(patchIndex), fragmentTextureCoordinate, gl_FragCoord.z);
}