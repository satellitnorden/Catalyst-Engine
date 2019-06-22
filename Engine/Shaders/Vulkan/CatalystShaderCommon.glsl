#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

//Constants.
#define ANTI_ALIASING_MODE_NONE (0)
#define ANTI_ALIASING_MODE_FAST_APPROXIMATE (1)
#define ANTI_ALIASING_MODE_TEMPORAL (2)
#define ANTI_ALIASING_MODE_FAST_APPROXIMATE_AND_TEMPORAL (3)

#define DIFFUSE_IRRADIANCE_MODE_SIMPLE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define SHADOWS_MODE_NONE (0)
#define SHADOWS_MODE_RAY_TRACED (1)

#define VOLUMETRIC_LIGHTING_MODE_NONE (0)
#define VOLUMETRIC_LIGHTING_MODE_RAY_TRACED (1)

#define MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES (512)

#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX (0)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX (1)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX (2)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX (3)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX (4)

#define NUMBER_OF_GLOBAL_SAMPLERS (5)

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

    layout (offset = 528) vec2 scaledResolution;
    layout (offset = 536) vec2 inverseScaledResolution;
    layout (offset = 544) vec2 previousFrameJitter;
    layout (offset = 552) vec2 currentFrameJitter;

    layout (offset = 560) float deltaTime;
    layout (offset = 564) float directionalLightIntensity;
    layout (offset = 568) float globalRandomSeed1;
    layout (offset = 572) float globalRandomSeed2;
    layout (offset = 576) float globalRandomSeed3;
    layout (offset = 580) float perceiverRotationVelocity;
    layout (offset = 584) float totalTime;
    layout (offset = 588) float windSpeed;

    layout (offset = 592) int antiAliasingMode;
    layout (offset = 596) int diffuseIrradianceMode;
    layout (offset = 600) int specularIrradianceMode;
    layout (offset = 604) int shadowsMode;
    layout (offset = 608) int volumetricLightingMode;

    layout (offset = 612) int activeNoiseTextureIndex;
    layout (offset = 616) float activeNoiseTextureOffsetX;
    layout (offset = 620) float activeNoiseTextureOffsetY;

    //Total size; 624
};

//The global textures.
layout (set = 0, binding = 1) uniform texture2D globalTextures[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];

//The global samplers.
layout (set = 0, binding = 2) uniform sampler globalSamplers[NUMBER_OF_GLOBAL_SAMPLERS];

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

/*
*   Upsamples a texture.
*/
vec4 Upsample(sampler2D lowresTexture, vec2 coordinate)
{
    vec4 result = vec4(0.0f);

    result += texture(lowresTexture, coordinate + vec2(-inverseScaledResolution.x, -inverseScaledResolution.y) * 0.5f);
    result += texture(lowresTexture, coordinate + vec2(-inverseScaledResolution.x, inverseScaledResolution.y) * 0.5f);
    result += texture(lowresTexture, coordinate + vec2(inverseScaledResolution.x, -inverseScaledResolution.y) * 0.5f);
    result += texture(lowresTexture, coordinate + vec2(inverseScaledResolution.x, inverseScaledResolution.y) * 0.5f);

    return result * 0.25f;
}

/*
*   Returns if a coordinate is valid.
*/
bool ValidCoordinate(vec2 coordinate)
{
    return  coordinate.x >= 0.0f
            && coordinate.x < 1.0f
            && coordinate.y >= 0.0f
            && coordinate.y < 1.0f;
}

#endif