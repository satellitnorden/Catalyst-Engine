//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) int patchIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 terrainInformation;

void main()
{
    //Write the terrain information.
    terrainInformation = vec4(float(patchIndex), fragmentTextureCoordinate, gl_FragCoord.z);
}