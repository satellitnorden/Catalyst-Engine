//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define EULERS_NUMBER 2.718281f
#define PHI 1.618033f
#define PI 3.141592f
#define SQUARE_ROOT_OF_TWO 1.414213f
#define VEGETATION_WIND_AFFECTION 0.15f

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) float cutoffDistanceSquared;
    layout (offset = 4) float halfCutoffDistanceSquared;
    layout (offset = 8) float inverseHalfCutoffDistanceSquared;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;
layout (location = 4) in float vertexModulatorFactor;
layout (location = 5) in mat4 vertexTransformationMatrix;

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;
layout (location = 1) out float fragmentLengthFactor;

/*
*   Returns the length of a vector squared.
*/
float LengthSquared(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

void main()
{
    //Calculate the final vertex position.
    vec3 finalVertexPosition = (vertexTransformationMatrix * vec4(vertexPosition, 1.0)).xyz;

    //Pass along the fragment texture coordinate.
    fragmentTextureCoordinate = vertexTextureCoordinate;

    //Calculate the length squared.
    float distanceToVertexSquared = LengthSquared(finalVertexPosition - cameraWorldPosition);

    fragmentLengthFactor = distanceToVertexSquared >= cutoffDistanceSquared ? 0.0f : distanceToVertexSquared <= halfCutoffDistanceSquared ? 1.0f : 1.0f - ((distanceToVertexSquared - halfCutoffDistanceSquared) * inverseHalfCutoffDistanceSquared);

    //Set the position.
    gl_Position = viewMatrix * vec4(finalVertexPosition, 1.0f);
}