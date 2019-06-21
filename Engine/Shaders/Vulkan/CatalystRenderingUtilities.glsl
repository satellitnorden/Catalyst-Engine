#if !defined(CATALYST_RENDERING_UTILITIES)
#define CATALYST_RENDERING_UTILITIES

//Preprocessor defines.
#define HIGHLIGHT_COLOR (vec3(0.0f, 1.0f, 1.0f))
#define MATERIAL_HIGHLIGHT_BIT (1 << 0)

/*
*	Calculates the highlight weight of a material.
*/
float CalculateHighlightWeight(vec3 viewDirection, vec3 shadingNormal, int materialProperties)
{
	//Calculate the highlight weight.
	return clamp((0.125f + (dot(viewDirection, shadingNormal) + 1.0f) * (sin(totalTime * 2.0f) + 1.0f)) * float(bool(materialProperties & MATERIAL_HIGHLIGHT_BIT)), 0.0f, 1.0f);
}
#endif