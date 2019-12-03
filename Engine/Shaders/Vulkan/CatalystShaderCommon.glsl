#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystGeometryMath.glsl"

//Constants.
#define AMBIENT_OCCLUSION_POWER (2.75f) //0.25f step.

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

#define MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES (1024)
#define MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS (255)

#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX (0)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX (1)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX (2)
#define GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX (3)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX (4)
#define GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX (5)

#define NUMBER_OF_GLOBAL_SAMPLERS (6)

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
layout (std140, set = 0, binding = 0) uniform GlobalUniformData
{
    layout (offset = 0) mat4 viewMatrixMinusOne;
    layout (offset = 64) mat4 UNUSED_1;
    layout (offset = 128) mat4 inversePerceiverMatrix;
    layout (offset = 192) mat4 inverseProjectionMatrix;
    layout (offset = 256) mat4 perceiverMatrix;
    layout (offset = 320) mat4 projectionMatrix;
    layout (offset = 384) mat4 viewMatrix;

    layout (offset = 448) vec3 upper_sky_color;
    layout (offset = 464) vec3 lower_sky_color;
    layout (offset = 480) vec3 perceiverWorldPositionMinusOne;
    layout (offset = 496) vec3 perceiverForwardVector;
    layout (offset = 512) vec3 perceiverWorldPosition;

    layout (offset = 528) vec2 scaledResolution;
    layout (offset = 536) vec2 inverseScaledResolution;
    layout (offset = 544) vec2 previousFrameJitter;
    layout (offset = 552) vec2 currentFrameJitter;

    layout (offset = 560) float DELTA_TIME;
    layout (offset = 564) float star_strength;
    layout (offset = 568) float globalRandomSeed1;
    layout (offset = 572) float globalRandomSeed2;
    layout (offset = 576) float globalRandomSeed3;
    layout (offset = 580) float perceiverRotationVelocity;
    layout (offset = 584) float totalTime;
    layout (offset = 588) float windSpeed;

    layout (offset = 592) int ambientOcclusionMode;
    layout (offset = 596) int motionBlurMode;
    layout (offset = 600) int specularIrradianceMode;
    layout (offset = 604) int shadowsMode;
    layout (offset = 608) int volumetricLightingMode;

    layout (offset = 612) float UNUSED_2;
    layout (offset = 616) float bloomIntensity;
    layout (offset = 620) float chromaticAberrationIntensity;
    layout (offset = 624) float volumetricLightingIntensity;

    layout (offset = 628) float depthOfFieldFocusDistance;

    layout (offset = 632) int activeNoiseTextureIndex;
    layout (offset = 636) float activeNoiseTextureOffsetX;
    layout (offset = 640) float activeNoiseTextureOffsetY;

    layout (offset = 644) float viewDistance;
    layout (offset = 648) float CLOUD_DENSITY;
    layout (offset = 652) float WETNESS;

    //Total size; 656
};

//The global textures.
layout (set = 0, binding = 1) uniform texture2D globalTextures[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];

//The global samplers.
layout (set = 0, binding = 2) uniform sampler globalSamplers[NUMBER_OF_GLOBAL_SAMPLERS];

//Global materials.
layout (std140, set = 0, binding = 3) uniform GlobalMaterials
{
    layout (offset = 0) Material GLOBAL_MATERIALS[MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS];
};

//The cloud texture.
layout (set = 0, binding = 4) uniform sampler3D CLOUD_TEXTURE;

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
*	Returns the sky color in the given direction.
*/
vec3 SkyColor(vec3 direction, bool include_stars)
{
    //Calculate the sky color.
    vec3 sky_color = mix(lower_sky_color, upper_sky_color, max(dot(direction, vec3(0.0f, 1.0f, 0.0f)), 0.0f));

    //Calculate the stars.
    float star_weight = 0.0f;

    if (include_stars)
    {
        if (star_strength > 0.0f)
        {
            /*
            *   Imagine that the ray hits at an imaginary sphere, some distance away.
            *   Then assign that hit position to a cell in 3D space.
            *   Generate a random number using that cell.
            *   And that's the star value. (:
            */
            vec3 hit_position = direction * 1024.0f;

            hit_position.x = float(int(hit_position.x)) / 1024.0f;
            hit_position.y = float(int(hit_position.y)) / 1024.0f;
            hit_position.z = float(int(hit_position.z)) / 1024.0f;

            float star_value = RandomFloat(hit_position.xy, hit_position.z);

            star_weight = float(star_value >= 0.9975f) * star_strength;
        }

        return mix(sky_color, vec3(1.0f), star_weight);
    }

	else
    {
        return sky_color + vec3(star_strength * 0.025f);
    }
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

/*
*   Calculates the ambient illumination intensity.
*/
float CalculateAmbientIlluminationIntensity()
{
    return mix(CalculateAverage(SkyColor(vec3(1.0f, 0.0f, 0.0f), false)), CalculateAverage(SkyColor(vec3(0.0f, 1.0f, 0.0f), false)), 0.5f);
}

/**************/
/*   CLOUDS   */
/**************/

//Cloud constants.
#define CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER (10000.0f)
#define CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER (20000.0f)
#define CLOUD_NUMBER_OF_SAMPLES (16) //Needs to be a multiple of 4.
#define CLOUD_NUMBER_OF_NOISE_TEXTURES (CLOUD_NUMBER_OF_SAMPLES / 4)
#define CLOUD_POSITION_SCALE (0.00001f)
#define CLOUD_PERSISTENCE (0.5f) //0.025f step.
#define CLOUD_LACUNARITY (4.0f) //0.25f step.
#define CLOUD_BASE_COLOR (vec3(0.8f, 0.9f, 1.0f))
#define CLOUD_DENSITY_MULTIPLIER (4.0f) //0.25f step.

/*
*   Calculates the cloud density multiplier for the given hit distance.
*/
float CalculateCloudDensityMultipluer(float hit_distance)
{
    return (1.0f - clamp((hit_distance - 100000.0f) * 0.00001f, 0.0f, 1.0f));
}

/*
*  Samples the cloud density at the given point.
*/
float SampleCloudDensity(vec3 point, int level)
{
    vec3 cloud_offset = -vec3(totalTime, totalTime * 0.1f, totalTime) * 128.0f;

    vec3 sample_point;
    float density_sample;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float total = 0.0f;
    float density = 0.0f;

    sample_point = ((point + (cloud_offset * SQUARE_ROOT_OF_TWO)) * frequency * SQUARE_ROOT_OF_TWO) * CLOUD_POSITION_SCALE;
    density_sample = texture(CLOUD_TEXTURE, sample_point).x;
    density += density_sample * amplitude;
    total += amplitude;
    amplitude *= CLOUD_PERSISTENCE;
    frequency *= CLOUD_LACUNARITY;

    if (level >= 1)
    {
        sample_point = ((point + (cloud_offset * HALF_PI)) * frequency * HALF_PI) * CLOUD_POSITION_SCALE;
        density_sample = texture(CLOUD_TEXTURE, sample_point).y;
        density += density_sample * amplitude;
        total += amplitude;
        amplitude *= CLOUD_PERSISTENCE;
        frequency *= CLOUD_LACUNARITY;
    }

    if (level >= 2)
    {
        sample_point = ((point + (cloud_offset * PHI)) * frequency * PHI) * CLOUD_POSITION_SCALE;
        density_sample = texture(CLOUD_TEXTURE, sample_point).z;
        density += density_sample * amplitude;
        total += amplitude;
        amplitude *= CLOUD_PERSISTENCE;
        frequency *= CLOUD_LACUNARITY;
    }

    if (level >= 3)
    {
        sample_point = ((point + (cloud_offset * EULERS_NUMBER)) * frequency * EULERS_NUMBER) * CLOUD_POSITION_SCALE;
        density_sample = texture(CLOUD_TEXTURE, sample_point).w;
        density += density_sample * amplitude;
        total += amplitude;
        amplitude *= CLOUD_PERSISTENCE;
        frequency *= CLOUD_LACUNARITY;
    }

    density /= total;

    density = max(density - (1.0f - CLOUD_DENSITY), 0.0f) * CLOUD_DENSITY_MULTIPLIER;

    return density;
}

/*
*  Returns the cloud density in the given direction.
*/
float SampleCloudDensityInDirection(vec3 point, vec3 direction, int level)
{
    //Calculate the start and end points.
    vec3 start;
    vec3 end;
    float cloud_density_multiplier;

    //Calculate the direction angle.
    float direction_angle = dot(direction, vec3(0.0f, 1.0f, 0.0f));

    //Is the direction pointing up?
    if (direction_angle > 0.0f)
    {
        //Is the point below the cloud plane?
        if (point.y < perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER)
        {
            float intersection_distance;

            LinePlaneIntersection(point, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
            start = point + direction * intersection_distance;

            cloud_density_multiplier = CalculateCloudDensityMultipluer(intersection_distance);

            LinePlaneIntersection(point, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
            end = point + direction * intersection_distance;
        }

        //Is the point above the cloud plane?
        else if (point.y > perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER)
        {
            //If the point is above the cloud plane and pointing up, then there's no density here. (:
            return 0.0f;
        }

        //Is the point contained inside the cloud plane?
        else
        {
            start = point;

            float intersection_distance;

            LinePlaneIntersection(point, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, -1.0f, 0.0f), intersection_distance);
            end = point + direction * intersection_distance;

            cloud_density_multiplier = CalculateCloudDensityMultipluer(intersection_distance);
        }
    }

    //Is the direction pointing down?
   else if (direction_angle < 0.0f)
   {
        //Is the point below the cloud plane?
        if (point.y < perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER)
        {
            //If the point is bloew the cloud plane and pointing down, then there's no density here. (:
            return 0.0f;
        }

        //Is the point above the cloud plane?
        else if (point.y > perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER)
        {
            float intersection_distance;

            LinePlaneIntersection(point, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_END_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, 1.0f, 0.0f), intersection_distance);
            start = point + direction * intersection_distance;

            cloud_density_multiplier = CalculateCloudDensityMultipluer(intersection_distance);

            LinePlaneIntersection(point, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, 1.0f, 0.0f), intersection_distance);
            end = point + direction * intersection_distance;
        }

        //Is the point contained inside the cloud plane?
        else
        {
            start = point;

            float intersection_distance;

            LinePlaneIntersection(point, direction, vec3(0.0f, perceiverWorldPosition.y + CLOUD_PLANE_START_HEIGHT_OVER_PERCEIVER, 0.0f), vec3(0.0f, 1.0f, 0.0f), intersection_distance);
            end = point + direction * intersection_distance;

            cloud_density_multiplier = CalculateCloudDensityMultipluer(intersection_distance);
        }
   }

   //Is the direction pointing straight forward?
   else
   {
        start = point;
        end = start + direction * 1000.0f;
   }

   //Calculate the density.
   float density = 0.0f;

   for (int i = 0; i < 4; ++i)
   {
      //Calculate the sample point.
      vec3 sample_point = mix(start, end, float(i) * 0.333333f);

      //Get the ensity at this point.
      density += SampleCloudDensity(sample_point, level);
   }

   //Return the total density.
   return (1.0f - exp(-density)) * cloud_density_multiplier;
}
#endif