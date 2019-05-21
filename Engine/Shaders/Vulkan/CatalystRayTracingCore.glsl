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
	vec4 randomVector;
	int currentRecursionDepth;

	vec3 radiance;
	vec3 albedo;
	vec3 geometryNormal;
	vec3 shadingNormal;
	float depth;
	int materialProperties;
	float roughness;
	float metallic;
	float ambientOcclusion;
	float emissive;
};

/*
*	Calculates the ray direction.
*/
vec3 CalculateRayDirection(vec2 coordinate)
{
    vec2 nearPlaneCoordinate = coordinate * 2.0f - 1.0f;
	vec4 viewSpacePosition = inverseProjectionMatrix * vec4(vec3(nearPlaneCoordinate, 1.0f), 1.0f);
	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldSpacePosition = inversePerceiverMatrix * viewSpacePosition;

	vec3 worldPosition = vec3(worldSpacePosition.x, worldSpacePosition.y, worldSpacePosition.z);

	return normalize(worldPosition - perceiverWorldPosition);
}

#endif