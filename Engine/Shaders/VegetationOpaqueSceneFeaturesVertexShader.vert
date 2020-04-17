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
    layout (offset = 0) float large_scale_wind_displacement_factor;
    layout (offset = 4) float medium_scale_wind_displacement_factor;
    layout (offset = 8) float small_scale_wind_displacement_factor;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;
layout (location = 4) in mat4 transformation;

//Out parameters.
layout (location = 0) out mat3 fragmentTangentSpaceMatrix;
layout (location = 3) out vec3 fragmentPreviousWorldPosition;
layout (location = 4) out vec3 fragmentCurrentWorldPosition;
layout (location = 5) out vec2 fragmentTextureCoordinate;
layout (location = 6) out float fragment_distance_squared;

void main()
{
	//Calculate the tangent/bitangent/normal.
	vec3 tangent = normalize(vec3(transformation * vec4(vertexTangent, 0.0f)));
	vec3 bitangent = normalize(vec3(transformation * vec4(cross(vertexNormal, vertexTangent), 0.0f)));
	vec3 normal = normalize(vec3(transformation * vec4(vertexNormal, 0.0f)));

	//Calculate the world position.
	fragmentPreviousWorldPosition = fragmentCurrentWorldPosition = vec3(transformation * vec4(vertexPosition, 1.0f));

	//Calculate the displacement multiplier.
	float displacementMultiplier = vertexPosition.y;

	//Apply the wind displacement.
	fragmentPreviousWorldPosition += CalculateWindDisplacement(transformation[3].xyz, fragmentCurrentWorldPosition, normal, totalTime - DELTA_TIME, large_scale_wind_displacement_factor, medium_scale_wind_displacement_factor, small_scale_wind_displacement_factor) * displacementMultiplier;
	fragmentCurrentWorldPosition += CalculateWindDisplacement(transformation[3].xyz, fragmentCurrentWorldPosition, normal, totalTime, large_scale_wind_displacement_factor, medium_scale_wind_displacement_factor, small_scale_wind_displacement_factor) * displacementMultiplier;

	fragmentTangentSpaceMatrix = mat3(tangent, bitangent, normal);
	fragmentTextureCoordinate = vertexTextureCoordinate;
	fragment_distance_squared = LengthSquared3(fragmentCurrentWorldPosition - PERCEIVER_WORLD_POSITION);

	gl_Position = viewMatrix * vec4(fragmentCurrentWorldPosition, 1.0f);
}