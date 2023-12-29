#version 460

#extension GL_ARB_separate_shader_objects : require
layout (early_fragment_tests) in;

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
    return NEAR_PLANE * FAR_PLANE / (FAR_PLANE + depth * (NEAR_PLANE - FAR_PLANE));
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
*	Evaluates tilt/bend. Returns the offset on the Z axis.
*/
float EvaluateTiltBend(float tilt, float bend, float normalized_height)
{
	float bend_alpha = bend >= 0.5f ? ((bend - 0.5f) * 2.0f) : (bend * 2.0f);
	return mix(0.0f, tilt, mix(InverseSquare(normalized_height), Square(normalized_height), bend_alpha));
}

/*
*	Rotates the given vector around the yaw.
*/
vec3 RotateYaw(vec3 X, float angle)
{
	float sine = sin(angle);
    float cosine = cos(angle);

    float temp = X.x * cosine + X.z * sine;
    X.z = -X.x * sine + X.z * cosine;
    X.x = temp;

    return X;
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
*   Hash function.
*/
uint Hash(inout uint seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);

    return seed;
}

/*
*   Given a seed, returns a random number.
*/
float RandomFloat(inout uint seed)
{
    return Hash(seed) * UINT32_MAXIMUM_RECIPROCAL;
}

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
vec3 CalculateWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal, vec4 wind_direction_speed, float wind_time)
{
	//Calculate the displacement.
	vec3 displacement = vec3(0.0f, 0.0f, 0.0f);

	//Add large scale motion.
	displacement.x += (sin(world_position.x + world_position.y + vertex_position.y + wind_time) + 0.75f) * wind_direction_speed.x * wind_direction_speed.w;
	displacement.z += (cos(world_position.z + world_position.y + vertex_position.y + wind_time) + 0.75f) * wind_direction_speed.z * wind_direction_speed.w;

	//Add medium scale motion.
	displacement.x += (sin((world_position.x + world_position.y + vertex_position.y + wind_time) * 2.0f) + 0.75f) * wind_direction_speed.x * wind_direction_speed.w * 0.5f;
	displacement.z += (cos((world_position.z + world_position.y + vertex_position.y + wind_time) * 2.0f) + 0.75f) * wind_direction_speed.z * wind_direction_speed.w * 0.5f;

	//Add small scale motion.
	displacement.x += (sin((world_position.x + world_position.y + vertex_position.y + wind_time) * 4.0f) + 0.75f) * wind_direction_speed.x * wind_direction_speed.w * 0.25f;
	displacement.z += (cos((world_position.z + world_position.y + vertex_position.y + wind_time) * 4.0f) + 0.75f) * wind_direction_speed.z * wind_direction_speed.w * 0.25f;

	//Modify the displacement so it doesn't affect the bottom of the mesh.
	displacement *= max(vertex_position.y * 0.125f, 0.0f);

	//Return the displacement.
	return displacement;
}

/*
*	Calculates previous wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculatePreviousWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	return CalculateWindDisplacement(world_position, vertex_position, normal, PREVIOUS_WIND_DIRECTION_SPEED, PREVIOUS_WIND_TIME);
}

/*
*	Calculates current wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculateCurrentWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	return CalculateWindDisplacement(world_position, vertex_position, normal, CURRENT_WIND_DIRECTION_SPEED, CURRENT_WIND_TIME);
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
	layout (offset = 16) uint MATERIAL_INDEX;
	layout (offset = 20) float VERTEX_FACTOR;
	layout (offset = 24) float THICKNESS;
	layout (offset = 28) float HEIGHT;
	layout (offset = 32) float TILT;
	layout (offset = 36) float BEND;
	layout (offset = 40) float FADE_OUT_DISTANCE;
};

layout (location = 0) in vec3 InWorldPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in float InX;
layout (location = 3) in float InThickness;
layout (location = 4) in float InAmbientOcclusion;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;

void main()
{
    float X_factor = InX * 5.0f;
    uint X_index_1 = uint(X_factor);
    uint X_index_2 = min(X_index_1 + 1, 4);
    float X_fractional = fract(X_factor);
    vec4 albedo_thickness;
    EVALUATE_ALBEDO_THICKNESS(MATERIALS[MATERIAL_INDEX], vec2(0.0f, 0.0f), SAMPLER, albedo_thickness);
    vec4 material_properties;
    EVALUATE_MATERIAL_PROPERTIES(MATERIALS[MATERIAL_INDEX], vec2(0.0f, 0.0f), SAMPLER, material_properties);
    float ALBEDO_MULTIPLIERS[5] = float[5]
    (
        0.825f,
        1.125f,
        0.825f,
        1.125f,
        0.825f
    );
    float albedo_multiplier = mix(ALBEDO_MULTIPLIERS[X_index_1], ALBEDO_MULTIPLIERS[X_index_2], X_fractional);
    float ROUGHNESS_MULTIPLIERS[5] = float[5]
    (
        1.0f,
        0.75f,
        1.0f,
        0.75f,
        1.0f
    );
    float roughness_multiplier = mix(ROUGHNESS_MULTIPLIERS[X_index_1], ROUGHNESS_MULTIPLIERS[X_index_2], X_fractional);
    vec3 shading_normal = normalize(InNormal);
    shading_normal.xz *= gl_FrontFacing ? -1.0f : 1.0f;
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(albedo_thickness.rgb*albedo_multiplier,InThickness);
	SceneFeatures2 = vec4(shading_normal,gl_FragCoord.z);
	SceneFeatures3 = material_properties*vec4(roughness_multiplier,1.0f,InAmbientOcclusion,1.0f);
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
