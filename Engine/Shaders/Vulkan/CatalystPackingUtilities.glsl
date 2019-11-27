#if !defined(CATALYST_PACKING_UTILITIES)
#define CATALYST_PACKING_UTILITIES

//Constants.
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_10_BIT_FLOAT (1023.0f)
#define MAXIMUM_11_BIT_FLOAT (2047.0f)
#define MAXIMUM_16_BIT_FLOAT (63535.0f)
#define MAXIMUM_8_BIT_UINT (255)
#define MAXIMUM_10_BIT_UINT (1023)
#define MAXIMUM_11_BIT_UINT (2047)
#define MAXIMUM_16_BIT_UINT (63535)

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
float PackNormal(vec3 normal)
{
    //Calculate the angles.
    vec2 angles = (vec2(atan(normal.y, normal.x) / PI, normal.z) + 1.0f) * 0.5f;

    //Construct the uint.
    uint packed_normal = 0;

    packed_normal |= uint(angles.x * MAXIMUM_16_BIT_FLOAT) << 16;
    packed_normal |= uint(angles.y * MAXIMUM_16_BIT_FLOAT);

    return float(packed_normal);
}

/*
*   Unpacks a normal into a vec3.
*/
vec3 UnpackNormal(float normal)
{   
    //Unpack the uint normal.
    uint uint_normal = uint(normal);

    //Calculate the encoding.
    vec2 encoding = vec2(float((uint_normal >> 16) & MAXIMUM_16_BIT_UINT) / MAXIMUM_16_BIT_FLOAT, float(uint_normal & MAXIMUM_16_BIT_UINT) / MAXIMUM_16_BIT_FLOAT);
    encoding = encoding * 2.0f - 1.0f;

    //Unpack the final normal.
    vec2 scth = vec2(sin(encoding.x * PI), cos(encoding.x * PI));
    vec2 scphi = vec2(sqrt(1.0 - encoding.y * encoding.y), encoding.y);

    return vec3(scth.y * scphi.x, scth.x * scphi.x, scphi.y);
}
#endif