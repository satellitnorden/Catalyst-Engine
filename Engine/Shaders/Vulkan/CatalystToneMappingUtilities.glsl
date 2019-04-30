#if !defined(CATALYST_TONE_MAPPING_UTILITIES)
#define CATALYST_TONE_MAPPING_UTILITIES

/*
*   Tone maps a fragment.
*/
vec3 ToneMap(vec3 fragment)
{
    return fragment / (fragment + vec3(1.0f));
}
#endif