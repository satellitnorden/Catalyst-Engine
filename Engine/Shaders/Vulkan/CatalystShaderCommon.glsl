#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

//Constants.
#define MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES (128)

#define EULERS_NUMBER (2.718281f)
#define PHI (1.618033f)
#define PI (3.141592f)
#define DOUBLE_PI (6.283185f)
#define HALF_PI (1.570796f)
#define INVERSE_PI (0.318309f)
#define SQUARE_ROOT_OF_TWO (1.414213f)
#define SQUARE_ROOT_OF_NINETY_NINE (9.949784f)

//Global uniform data.
layout (std140, set = 0, binding = 0) uniform GlobalUniformData
{
    layout (offset = 0) mat4 viewMatrixMinusOne;
    layout (offset = 64) mat4 UNUSED;
    layout (offset = 128) mat4 inversePerceiverMatrix;
    layout (offset = 192) mat4 inverseProjectionMatrix;
    layout (offset = 256) mat4 perceiverMatrix;
    layout (offset = 320) mat4 projectionMatrix;
    layout (offset = 384) mat4 viewMatrix;

    layout (offset = 448) vec3 directionalLightColor;
    layout (offset = 464) vec3 perceiverWorldPositionMinusOne;
    layout (offset = 480) vec3 perceiverForwardVector;
    layout (offset = 496) vec3 perceiverVelocity;
    layout (offset = 512) vec3 perceiverWorldPosition;

    layout (offset = 528) float deltaTime;
    layout (offset = 532) float directionalLightIntensity;
    layout (offset = 536) float globalRandomSeed1;
    layout (offset = 540) float globalRandomSeed2;
    layout (offset = 544) float globalRandomSeed3;
    layout (offset = 548) float perceiverRotationVelocity;
    layout (offset = 552) float totalTime;
    layout (offset = 556) float windSpeed;

    //Total size; 560
};

//The global textures.
layout (set = 0, binding = 1) uniform sampler2D globalTextures[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];

/*
*   Defines the bit at the specified index.
*/
#define BIT(index) (1 << (index))

/*
*   Calculates the average of a fragment.
*/
float CalculateAverage(vec3 fragment)
{
    return fragment.r * 0.2126f + fragment.g * 0.7152f + fragment.b * 0.0722f;
}

/*
*   Hash function taking a lone uint.
*/
uint Hash1(uint x)
{
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);

    return x;
}

/*
*   Hash function taking a vector of uint's.
*/
uint Hash3(uvec3 x)
{
    return Hash1(x.x ^ Hash1(x.y) ^ Hash1(x.z));
}

/*
*   Returns the length of a vector with two components squared.
*/
float LengthSquared2(vec2 vector)
{
    return vector.x * vector.x + vector.y * vector.y;
}

/*
*   Returns the length of a vector with three components squared.
*/
float LengthSquared3(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}


/*
*	Linear interpolation between three values.
*/
float LinearInterpolation(float a, float b, float c, float alpha)
{
	return alpha >= 0.5f ? mix(b, c, (alpha - 0.5f) * 2.0f) : mix(a, b, alpha  * 2.0f);
}

/*
*   Given a coordinate and a seed, returns a random number.
*/
#define RANDOM_FLOAT_IMPLEMENTATION 0

float RandomFloat(vec2 coordinate, float seed)
{

#if RANDOM_FLOAT_IMPLEMENTATION == 0

    return fract(sin(dot(vec2(coordinate.x * (EULERS_NUMBER + seed + 1.0f), coordinate.y * (PHI + seed + 1.0f)), vec2(12.9898f, 78.233f))) * 43758.5453f);

#elif RANDOM_FLOAT_IMPLEMENTATION == 1

    uint h = Hash3(uvec3(floatBitsToUint(coordinate.x), floatBitsToUint(coordinate.y), floatBitsToUint(seed)));

    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    
    float r2 = uintBitsToFloat(h);

    return r2 - 1.0f;
    
#endif
}

/*
*   Scales a value from one range to another.
*/
float Scale(float value, float originalMinimum, float originalMaximum, float newMinimum, float newMaximum)
{
    return (((value - originalMinimum) * (newMaximum - newMinimum)) / (originalMaximum - originalMinimum)) + newMinimum;
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
}

/*
*   Returns a smoothed vector in the range 0.0f-1.0f.
*/
vec3 SmoothStep(vec3 vector)
{
    return vec3(SmoothStep(vector.x), SmoothStep(vector.y), SmoothStep(vector.z));
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmootherStep(float number)
{
    return number * number * number * (number * (number * 6.0f - 15.0f) + 10.0f);
}

/*
*   Returns a smoothed vector in the range 0.0f-1.0f.
*/
vec3 SmootherStep(vec3 vector)
{
    return vec3(SmootherStep(vector.x), SmootherStep(vector.y), SmootherStep(vector.z));
}

#endif