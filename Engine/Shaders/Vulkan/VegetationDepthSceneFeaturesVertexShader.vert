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
layout (location = 4) in mat4 transformation;

//Out parameters.
layout (location = 0) out vec3 fragmentCurrentWorldPosition;
layout (location = 1) out vec2 fragmentTextureCoordinate;
layout (location = 2) out float fragmentDistanceSquared;

void main()
{
	//Calculate the normal.
	vec3 normal = normalize(vec3(transformation * vec4(vertexNormal, 0.0f)));

	//Calculate the world position.
	vec3 worldPosition = vec3(transformation * vec4(vertexPosition, 1.0f));

	//Calculate the displacement multiplier.
	float displacementMultiplier = vertexPosition.y;

	//Apply the wind displacement.
	worldPosition += CalculateWindDisplacement(transformation[3].xyz, worldPosition, normal) * displacementMultiplier;

	fragmentCurrentWorldPosition = worldPosition;
	fragmentTextureCoordinate = vertexTextureCoordinate;
	fragmentDistanceSquared = LengthSquared3(fragmentCurrentWorldPosition - perceiverWorldPosition);

	gl_Position = viewMatrix * vec4(fragmentCurrentWorldPosition, 1.0f);
}