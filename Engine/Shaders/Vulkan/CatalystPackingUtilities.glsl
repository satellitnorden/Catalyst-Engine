#if !defined(CATALYST_PACKING_UTILITIES)
#define CATALYST_PACKING_UTILITIES

//Constants.
#define PACK_NORMALS_8_BIT 0
#define MAXIMUM_8_BIT_UINT (255)
#define MAXIMUM_10_BIT_UINT (1023)
#define MAXIMUM_11_BIT_UINT (2047)

/*
*   Packs a normal into one float.
*/
float PackNormal(vec3 normal)
{
    //Normalize the normal into a [0.0f, 1.0f] range.
	normal.x = normal.x * 0.5f + 0.5f;
	normal.y = normal.y * 0.5f + 0.5f;
	normal.z = normal.z * 0.5f + 0.5f;

    //Construct the uint.
    uint packed_normal = 0;

#if PACK_NORMALS_8_BIT
    packed_normal |= uint(normal.x * MAXIMUM_8_BIT_UINT) << 16;
    packed_normal |= uint(normal.y * MAXIMUM_8_BIT_UINT) << 8;
    packed_normal |= uint(normal.z * MAXIMUM_8_BIT_UINT);
#else
    packed_normal |= uint(normal.x * MAXIMUM_11_BIT_UINT) << 21;
    packed_normal |= uint(normal.y * MAXIMUM_10_BIT_UINT) << 11;
    packed_normal |= uint(normal.z * MAXIMUM_11_BIT_UINT);
 #endif

    return uintBitsToFloat(packed_normal);
}

/*
*   Unpacks a normal into a vec3.
*/
vec3 UnpackNormal(float normal)
{   
    uint uint_normal = floatBitsToUint(normal);

    //Upack the normal.
    vec3 unpacked_normal;

#if PACK_NORMALS_8_BIT
    unpacked_normal.x = float((uint_normal >> 16) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_UINT;
    unpacked_normal.y = float((uint_normal >> 8) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_UINT;
    unpacked_normal.z = float(uint_normal & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_UINT;
#else
    unpacked_normal.x = float(uint_normal >> 21) / MAXIMUM_11_BIT_UINT;
    unpacked_normal.y = float((uint_normal >> 11) & MAXIMUM_10_BIT_UINT) / MAXIMUM_10_BIT_UINT;
    unpacked_normal.z = float(uint_normal & MAXIMUM_11_BIT_UINT) / MAXIMUM_11_BIT_UINT;
#endif

    //Unnormalize the normal into a [-1.0f, 1.0f] range.
    unpacked_normal.x = unpacked_normal.x * 2.0f - 1.0f;
	unpacked_normal.y = unpacked_normal.y * 2.0f - 1.0f;
	unpacked_normal.z = unpacked_normal.z * 2.0f - 1.0f;

    return normalize(unpacked_normal);
}
#endif