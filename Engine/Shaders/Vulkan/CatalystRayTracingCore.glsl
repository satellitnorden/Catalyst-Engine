#if !defined(CATALYST_RAY_TRACING_CORE)
#define CATALYST_RAY_TRACING_CORE

//Preprocessor defines.
#define CATALYST_RAY_TRACING_T_MINIMUM (0.0f)
#define CATALYST_RAY_TRACING_T_MAXIMUM (100.0f)

/*
*	Ray payload struct definition.
*/
struct PrimaryRayPayload
{
	int currentRecursionDepth;

	vec3 diffuseIrradiance;
	vec3 specularIrradiance;
	vec3 albedo;
	vec3 normal;
	float depth;
	float roughness;
	float metallic;
	float ambientOcclusion;
	uint hitInstanceID;
	uint hitPrimitiveID;

	vec3 radiance;
};

/*
*	Calculates the ray direction.
*/
vec3 CalculateRayDirection(vec2 coordinate)
{
    vec2 nearPlaneCoordinate = coordinate * 2.0f - 1.0f;
	vec4 viewSpacePosition = inverseProjectionMatrix * vec4(vec3(nearPlaneCoordinate, 1.0f), 1.0f);
	float inverseViewSpacePositionDenominator = 1.0f / viewSpacePosition.w;
	viewSpacePosition *= inverseViewSpacePositionDenominator;
	vec4 worldSpacePosition = inversePerceiverMatrix * viewSpacePosition;

	vec3 worldPosition = vec3(worldSpacePosition.x, worldSpacePosition.y, worldSpacePosition.z);

	return normalize(worldPosition - perceiverWorldPosition);
}

/*
*	Returns the specular component of a surface.
*/
float GetSpecularComponent(float roughness, float metallic)
{
	return roughness * (1.0f - metallic);
}

#endif