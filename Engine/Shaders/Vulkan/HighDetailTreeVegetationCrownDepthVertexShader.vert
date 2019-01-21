//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystVegetationUtilities.glsl"

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;
layout (location = 4) in float vertexModulatorFactor;
layout (location = 5) in mat4 vertexTransformationMatrix;

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{
    //Calculate the final vertex position.
    vec3 finalVertexPosition = (vertexTransformationMatrix * vec4(vertexPosition, 1.0)).xyz;
    finalVertexPosition += CalculateTreeVegetationWindModulator(gl_InstanceIndex) * vertexModulatorFactor;

    //Pass along the fragment texture coordinate.
    fragmentTextureCoordinate = vertexTextureCoordinate;

    //Set the position.
    gl_Position = viewMatrix * vec4(finalVertexPosition, 1.0f);
}