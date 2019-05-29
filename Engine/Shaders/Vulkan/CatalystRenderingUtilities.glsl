#if !defined(CATALYST_RENDERING_UTILITIES)
#define CATALYST_RENDERING_UTILITIES

//Preprocessor defines.
#define HIGHLIGHT_COLOR (vec3(0.0f, 1.0f, 1.0f))
#define MATERIAL_HIGHLIGHT_BIT (1 << 0)
#define MATERIAL_TRANSLUCENT_BIT (1 << 1)

/*
*	Calculates the highlight of an object.
*/
vec3 CalculateHighlight(vec3 viewDirection, vec3 shadingNormal, int materialProperties)
{
	return HIGHLIGHT_COLOR * (0.125f + pow(max(((dot(viewDirection, shadingNormal) + 1.0f) * 0.5f) * ((sin(totalTime * 2.0f) + 1.0f) * 0.5f), 0.0f), 2.0f)) * float(bool(materialProperties & MATERIAL_HIGHLIGHT_BIT));
}
#endif