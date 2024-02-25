#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
layout (early_fragment_tests) in;

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
#define UINT16_MAXIMUM (0xffff)
#define UINT16_RECIPROCAL (1.525902189669642e-5f)
#define FLOAT32_EPSILON (1.192092896e-07F)
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_8_BIT_UINT (255)
#define UINT32_MAXIMUM_RECIPROCAL (2.328306437080797e-10f)

#define PI (3.141592f)
#define SQUARE_ROOT_OF_TWO (1.414213f)

#define saturate(X) clamp(X, 0.0f, 1.0f)

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

layout (std140, set = 1, binding = 0) uniform Camera
{
	layout (offset = 0) mat4 WORLD_TO_CLIP_MATRIX;
	layout (offset = 64) mat4 WORLD_TO_CAMERA_MATRIX;
	layout (offset = 128) mat4 PREVIOUS_WORLD_TO_CLIP_MATRIX;
	layout (offset = 192) mat4 INVERSE_WORLD_TO_CAMERA_MATRIX;
	layout (offset = 256) mat4 INVERSE_CAMERA_TO_CLIP_MATRIX;
	layout (offset = 320) vec3 CAMERA_WORLD_POSITION;
	layout (offset = 336) vec3 CAMERA_FORWARD_VECTOR;
	layout (offset = 352) vec2 CURRENT_FRAME_JITTER;
	layout (offset = 360) float NEAR_PLANE;
	layout (offset = 364) float FAR_PLANE;
};

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
};

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec4 PREVIOUS_WIND_DIRECTION_SPEED;
	layout (offset = 16) vec4 CURRENT_WIND_DIRECTION_SPEED;
	layout (offset = 32) float PREVIOUS_WIND_TIME;
	layout (offset = 36) float CURRENT_WIND_TIME;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

/*
*   Linearizes a depth value.
*/
float LinearizeDepth(float depth)
{
    return ((FAR_PLANE * NEAR_PLANE) / (depth * (FAR_PLANE - NEAR_PLANE) + NEAR_PLANE));
}

/*
*   Calculates the view space position.
*/
vec3 CalculateViewSpacePosition(vec2 texture_coordinate, float depth)
{
    vec2 near_plane_coordinate = texture_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = INVERSE_CAMERA_TO_CLIP_MATRIX * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position.xyz *= inverse_view_space_position_denominator;

    return view_space_position.xyz;
}

/*
*   Calculates the world position.
*/
vec3 CalculateWorldPosition(vec2 screen_coordinate, float depth)
{
    vec2 near_plane_coordinate = screen_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = INVERSE_CAMERA_TO_CLIP_MATRIX * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position *= inverse_view_space_position_denominator;
    vec4 world_space_position = INVERSE_WORLD_TO_CAMERA_MATRIX * view_space_position;

    return world_space_position.xyz;
}

/*
*   Returns the current screen coordinate with the given view matrix and world position.
*/
vec2 CalculateCurrentScreenCoordinate(vec3 world_position)
{
  vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
  float denominator = 1.0f / view_space_position.w;
  view_space_position.xy *= denominator;

  return view_space_position.xy * 0.5f + 0.5f;
}

/*
*   Returns the previous screen coordinate with the given view matrix and world position.
*/
vec2 CalculatePreviousScreenCoordinate(vec3 world_position)
{
  vec4 view_space_position = PREVIOUS_WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
  float denominator = 1.0f / view_space_position.w;
  view_space_position.xy *= denominator;

  return view_space_position.xy * 0.5f + 0.5f;
}

/*
*   Calculates a screen position, including the (linearized) depth from the given world position.
*/
vec3 CalculateScreenPosition(vec3 world_position)
{
    vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
    float view_space_position_coefficient_reciprocal = 1.0f / view_space_position.w;
    view_space_position.xyz *= view_space_position_coefficient_reciprocal;

    view_space_position.xy = view_space_position.xy * 0.5f + 0.5f;
    view_space_position.z = LinearizeDepth(view_space_position.z);
    
    return view_space_position.xyz;
}

/*
*	Calculates the water height at the given position.
*/
float WaterHeightAtPosition(vec3 world_position)
{
	//Constants.
	#define AMPLITUDE (0.5f)

	//Cache the current time. Assume 60 FPS for now.
	float current_time = float(FRAME) / 60.0f;

	//Accumulate the height.
	float water_height = 0.0f;

	{
		float sum = 0.0f;
		float amplitude = 0.25f;
		float frequency = 0.125f;
		
		float direction_seed = 0.0f;
		vec2 direction;

		for (uint i = 0; i < 8; ++i)
		{
			//"Randomize" the direction for every wave.
			direction = normalize(vec2(cos(direction_seed), sin(direction_seed)));
			direction_seed += 1253.2131f;

			//Calculate the wave coordinate.
			float wave_coordinate = world_position.x * direction.x + world_position.z * direction.y;

			//Calculate the wave length.
			float wave_length = 2.0f / frequency;

			water_height += amplitude * sin(wave_coordinate * wave_length + current_time * (2.0f / wave_length));
			sum += amplitude;
			amplitude *= 0.5f;
			frequency *= 2.0f;
		}

		water_height /= sum;
	}

	//Return the wave height.
	return water_height;
}

/*
*	Calculates the water height at the given position.
*/
vec3 WaterNormalAtPosition(vec3 world_position)
{
	float left = WaterHeightAtPosition(world_position + vec3(-1.0f, 0.0f, 0.0f));
	float right = WaterHeightAtPosition(world_position + vec3(1.0f, 0.0f, 0.0f));
	float down = WaterHeightAtPosition(world_position + vec3(0.0f, 0.0f, -1.0f));
	float up = WaterHeightAtPosition(world_position + vec3(0.0f, 0.0f, 1.0f));

	return normalize(vec3(left - right, 2.0f, down - up));
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 WORLD_POSITION;
	layout (offset = 8) uint BORDERS;
	layout (offset = 12) float PATCH_RESOLUTION_RECIPROCAL;
	layout (offset = 16) float PATCH_SIZE;
	layout (offset = 20) uint TEXTURE_INDEX;
};

layout (set = 1, binding = 4) uniform sampler2D SceneFeatures1Input;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2Input;

layout (location = 0) in vec3 InWorldPosition;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;

void main()
{
    #define WATER_ALBEDO (vec3(0.0f, 0.125f * 0.25f, 0.5f * 0.5f))
	float current_time = float(FRAME) / 60.0f * 0.125f * 0.125f;
    float right_height;
    {
        vec2 sample_position = InWorldPosition.xz + vec2(1.0f, 0.0f);
        float water_texture_sample_1 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 0.125f + vec2(-current_time, 0.0f)).x;
        float water_texture_sample_2 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 0.25f + vec2(current_time, 0.0f)).y;
        float water_texture_sample_3 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 0.5f + vec2(0.0f, -current_time)).z;
        float water_texture_sample_4 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 1.0f + vec2(0.0f, current_time)).w;
        right_height =  (water_texture_sample_1 * 1.0f
                        + water_texture_sample_2 * 0.5f
                        + water_texture_sample_3 * 0.25f
                        + water_texture_sample_4 * 0.125f)
                        / 1.875f * 2.0f;
    }
    float up_height;
    {
        vec2 sample_position = InWorldPosition.xz + vec2(0.0f, 1.0f);
        float water_texture_sample_1 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 0.125f + vec2(-current_time, 0.0f)).x;
        float water_texture_sample_2 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 0.25f + vec2(current_time, 0.0f)).y;
        float water_texture_sample_3 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 0.5f + vec2(0.0f, -current_time)).z;
        float water_texture_sample_4 = texture(sampler2D(TEXTURES[TEXTURE_INDEX], SAMPLER), sample_position * 1.0f + vec2(0.0f, current_time)).w;
        up_height =     (water_texture_sample_1 * 1.0f
                        + water_texture_sample_2 * 0.5f
                        + water_texture_sample_3 * 0.25f
                        + water_texture_sample_4 * 0.125f)
                        / 1.875f * 2.0f;
    }
    vec3 water_normal = normalize(vec3(InWorldPosition.y - right_height, 1.0f, InWorldPosition.y - up_height));
    vec2 screen_coordinate = (vec2(gl_FragCoord.xy) + 0.5f) * INVERSE_FULL_MAIN_RESOLUTION;
    vec4 scene_features_2 = texture(SceneFeatures2Input, screen_coordinate);
    float surface_depth = LinearizeDepth(gl_FragCoord.z);
    float underwater_depth = LinearizeDepth(scene_features_2.w);
    float underwater_weight = exp(-(underwater_depth - surface_depth) * 0.125f * 0.25f);
    vec4 scene_features_1 = texture(SceneFeatures1Input, screen_coordinate);
    vec3 underwater_albedo = scene_features_1.rgb;
    underwater_albedo *= 0.5f;
    underwater_albedo *= underwater_weight;
    vec3 final_albedo = mix(WATER_ALBEDO, underwater_albedo, underwater_weight);
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(final_albedo,0.0f);
	SceneFeatures2 = vec4(water_normal,gl_FragCoord.z);
	SceneFeatures3 = vec4(0.125f,0.0f,1.0f,0.0f);
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
