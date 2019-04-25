#if !defined(CATALYST_RAY_TRACING_CORE)
#define CATALYST_RAY_TRACING_CORE

//Preprocessor defines.
#define CATALYST_RAY_TRACING_T_MINIMUM (0.0001f)
#define CATALYST_RAY_TRACING_T_MAXIMUM (100.0f)
#define CATALYST_RAY_TRACING_MAXIMUM_DEPTH (2)
#define CATALYST_RAY_TRACING_SPECULAR_POWER (2.0f)

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

/*
*	Returns the specular component of a surface.
*/
float GetSpecularComponent(float roughness, float metallic)
{
	return pow(roughness * (1.0f - metallic), CATALYST_RAY_TRACING_SPECULAR_POWER);
}

#endif