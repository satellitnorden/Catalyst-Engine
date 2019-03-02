//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

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
layout (location = 4) in mat4 transformationMatrix;

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out float fragmentLengthFactor;

void main()
{
    //Calculate the fragment normal.
    fragmentNormal = normalize(vec3(transformationMatrix * vec4(vertexNormal, 0.0f)));
    
    //Calculate the transformed position.
    vec4 transformedPosition = transformationMatrix * vec4(vertexPosition, 1.0);

    //Calculate the fragment length factor.
    float distanceToVertexSquared = LengthSquared3(transformedPosition.xyz - perceiverWorldPosition);
    fragmentLengthFactor = distanceToVertexSquared >= cutoffDistanceSquared ? 0.0f : distanceToVertexSquared <= halfCutoffDistanceSquared ? 1.0f : 1.0f - ((distanceToVertexSquared - halfCutoffDistanceSquared) * inverseHalfCutoffDistanceSquared);

    //Write the position.
    gl_Position = directionalLightViewMatrix * transformationMatrix * vec4(vertexPosition, 1.0);
}