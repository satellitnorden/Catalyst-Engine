#if !defined(CATALYST_PACKING_UTILITIES)
#define CATALYST_PACKING_UTILITIES

//Constants.
#define MAXIMUM_10_BIT_FLOAT (1023.0f)
#define MAXIMUM_11_BIT_FLOAT (2047.0f)
#define MAXIMUM_16_BIT_FLOAT (63535.0f)
#define MAXIMUM_10_BIT_UINT (1023)
#define MAXIMUM_11_BIT_UINT (2047)
#define MAXIMUM_16_BIT_UINT (63535)

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

    /*
    //Normalize the normal into a [0.0f, 1.0f] range.
	normal.x = normal.x * 0.5f + 0.5f;
	normal.y = normal.y * 0.5f + 0.5f;
	normal.z = normal.z * 0.5f + 0.5f;

    //Construct the uint.
    uint packed_normal = 0;

    packed_normal |= uint(normal.x * MAXIMUM_11_BIT_FLOAT) << 21;
    packed_normal |= uint(normal.y * MAXIMUM_10_BIT_FLOAT) << 11;
    packed_normal |= uint(normal.z * MAXIMUM_11_BIT_FLOAT);

    return float(packed_normal);
    */
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

    /*
    uint uint_normal = uint(normal);

    //Upack the normal.
    vec3 unpacked_normal;

    unpacked_normal.x = float((uint_normal >> 21) & MAXIMUM_11_BIT_UINT) / MAXIMUM_11_BIT_FLOAT;
    unpacked_normal.y = float((uint_normal >> 11) & MAXIMUM_10_BIT_UINT) / MAXIMUM_10_BIT_FLOAT;
    unpacked_normal.z = float(uint_normal & MAXIMUM_11_BIT_UINT) / MAXIMUM_11_BIT_FLOAT;

    //Unnormalize the normal into a [-1.0f, 1.0f] range.
    unpacked_normal.x = unpacked_normal.x * 2.0f - 1.0f;
	unpacked_normal.y = unpacked_normal.y * 2.0f - 1.0f;
	unpacked_normal.z = unpacked_normal.z * 2.0f - 1.0f;

    return normalize(unpacked_normal);
    */
}

/*
*   Packs a velocity into one float.
*/
float PackVelocity(vec2 velocity)
{
    //Normalize the velocity into a [0.0f, 1.0f] range.
    velocity.x = clamp(velocity.x, -1.0f, 1.0f) * 0.5f + 0.5f;
    velocity.y = clamp(velocity.y, -1.0f, 1.0f) * 0.5f + 0.5f;

    //Construct the uint.
    uint packed_velocity = 0;

    packed_velocity |= uint(velocity.x * MAXIMUM_16_BIT_FLOAT) << 16;
    packed_velocity |= uint(velocity.y * MAXIMUM_16_BIT_FLOAT);

    return float(packed_velocity);
}

/*
*   Unpacks a velocity into a vec2.
*/
vec2 UnpackVelocity(float velocity)
{   
    uint uint_velocity = uint(velocity);

    //Upack the velocity.
    vec2 unpacked_velocity;

    unpacked_velocity.x = float((uint_velocity >> 16) & MAXIMUM_16_BIT_UINT) / MAXIMUM_16_BIT_FLOAT;
    unpacked_velocity.y = float(uint_velocity & MAXIMUM_16_BIT_UINT) / MAXIMUM_16_BIT_FLOAT;

    //Unnormalize the velocity into a [-1.0f, 1.0f] range.
    unpacked_velocity.x = unpacked_velocity.x * 2.0f - 1.0f;
    unpacked_velocity.y = unpacked_velocity.y * 2.0f - 1.0f;

    return unpacked_velocity;
}
#endif