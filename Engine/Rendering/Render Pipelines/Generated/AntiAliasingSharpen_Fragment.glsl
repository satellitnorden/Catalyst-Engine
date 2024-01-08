#version 460

#extension GL_ARB_separate_shader_objects : require
//Constants.
#define MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES (4096)
#define MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS (512)

#define NUMBER_OF_BLUE_NOISE_TEXTURES (32)
#define BLUE_NOISE_TEXTURE_RESOLUTION (32)

#define MATERIAL_PROPERTY_TYPE_MASKED (1 << 0)
#define MATERIAL_PROPERTY_TYPE_OPAQUE (1 << 1)
#define MATERIAL_PROPERTY_TYPE_TRANSLUCENT (1 << 2)
#define MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE (1 << 3)
#define MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE (1 << 4)
#define MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE (1 << 5)
#define MATERIAL_PROPERTY_OPACITY_TEXTURE (1 << 6)

#define FLOAT32_MAXIMUM (3.402823466e+38F)
#define UINT8_MAXIMUM (0xff)
#define FLOAT32_EPSILON (1.192092896e-07F)
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_8_BIT_UINT (255)
#define UINT32_MAXIMUM_RECIPROCAL (2.328306437080797e-10f)

#define PI (3.141592f)
#define SQUARE_ROOT_OF_TWO (1.414213f)

/*
*   Defines the bit at the specified index.
*/
#define BIT(INDEX) (1 << (INDEX))

/*
*   Tests the bit of the specified bitfield
*/
#define TEST_BIT(BITFIELD, BIT) ((BITFIELD & BIT) == BIT)

/*
*	Evaluates the albedo/thickness of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_ALBEDO_THICKNESS(MATERIAL, COORDINATE, SAMPLER, ALBEDO_THICKNESS)							\
{																											\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE))							\
	{																										\
		ALBEDO_THICKNESS = texture(sampler2D(TEXTURES[MATERIAL._AlbedoThickness], SAMPLER), COORDINATE);	\
	}																										\
																											\
	else																									\
	{																										\
		ALBEDO_THICKNESS = UnpackColor(MATERIAL._AlbedoThickness);											\
	}																										\
}

/*
*	Evaluates the normal map/displacement of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_NORMAL_MAP_DISPLACEMENT(MATERIAL, COORDINATE, SAMPLER, NORMAL_MAP_DISPLACEMENT)						\
{																														\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))								\
	{																													\
		NORMAL_MAP_DISPLACEMENT = texture(sampler2D(TEXTURES[MATERIAL._NormalMapDisplacement], SAMPLER), COORDINATE);	\
	}																													\
																														\
	else																												\
	{																													\
		NORMAL_MAP_DISPLACEMENT = UnpackColor(MATERIAL._NormalMapDisplacement);											\
	}																													\
}

/*
*	Evaluates the material properties of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_MATERIAL_PROPERTIES(MATERIAL, COORDINATE, SAMPLER, MATERIAL_PROPERTIES)							\
{																													\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE))								\
	{																												\
		MATERIAL_PROPERTIES = texture(sampler2D(TEXTURES[MATERIAL._MaterialProperties], SAMPLER), COORDINATE);		\
	}																												\
																													\
	else																											\
	{																												\
		MATERIAL_PROPERTIES = UnpackColor(MATERIAL._MaterialProperties);											\
	}																												\
}

/*
*	Evaluates the opacity of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_OPACITY(MATERIAL, COORDINATE, SAMPLER, OPACITY)							\
{																							\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_OPACITY_TEXTURE))					\
	{																						\
		OPACITY = texture(sampler2D(TEXTURES[MATERIAL._Opacity], SAMPLER), COORDINATE).x;	\
	}																						\
																							\
	else																					\
	{																						\
		OPACITY = UnpackColor(MATERIAL._Opacity).x;											\
	}																						\
}

/*
*	Material struct definition.
*/
struct Material
{
	uint _Properties;
	uint _AlbedoThickness;
	uint _NormalMapDisplacement;
	uint _MaterialProperties;
	uint _Opacity;
	float _EmissiveMultiplier;
    uint _Padding1;
    uint _Padding2;
};

//The textures.
layout (set = 0, binding = 0) uniform texture2D TEXTURES[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];

//Materials.
layout (std140, set = 0, binding = 1) uniform GlobalMaterials
{
    layout (offset = 0) Material MATERIALS[MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS];
};

//The blue noise textures.
layout (set = 0, binding = 2) uniform sampler2D BLUE_NOISE_TEXTURES[NUMBER_OF_BLUE_NOISE_TEXTURES];

/*
*	Returns the square of the given number.
*/
float Square(float X)
{
	return X * X;
}

/*
*	Returns the inverse square of the given number.
*/
float InverseSquare(float X)
{
	return 1.0f - Square(1.0f - X);
}

/*
*   Returns the length of a vector with three components squared.
*/
float LengthSquared3(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/*
*   Calculates the luminance of a color.
*/
float Luminance(vec3 color)
{
    return color.r * 0.2126f + color.g * 0.7152f + color.b * 0.0722f;
}

/*
*   Unpacks a color into a vec4.
*/
vec4 UnpackColor(uint color)
{   
    vec4 unpacked;

    unpacked.r = float(color & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;
    unpacked.g = float((color >> 8) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;
    unpacked.b = float((color >> 16) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;
    unpacked.a = float((color >> 24) & MAXIMUM_8_BIT_UINT) / MAXIMUM_8_BIT_FLOAT;

    return unpacked;
}

/*
*   Returns if the given screen coordinate is valid.
*/
bool ValidScreenCoordinate(vec2 X)
{
    return  X.x >= 0.0f
            && X.x < 1.0f
            && X.y >= 0.0f
            && X.y < 1.0f;
}

layout (std140, set = 1, binding = 0) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) uint BLUE_NOISE_TEXTURE_INDEX;
};

/*
*	Approximates the inverse gamma transformation of a fragment to determine it's perceptual luminance.
*/
float PerceptualLuminance(vec3 fragment)
{
	return sqrt(dot(fragment, vec3(0.299f, 0.587f, 0.114f)));
}

layout (location = 0) in vec2 InScreenCoordinate;

layout (set = 1, binding = 1) uniform sampler2D InputRenderTarget;
layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2;

layout (location = 0) out vec4 OutputRenderTarget;

void main()
{
    vec3 samples[3][3];
    float minimum_luminance = 1.0f;
    float maximum_luminance = 0.0f;
    for (int Y = -1; Y <= 1; ++Y)
    {
        for (int X = -1; X <= 1; ++X)
        {
            vec2 sample_coordinate = InScreenCoordinate + vec2(X, Y) * INVERSE_FULL_MAIN_RESOLUTION;
            vec3 _sample = texture(InputRenderTarget, sample_coordinate).rgb;
            samples[X + 1][Y + 1] = _sample;
            float sample_luminance = PerceptualLuminance(_sample);
            minimum_luminance = min(minimum_luminance, sample_luminance);
            maximum_luminance = max(maximum_luminance, sample_luminance);
        }
    }
    float sharpen_weight = (1.0f - (maximum_luminance - minimum_luminance));
    sharpen_weight = smoothstep(0.9f, 1.0f, sharpen_weight);
    float sharpen_kernel[3][3];
    sharpen_kernel[0][0] = sharpen_kernel[0][2] = sharpen_kernel[2][0] = sharpen_kernel[2][2] = -0.5f * sharpen_weight;
    sharpen_kernel[0][1] = sharpen_kernel[1][0] = sharpen_kernel[1][2] = sharpen_kernel[2][1] = -1.0f * sharpen_weight;
    sharpen_kernel[1][1] = 6.0f * sharpen_weight + 1.0f;
    vec3 sharpened_sample = vec3(0.0f);
    for (int Y = -1; Y <= 1; ++Y)
    {
        for (int X = -1; X <= 1; ++X)
        {
            sharpened_sample += samples[X + 1][Y + 1] * sharpen_kernel[X + 1][Y + 1];
        }
    }
#if 0
    vec3 output_fragment = vec3(0.0f);
    if (gl_FragCoord.x > 960.0f)
    {
        output_fragment = sharpened_sample;
    }
    else
    {
        output_fragment = texture(InputRenderTarget, InScreenCoordinate).rgb;
    }
    output_fragment *= min(abs(gl_FragCoord.x - 960.0f) * 0.25f, 1.0f);
	OutputRenderTarget = vec4(output_fragment,sharpen_weight);
#else
	OutputRenderTarget = vec4(sharpened_sample,sharpen_weight);
#endif
}
