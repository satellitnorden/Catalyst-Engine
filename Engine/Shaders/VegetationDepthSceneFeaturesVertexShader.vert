//Includes.
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
layout (location = 0) out vec2 fragmentTextureCoordinate;
layout (location = 1) out float fragmentDistanceSquared;

void CatalystShaderMain()
{
	//Calculate the normal.
	vec3 normal = normalize(vec3(transformation * vec4(vertexNormal, 0.0f)));

	//Calculate the world position.
	vec3 worldPosition = vec3(transformation * vec4(vertexPosition, 1.0f));

	//Calculate the displacement multiplier.
	float displacementMultiplier = vertexPosition.y;

	//Apply the wind displacement.
	worldPosition += CalculateWindDisplacement(transformation[3].xyz, worldPosition, normal, totalTime, large_scale_wind_displacement_factor, medium_scale_wind_displacement_factor, small_scale_wind_displacement_factor) * displacementMultiplier;

	fragmentTextureCoordinate = vertexTextureCoordinate;
	fragmentDistanceSquared = LengthSquared3(worldPosition - PERCEIVER_WORLD_POSITION);

	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(worldPosition, 1.0f);
}