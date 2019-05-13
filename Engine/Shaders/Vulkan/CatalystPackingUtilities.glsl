#if !defined(CATALYST_PACKING_UTILITIES)
#define CATALYST_PACKING_UTILITIES

/*
*	Packs a normal.
*/
float PackNormal(vec3 normal)
{
	normal = (normal + 1.0f) * 0.5f;

	return uintBitsToFloat((uint(normal.x * 2047.0f) << 21) | (uint(normal.y * 2047.0f) << 10) | uint(normal.z * 1023.0f));
}

/*
*	Unpacks a normal.
*/
vec3 UnpackNormal(float normal)
{
	uint normalUint = floatBitsToUint(normal);

	return normalize(vec3(	float((normalUint >> 21) & 2047) / 2047.0f,
							float((normalUint >> 10) & 2047) / 2047.0f,
							float(normalUint & 1023) / 1023.0f) * 2.0f - 1.0f);
}
#endif