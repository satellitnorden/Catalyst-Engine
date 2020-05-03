#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

//Includes.
#define CATALYST_SHADER_LANGUAGE_GLSL
#include "..\Include\Rendering\Native\Shader\CatalystShaderConstants.h"

//Constants.
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX    (0)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX           (1)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX   (2)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX          (3)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX  (4)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX         (5)
#define GLOBAL_SAMPLER_NUMBER_OF_GLOBAL_SAMPLERS                                            (6)

#define FLOAT_MAXIMUM (3.402823466e+38F)
#define UINT32_MAXIMUM (0xffffffff)
#define UINT32_MAXIMUM_RECIPROCAL (2.328306437080797e-10f)

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
	uint albedo_texture_index;
	uint normal_map_texture_index;
	uint material_properties_texture_index;
	uint optional_texture_index;
	uint type;
	uint properties;
	float luminance_multiplier;
	float thickness;
};

//Global uniform data.
layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    layout (offset = 0) mat4 INVERSE_PERCEIVER_TO_CLIP_MATRIX;
    layout (offset = 64) mat4 INVERSE_WORLD_TO_PERCEIVER_MATRIX;
    layout (offset = 128) mat4 PREVIOUS_WORLD_TO_CLIP_MATRIX;
    layout (offset = 192) mat4 WORLD_TO_CLIP_MATRIX;
    layout (offset = 256) mat4 WORLD_TO_PERCEIVER_MATRIX;

    layout (offset = 320) vec3 PERCEIVER_FORWARD_VECTOR;
    layout (offset = 336) vec3 PERCEIVER_WORLD_POSITION;

    layout (offset = 352) vec2 INVERSE_SCALED_RESOLUTION;
    layout (offset = 360) vec2 previousFrameJitter;
    layout (offset = 368) vec2 currentFrameJitter;

    layout (offset = 376) float DELTA_TIME;
    layout (offset = 380) float globalRandomSeed1;
    layout (offset = 384) float globalRandomSeed2;
    layout (offset = 388) float globalRandomSeed3;
    layout (offset = 392) float totalTime;
    layout (offset = 396) float windSpeed;

    layout (offset = 400) float bloomIntensity;

    layout (offset = 404) float ASPECT_RATIO;

    layout (offset = 408) uint CURRENT_BLUE_NOISE_TEXTURE_INDEX;
    layout (offset = 412) float CURRENT_BLUE_NOISE_TEXTURE_OFFSET_X;
    layout (offset = 416) float CURRENT_BLUE_NOISE_TEXTURE_OFFSET_Y;

    layout (offset = 420) float VIEW_DISTANCE;
    layout (offset = 424) float CLOUD_DENSITY;
    layout (offset = 428) float WETNESS;
    layout (offset = 432) float NEAR_PLANE;
    layout (offset = 436) float FAR_PLANE;

    layout (offset = 440) uint TERRAIN_HEIGHT_MAP_TEXTURE_INDEX;
    layout (offset = 444) uint TERRAIN_INDEX_MAP_TEXTURE_INDEX;
    layout (offset = 448) uint TERRAIN_BLEND_MAP_TEXTURE_INDEX;
    layout (offset = 452) float TERRAIN_MAP_RESOLUTION;

    //Total size; 456
};

//The render targets.
layout (set = 0, binding = 1) uniform texture2D RENDER_TARGETS[NUMBER_OF_RENDER_TARGETS];

//The global textures.
layout (set = 0, binding = 2) uniform texture2D GLOBAL_TEXTURES[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];

//The global samplers.
layout (set = 0, binding = 3) uniform sampler GLOBAL_SAMPLERS[GLOBAL_SAMPLER_NUMBER_OF_GLOBAL_SAMPLERS];

//Global materials.
layout (std140, set = 0, binding = 4) uniform GlobalMaterials
{
    layout (offset = 0) Material GLOBAL_MATERIALS[MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS];
};

//The blue noise textures.
layout (set = 0, binding = 5) uniform sampler2D BLUE_NOISE_TEXTURES[NUMBER_OF_BLUE_NOISE_TEXTURES];

//The cloud texture.
layout (set = 0, binding = 6) uniform sampler3D CLOUD_TEXTURE;

//The sky textures.
layout (set = 0, binding = 7, rgba32f) uniform imageCube SKY_IMAGES[NUMBER_OF_SKY_TEXTURES];

//The sky textures.
layout (set = 0, binding = 8) uniform samplerCube SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES];

//Hammersley hemisphere uniform buffer
layout (std140, set = 0, binding = 9) uniform HammersleyHemisphereSamples
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
    vec4 view_space_position = INVERSE_PERCEIVER_TO_CLIP_MATRIX * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position.xyz *= inverse_view_space_position_denominator;

    return view_space_position.xyz;
}

/*
*   Calculates the world position.
*/
vec3 CalculateWorldPosition(vec2 texture_coordinate, float depth)
{
    vec2 near_plane_coordinate = texture_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = INVERSE_PERCEIVER_TO_CLIP_MATRIX * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position *= inverse_view_space_position_denominator;
    vec4 world_space_position = INVERSE_WORLD_TO_PERCEIVER_MATRIX * view_space_position;

    return world_space_position.xyz;
}

/*
*   Hash function taking a uint.
*/
uint Hash1(uint seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);

    return seed;
}

/*
*   Hash function taking a uvec2.
*/
uint Hash2(uvec2 seed)
{
    return Hash1(seed.x) ^ Hash1(seed.y);
}

/*
*   Hash function taking a uvec3.
*/
uint Hash3(uvec3 seed)
{
    return Hash1(seed.x) ^ Hash1(seed.y) ^ Hash1(seed.z);
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
float RandomFloat(vec2 coordinate, float seed)
{
	uint hash = Hash3(uvec3(floatBitsToUint(coordinate.x), floatBitsToUint(coordinate.y), floatBitsToUint(coordinate.x + coordinate.y + seed)));
    
    return float(hash) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Samples the current blue noise texture at the given coordinate and index.
*/
vec4 SampleBlueNoiseTexture(uvec2 coordinate, uint index)
{
    return texture(BLUE_NOISE_TEXTURES[(CURRENT_BLUE_NOISE_TEXTURE_INDEX + index) & (NUMBER_OF_BLUE_NOISE_TEXTURES - 1)], vec2(coordinate) / float(BLUE_NOISE_TEXTURE_RESOLUTION) + vec2(CURRENT_BLUE_NOISE_TEXTURE_OFFSET_X, CURRENT_BLUE_NOISE_TEXTURE_OFFSET_Y));
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
*   Samples the sky diffuse.
*/
vec3 SampleSkyDiffuse(vec3 normal)
{
    return texture(SKY_TEXTURES[NUMBER_OF_SKY_TEXTURES - 1], normal).rgb;
}

/*
*   Samples the sky specular.
*/
vec3 SampleSkySpecular(vec3 view_direction, vec3 normal, float roughness, float metallic)
{
    //Calculate the reflection vector.
    vec3 reflection_vector = reflect(view_direction, normal);

    //Calculate the indices for the sky textures.
    float float_index = roughness * (1.0f - metallic) * float(NUMBER_OF_SKY_TEXTURES - 1);

    uint first_index = uint(float_index);
    uint second_index = first_index + 1;

    float alpha = fract(float_index);

    return mix(texture(SKY_TEXTURES[first_index], reflection_vector).rgb, texture(SKY_TEXTURES[second_index], reflection_vector).rgb, alpha);
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

    result += texture(lowresTexture, coordinate + vec2(-INVERSE_SCALED_RESOLUTION.x, -INVERSE_SCALED_RESOLUTION.y));
    result += texture(lowresTexture, coordinate + vec2(-INVERSE_SCALED_RESOLUTION.x, INVERSE_SCALED_RESOLUTION.y));
    result += texture(lowresTexture, coordinate + vec2(INVERSE_SCALED_RESOLUTION.x, -INVERSE_SCALED_RESOLUTION.y));
    result += texture(lowresTexture, coordinate + vec2(INVERSE_SCALED_RESOLUTION.x, INVERSE_SCALED_RESOLUTION.y));

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