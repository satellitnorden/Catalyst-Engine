#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
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
};

/*
*	Approximates the inverse gamma transformation of a fragment to determine it's perceptual luminance.
*/
float PerceptualLuminance(vec3 fragment)
{
	return sqrt(dot(fragment, vec3(0.299f, 0.587f, 0.114f)));
}

layout (set = 1, binding = 1) uniform sampler2D InputRenderTarget;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 OutputRenderTarget;

void main()
{
    #define EDGE_THRESHOLD_MINIMUM (0.0312f)
    #define EDGE_THRESHOLD_MAXIMUM (0.125f)
    #define ITERATIONS (12)
    #define QUALITY(INDEX) (1.0f * float(INDEX) * 0.25f)
    #define SUBPIXEL_QUALITY (0.75f)
    vec3 center_color = texture(InputRenderTarget, InScreenCoordinate).rgb;
    float center_luminance = PerceptualLuminance(center_color);
    float down_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(0.0f, -1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float up_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(0.0f, 1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float left_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(-1.0f, 0.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float right_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(1.0f, 0.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float minimum_luminance = min(center_luminance, min(min(down_luminance, up_luminance), min(left_luminance, right_luminance)));
    float maximum_luminance = max(center_luminance, max(max(down_luminance, up_luminance), max(left_luminance, right_luminance)));
    float luminance_range = maximum_luminance - minimum_luminance;
    /*
    if (luminance_range < max(EDGE_THRESHOLD_MINIMUM, maximum_luminance * EDGE_THRESHOLD_MAXIMUM))
    {
	OutputRenderTarget = vec4(center_color,1.0f);
        return;
    }
    */
    float down_left_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(-1.0f, -1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float up_right_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(1.0f, 1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float up_left_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(-1.0f, 1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float down_right_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(1.0f, -1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float down_up_luminance = down_luminance + up_luminance;
    float left_right_luminance = left_luminance + right_luminance;
    float left_corners_luminance = down_left_luminance + up_left_luminance;
    float down_corners_luminance = down_left_luminance + down_right_luminance;
    float right_corners_luminance = down_right_luminance + up_right_luminance;
    float up_corners_luminance = up_right_luminance + up_left_luminance;
    float horizontal_edge =  abs(-2.0f * left_luminance + left_corners_luminance) + abs(-2.0f * center_luminance + down_up_luminance ) * 2.0f + abs(-2.0f * right_luminance + right_corners_luminance);
    float vertical_edge = abs(-2.0f * up_luminance + up_corners_luminance) + abs(-2.0f * center_luminance + left_right_luminance) * 2.0 + abs(-2.0f * down_luminance + down_corners_luminance);
    bool is_horizontal_edge = horizontal_edge >= vertical_edge;
    float luminance_1 = is_horizontal_edge ? down_luminance : left_luminance;
    float luminance_2 = is_horizontal_edge ? up_luminance : right_luminance;
    float gradient_1 = luminance_1 - center_luminance;
    float gradient_2 = luminance_2 - center_luminance;
    bool is_1_steepest = abs(gradient_1) >= abs(gradient_2);
    float scaled_gradient = 0.25f * max(abs(gradient_1), abs(gradient_2));
    float step_length = is_horizontal_edge ? INVERSE_FULL_MAIN_RESOLUTION.y : INVERSE_FULL_MAIN_RESOLUTION.x;
    float local_average_luminance = 0.0f;
    if (is_1_steepest)
    {
        step_length = -step_length;
        local_average_luminance = 0.5f * (luminance_1 + center_luminance);
    }
    else
    {
        local_average_luminance = 0.5f * (luminance_2 + center_luminance);
    }
    vec2 current_screen_coordinate = InScreenCoordinate;
    if (is_horizontal_edge)
    {
        current_screen_coordinate.y += step_length * 0.5f;
    }
    else
    {
        current_screen_coordinate.x += step_length * 0.5f;
    }
    vec2 offset = is_horizontal_edge ? vec2(INVERSE_FULL_MAIN_RESOLUTION.x, 0.0f) : vec2(0.0f, INVERSE_FULL_MAIN_RESOLUTION.y);
    vec2 screen_coordinate_1 = current_screen_coordinate - offset;
    vec2 screen_coordinate_2 = current_screen_coordinate + offset;
    float end_luminance_1 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_1).rgb);
    float end_luminance_2 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_2).rgb);
    end_luminance_1 -= local_average_luminance;
    end_luminance_2 -= local_average_luminance;
    bool reached_1 = abs(end_luminance_1) >= scaled_gradient;
    bool reached_2 = abs(end_luminance_2) >= scaled_gradient;
    bool reached_both = reached_1 && reached_2;
    if (!reached_1)
    {
        screen_coordinate_1 -= offset;
    }
    if (!reached_2)
    {
        screen_coordinate_2 += offset;
    }
    if (!reached_both)
    {
        for (uint i = 2; i < ITERATIONS; ++i)
        {
            if (!reached_1)
            {
                end_luminance_1 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_1).rgb);
                end_luminance_1 = end_luminance_1 - local_average_luminance;
            }
            if (!reached_2)
            {
                end_luminance_2 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_2).rgb);
                end_luminance_2 = end_luminance_2 - local_average_luminance;
            }
            bool reached_1 = abs(end_luminance_1) >= scaled_gradient;
            bool reached_2 = abs(end_luminance_2) >= scaled_gradient;
            bool reached_both = reached_1 && reached_2;
            if (!reached_1)
            {
                screen_coordinate_1 -= offset;
            }
            if (!reached_2)
            {
                screen_coordinate_2 += offset;
            }
            if (reached_both)
            {
                break;
            }
        }
    }
    float distance_1 = is_horizontal_edge ? (InScreenCoordinate.x - screen_coordinate_1.x) : (InScreenCoordinate.y - screen_coordinate_1.y);
    float distance_2 = is_horizontal_edge ? (screen_coordinate_2.x - InScreenCoordinate.x) : (screen_coordinate_2.y - InScreenCoordinate.y);
    bool is_direction_1 = distance_1 < distance_2;
    float final_distance = min(distance_1, distance_2);
    float edge_thickness = (distance_1 + distance_2);
    float texel_offset = -final_distance / edge_thickness + 0.5f;
    bool is_center_luminance_smaller = center_luminance < local_average_luminance;
    bool is_correct_variation = ((is_direction_1 ? end_luminance_1 : end_luminance_2) < 0.0f) != is_center_luminance_smaller;
    float final_offset = is_correct_variation ? texel_offset : 0.0f;
    {
        float average_luminance = (1.0f / 12.0f) * (2.0f * (down_up_luminance + left_right_luminance) + left_corners_luminance + right_corners_luminance);
        float sub_pixel_offset_1 = clamp(abs(average_luminance - center_luminance) / luminance_range, 0.0f, 1.0f);
        float sub_pixel_offset_2 = (-2.0f * sub_pixel_offset_1 + 3.0f) * sub_pixel_offset_1 * sub_pixel_offset_1;
        float sub_pixel_offset_final = sub_pixel_offset_2 * sub_pixel_offset_2 * SUBPIXEL_QUALITY;
        final_offset = max(final_offset, sub_pixel_offset_final);
    }
    vec2 final_screen_coordinate = InScreenCoordinate;
    if (is_horizontal_edge)
    {
        final_screen_coordinate.y += final_offset * step_length;
    }
    else
    {
        final_screen_coordinate.x += final_offset * step_length;
    }
    vec3 final_color = texture(InputRenderTarget, final_screen_coordinate).rgb;
	OutputRenderTarget = vec4(final_color,1.0f);
}
