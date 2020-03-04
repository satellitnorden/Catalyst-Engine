#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

//Includes.
#define CATALYST_SHADER_LANGUAGE_GLSL
#include "..\..\Include\Rendering\Native\Shader\CatalystShaderConstants.h"

#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"

//Constants.
#define AMBIENT_OCCLUSION_POWER (2.0f) //0.25f step.

#define AMBIENT_OCCLUSION_MODE_NONE (0)
#define AMBIENT_OCCLUSION_MODE_SCREEN_SPACE (1)

#define MOTION_BLUR_MODE_NONE (0)
#define MOTION_BLUR_MODE_HALF (1)
#define MOTION_BLUR_MODE_FULL (2)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define SHADOWS_MODE_NONE (0)
#define SHADOWS_MODE_RAY_TRACED (1)

#define VOLUMETRIC_LIGHTING_MODE_NONE (0)
#define VOLUMETRIC_LIGHTING_MODE_RAY_TRACED (1)

#define RENDER_TARGET_SCENE_FEATURES_1_INDEX                        (0)
#define RENDER_TARGET_SCENE_FEATURES_2_INDEX                        (1)
#define RENDER_TARGET_SCENE_FEATURES_3_INDEX                        (2)
#define RENDER_TARGET_AMBIENT_OCCLUSION_INDEX                       (3)
#define RENDER_TARGET_SCENE_INDEX                                   (4)
#define RENDER_TARGET_TEMPORAL_ANTI_ALIASING_BUFFER_1_INDEX         (5)
#define RENDER_TARGET_TEMPORAL_ANTI_ALIASING_BUFFER_2_INDEX         (6)
#define RENDER_TARGET_INTERMEDIATE_R32G32B32A32_FLOAT_1_INDEX       (7)
#define RENDER_TARGET_INTERMEDIATE_R32G32B32A32_FLOAT_2_INDEX       (8)
#define RENDER_TARGET_INTERMEDIATE_R8_BYTE_INDEX                    (9)
#define RENDER_TARGET_INTERMEDIATE_HALF_R32G32B32A32_FLOAT_1_INDEX  (10)
#define RENDER_TARGET_INTERMEDIATE_HALF_R32G32B32A32_FLOAT_2_INDEX  (11)
#define RENDER_TARGET_INTERMEDIATE_QUARTER_R32G32B32A32_FLOAT_INDEX (12)
#define RENDER_TARGET_INTERMEDIATE_EIGHTH_R32G32B32A32_FLOAT_INDEX  (13)
#define RENDER_TARGET_SCREEN_INDEX                                  (14)
#define RENDER_TARGET_NUMBER_OF_RENDER_TARGETS                      (15)

#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX    (0)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX           (1)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX   (2)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX          (3)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX  (4)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX         (5)
#define GLOBAL_SAMPLER_NUMBER_OF_GLOBAL_SAMPLERS                                            (6)

#define FLOAT_MAXIMUM (3.402823466e+38F)

#define EULERS_NUMBER (2.718281f)
#define PHI (1.618033f)
#define PI (3.141592f)
#define DOUBLE_PI (6.283185f)
#define HALF_PI (1.570796f)
#define INVERSE_PI (0.318309f)
#define SQUARE_ROOT_OF_TWO (1.414213f)
#define SQUARE_ROOT_OF_NINETY_NINE (9.949784f)

#define LIGHT_TYPE_DIRECTIONAL (0)
#define LIGHT_TYPE_POINT (1)

/*
*	Material struct definition.
*/
struct Material
{
	int albedo_texture_index;
	int normal_map_texture_index;
	int material_properties_texture_index;
	int optional_texture_index;
	int type;
	int properties;
	float luminance_multiplier;
	float thickness;
};

//Global uniform data.
layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    layout (offset = 0) mat4 viewMatrixMinusOne;
    layout (offset = 64) mat4 UNUSED_1;
    layout (offset = 128) mat4 inversePerceiverMatrix;
    layout (offset = 192) mat4 inverseProjectionMatrix;
    layout (offset = 256) mat4 perceiverMatrix;
    layout (offset = 320) mat4 projectionMatrix;
    layout (offset = 384) mat4 viewMatrix;

    layout (offset = 448) vec3 UNUSED_X;
    layout (offset = 464) vec3 UNUSED_Y;
    layout (offset = 480) vec3 UNUSED_6;
    layout (offset = 496) vec3 perceiverForwardVector;
    layout (offset = 512) vec3 PERCEIVER_WORLD_POSITION;

    layout (offset = 528) vec2 scaledResolution;
    layout (offset = 536) vec2 inverseScaledResolution;
    layout (offset = 544) vec2 previousFrameJitter;
    layout (offset = 552) vec2 currentFrameJitter;

    layout (offset = 560) float DELTA_TIME;
    layout (offset = 564) float UNUSED_Z;
    layout (offset = 568) float globalRandomSeed1;
    layout (offset = 572) float globalRandomSeed2;
    layout (offset = 576) float globalRandomSeed3;
    layout (offset = 580) float UNUSED_5;
    layout (offset = 584) float totalTime;
    layout (offset = 588) float windSpeed;

    layout (offset = 592) int ambientOcclusionMode;
    layout (offset = 596) int motionBlurMode;
    layout (offset = 600) int UNUSED_3;
    layout (offset = 604) int shadowsMode;
    layout (offset = 608) int UNUSED_4;

    layout (offset = 612) float UNUSED_2;
    layout (offset = 616) float bloomIntensity;
    layout (offset = 620) float chromaticAberrationIntensity;
    layout (offset = 624) float UNUSED_W;

    layout (offset = 628) float depthOfFieldFocusDistance;

    layout (offset = 632) int activeNoiseTextureIndex;
    layout (offset = 636) float activeNoiseTextureOffsetX;
    layout (offset = 640) float activeNoiseTextureOffsetY;

    layout (offset = 644) float VIEW_DISTANCE;
    layout (offset = 648) float CLOUD_DENSITY;
    layout (offset = 652) float WETNESS;
    layout (offset = 656) float NEAR_PLANE;
    layout (offset = 660) float FAR_PLANE;

    //Total size; 664
};

//The global textures.
layout (set = 0, binding = 1) uniform texture2D GLOBAL_TEXTURES[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];

//The global samplers.
layout (set = 0, binding = 2) uniform sampler GLOBAL_SAMPLERS[GLOBAL_SAMPLER_NUMBER_OF_GLOBAL_SAMPLERS];

//Global materials.
layout (std140, set = 0, binding = 3) uniform GlobalMaterials
{
    layout (offset = 0) Material GLOBAL_MATERIALS[MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS];
};

//The cloud texture.
layout (set = 0, binding = 4) uniform sampler3D CLOUD_TEXTURE;

//The sky textures.
layout (set = 0, binding = 5, rgba32f) uniform imageCube SKY_IMAGES[NUMBER_OF_SKY_TEXTURES];

//The sky textures.
layout (set = 0, binding = 6) uniform samplerCube SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES];

//Hammersley hemisphere uniform buffer
layout (std140, set = 0, binding = 7) uniform HammersleyHemisphereSamples
{
    layout (offset = 0) vec4 HAMMERSLEY_HEMISPHERE_SAMPLES[64];
};

/*
*   Defines the bit at the specified index.
*/
#define BIT(INDEX) (1 << (INDEX))

/*
*   Tests the bit of the specified bitfield
*/
#define TEST_BIT(BITFIELD, BIT) ((BITFIELD & BIT) == BIT)

/*
*   Calculates the average of a fragment.
*/
float CalculateAverage(vec3 fragment)
{
    return fragment.r * 0.2126f + fragment.g * 0.7152f + fragment.b * 0.0722f;
}

/*
*   Calculates a Gram-Schmidt rotation matrix based on a normal and a random tilt.
*/
mat3 CalculateGramSchmidtRotationMatrix(vec3 normal, vec3 random_tilt)
{
    vec3 random_tangent = normalize(random_tilt - normal * dot(random_tilt, normal));
    vec3 random_bitangent = cross(normal, random_tangent);

    return mat3(random_tangent, random_bitangent, normal);
}

/*
*   Calculates the view space position.
*/
vec3 CalculateViewSpacePosition(vec2 texture_coordinate, float depth)
{
    vec2 near_plane_coordinate = texture_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = inverseProjectionMatrix * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position *= inverse_view_space_position_denominator;

    return view_space_position.xyz;
}

/*
*   Calculates the world position.
*/
vec3 CalculateWorldPosition(vec2 texture_coordinate, float depth)
{
    vec2 near_plane_coordinate = texture_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = inverseProjectionMatrix * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position *= inverse_view_space_position_denominator;
    vec4 world_space_position = inversePerceiverMatrix * view_space_position;

    return world_space_position.xyz;
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
*   Linearizes a depth value.
*/
float LinearizeDepth(float depth)
{
    return NEAR_PLANE * FAR_PLANE / (FAR_PLANE + depth * (NEAR_PLANE - FAR_PLANE));
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
*   Samples a Hammersley hemisphere sample. 
*/
void SampleHammersleyHemisphereSample(uint index, out vec3 direction, out float length)
{
    vec4 hemisphere_sample = HAMMERSLEY_HEMISPHERE_SAMPLES[index & 63];

    direction = hemisphere_sample.xyz;
    length = hemisphere_sample.w;
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
*   Given a number, returns the square of that number.
*/
 float Square(float number)
{
    return number * number;
}

/*
*   Upsamples a texture.
*/
vec4 Upsample(sampler2D lowresTexture, vec2 coordinate)
{
    vec4 result = vec4(0.0f);

    result += texture(lowresTexture, coordinate + vec2(-inverseScaledResolution.x, -inverseScaledResolution.y));
    result += texture(lowresTexture, coordinate + vec2(-inverseScaledResolution.x, inverseScaledResolution.y));
    result += texture(lowresTexture, coordinate + vec2(inverseScaledResolution.x, -inverseScaledResolution.y));
    result += texture(lowresTexture, coordinate + vec2(inverseScaledResolution.x, inverseScaledResolution.y));

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