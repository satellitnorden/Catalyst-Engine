//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystVegetationUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) float fadeStartDistanceSquared;
    layout (offset = 4) float inverseFadeDistanceSquared;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;
layout (location = 4) in float vertexModulatorFactor;
layout (location = 5) in mat4 vertexTransformationMatrix;

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out vec2 fragmentTextureCoordinate;
layout (location = 2) out float fragmentOpacity;

void main()
{
    //Pass along the fragment normal.
    fragmentNormal = normalize(vec3(vertexTransformationMatrix * vec4(vertexNormal, 0.0f)));

    //Pass along the texture coordinate to the fragment shader.
    fragmentTextureCoordinate = vertexTextureCoordinate;

    //Calculate the world position.
    vec3 worldPosition = (vertexTransformationMatrix * vec4(vertexPosition, 1.0)).xyz;
    worldPosition += CalculateTreeVegetationWindModulator(vertexTransformationMatrix[3].xyz) * vertexModulatorFactor;

    //Calculate the fragment opacity.
    float distanceToVertexSquared = LengthSquared3(worldPosition - cameraWorldPosition);
    fragmentOpacity = 1.0f - clamp((distanceToVertexSquared - fadeStartDistanceSquared) * inverseFadeDistanceSquared, 0.0f, 1.0f);

    //Write the position.
    gl_Position = viewMatrix * vec4(worldPosition, 1.0);
}