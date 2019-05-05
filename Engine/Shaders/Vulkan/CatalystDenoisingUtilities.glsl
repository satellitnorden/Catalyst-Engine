#if !defined(CATALYST_DENOISING_UTILITIES)
#define CATALYST_DENOISING_UTILITIES

//Constants.
#define MAXIMUM_VISIBILITY_TERM (65535) //2 ^ 16 - 1

/*
*	Packs the visibility term.
*/
float PackVisibilityTerm(float original, float intermediate)
{
	return uintBitsToFloat((uint(original * MAXIMUM_VISIBILITY_TERM) << 16) | (uint(intermediate * MAXIMUM_VISIBILITY_TERM) & MAXIMUM_VISIBILITY_TERM));
}

/*
*	Unpacks the visibility term.
*/
float UnpackVisibilityTerm(float packed, int index)
{
	return index == 0 ? (float(floatBitsToUint(packed) >> 16) / MAXIMUM_VISIBILITY_TERM) : (float(floatBitsToUint(packed) & MAXIMUM_VISIBILITY_TERM) / MAXIMUM_VISIBILITY_TERM);
}
#endif