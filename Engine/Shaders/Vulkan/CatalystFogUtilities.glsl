#if !defined(CATALYST_FOG_UTILITIES)
#define CATALYST_FOG_UTILITIES

//Preprocessor defines.
#define FOG_AMBIENT_STRENGTH (0.25f)
#define FOG_BASE_COLOR (vec3(0.8f, 0.9f, 1.0f))

/*
*	Calculates the fog color.
*/
vec3 CalculateFogColor(vec3 viewDirection)
{
	return mix(FOG_BASE_COLOR, directionalLightColor, pow(max(dot(viewDirection, -directionalLightDirection), 0.0f), 2.0f)) * directionalLightIntensity;
}

#endif