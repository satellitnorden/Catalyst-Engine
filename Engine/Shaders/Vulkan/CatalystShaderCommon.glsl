#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

//Constants.
#define MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES (16)

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
    //Viewer data.
    layout (offset = 0) mat4 directionalLightViewMatrix; //Offset; 0 - Size; 64
    layout (offset = 64) mat4 inversePerceiverMatrix; //Offset; 64 - Size; 64
    layout (offset = 128) mat4 inverseProjectionMatrix; //Offset; 128 - Size; 64
    layout (offset = 192) mat4 perceiverMatrix; //Offset; 192 - Size; 64
    layout (offset = 256) mat4 projectionMatrix; //Offset; 256 - Size; 64
    layout (offset = 320) mat4 viewMatrix; //Offset; 320 - Size; 64

    layout (offset = 384) vec3 directionalLightColor; //Offset; 384 - Size; 16
    layout (offset = 400) vec3 directionalLightDirection; //Offset; 400 - Size; 16
    layout (offset = 416) vec3 perceiverWorldPosition; //Offset; 416 - Size; 16
    layout (offset = 432) vec3 windDirection; //Offset; 432 - Size; 16

    layout (offset = 448) float deltaTime; //Offset; 448 - Size; 4
    layout (offset = 452) float directionalLightIntensity; //Offset; 452 - Size; 4
    layout (offset = 456) float environmentBlend; //Offset; 456 - Size; 4
    layout (offset = 460) float globalRandomSeed1; //Offset; 460 - Size; 4
    layout (offset = 464) float globalRandomSeed2; //Offset; 464 - Size; 4
    layout (offset = 468) float globalRandomSeed3; //Offset; 468 - Size; 4
    layout (offset = 472) float totalTime; //Offset; 472 - Size; 4
    layout (offset = 476) float windSpeed; //Offset; 476 - Size; 4

    //Total size; 480
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
*   Calculates a fragment's view space position.
*/
vec3 CalculateFragmentViewSpacePosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(vec3(nearPlaneCoordinate, depth), 1.0f);
    float inverseViewSpacePositionDenominator = 1.0f / viewSpacePosition.w;
    viewSpacePosition *= inverseViewSpacePositionDenominator;

    return viewSpacePosition.xyz;
}

/*
*   Calculates a fragment's world position.
*/
vec3 CalculateFragmentWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(vec3(nearPlaneCoordinate, depth), 1.0f);
    float inverseViewSpacePositionDenominator = 1.0f / viewSpacePosition.w;
    viewSpacePosition *= inverseViewSpacePositionDenominator;
    vec4 worldSpacePosition = inversePerceiverMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
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
#define RANDOM_FLOAT_IMPLEMENTATION 1

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