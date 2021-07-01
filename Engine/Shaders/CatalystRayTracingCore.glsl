#if !defined(CATALYST_RAY_TRACING_CORE)
#define CATALYST_RAY_TRACING_CORE

//Preprocessor defines.
#define CATALYST_RAY_TRACING_T_MINIMUM (0.01f)
#define CATALYST_RAY_TRACING_T_MAXIMUM (1000.0f)

/*
*	Path tracing ray payload struct definition.
*/
struct PathTracingRayPayload
{
	//Input data.
	uint current_recursion_depth;
	vec4 random_noise;
	vec4 random_hemisphere_sample;

	//Output data.
	vec3 radiance;
	vec3 albedo;
	vec3 shading_normal;
	float hit_distance;
	vec4 material_properties;
};

/*
*	Calculates the ray direction.
*/
vec3 CalculateRayDirection(vec2 coordinate)
{
    vec2 nearPlaneCoordinate = coordinate * 2.0f - 1.0f;
	vec4 viewSpacePosition = INVERSE_PERCEIVER_TO_CLIP_MATRIX * vec4(vec3(nearPlaneCoordinate, 1.0f), 1.0f);
	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldSpacePosition = INVERSE_WORLD_TO_PERCEIVER_MATRIX * viewSpacePosition;

	vec3 worldPosition = vec3(worldSpacePosition.x, worldSpacePosition.y, worldSpacePosition.z);

	return normalize(worldPosition - PERCEIVER_WORLD_POSITION);
}

#endif