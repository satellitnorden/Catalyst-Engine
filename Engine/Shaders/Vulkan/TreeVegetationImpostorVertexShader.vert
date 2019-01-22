//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in mat4 vertexTransformationMatrix;

//Out parameters.
layout (location = 0) out mat4 geometryTransformationMatrix;

void main()
{
    //Pass along the transformation matrix.
    geometryTransformationMatrix = vertexTransformationMatrix;
} 