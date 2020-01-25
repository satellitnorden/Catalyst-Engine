#if !defined(CATALYST_PACKING_UTILITIES)
#define CATALYST_PACKING_UTILITIES

//Constants.
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_15_BIT_FLOAT (32767.0f)
#define MAXIMUM_8_BIT_UINT (255)
#define MAXIMUM_15_BIT_UINT (32767)

/*
*   Unpacks a color into a vec4.
*/
vec4 UnpackColor(int color)
{   
    vec4 unpacked;

    unpacked.r = float(color & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;
    unpacked.g = float((color >> 8) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;
    unpacked.b = float((color >> 16) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;
    unpacked.a = float((color >> 24) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;

    return unpacked;
}

/*
*   Packs a normal into one float.
*/
float PackNormal(vec3 unpacked_normal)
{
    //Normalize the x and z axes.
    unpacked_normal.xz = unpacked_normal.xz * 0.5f + 0.5f;

    //Construct the packed normal.
    uint packed_normal = 0;

    packed_normal |= uint(unpacked_normal.x * MAXIMUM_15_BIT_FLOAT) << 16;
    packed_normal |= uint(unpacked_normal.z * MAXIMUM_15_BIT_FLOAT);

    //Set the sign bit.
    packed_normal = unpacked_normal.y >= 0.0f ? packed_normal : packed_normal | BIT(15);

    return float(packed_normal);
}

/*
*   Unpacks a normal into a vec3.
*/
vec3 UnpackNormal(float packed_normal)
{   
    //Unpack the normal.
    uint packed_normal_uint = uint(packed_normal);

    vec3 normal;

    normal.x = float((packed_normal_uint >> 16) & MAXIMUM_15_BIT_UINT) / MAXIMUM_15_BIT_FLOAT;
    normal.z = float(packed_normal_uint & MAXIMUM_15_BIT_UINT) / MAXIMUM_15_BIT_FLOAT;

    normal.xz = normal.xz * 2.0f - 1.0f;

    normal.y = sqrt(1.0f - dot(normal.xz, normal.xz));

    //Invert the y component depending on the sign bit.
    normal.y = TEST_BIT(packed_normal_uint, BIT(15)) ? -normal.y : normal.y;

    return normal;
}
#endif