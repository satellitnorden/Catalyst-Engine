#if !defined(CATALYST_RAY_TRACING_CORE)
#define CATALYST_RAY_TRACING_CORE

//Preprocessor defines.
#define CATALYST_RAY_TRACING_T_MINIMUM (0.0001f)
#define CATALYST_RAY_TRACING_T_MAXIMUM (100.0f)
#define CATALYST_RAY_TRACING_MAXIMUM_DEPTH (2)

/*
*	Ray payload struct definition.
*/
struct RayPayload
{
	int currentRecursionDepth;

	vec3 radiance;
	vec3 indirectLighting;
	vec3 directLighting;
	vec3 normal;
	float depth;
	float roughness;
	float metallic;
	float ambientOcclusion;
};

#endif