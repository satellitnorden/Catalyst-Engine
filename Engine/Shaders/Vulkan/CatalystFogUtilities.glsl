#if !defined(CATALYST_FOG_UTILITIES)
#define CATALYST_FOG_UTILITIES

//Preprocessor defines.
#define FOG_AMBIENT_STRENGTH (0.025f)
#define FOG_BASE_COLOR (vec3(0.8f, 0.9f, 1.0f))
#define FOG_DIRECTIONAL_LIGHT_SCALE (0.125f)

/*
*	Calculates the fog color.
*/
vec3 CalculateFogColor(vec3 viewDirection)
{
	float directionalLightInfluence = directionalLightIntensity * FOG_DIRECTIONAL_LIGHT_SCALE;
	vec3 baseColor = mix(FOG_BASE_COLOR, directionalLightColor, pow(clamp(dot(viewDirection, -directionalLightDirection) + directionalLightInfluence, 0.0f, 1.0f), 4.0f));

	return (baseColor * FOG_AMBIENT_STRENGTH) + (baseColor * directionalLightInfluence);
}
#endif