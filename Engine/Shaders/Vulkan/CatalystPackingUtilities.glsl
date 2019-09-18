#if !defined(CATALYST_PACKING_UTILITIES)
#define CATALYST_PACKING_UTILITIES

/*
*   Packs a normal into one float.
*/
float PackNormal(vec3 normal)
{
	normal.x = normal.x * 0.5f + 0.5f;
	normal.y = normal.y * 0.5f + 0.5f;
	normal.z = normal.z * 0.5f + 0.5f;

    return uintBitsToFloat((uint(normal.x * 255.0f) << 16) | (uint(normal.y * 255.0f) << 8) | uint(normal.z * 255.0f));
}

/*
*   Unpacks a normal into a vec3.
*/
vec3 UnpackNormal(float normal)
{   
    vec3 normal3;
    uint uint_normal = floatBitsToUint(normal);

    normal3.x = float(uint_normal >> 16) / 255.0f;
    normal3.y = float((uint_normal >> 8) & 255) / 255.0f;
    normal3.z = float(uint_normal & 255) / 255.0f;

    normal3.x = normal3.x * 2.0f - 1.0f;
	normal3.y = normal3.y * 2.0f - 1.0f;
	normal3.z = normal3.z * 2.0f - 1.0f;

    return normal3;
}
#endif