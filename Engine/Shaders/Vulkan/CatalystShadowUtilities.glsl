#if !defined(CATALYST_SHADOW_UTILITIES)
#define CATALYST_SHADOW_UTILITIES

/*
*   Returns a directional shadow bias based on the normal of a surface.
*/
float CalculateDirectionalShadowBias(vec3 normal)
{
    return mix(0.001f, 0.0005f, max(dot(normal, -directionalLightDirection), 0.0f));
}

#endif