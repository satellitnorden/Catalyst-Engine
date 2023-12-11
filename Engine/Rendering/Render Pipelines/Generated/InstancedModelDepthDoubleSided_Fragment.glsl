#version 460

//Constants.
#define MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES (4096)
#define MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS (512)

#define MATERIAL_PROPERTY_TYPE_MASKED (1 << 0)
#define MATERIAL_PROPERTY_TYPE_OPAQUE (1 << 1)
#define MATERIAL_PROPERTY_TYPE_TRANSLUCENT (1 << 2)
#define MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE (1 << 3)
#define MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE (1 << 4)
#define MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE (1 << 5)
#define MATERIAL_PROPERTY_OPACITY_TEXTURE (1 << 6)

#define FLOAT32_MAXIMUM (3.402823466e+38F)
#define FLOAT32_EPSILON (1.192092896e-07F)
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_8_BIT_UINT (255)

/*
*   Tests the bit of the specified bitfield
*/
#define TEST_BIT(BITFIELD, BIT) ((BITFIELD & BIT) == BIT)

/*
*	Evaluates the albedo/thickness of the given material at the given coordinate.
*	Assumes a sampler named 'SAMPLER' has been defined.
*/
#define EVALUATE_ALBEDO_THICKNESS(MATERIAL, COORDINATE, ALBEDO_THICKNESS)									\
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
*	Evaluates the normal map/displacement of the given material at the given coordinate.
*	Assumes a sampler named 'SAMPLER' has been defined.
*/
#define EVALUATE_NORMAL_MAP_DISPLACEMENT(MATERIAL, COORDINATE, NORMAL_MAP_DISPLACEMENT)									\
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
*	Evaluates the material properties of the given material at the given coordinate.
*	Assumes a sampler named 'SAMPLER' has been defined.
*/
#define EVALUATE_MATERIAL_PROPERTIES(MATERIAL, COORDINATE, MATERIAL_PROPERTIES)										\
{																													\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE))								\
	{																												\
		MATERIAL_PROPERTIES = texture(sampler2D(TEXTURES[MATERIAL._MaterialProperties], SAMPLER), COORDINATE);	\
	}																												\
																													\
	else																											\
	{																												\
		MATERIAL_PROPERTIES = UnpackColor(MATERIAL._MaterialProperties);											\
	}																												\
}

/*
*	Evaluates the opacity of the given material at the given coordinate.
*	Assumes a sampler named 'SAMPLER' has been defined.
*/
#define EVALUATE_OPACITY(MATERIAL, COORDINATE, OPACITY)										\
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

/*
*   Returns the length of a vector with three components squared.
*/
float LengthSquared3(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
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
	layout (offset = 0) uint FRAME;
};

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec4 WIND_DIRECTION_SPEED;
	layout (offset = 16) float WIND_TIME;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

//Constants.
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_1 	(1 << 0)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_2 	(1 << 1)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_3 	(1 << 2)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_4 	(1 << 3)
#define MODEL_FLAG_IS_VEGETATION 				(1 << 4)

/*
*	Returns the interleaved gradient noise for the given coordinate at the given frame.
*/
float InterleavedGradientNoise(uvec2 coordinate, uint frame)
{
	frame = frame % 64;

	float x = float(coordinate.x) + 5.588238f * float(frame);
	float y = float(coordinate.y) + 5.588238f * float(frame);

	return mod(52.9829189f * mod(0.06711056f * x + 0.00583715f * y, 1.0f), 1.0f);
}

/*
*	Calculates wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculateWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	//Calculate the displacement.
	vec3 displacement = vec3(0.0f, 0.0f, 0.0f);

	//Add large scale motion.
	displacement.x += (sin(world_position.x + WIND_TIME) + 0.125f) * WIND_DIRECTION_SPEED.x * WIND_DIRECTION_SPEED.w;
	displacement.z += (cos(world_position.z + WIND_TIME) + 0.125f) * WIND_DIRECTION_SPEED.z * WIND_DIRECTION_SPEED.w;

	//Add medium scale motion.
	displacement.x += (sin((world_position.x + WIND_TIME) * 2.0f) + 0.25f) * WIND_DIRECTION_SPEED.x * WIND_DIRECTION_SPEED.w * 0.5f;
	displacement.z += (cos((world_position.z + WIND_TIME) * 2.0f) + 0.25f) * WIND_DIRECTION_SPEED.z * WIND_DIRECTION_SPEED.w * 0.5f;

	//Add small scale motion.
	displacement.x += (sin((world_position.x + WIND_TIME) * 4.0f) + 0.375f) * WIND_DIRECTION_SPEED.x * WIND_DIRECTION_SPEED.w * 0.25f;
	displacement.z += (cos((world_position.z + WIND_TIME) * 4.0f) + 0.375f) * WIND_DIRECTION_SPEED.z * WIND_DIRECTION_SPEED.w * 0.25f;

	//Modify the displacement so it doesn't affect the bottom of the mesh.
	displacement *= max(vertex_position.y * 0.125f, 0.0f);

	//Return the displacement.
	return displacement;
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
	layout (offset = 16) uint MODEL_FLAGS;
	layout (offset = 20) float START_FADE_OUT_DISTANCE_SQUARED;
	layout (offset = 24) float END_FADE_OUT_DISTANCE_SQUARED;
	layout (offset = 28) uint MATERIAL_INDEX;
};

layout (location = 0) in vec2 InTextureCoordinate;
layout (location = 1) in float InFadeOpacity;

void main()
{
    float opacity = 1.0f;
    if (TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_MASKED))
    {
        EVALUATE_OPACITY(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, opacity);
    }
    float noise_value = InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME);
    if (opacity < 0.5f
    || (InFadeOpacity < 1.0f && InFadeOpacity < noise_value))
    {
        discard;
    }
}