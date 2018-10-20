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
    layout (offset = 0) float windModulatorFactor;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;
layout (location = 4) in float vertexModulatorFactor;
layout (location = 5) in mat4 vertexTransformationMatrix;

//Out parameters.
layout (location = 0) out mat3 fragmentTangentSpaceMatrix;
layout (location = 3) out vec2 fragmentTextureCoordinate;

/*
*   Calculates the wind modulator.
*/
vec3 CalculateWindModulator()
{
    float xModulator = sin(vertexPosition.x + vertexPosition.y  + totalGameTime * windSpeed * EULERS_NUMBER * VEGETATION_WIND_AFFECTION) * cos(vertexPosition.x + vertexPosition.z + totalGameTime * windSpeed * PHI * VEGETATION_WIND_AFFECTION) + 1.0f;
    float zModulator = cos(vertexPosition.z + vertexPosition.y + totalGameTime * windSpeed * PI * VEGETATION_WIND_AFFECTION) * sin(vertexPosition.z + vertexPosition.x + totalGameTime * windSpeed * SQUARE_ROOT_OF_TWO * VEGETATION_WIND_AFFECTION) + 1.0f;

    return vec3(xModulator * windDirection.x, 0.0f, zModulator * windDirection.z) * windSpeed * VEGETATION_WIND_AFFECTION;
}

void main()
{
    //Calculate the final vertex position.
    vec3 finalVertexPosition = (vertexTransformationMatrix * vec4(vertexPosition, 1.0)).xyz;
    finalVertexPosition += CalculateWindModulator() * windModulatorFactor * vertexModulatorFactor;

    //Calculate the fragment tangent space matrix.
    vec3 tangent = normalize(vec3(vertexTransformationMatrix * vec4(vertexTangent, 0.0f)));
    vec3 bitangent = normalize(vec3(vertexTransformationMatrix * vec4(cross(vertexNormal, vertexTangent), 0.0f)));
    vec3 normal = normalize(vec3(vertexTransformationMatrix * vec4(vertexNormal, 0.0f)));

    fragmentTangentSpaceMatrix = mat3(tangent, bitangent, normal);

    //Pass along the fragment texture coordinate.
    fragmentTextureCoordinate = vertexTextureCoordinate;

    //Set the position.
    gl_Position = viewMatrix * vec4(finalVertexPosition, 1.0f);
}