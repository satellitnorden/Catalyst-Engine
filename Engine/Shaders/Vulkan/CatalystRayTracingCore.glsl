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
	vec3 radiance;
	int currentRecursionDepth;
};

#endif