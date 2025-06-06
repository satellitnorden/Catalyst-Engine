�nX/J1�7�L~�|Terrain_RenderPipeline                                           ��      #     ~                GLSL.std.450              
        main    �   �   m  `  g       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Terrain_Vertex.glsl  ?   �     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
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
#define UINT16_MAXIMUM (0xffff)
#define UINT16_RECIPROCAL (1.525902189669642e-5f)
#define UINT32_MAXIMUM (0xffffffff)
#define FLOAT32_EPSILON (1.192092896e-07F)
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_8_BIT_UINT (255)
#define UINT32_MAXIMUM_RECIPROCAL (2.328306437080797e-10f)

#define DIVIDE_BY_ZERO_SAFE_EPSILON (FLOAT32_EPSILON * 1.0f)

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
*	Evaluates the average albedo/thickness of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_ALBEDO_THICKNESS(MATERIAL, COORDINATE, SAMPLER, ALBEDO_THICKNESS)					\
{																											\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE))							\
	{																										\
		ALBEDO_THICKNESS = TEXTURE_AVERAGE_VALUES[MATERIAL._AlbedoThickness];								\
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
*	Evaluates the average normal map/displacement of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_NORMAL_MAP_DISPLACEMENT(MATERIAL, COORDINATE, SAMPLER, NORMAL_MAP_DISPLACEMENT)				\
{																														\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))								\
	{																													\
		NORMAL_MAP_DISPLACEMENT = TEXTURE_AVERAGE_VALUES[MATERIAL._NormalMapDisplacement];								\
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
*	Evaluates the average material properties of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_MATERIAL_PROPERTIES(MATERIAL, COORDINATE, SAMPLER, MATERIAL_PROPERTIES)					\
{																													\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE))								\
	{																												\
		MATERIAL_PROPERTIES = TEXTURE_AVERAGE_VALUES[MATERIAL._MaterialProperties];									\
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
*	Evaluates the average opacity of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_OPACITY(MATERIAL, COORDINATE, SAMPLER, OPACITY)					\
{																							\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_OPACITY_TEXTURE))					\
	{																						\
		OPACITY = TEXTURE_AVERAGE_VALUES[MATERIAL._Opacity];								\
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

//The texture average values.
layout (std140, set = 0, binding = 1) uniform GlobalTextureAverageValues
{
    layout (offset = 0) vec4 TEXTURE_AVERAGE_VALUES[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];
};

//Materials.
layout (std140, set = 0, binding = 2) uniform GlobalMaterials
{
    layout (offset = 0) Material MATERIALS[MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS];
};

//The blue noise textures.
layout (set = 0, binding = 3) uniform sampler2D BLUE_NOISE_TEXTURES[NUMBER_OF_BLUE_NOISE_TEXTURES];

//The sky texture.
layout (set = 0, binding = 4) uniform samplerCube SKY_TEXTURE;

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
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
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

layout (set = 1, binding = 1) uniform sampler HEIGHT_MAP_SAMPLER;
layout (set = 1, binding = 2) uniform sampler NORMAL_MAP_SAMPLER;
layout (set = 1, binding = 3) uniform sampler INDEX_BLEND_MAP_SAMPLER;
layout (set = 1, binding = 4) uniform sampler MATERIAL_SAMPLER;

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
*   Combines two hashes.
*/
uint CombineHash(uint hash_1, uint hash_2)
{
    return 3141592653 * hash_1 + hash_2;
}

/*
*   Hash function taking a uint.
*/
uint Hash(uint seed)
{
    seed ^= seed >> 17;
    seed *= 0xed5ad4bbU;
    seed ^= seed >> 11;
    seed *= 0xac4c1b51U;
    seed ^= seed >> 15;
    seed *= 0x31848babU;
    seed ^= seed >> 14;
    return seed;
}

/*
*   Hash function taking a uvec2.
*/
uint Hash2(uvec2 seed)
{
    return Hash(seed.x) ^ Hash(seed.y);
}

/*
*   Hash function taking a uvec3.
*/
uint Hash3(uvec3 seed)
{
    //return Hash( Hash( Hash( Hash(seed.x) ^ Hash(seed.y) ^ Hash(seed.z) ) ) );
    //return Hash( Hash( Hash(seed.x) + Hash(seed.y) ) + Hash(seed.z) );
    return Hash( CombineHash(CombineHash(Hash(seed.x), Hash(seed.y)), Hash(seed.z)) );
}

/*
*   Given a seed, returns a random number.
*/
float RandomFloat(inout uint seed)
{
    return Hash(seed) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate and a seed, returns a random number.
*/
float RandomFloat(uvec2 coordinate, uint seed)
{
    return float(Hash3(uvec3(coordinate.xy, seed))) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate, returns a random number.
*/
float RandomFloat(vec2 coordinate)
{
    return fract(sin(dot(coordinate, vec2(12.9898f, 78.233f))) * 43758.5453f);
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

//Constants.
#define TERRAIN_MATERIAL_SCALE (0.5f)
#define TERRAIN_DISPLACEMENT_POWER (8.0f)
#define TERRAIN_MINIMUM_DISPLACEMENT_WEIGHT (FLOAT32_EPSILON)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    //The terrain normal.
    vec3 _TerrainNormal;

    //The terrain height.
    float _TerrainHeight;

    //The albedo.
    vec3 _Albedo;

    //The thickness.
    float _Thickness;

    //The normal map.
    vec3 _NormalMap;

    //The displacement.
    float _Displacement;

    //The material properties.
    vec4 _MaterialProperties;
};

/*
*   Calculates the material coordinate for the given world position.
*/
vec2 CalculateTerrainMaterialCoordinate(vec3 world_position, vec3 normal, vec2 tile_index)
{
#if 1
    //Take the absolute of the normal.
    vec3 absolute_normal = abs(normal);

    //Calculate the material coordinate.
    bool x_dominant = absolute_normal.x > absolute_normal.y && absolute_normal.x > absolute_normal.z;
    bool z_dominant = absolute_normal.z > absolute_normal.x && absolute_normal.z > absolute_normal.y;
    vec2 material_coordinate = world_position.yz * float(x_dominant) + world_position.xy * float(z_dominant) + world_position.xz * float(!x_dominant && !z_dominant);

    material_coordinate *= TERRAIN_MATERIAL_SCALE;

    //Calculate the random rotation.
    float random_rotation = mix(-PI, PI, RandomFloat(tile_index));

    //Randomly rotate the tile.
    material_coordinate -= tile_index;
    material_coordinate -= 0.5f;
    material_coordinate = RotateYaw(vec3(material_coordinate.x, 0.0f, material_coordinate.y), random_rotation).xz;
    material_coordinate += 0.5f;
    material_coordinate += tile_index;

    //Return the material_coordinate!
    return material_coordinate;
#else
    return vec2(world_position.x, world_position.z) * 0.5f;
#endif
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 WORLD_POSITION;
	layout (offset = 8) vec2 MINIMUM_HEIGHT_MAP_COORDINATE;
	layout (offset = 16) vec2 MAXIMUM_HEIGHT_MAP_COORDINATE;
	layout (offset = 24) uint BORDERS;
	layout (offset = 28) float PATCH_RESOLUTION_RECIPROCAL;
	layout (offset = 32) float PATCH_SIZE;
	layout (offset = 36) uint HEIGHT_MAP_TEXTURE_INDEX;
	layout (offset = 40) uint NORMAL_MAP_TEXTURE_INDEX;
	layout (offset = 44) uint INDEX_MAP_TEXTURE_INDEX;
	layout (offset = 48) uint BLEND_MAP_TEXTURE_INDEX;
	layout (offset = 52) float MAP_RESOLUTION;
	layout (offset = 56) float MAP_RESOLUTION_RECIPROCAL;
};

layout (location = 0) in vec2 InPosition;
layout (location = 1) in uint InBorders;

layout (location = 0) out vec3 OutWorldPosition;
layout (location = 1) out vec2 OutHeightMapTextureCoordinate;

void main()
{
    vec2 stitched_position = InPosition;
    {
	    float is_left_multiplier = float((InBorders & BIT(0)) & (BORDERS & BIT(0)));
    	float is_right_multiplier = float((InBorders & BIT(2)) & (BORDERS & BIT(2)));
	    float vertical_offset = min(is_left_multiplier + is_right_multiplier, 1.0f);
        float is_down_multiplier = float((InBorders & BIT(4)) & (BORDERS & BIT(4)));
    	float is_up_multiplier = float((InBorders & BIT(6)) & (BORDERS & BIT(6)));
	    float horizontal_offset = min(is_down_multiplier + is_up_multiplier, 1.0f);
	    stitched_position.y -= PATCH_RESOLUTION_RECIPROCAL * vertical_offset;
	    stitched_position.x -= PATCH_RESOLUTION_RECIPROCAL * horizontal_offset;
    }
    {
	    float is_left_multiplier = float((InBorders & BIT(1)) & (BORDERS & BIT(1)));
    	float is_right_multiplier = float((InBorders & BIT(3)) & (BORDERS & BIT(3)));
	    float vertical_offset = min(is_left_multiplier + is_right_multiplier, 1.0f);
        float is_down_multiplier = float((InBorders & BIT(5)) & (BORDERS & BIT(5)));
    	float is_up_multiplier = float((InBorders & BIT(7)) & (BORDERS & BIT(7)));
	    float horizontal_offset = min(is_down_multiplier + is_up_multiplier, 1.0f);
	    stitched_position.y -= (PATCH_RESOLUTION_RECIPROCAL * 2.0f) * vertical_offset;
	    stitched_position.x -= (PATCH_RESOLUTION_RECIPROCAL * 2.0f) * horizontal_offset;
    }
    vec2 height_map_coordinate = vec2(mix(MINIMUM_HEIGHT_MAP_COORDINATE.x, MAXIMUM_HEIGHT_MAP_COORDINATE.x, stitched_position.x), mix(MINIMUM_HEIGHT_MAP_COORDINATE.y, MAXIMUM_HEIGHT_MAP_COORDINATE.y, stitched_position.y));
    OutWorldPosition.x = WORLD_POSITION.x + mix(-(PATCH_SIZE * 0.5f), (PATCH_SIZE * 0.5f), stitched_position.x);
    OutWorldPosition.y = 0.0f;
    OutWorldPosition.z = WORLD_POSITION.y + mix(-(PATCH_SIZE * 0.5f), (PATCH_SIZE * 0.5f), stitched_position.y);
    {
        float height_1 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float height_2 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float height_3 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float height_4 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float blend_1 = mix(height_1, height_2, fract(height_map_coordinate.y * MAP_RESOLUTION));
	    float blend_2 = mix(height_3, height_4, fract(height_map_coordinate.y * MAP_RESOLUTION));
	    float final_height = mix(blend_1, blend_2, fract(height_map_coordinate.x * MAP_RESOLUTION));
        OutWorldPosition.y = final_height;
    }
    vec3 normals[4];
    vec3 normal;
    {
        normals[0] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        normals[1] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        normals[2] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        normals[3] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        vec3 blend_1 = mix(normals[0], normals[1], fract(height_map_coordinate.y * MAP_RESOLUTION));
	    vec3 blend_2 = mix(normals[2], normals[3], fract(height_map_coordinate.y * MAP_RESOLUTION));
	    normal = mix(blend_1, blend_2, fract(height_map_coordinate.x * MAP_RESOLUTION));
        normal = normal * 2.0f - 1.0f;
        normal = normalize(normal);
    }
    OutHeightMapTextureCoordinate = height_map_coordinate;
    float displacement_multiplier = 1.0f - min(length(OutWorldPosition - CAMERA_WORLD_POSITION) / 64.0f, 1.0f);
    if (displacement_multiplier > 0.0f)
    {
        vec2 base_coordinate = vec2
        (
            float(uint(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION)) + 0.5f,
            float(uint(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION)) + 0.5f
        ) * MAP_RESOLUTION_RECIPROCAL;
        vec2 sample_coordinates[4];
        sample_coordinates[0] = base_coordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_coordinates[1] = base_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_coordinates[2] = base_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_coordinates[3] = base_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        float displacements[4];
        for (uint sample_index = 0; sample_index < 4; ++sample_index)
        {
            vec3 terrain_normal = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), sample_coordinates[sample_index]).xyz * 2.0f - 1.0f;
            vec4 index_map = texture(sampler2D(TEXTURES[INDEX_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), sample_coordinates[sample_index]);
            vec4 blend_map = texture(sampler2D(TEXTURES[BLEND_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), sample_coordinates[sample_index]);
            Material materials[4];
            materials[0] = MATERIALS[uint(index_map[0] * float(UINT8_MAXIMUM))];
            materials[1] = MATERIALS[uint(index_map[1] * float(UINT8_MAXIMUM))];
            materials[2] = MATERIALS[uint(index_map[2] * float(UINT8_MAXIMUM))];
            materials[3] = MATERIALS[uint(index_map[3] * float(UINT8_MAXIMUM))];
            vec2 material_texture_coordinate = CalculateTerrainMaterialCoordinate(OutWorldPosition, terrain_normal, sample_coordinates[sample_index] * MAP_RESOLUTION * TERRAIN_MATERIAL_SCALE);
            vec4 normal_map_displacements[4];
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[0], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[0]);
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[1], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[1]);
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[2], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[2]);
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[3], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[3]);
            float weights[4];
            float weights_sum = 0.0f;
            for (uint weight_index = 0; weight_index < 4; ++weight_index)
            {
                weights[weight_index] = max(pow(blend_map[weight_index] * normal_map_displacements[weight_index].w, TERRAIN_DISPLACEMENT_POWER), TERRAIN_MINIMUM_DISPLACEMENT_WEIGHT);
                weights_sum += weights[weight_index];
            }
            float weights_sum_reciprocal = 1.0f / weights_sum;
            for (uint weight_index = 0; weight_index < 4; ++weight_index)
            {
                weights[weight_index] *= weights_sum_reciprocal;
            }
            displacements[sample_index] =   normal_map_displacements[0].w * weights[0]
                                            + normal_map_displacements[1].w * weights[1]
                                            + normal_map_displacements[2].w * weights[2]
                                            + normal_map_displacements[3].w * weights[3];
        }
        float horizontal_weight = fract(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION);
        float vertical_weight = fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION);
        float weights[4];
        {
            weights[0] = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
            weights[1] = (1.0f - horizontal_weight) * (vertical_weight);
            weights[2] = (horizontal_weight) * (1.0f - vertical_weight);
            weights[3] = (horizontal_weight) * (vertical_weight);
            float weights_sum = 0.0f;
            for (uint weight_index = 0; weight_index < 4; ++weight_index)
            {
                weights[weight_index] = max(pow(weights[weight_index] * displacements[weight_index], TERRAIN_DISPLACEMENT_POWER), TERRAIN_MINIMUM_DISPLACEMENT_WEIGHT);
                weights_sum += weights[weight_index];
            }
            float weights_sum_reciprocal = 1.0f / weights_sum;
            for (uint weight_index = 0; weight_index < 4; ++weight_index)
            {
                weights[weight_index] *= weights_sum_reciprocal;
            }
        }
	    float final_displacement =  displacements[0] * weights[0]
                                    + displacements[1] * weights[1]
                                    + displacements[2] * weights[2]
                                    + displacements[3] * weights[3];
        OutWorldPosition += normal * mix(-0.125f, 0.1875f, final_displacement) * displacement_multiplier; //Slight bias for upward displacement.
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition,1.0f);
}     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        RotateYaw(vf3;f1;        X        angle        RandomFloat(vf2;         coordinate    !   CalculateTerrainMaterialCoordinate(vf3;vf3;vf2;      world_position       normal        tile_index    $   unpacked      I   sine      L   cosine    O   temp      t   absolute_normal   y   x_dominant    �   z_dominant    �   material_coordinate   �   random_rotation   �   param     �   param     �   param     �   stitched_position     �   InPosition    �   is_left_multiplier    �   InBorders     �   PushConstantData      �       WORLD_POSITION    �      MINIMUM_HEIGHT_MAP_COORDINATE     �      MAXIMUM_HEIGHT_MAP_COORDINATE     �      BORDERS  
 �      PATCH_RESOLUTION_RECIPROCAL   �      PATCH_SIZE   
 �      HEIGHT_MAP_TEXTURE_INDEX     
 �      NORMAL_MAP_TEXTURE_INDEX     	 �      INDEX_MAP_TEXTURE_INDEX  	 �   	   BLEND_MAP_TEXTURE_INDEX   �   
   MAP_RESOLUTION   
 �      MAP_RESOLUTION_RECIPROCAL     �         �   is_right_multiplier   �   vertical_offset   �   is_down_multiplier    �   is_up_multiplier        horizontal_offset       is_left_multiplier    "  is_right_multiplier   +  vertical_offset   0  is_down_multiplier    9  is_up_multiplier      B  horizontal_offset     Z  height_map_coordinate     m  OutWorldPosition      �  height_1      �  TEXTURES      �  HEIGHT_MAP_SAMPLER    �  height_2      �  height_3      �  height_4      �  blend_1   �  blend_2   �  final_height      �  normals   �  NORMAL_MAP_SAMPLER    4  blend_1   A  blend_2   N  normal   
 `  OutHeightMapTextureCoordinate     b  displacement_multiplier   e  Camera   	 e      WORLD_TO_CLIP_MATRIX     	 e     WORLD_TO_CAMERA_MATRIX    e     PREVIOUS_WORLD_TO_CLIP_MATRIX     e     INVERSE_WORLD_TO_CAMERA_MATRIX    e     INVERSE_CAMERA_TO_CLIP_MATRIX    	 e     CAMERA_WORLD_POSITION    	 e     CAMERA_FORWARD_VECTOR    	 e     CURRENT_FRAME_JITTER      e     NEAR_PLANE    e  	   FAR_PLANE     g        u  base_coordinate   �  sample_coordinates    �  sample_index      �  terrain_normal    �  index_map     �  INDEX_BLEND_MAP_SAMPLER   �  blend_map     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  materials     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �       	 <  material_texture_coordinate   D  param     F  param     H  param    	 R  normal_map_displacements      W  MATERIAL_SAMPLER      ^  param     s  param     �  param     �  param     �  weights_sum   �  weight_index      �  weights   �  weights_sum_reciprocal    �  weight_index      �  displacements     �  horizontal_weight     �  vertical_weight   �  weights     weights_sum     weight_index      /  weights_sum_reciprocal    2  weight_index      B  final_displacement    e  gl_PerVertex      e      gl_Position   e     gl_PointSize      e     gl_ClipDistance   e     gl_CullDistance   g       	 t  GlobalTextureAverageValues   	 t      TEXTURE_AVERAGE_VALUES    v        y  BLUE_NOISE_TEXTURES   }  SKY_TEXTURE G  �          G  �         H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   H  �      #   (   H  �      #   ,   H  �   	   #   0   H  �   
   #   4   H  �      #   8   G  �      G  m         G  �  "       G  �  !       G  �  "      G  �  !      G  �  "      G  �  !      G  `        H  e         H  e      #       H  e            H  e        H  e     #   @   H  e           H  e        H  e     #   �   H  e           H  e        H  e     #   �   H  e           H  e        H  e     #      H  e           H  e     #   @  H  e     #   P  H  e     #   `  H  e     #   h  H  e  	   #   l  G  e     G  g  "      G  g  !       G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  W  "      G  W  !      H  e             H  e           H  e           H  e           G  e     G  s        H  t      #       G  t     G  v  "       G  v  !      G  y  "       G  y  !      G  }  "       G  }  !           !                                   	         
   	      !     
           	                           	   !                   	                  !     	      !                    #      
   +     &   �   +  	   )     C+     +         .          +  .   /      +     4      +  .   7      +     <      +  .   ?      +     D      +  	   j   9�OA+  	   k   Lw�B,     l   j   k   +  	   o   ��*G  w      x      w   +  	   �       +  	   �     �?+  	   �      ?+  	   �   �I�+  	   �   �I@   �         ;  �   �         �         ;  �   �        �               	   	               	   	      �   	   �   ;  �   �   	   +  .   �         �   	      +     �      +     �      +     �   @   +  .           	  	   	   +     $     +     2      +     ;  �   +  	   I     @+  .   [     +  .   ^        l        ;  l  m     +  .   n      +  .   q        }     	    	 �  	                         +     �       �  �  �     �      �  ;  �  �      +  .   �        �      �    �     �      �  ;  �  �        �  �  ,     �  �   �   +  .   �     ,     �  �   �   ,     �  �   �   ,     �  �   �   +  .   �  
     �     �      �     �  +  .   �     ;  �  �         _        ;  _  `       d  
        e  d  d  d  d  d           	   	      f     e  ;  f  g        h        +  	   m    �B  �     �      �     �  ;  �  �      +  .   �  	    
 �                 	           �  �  �      �     �   
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �        �     �     �     �    P  
   �      Q     P  ;  �  W        �  	   �      �     �  +  	   �     A+  	   �     4+  	   [     �+  	   \    @>  d  	   4     e  
   	   d  d     f     e  ;  f  g        h     d     q     
     s  
   �    t  s     u     t  ;  u  v       w  �  2     x      w  ;  x  y       	 z  	                           {  z     |      {  ;  |  }           1     6               �     ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;          ;          ;     "     ;     +     ;     0     ;     9     ;     B     ;     Z     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  �  �     ;     4     ;     A     ;     N     ;     b     ;     u     ;  �  �     ;     �     ;     �     ;  #   �     ;  #   �     ;  �  �     ;     <     ;     D     ;     F     ;     H     ;  Q  R     ;     ^     ;     s     ;     �     ;     �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;  �  �     ;          ;          ;     /     ;     2     ;     B          3      =     �   �   >  �   �        5      =     �   �   �     �   �   4   A  �   �   �   �   =     �   �   �     �   �   4   �     �   �   �   p  	   �   �   >  �   �        6      =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        7      =  	   �   �   =  	   �   �   �  	   �   �   �     	   �      %   �   �   >  �   �        8      =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        9      =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   �        �   �   �       �      p  	       >  �          :      =  	     �   =  	     �   �  	           	        %     �   >           ;      A  	  
  �     =  	     
  =  	     �   �  	         A       �   4   =  	       �  	         A       �   4   >           <      A  	    �     =  	       =  	       �  	         A       �   +   =  	       �  	         A       �   +   >           ?      =       �   �         <   A  �     �   �   =         �         <   �            p  	   !     >    !       @      =     #  �   �     %  #  $  A  �   &  �   �   =     '  &  �     (  '  $  �     )  %  (  p  	   *  )  >  "  *       A      =  	   ,    =  	   -  "  �  	   .  ,  -    	   /     %   .  �   >  +  /       B      =     1  �   �     3  1  2  A  �   4  �   �   =     5  4  �     6  5  2  �     7  3  6  p  	   8  7  >  0  8       C      =     :  �   �     <  :  ;  A  �   =  �   �   =     >  =  �     ?  >  ;  �     @  <  ?  p  	   A  @  >  9  A       D      =  	   C  0  =  	   D  9  �  	   E  C  D    	   F     %   E  �   >  B  F       E      A  	  G  �     =  	   H  G  �  	   J  H  I  =  	   K  +  �  	   L  J  K  A     M  �   4   =  	   N  M  �  	   O  N  L  A     P  �   4   >  P  O       F      A  	  Q  �     =  	   R  Q  �  	   S  R  I  =  	   T  B  �  	   U  S  T  A     V  �   +   =  	   W  V  �  	   X  W  U  A     Y  �   +   >  Y  X       H      A  	  \  �   [  +   =  	   ]  \  A  	  _  �   ^  +   =  	   `  _  A     a  �   +   =  	   b  a    	   c     .   ]  `  b  A  	  d  �   [  4   =  	   e  d  A  	  f  �   ^  4   =  	   g  f  A     h  �   4   =  	   i  h    	   j     .   e  g  i  P     k  c  j  >  Z  k       I      A  	  o  �   n  +   =  	   p  o  A  	  r  �   q  =  	   s  r  �  	   t  s  �     	   u  t  A  	  v  �   q  =  	   w  v  �  	   x  w  �   A     y  �   +   =  	   z  y    	   {     .   u  x  z  �  	   |  p  {  A  }  ~  m  +   >  ~  |       J      A  }    m  4   >    �        K      A  	  �  �   n  4   =  	   �  �  A  	  �  �   q  =  	   �  �  �  	   �  �  �     	   �  �  A  	  �  �   q  =  	   �  �  �  	   �  �  �   A     �  �   4   =  	   �  �    	   �     .   �  �  �  �  	   �  �  �  A  }  �  m  <   >  �  �       M      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �       N      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �       O      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �       P      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �       Q      =  	   �  �  =  	   �  �  A     �  Z  4   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �       R      =  	   �  �  =  	   �  �  A     �  Z  4   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �       S      =  	   �  �  =  	   �  �  A     �  Z  +   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �       T      =  	   �  �  A  }  �  m  4   >  �  �       Y      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �     �  V  �    �     =       Z  A  	    �   �  =  	       �       �    �           X  
            �   O                     A     	  �  n  >  	         Z      A  �   
  �   �  =       
  A  �    �    =  �      =  �    �  V  �        =       Z  A  	    �   �  =  	       �       �    �           X  
            �   O                     A       �  [  >           [      A  �     �   �  =         A  �    �    =  �      =  �    �  V  �        =       Z  A  	    �   �  =  	        �     !  �     �     "    !  X  
   #    "     �   O     $  #  #            A     %  �  ^  >  %  $       \      A  �   &  �   �  =     '  &  A  �  (  �  '  =  �  )  (  =  �  *  �  V  �  +  )  *  =     ,  Z  A  	  -  �   �  =  	   .  -  �     /  �  .  �     0  ,  /  X  
   1  +  0     �   O     2  1  1            A     3  �  �   >  3  2       ]      A     5  �  n  =     6  5  A     7  �  [  =     8  7  A     9  Z  4   =  	   :  9  A  	  ;  �   �  =  	   <  ;  �  	   =  :  <    	   >     
   =  P     ?  >  >  >       @     .   6  8  ?  >  4  @       ^      A     B  �  ^  =     C  B  A     D  �  �   =     E  D  A     F  Z  4   =  	   G  F  A  	  H  �   �  =  	   I  H  �  	   J  G  I    	   K     
   J  P     L  K  K  K       M     .   C  E  L  >  A  M       _      =     O  4  =     P  A  A     Q  Z  +   =  	   R  Q  A  	  S  �   �  =  	   T  S  �  	   U  R  T    	   V     
   U  P     W  V  V  V       X     .   O  P  W  >  N  X       `      =     Y  N  �     Z  Y  I  P     [  �   �   �   �     \  Z  [  >  N  \       a      =     ]  N       ^     E   ]  >  N  ^       c      =     a  Z  >  `  a       d      =     c  m  A  h  i  g  q  =     j  i  �     k  c  j    	   l     B   k  �  	   n  l  m    	   o     %   n  �   �  	   p  �   o  >  b  p       e      =  	   q  b  �  w   r  q  �   �  t      �  r  s  t  �  s       i      A  }  v  `  +   =  	   w  v  A  	  x  �   �  =  	   y  x  �  	   z  w  y  m     {  z  p  	   |  {  �  	   }  |  �        j      A  }  ~  `  4   =  	     ~  A  	  �  �   �  =  	   �  �  �  	   �    �  m     �  �  p  	   �  �  �  	   �  �  �   P     �  }  �       k      A  	  �  �   �  =  	   �  �  �     �  �  �       g      >  u  �       m      =     �  u  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  n  >  �  �       n      =     �  u  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  [  >  �  �       o      =     �  u  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  ^  >  �  �       p      =     �  u  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  �   >  �  �       r      >  �  +   �  �  �  �       r      �  �  �      �  �  �  �       r      =     �  �  �  w   �  �  �   �  �  �  �  �  �       t      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  A     �  �  �  =     �  �  X  
   �  �  �     �   O     �  �  �            �     �  �  I  P     �  �   �   �   �     �  �  �  >  �  �       u      A  �   �  �   /   =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  A     �  �  �  =     �  �  X  
   �  �  �     �   >  �  �       v      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  A     �  �  �  =     �  �  X  
   �  �  �     �   >  �  �       x      A     �  �  +   =  	   �  �  �  	   �  �  )   m     �  �  A  �  �  �  n  �  =  �  �  �  A  �  �  �  n  Q     �  �      A     �  �  n  >  �  �  Q     �  �     A     �  �  [  >  �  �  Q     �  �     A     �  �  ^  >  �  �  Q     �  �     A     �  �  �   >  �  �  Q     �  �     A     �  �    >  �  �  Q  	   �  �     A     �  �  q  >  �  �  Q     �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �  >  �  �       y      A     �  �  4   =  	   �  �  �  	   �  �  )   m     �  �  A  �  �  �  n  �  =  �  �  �  A  �  �  �  [  Q     �  �      A     �  �  n  >  �  �  Q        �     A       �  [  >       Q       �     A       �  ^  >      Q       �     A       �  �   >      Q       �     A       �    >      Q  	     �     A     	  �  q  >  	    Q     
  �     A       �  �  >    
  Q       �     A       �  �  >           z      A       �  <   =  	       �  	       )   m         A  �    �  n    =  �      A  �    �  ^  Q             A         n  >      Q            A         [  >      Q            A         ^  >      Q            A         �   >      Q            A           >      Q  	          A          q  >       Q     !       A     "    �  >  "  !  Q     #       A     $    �  >  $  #       {      A     %  �  D   =  	   &  %  �  	   '  &  )   m     (  '  A  �  )  �  n  (  =  �  *  )  A  �  +  �  �   Q     ,  *      A     -  +  n  >  -  ,  Q     .  *     A     /  +  [  >  /  .  Q     0  *     A     1  +  ^  >  1  0  Q     2  *     A     3  +  �   >  3  2  Q     4  *     A     5  +    >  5  4  Q  	   6  *     A     7  +  q  >  7  6  Q     8  *     A     9  +  �  >  9  8  Q     :  *     A     ;  +  �  >  ;  :       |      =     =  �  A     >  �  =  =     ?  >  A  	  @  �   �  =  	   A  @  �     B  ?  A  �     C  B  �   =     E  m  >  D  E  =     G  �  >  F  G  >  H  C  9     I  !   D  F  H  >  <  I       ~      A     J  �  n  n  =     K  J  �     L  K  �   �  w   M  L  �   �  O      �  M  N  ]  �  N       ~      A     S  �  n  ^  =     T  S  A  �  U  �  T  =  �  V  U  =  �  X  W  V  �  Y  V  X  =     Z  <  X  
   [  Y  Z     �   A  #   \  R  n  >  \  [  �  O  �  ]       ~      A     _  �  n  ^  =     `  _  >  ^  `  9  
   a     ^  A  #   b  R  n  >  b  a  �  O  �  O             A     c  �  [  n  =     d  c  �     e  d  �   �  w   f  e  �   �  h      �  f  g  r  �  g             A     i  �  [  ^  =     j  i  A  �  k  �  j  =  �  l  k  =  �  m  W  V  �  n  l  m  =     o  <  X  
   p  n  o     �   A  #   q  R  [  >  q  p  �  h  �  r             A     t  �  [  ^  =     u  t  >  s  u  9  
   v     s  A  #   w  R  [  >  w  v  �  h  �  h       �      A     x  �  ^  n  =     y  x  �     z  y  �   �  w   {  z  �   �  }      �  {  |  �  �  |       �      A     ~  �  ^  ^  =       ~  A  �  �  �    =  �  �  �  =  �  �  W  V  �  �  �  �  =     �  <  X  
   �  �  �     �   A  #   �  R  ^  >  �  �  �  }  �  �       �      A     �  �  ^  ^  =     �  �  >  �  �  9  
   �     �  A  #   �  R  ^  >  �  �  �  }  �  }       �      A     �  �  �   n  =     �  �  �     �  �  �   �  w   �  �  �   �  �      �  �  �  �  �  �       �      A     �  �  �   ^  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  W  V  �  �  �  �  =     �  <  X  
   �  �  �     �   A  #   �  R  �   >  �  �  �  �  �  �       �      A     �  �  �   ^  =     �  �  >  �  �  9  
   �     �  A  #   �  R  �   >  �  �  �  �  �  �       �      >  �  �        �      >  �  +   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �  w   �  �  �   �  �  �  �  �  �       �      =     �  �  =     �  �  A     �  �  �  =  	   �  �  =     �  �  A     �  R  �  D   =  	   �  �  �  	   �  �  �    	   �        �  �    	   �     (   �  �  A     �  �  �  >  �  �       �      =     �  �  A     �  �  �  =  	   �  �  =  	   �  �  �  	   �  �  �  >  �  �  �  �  �  �       �      =     �  �  �     �  �  [  >  �  �  �  �  �  �       �      =  	   �  �  �  	   �  �   �  >  �  �       �      >  �  +   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �  w   �  �  �   �  �  �  �  �  �       �      =     �  �  =  	   �  �  A     �  �  �  =  	   �  �  �  	   �  �  �  A     �  �  �  >  �  �  �  �  �  �       �      =     �  �  �     �  �  [  >  �  �  �  �  �  �       �      =     �  �  A     �  R  n  D   =  	   �  �  A     �  �  n  =  	   �  �  �  	   �  �  �       �      A     �  R  [  D   =  	   �  �  A     �  �  [  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  R  ^  D   =  	   �  �  A     �  �  ^  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  R  �   D   =  	   �  �  A     �  �  �   =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  �  �  >  �  �  �  �  �  �       r      =     �  �  �     �  �  [  >  �  �  �  �  �  �       �      A  }  �  `  +   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  >  �  �       �      A  }  �  `  4   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  >  �  �       �      =  	      �  �  	     �      =  	     �  �  	     �     �  	         A       �  n  >           �      =  	     �  �  	     �     =  	     �  �  	   	      A     
  �  [  >  
  	       �      =  	     �  =  	     �  �  	     �     �  	         A       �  ^  >           �      =  	     �  =  	     �  �  	         A       �  �   >           �      >    �        �      >    +   �    �         �      �          �    �         �      =         �  w       �   �        �         �      =         =         A       �    =  	        =     !    A     "  �  !  =  	   #  "  �  	   $     #    	   %        $  �    	   &     (   %  �  A     '  �    >  '  &       �      =     (    A     )  �  (  =  	   *  )  =  	   +    �  	   ,  +  *  >    ,  �    �         �      =     -    �     .  -  [  >    .  �    �         �      =  	   0    �  	   1  �   0  >  /  1       �      >  2  +   �  3  �  3       �      �  5  6      �  7  �  7       �      =     8  2  �  w   9  8  �   �  9  4  5  �  4       �      =     :  2  =  	   ;  /  A     <  �  :  =  	   =  <  �  	   >  =  ;  A     ?  �  :  >  ?  >  �  6  �  6       �      =     @  2  �     A  @  [  >  2  A  �  3  �  5       �      A     C  �  n  =  	   D  C  A     E  �  n  =  	   F  E  �  	   G  D  F       �      A     H  �  [  =  	   I  H  A     J  �  [  =  	   K  J  �  	   L  I  K  �  	   M  G  L       �      A     N  �  ^  =  	   O  N  A     P  �  ^  =  	   Q  P  �  	   R  O  Q  �  	   S  M  R       �      A     T  �  �   =  	   U  T  A     V  �  �   =  	   W  V  �  	   X  U  W  �  	   Y  S  X       �      >  B  Y       �      =     Z  N  =  	   ]  B    	   ^     .   [  \  ]  �     _  Z  ^  =  	   `  b  �     a  _  `  =     b  m  �     c  b  a  >  m  c  �  t  �  t       �      A  h  i  g  n  =  d  j  i  =     k  m  Q  	   l  k      Q  	   m  k     Q  	   n  k     P  
   o  l  m  n  �   �  
   p  j  o  A  q  r  g  n  >  r  p  �  8       �      6  
             7        �     ;  #   $                 =     %      �     '   %   &   p  	   (   '   �  	   *   (   )   A     ,   $   +   >  ,   *              =     -      �     0   -   /   �     1   0   &   p  	   2   1   �  	   3   2   )   A     5   $   4   >  5   3              =     6      �     8   6   7   �     9   8   &   p  	   :   9   �  	   ;   :   )   A     =   $   <   >  =   ;              =     >      �     @   >   ?   �     A   @   &   p  	   B   A   �  	   C   B   )   A     E   $   D   >  E   C              =  
   F   $   �  F   8       u  #   6               7        7        �     ;     I      ;     L      ;     O           w      =  	   J        	   K         J   >  I   K        x      =  	   M        	   N         M   >  L   N        z      A     P      +   =  	   Q   P   =  	   R   L   �  	   S   Q   R   A     T      <   =  	   U   T   =  	   V   I   �  	   W   U   V   �  	   X   S   W   >  O   X        {      A     Y      +   =  	   Z   Y     	   [   Z   =  	   \   I   �  	   ]   [   \   A     ^      <   =  	   _   ^   =  	   `   L   �  	   a   _   `   �  	   b   ]   a   A     c      <   >  c   b        |      =  	   d   O   A     e      +   >  e   d        ~      =     f      �  f   8       �  "   6  	             7        �          �      =     i      �  	   m   i   l     	   n         m   �  	   p   n   o     	   q      
   p   �  q   8       �  Z   6     !          7        7        7         �  "   ;     t      ;  x   y      ;  x   �      ;     �      ;     �      ;     �      ;     �      ;     �                 =     u           v         u   >  t   v              A     z   t   +   =  	   {   z   A     |   t   4   =  	   }   |   �  w   ~   {   }   �  �       �  ~      �   �                A     �   t   +   =  	   �   �   A     �   t   <   =  	   �   �   �  w   �   �   �   �  �   �  �   �  w   �   ~   "   �                 >  y   �              A     �   t   <   =  	   �   �   A     �   t   +   =  	   �   �   �  w   �   �   �   �  �       �  �   �   �   �  �              A     �   t   <   =  	   �   �   A     �   t   4   =  	   �   �   �  w   �   �   �   �  �   �  �   �  w   �   �   �   �   �              >  �   �              =     �      O     �   �   �         =  w   �   y   �  	   �   �   �   �   �     �   �   �   =     �      O     �   �   �          =  w   �   �   �  	   �   �   �   �   �     �   �   �   �     �   �   �   =     �      O     �   �   �          =  w   �   y   �  w   �   �   =  w   �   �   �  w   �   �   �  w   �   �   �   �  	   �   �   �   �   �     �   �   �   �     �   �   �   >  �   �              =     �   �   �     �   �   �   >  �   �              =     �       >  �   �   9  	   �      �     	   �      .   �   �   �   >  �   �              =     �       =     �   �   �     �   �   �   >  �   �              =     �   �   P     �   �   �   �     �   �   �   >  �   �              A     �   �   +   =  	   �   �   A     �   �   4   =  	   �   �   P     �   �   �   �   >  �   �   =  	   �   �   >  �   �   9     �      �   �   O     �   �   �          >  �   �              =     �   �   P     �   �   �   �     �   �   �   >  �   �              =     �       =     �   �   �     �   �   �   >  �   �              =     �   �   �  �   8       #     \                GLSL.std.450                     main    =  $  6  ?  B  I  L               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Terrain_Fragment.glsl    o   �     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
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
#define UINT32_MAXIMUM (0xffffffff)
#define FLOAT32_EPSILON (1.192092896e-07F)
#define MAXIMUM_8_BIT_FLOAT (255.0f)
#define MAXIMUM_8_BIT_UINT (255)
#define UINT32_MAXIMUM_RECIPROCAL (2.328306437080797e-10f)

#define DIVIDE_BY_ZERO_SAFE_EPSILON (FLOAT32_EPSILON * 1.0f)

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
*	Evaluates the average albedo/thickness of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_ALBEDO_THICKNESS(MATERIAL, COORDINATE, SAMPLER, ALBEDO_THICKNESS)					\
{																											\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_ALBEDO_THICKNESS_TEXTURE))							\
	{																										\
		ALBEDO_THICKNESS = TEXTURE_AVERAGE_VALUES[MATERIAL._AlbedoThickness];								\
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
*	Evaluates the average normal map/displacement of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_NORMAL_MAP_DISPLACEMENT(MATERIAL, COORDINATE, SAMPLER, NORMAL_MAP_DISPLACEMENT)				\
{																														\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_NORMAL_MAP_DISPLACEMENT_TEXTURE))								\
	{																													\
		NORMAL_MAP_DISPLACEMENT = TEXTURE_AVERAGE_VALUES[MATERIAL._NormalMapDisplacement];								\
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
*	Evaluates the average material properties of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_MATERIAL_PROPERTIES(MATERIAL, COORDINATE, SAMPLER, MATERIAL_PROPERTIES)					\
{																													\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_MATERIAL_PROPERTIES_TEXTURE))								\
	{																												\
		MATERIAL_PROPERTIES = TEXTURE_AVERAGE_VALUES[MATERIAL._MaterialProperties];									\
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
*	Evaluates the average opacity of the given material at the given coordinate with the given sampler.
*/
#define EVALUATE_AVERAGE_OPACITY(MATERIAL, COORDINATE, SAMPLER, OPACITY)					\
{																							\
	if (TEST_BIT(MATERIAL._Properties, MATERIAL_PROPERTY_OPACITY_TEXTURE))					\
	{																						\
		OPACITY = TEXTURE_AVERAGE_VALUES[MATERIAL._Opacity];								\
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

//The texture average values.
layout (std140, set = 0, binding = 1) uniform GlobalTextureAverageValues
{
    layout (offset = 0) vec4 TEXTURE_AVERAGE_VALUES[MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES];
};

//Materials.
layout (std140, set = 0, binding = 2) uniform GlobalMaterials
{
    layout (offset = 0) Material MATERIALS[MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS];
};

//The blue noise textures.
layout (set = 0, binding = 3) uniform sampler2D BLUE_NOISE_TEXTURES[NUMBER_OF_BLUE_NOISE_TEXTURES];

//The sky texture.
layout (set = 0, binding = 4) uniform samplerCube SKY_TEXTURE;

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
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
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

layout (set = 1, binding = 1) uniform sampler HEIGHT_MAP_SAMPLER;
layout (set = 1, binding = 2) uniform sampler NORMAL_MAP_SAMPLER;
layout (set = 1, binding = 3) uniform sampler INDEX_BLEND_MAP_SAMPLER;
layout (set = 1, binding = 4) uniform sampler MATERIAL_SAMPLER;

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
*   Combines two hashes.
*/
uint CombineHash(uint hash_1, uint hash_2)
{
    return 3141592653 * hash_1 + hash_2;
}

/*
*   Hash function taking a uint.
*/
uint Hash(uint seed)
{
    seed ^= seed >> 17;
    seed *= 0xed5ad4bbU;
    seed ^= seed >> 11;
    seed *= 0xac4c1b51U;
    seed ^= seed >> 15;
    seed *= 0x31848babU;
    seed ^= seed >> 14;
    return seed;
}

/*
*   Hash function taking a uvec2.
*/
uint Hash2(uvec2 seed)
{
    return Hash(seed.x) ^ Hash(seed.y);
}

/*
*   Hash function taking a uvec3.
*/
uint Hash3(uvec3 seed)
{
    //return Hash( Hash( Hash( Hash(seed.x) ^ Hash(seed.y) ^ Hash(seed.z) ) ) );
    //return Hash( Hash( Hash(seed.x) + Hash(seed.y) ) + Hash(seed.z) );
    return Hash( CombineHash(CombineHash(Hash(seed.x), Hash(seed.y)), Hash(seed.z)) );
}

/*
*   Given a seed, returns a random number.
*/
float RandomFloat(inout uint seed)
{
    return Hash(seed) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate and a seed, returns a random number.
*/
float RandomFloat(uvec2 coordinate, uint seed)
{
    return float(Hash3(uvec3(coordinate.xy, seed))) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate, returns a random number.
*/
float RandomFloat(vec2 coordinate)
{
    return fract(sin(dot(coordinate, vec2(12.9898f, 78.233f))) * 43758.5453f);
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

//Constants.
#define TERRAIN_MATERIAL_SCALE (0.5f)
#define TERRAIN_DISPLACEMENT_POWER (8.0f)
#define TERRAIN_MINIMUM_DISPLACEMENT_WEIGHT (FLOAT32_EPSILON)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    //The terrain normal.
    vec3 _TerrainNormal;

    //The terrain height.
    float _TerrainHeight;

    //The albedo.
    vec3 _Albedo;

    //The thickness.
    float _Thickness;

    //The normal map.
    vec3 _NormalMap;

    //The displacement.
    float _Displacement;

    //The material properties.
    vec4 _MaterialProperties;
};

/*
*   Calculates the material coordinate for the given world position.
*/
vec2 CalculateTerrainMaterialCoordinate(vec3 world_position, vec3 normal, vec2 tile_index)
{
#if 1
    //Take the absolute of the normal.
    vec3 absolute_normal = abs(normal);

    //Calculate the material coordinate.
    bool x_dominant = absolute_normal.x > absolute_normal.y && absolute_normal.x > absolute_normal.z;
    bool z_dominant = absolute_normal.z > absolute_normal.x && absolute_normal.z > absolute_normal.y;
    vec2 material_coordinate = world_position.yz * float(x_dominant) + world_position.xy * float(z_dominant) + world_position.xz * float(!x_dominant && !z_dominant);

    material_coordinate *= TERRAIN_MATERIAL_SCALE;

    //Calculate the random rotation.
    float random_rotation = mix(-PI, PI, RandomFloat(tile_index));

    //Randomly rotate the tile.
    material_coordinate -= tile_index;
    material_coordinate -= 0.5f;
    material_coordinate = RotateYaw(vec3(material_coordinate.x, 0.0f, material_coordinate.y), random_rotation).xz;
    material_coordinate += 0.5f;
    material_coordinate += tile_index;

    //Return the material_coordinate!
    return material_coordinate;
#else
    return vec2(world_position.x, world_position.z) * 0.5f;
#endif
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 WORLD_POSITION;
	layout (offset = 8) vec2 MINIMUM_HEIGHT_MAP_COORDINATE;
	layout (offset = 16) vec2 MAXIMUM_HEIGHT_MAP_COORDINATE;
	layout (offset = 24) uint BORDERS;
	layout (offset = 28) float PATCH_RESOLUTION_RECIPROCAL;
	layout (offset = 32) float PATCH_SIZE;
	layout (offset = 36) uint HEIGHT_MAP_TEXTURE_INDEX;
	layout (offset = 40) uint NORMAL_MAP_TEXTURE_INDEX;
	layout (offset = 44) uint INDEX_MAP_TEXTURE_INDEX;
	layout (offset = 48) uint BLEND_MAP_TEXTURE_INDEX;
	layout (offset = 52) float MAP_RESOLUTION;
	layout (offset = 56) float MAP_RESOLUTION_RECIPROCAL;
};

layout (location = 0) in vec3 InWorldPosition;
layout (location = 1) in vec2 InHeightMapTextureCoordinate;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;

void main()
{
    vec2 base_coordinate = vec2
    (
        float(uint(InHeightMapTextureCoordinate.x * MAP_RESOLUTION)) + 0.5f,
        float(uint(InHeightMapTextureCoordinate.y * MAP_RESOLUTION)) + 0.5f
    ) * MAP_RESOLUTION_RECIPROCAL;
    vec2 sample_coordinates[4];
    sample_coordinates[0] = base_coordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
    sample_coordinates[1] = base_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
    sample_coordinates[2] = base_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
    sample_coordinates[3] = base_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
    TerrainMaterial terrain_materials[4];
    for (uint sample_index = 0; sample_index < 4; ++sample_index)
    {
        vec3 terrain_normal = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), sample_coordinates[sample_index]).xyz * 2.0f - 1.0f;
        vec4 index_map = texture(sampler2D(TEXTURES[INDEX_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), sample_coordinates[sample_index]);
        vec4 blend_map = texture(sampler2D(TEXTURES[BLEND_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), sample_coordinates[sample_index]);
        Material materials[4];
        materials[0] = MATERIALS[uint(index_map[0] * float(UINT8_MAXIMUM))];
        materials[1] = MATERIALS[uint(index_map[1] * float(UINT8_MAXIMUM))];
        materials[2] = MATERIALS[uint(index_map[2] * float(UINT8_MAXIMUM))];
        materials[3] = MATERIALS[uint(index_map[3] * float(UINT8_MAXIMUM))];
        vec2 material_texture_coordinate = CalculateTerrainMaterialCoordinate(InWorldPosition, terrain_normal, sample_coordinates[sample_index] * MAP_RESOLUTION * TERRAIN_MATERIAL_SCALE);
        vec4 normal_map_displacements[4];
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[0], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[0]);
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[1], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[1]);
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[2], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[2]);
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[3], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[3]);
        float weights[4];
        float weights_sum = 0.0f;
        for (uint weight_index = 0; weight_index < 4; ++weight_index)
        {
            weights[weight_index] = max(pow(blend_map[weight_index] * normal_map_displacements[weight_index].w, TERRAIN_DISPLACEMENT_POWER), TERRAIN_MINIMUM_DISPLACEMENT_WEIGHT);
            weights_sum += weights[weight_index];
        }
        float weights_sum_reciprocal = 1.0f / weights_sum;
        for (uint weight_index = 0; weight_index < 4; ++weight_index)
        {
            weights[weight_index] *= weights_sum_reciprocal;
        }
        vec4 albedo_thicknesses[4];
        EVALUATE_ALBEDO_THICKNESS(materials[0], material_texture_coordinate, MATERIAL_SAMPLER, albedo_thicknesses[0]);
        EVALUATE_ALBEDO_THICKNESS(materials[1], material_texture_coordinate, MATERIAL_SAMPLER, albedo_thicknesses[1]);
        EVALUATE_ALBEDO_THICKNESS(materials[2], material_texture_coordinate, MATERIAL_SAMPLER, albedo_thicknesses[2]);
        EVALUATE_ALBEDO_THICKNESS(materials[3], material_texture_coordinate, MATERIAL_SAMPLER, albedo_thicknesses[3]);
        vec4 material_properties[4];
        EVALUATE_MATERIAL_PROPERTIES(materials[0], material_texture_coordinate, MATERIAL_SAMPLER, material_properties[0]);
        EVALUATE_MATERIAL_PROPERTIES(materials[1], material_texture_coordinate, MATERIAL_SAMPLER, material_properties[1]);
        EVALUATE_MATERIAL_PROPERTIES(materials[2], material_texture_coordinate, MATERIAL_SAMPLER, material_properties[2]);
        EVALUATE_MATERIAL_PROPERTIES(materials[3], material_texture_coordinate, MATERIAL_SAMPLER, material_properties[3]);
        terrain_materials[sample_index]._TerrainNormal = terrain_normal;
        terrain_materials[sample_index]._TerrainHeight = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), sample_coordinates[sample_index]).x;
        terrain_materials[sample_index]._Albedo =   albedo_thicknesses[0].rgb * weights[0]
                                                    + albedo_thicknesses[1].rgb * weights[1]
                                                    + albedo_thicknesses[2].rgb * weights[2]
                                                    + albedo_thicknesses[3].rgb * weights[3];
        terrain_materials[sample_index]._Thickness =    albedo_thicknesses[0].w * weights[0]
                                                        + albedo_thicknesses[1].w * weights[1]
                                                        + albedo_thicknesses[2].w * weights[2]
                                                        + albedo_thicknesses[3].w * weights[3];
        terrain_materials[sample_index]._NormalMap =    normal_map_displacements[0].xyz * weights[0]
                                                        + normal_map_displacements[1].xyz * weights[1]
                                                        + normal_map_displacements[2].xyz * weights[2]
                                                        + normal_map_displacements[3].xyz * weights[3];
        terrain_materials[sample_index]._Displacement = normal_map_displacements[0].w * weights[0]
                                                        + normal_map_displacements[1].w * weights[1]
                                                        + normal_map_displacements[2].w * weights[2]
                                                        + normal_map_displacements[3].w * weights[3];
        terrain_materials[sample_index]._MaterialProperties =   material_properties[0] * weights[0]
                                                                + material_properties[1] * weights[1]
                                                                + material_properties[2] * weights[2]
                                                                + material_properties[3] * weights[3];
    }
    float horizontal_weight = fract(InHeightMapTextureCoordinate.x * MAP_RESOLUTION);
    float vertical_weight = fract(InHeightMapTextureCoordinate.y * MAP_RESOLUTION);
    float weights[4];
    float biased_weights[4];
    {
        weights[0] = biased_weights[0] = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
        weights[1] = biased_weights[1] = (1.0f - horizontal_weight) * (vertical_weight);
        weights[2] = biased_weights[2] = (horizontal_weight) * (1.0f - vertical_weight);
        weights[3] = biased_weights[3] = (horizontal_weight) * (vertical_weight);
        float weights_sum = 0.0f;
        float biased_weights_sum = 0.0f;
        for (uint weight_index = 0; weight_index < 4; ++weight_index)
        {
            biased_weights[weight_index] = max(pow(biased_weights[weight_index] * terrain_materials[weight_index]._Displacement, TERRAIN_DISPLACEMENT_POWER), TERRAIN_MINIMUM_DISPLACEMENT_WEIGHT);
            weights_sum += weights[weight_index];
            biased_weights_sum += biased_weights[weight_index];
        }
        float weights_sum_reciprocal = 1.0f / weights_sum;
        float biased_weights_sum_reciprocal = 1.0f / biased_weights_sum;
        for (uint weight_index = 0; weight_index < 4; ++weight_index)
        {
            weights[weight_index] *= weights_sum_reciprocal;
            biased_weights[weight_index] *= biased_weights_sum_reciprocal;
        }
    }
    TerrainMaterial final_terrain_material = terrain_materials[0];
    final_terrain_material._TerrainNormal =         terrain_materials[0]._TerrainNormal * weights[0]
                                                    + terrain_materials[1]._TerrainNormal * weights[1]
                                                    + terrain_materials[2]._TerrainNormal * weights[2]
                                                    + terrain_materials[3]._TerrainNormal * weights[3];
    final_terrain_material._TerrainHeight =         terrain_materials[0]._TerrainHeight * weights[0]
                                                    + terrain_materials[1]._TerrainHeight * weights[1]
                                                    + terrain_materials[2]._TerrainHeight * weights[2]
                                                    + terrain_materials[3]._TerrainHeight * weights[3];
    final_terrain_material._Albedo =                terrain_materials[0]._Albedo * biased_weights[0]
                                                    + terrain_materials[1]._Albedo * biased_weights[1]
                                                    + terrain_materials[2]._Albedo * biased_weights[2]
                                                    + terrain_materials[3]._Albedo * biased_weights[3];
    final_terrain_material._Thickness =             terrain_materials[0]._Thickness * biased_weights[0]
                                                    + terrain_materials[1]._Thickness * biased_weights[1]
                                                    + terrain_materials[2]._Thickness * biased_weights[2]
                                                    + terrain_materials[3]._Thickness * biased_weights[3];
    final_terrain_material._NormalMap =             terrain_materials[0]._NormalMap * biased_weights[0]
                                                    + terrain_materials[1]._NormalMap * biased_weights[1]
                                                    + terrain_materials[2]._NormalMap * biased_weights[2]
                                                    + terrain_materials[3]._NormalMap * biased_weights[3];
    final_terrain_material._Displacement =          terrain_materials[0]._Displacement * biased_weights[0]
                                                    + terrain_materials[1]._Displacement * biased_weights[1]
                                                    + terrain_materials[2]._Displacement * biased_weights[2]
                                                    + terrain_materials[3]._Displacement * biased_weights[3];
    final_terrain_material._MaterialProperties =    terrain_materials[0]._MaterialProperties * biased_weights[0]
                                                    + terrain_materials[1]._MaterialProperties * biased_weights[1]
                                                    + terrain_materials[2]._MaterialProperties * biased_weights[2]
                                                    + terrain_materials[3]._MaterialProperties * biased_weights[3];
    vec3 shading_normal;
    {
        mat3 tangent_space_matrix = CalculateGramSchmidtRotationMatrix(final_terrain_material._TerrainNormal, vec3(0.0f, 0.0f, 1.0f));
        shading_normal = normalize(tangent_space_matrix * normalize(final_terrain_material._NormalMap * 2.0f - 1.0f));
    }
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(final_terrain_material._Albedo,final_terrain_material._Thickness);
	SceneFeatures2 = vec4(shading_normal,gl_FragCoord.z);
	SceneFeatures3 = final_terrain_material._MaterialProperties;
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       RotateYaw(vf3;f1;        X        angle     #   CalculateGramSchmidtRotationMatrix(vf3;vf3;   !   normal    "   random_tilt   (   RandomFloat(vf2;      '   coordinate    .   CalculateTerrainMaterialCoordinate(vf3;vf3;vf2;   +   world_position    ,   normal    -   tile_index    1   unpacked      V   view_space_position   X   Camera   	 X       WORLD_TO_CLIP_MATRIX     	 X      WORLD_TO_CAMERA_MATRIX    X      PREVIOUS_WORLD_TO_CLIP_MATRIX     X      INVERSE_WORLD_TO_CAMERA_MATRIX    X      INVERSE_CAMERA_TO_CLIP_MATRIX    	 X      CAMERA_WORLD_POSITION    	 X      CAMERA_FORWARD_VECTOR    	 X      CURRENT_FRAME_JITTER      X      NEAR_PLANE    X   	   FAR_PLANE     Z         f   denominator   z   view_space_position   �   denominator   �   sine      �   cosine    �   temp      �   random_tangent    �   random_bitangent      �   absolute_normal   �   x_dominant    �   z_dominant      material_coordinate     random_rotation     param     -  param     .  param     ;  base_coordinate  
 =  InHeightMapTextureCoordinate      A  PushConstantData      A      WORLD_POSITION    A     MINIMUM_HEIGHT_MAP_COORDINATE     A     MAXIMUM_HEIGHT_MAP_COORDINATE     A     BORDERS  
 A     PATCH_RESOLUTION_RECIPROCAL   A     PATCH_SIZE   
 A     HEIGHT_MAP_TEXTURE_INDEX     
 A     NORMAL_MAP_TEXTURE_INDEX     	 A     INDEX_MAP_TEXTURE_INDEX  	 A  	   BLEND_MAP_TEXTURE_INDEX   A  
   MAP_RESOLUTION   
 A     MAP_RESOLUTION_RECIPROCAL     C        \  sample_coordinates    {  sample_index      �  terrain_normal    �  TEXTURES      �  NORMAL_MAP_SAMPLER    �  index_map     �  INDEX_BLEND_MAP_SAMPLER   �  blend_map     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  materials     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �       	 "  material_texture_coordinate   $  InWorldPosition   ,  param     .  param     0  param    	 ;  normal_map_displacements      @  MATERIAL_SAMPLER      G  param     \  param     q  param     �  param     �  weights_sum   �  weight_index      �  weights   �  weights_sum_reciprocal    �  weight_index      �  albedo_thicknesses    �  param     �  param     �  param       param       material_properties   &  param     ;  param     P  param     e  param     j  TerrainMaterial   j      _TerrainNormal    j     _TerrainHeight    j     _Albedo   j     _Thickness    j     _NormalMap    j     _Displacement     j     _MaterialProperties   m  terrain_materials     v  HEIGHT_MAP_SAMPLER      horizontal_weight       vertical_weight     weights     biased_weights    .  weights_sum   /  biased_weights_sum    0  weight_index      O  weights_sum_reciprocal   
 R  biased_weights_sum_reciprocal     U  weight_index      l  final_terrain_material      tangent_space_matrix        param       param       shading_normal    )  velocity      *  param     -  param     6  SceneFeatures1    ?  SceneFeatures2    B  gl_FragCoord      I  SceneFeatures3    L  SceneFeatures4   	 R  GlobalTextureAverageValues   	 R      TEXTURE_AVERAGE_VALUES    T        W  BLUE_NOISE_TEXTURES   [  SKY_TEXTURE H  X          H  X       #       H  X             H  X         H  X      #   @   H  X            H  X         H  X      #   �   H  X            H  X         H  X      #   �   H  X            H  X         H  X      #      H  X            H  X      #   @  H  X      #   P  H  X      #   `  H  X      #   h  H  X   	   #   l  G  X      G  Z   "      G  Z   !       G  =        H  A      #       H  A     #      H  A     #      H  A     #      H  A     #      H  A     #       H  A     #   $   H  A     #   (   H  A     #   ,   H  A  	   #   0   H  A  
   #   4   H  A     #   8   G  A     G  �  "       G  �  !       G  �  "      G  �  !      G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  $         G  @  "      G  @  !      G  v  "      G  v  !      G  6         G  ?        G  B        G  I        G  L        G  Q        H  R      #       G  R     G  T  "       G  T  !      G  W  "       G  W  !      G  [  "       G  [  !           !                                   	         
   	      !     
           	                       	      !                    	   !                         !                  %         !  &   	   %   !  *            %      0      
   +     3   �   +  	   6     C+     8         ;          +  ;   <      +     A      +  ;   D      +     I      +  ;   L      +     Q        W   
        X   W   W   W   W   W            	   	      Y      X   ;  Y   Z      +  ;   [          \      W   +  	   `     �?+  	   t      ?+  ;   {      +  	   �       +  	   �   9�OA+  	   �   Lw�B,     �   �   �   +  	   �   ��*G  �      �      �   +  	     �I�+  	     �I@   <        ;  <  =        >     	     A              	   	               	   	      B  	   A  ;  B  C  	   +  ;   D  
      E  	   	   +  ;   U     +     Y       Z     Y     [     Z  ,     ^  �   �   +  ;   d     ,     f  �   `   ,     m  `   �   +  ;   s     ,     u  `   `    	 �  	                         +     �       �  �  �     �      �  ;  �  �      +  ;   �        �  	         �      �    �     �      �  ;  �  �        �  �  +  	   �     @;  �  �      +  ;   �  	    
 �                 	           �  �  Y     �     �   
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �        �     �     �     �  +  ;   �     +  ;   �     +  ;   �        #        ;  #  $     +     4       9  
   Y     :     9  ;  �  @        �  	   Y     �     �  +  	   �     A+  	   �     4+     �     +            	 j     	      	      	   
     k  j  Y     l     k  ;  �  v         k     j             ,       �   �   `      1           5     
   ;  5  6     ;  5  ?        A     
   ;  A  B     ;  5  I     ;  5  L       Q  
   �    R  Q     S     R  ;  S  T       U  �       V      U  ;  V  W       	 X  	                           Y  X     Z      Y  ;  Z  [           5     6               �     ;  %   ;     ;  [  \     ;     {     ;     �     ;  0   �     ;  0   �     ;  �  �     ;  %   "     ;     ,     ;     .     ;  %   0     ;  :  ;     ;     G     ;     \     ;     q     ;     �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;  :  �     ;     �     ;     �     ;     �     ;          ;  :       ;     &     ;     ;     ;     P     ;     e     ;  l  m     ;          ;          ;  �       ;  �       ;     .     ;     /     ;     0     ;     O     ;     R     ;     U     ;  k  l     ;         ;          ;          ;          ;  %   )     ;     *     ;     -          9      A  >  ?  =  8   =  	   @  ?  A  E  F  C  D  =  	   G  F  �  	   H  @  G  m     I  H  p  	   J  I  �  	   K  J  t        :      A  >  L  =  A   =  	   M  L  A  E  N  C  D  =  	   O  N  �  	   P  M  O  m     Q  P  p  	   R  Q  �  	   S  R  t   P     T  K  S       ;      A  E  V  C  U  =  	   W  V  �     X  T  W       7      >  ;  X       =      =     ]  ;  A  E  _  C  U  =  	   `  _  �     a  ^  `  �     b  ]  a  A  %   c  \  [   >  c  b       >      =     e  ;  A  E  g  C  U  =  	   h  g  �     i  f  h  �     j  e  i  A  %   k  \  d  >  k  j       ?      =     l  ;  A  E  n  C  U  =  	   o  n  �     p  m  o  �     q  l  p  A  %   r  \  {   >  r  q       @      =     t  ;  A  E  v  C  U  =  	   w  v  �     x  u  w  �     y  t  x  A  %   z  \  s  >  z  y       B      >  {  8   �  |  �  |       B      �  ~        �  �  �  �       B      =     �  {  �  �   �  �  Y  �  �  }  ~  �  }       D      A  �  �  C  �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  {  A  %   �  \  �  =     �  �  W  
   �  �  �  O     �  �  �            �     �  �  �  P     �  `   `   `   �     �  �  �  >  �  �       E      A  �  �  C  <   =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  {  A  %   �  \  �  =     �  �  W  
   �  �  �  >  �  �       F      A  �  �  C  �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  {  A  %   �  \  �  =     �  �  W  
   �  �  �  >  �  �       H      A     �  �  8   =  	   �  �  �  	   �  �  6   m     �  �  A  �  �  �  [   �  =  �  �  �  A  �  �  �  [   Q     �  �      A     �  �  [   >  �  �  Q     �  �     A     �  �  d  >  �  �  Q     �  �     A     �  �  {   >  �  �  Q     �  �     A     �  �  s  >  �  �  Q     �  �     A     �  �  �  >  �  �  Q  	   �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �  >  �  �       I      A     �  �  A   =  	   �  �  �  	   �  �  6   m     �  �  A  �  �  �  [   �  =  �  �  �  A  �  �  �  d  Q     �  �      A     �  �  [   >  �  �  Q     �  �     A     �  �  d  >  �  �  Q     �  �     A     �  �  {   >  �  �  Q     �  �     A     �  �  s  >  �  �  Q     �  �     A     �  �  �  >  �  �  Q  	   �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �  >  �  �       J      A     �  �  I   =  	   �  �  �  	   �  �  6   m     �  �  A  �  �  �  [   �  =  �  �  �  A  �  �  �  {   Q     �  �      A     �  �  [   >  �  �  Q     �  �     A     �  �  d  >  �  �  Q     �  �     A        �  {   >     �  Q       �     A       �  s  >      Q       �     A       �  �  >      Q  	     �     A       �  �  >      Q       �     A       �  �  >      Q     	  �     A     
  �  �  >  
  	       K      A       �  Q   =  	       �  	       6   m         A  �    �  [     =  �      A  �    �  s  Q             A         [   >      Q            A         d  >      Q            A         {   >      Q            A         s  >      Q            A         �  >      Q  	          A         �  >      Q            A         �  >      Q             A     !    �  >  !          L      =     %  {  A  %   &  \  %  =     '  &  A  E  (  C  D  =  	   )  (  �     *  '  )  �     +  *  t   =     -  $  >  ,  -  =     /  �  >  .  /  >  0  +  9     1  .   ,  .  0  >  "  1       N      A     2  �  [   [   =     3  2  �     5  3  4  �  �   6  5  4  �  8      �  6  7  F  �  7       N      A     <  �  [   {   =     =  <  A  �  >  �  =  =  �  ?  >  =  �  A  @  V  �  B  ?  A  =     C  "  W  
   D  B  C  A  0   E  ;  [   >  E  D  �  8  �  F       N      A     H  �  [   {   =     I  H  >  G  I  9  
   J     G  A  0   K  ;  [   >  K  J  �  8  �  8       O      A     L  �  d  [   =     M  L  �     N  M  4  �  �   O  N  4  �  Q      �  O  P  [  �  P       O      A     R  �  d  {   =     S  R  A  �  T  �  S  =  �  U  T  =  �  V  @  V  �  W  U  V  =     X  "  W  
   Y  W  X  A  0   Z  ;  d  >  Z  Y  �  Q  �  [       O      A     ]  �  d  {   =     ^  ]  >  \  ^  9  
   _     \  A  0   `  ;  d  >  `  _  �  Q  �  Q       P      A     a  �  {   [   =     b  a  �     c  b  4  �  �   d  c  4  �  f      �  d  e  p  �  e       P      A     g  �  {   {   =     h  g  A  �  i  �  h  =  �  j  i  =  �  k  @  V  �  l  j  k  =     m  "  W  
   n  l  m  A  0   o  ;  {   >  o  n  �  f  �  p       P      A     r  �  {   {   =     s  r  >  q  s  9  
   t     q  A  0   u  ;  {   >  u  t  �  f  �  f       Q      A     v  �  s  [   =     w  v  �     x  w  4  �  �   y  x  4  �  {      �  y  z  �  �  z       Q      A     |  �  s  {   =     }  |  A  �  ~  �  }  =  �    ~  =  �  �  @  V  �  �    �  =     �  "  W  
   �  �  �  A  0   �  ;  s  >  �  �  �  {  �  �       Q      A     �  �  s  {   =     �  �  >  �  �  9  
   �     �  A  0   �  ;  s  >  �  �  �  {  �  {       S      >  �  �        T      >  �  8   �  �  �  �       T      �  �  �      �  �  �  �       T      =     �  �  �  �   �  �  Y  �  �  �  �  �  �       V      =     �  �  =     �  �  A     �  �  �  =  	   �  �  =     �  �  A     �  ;  �  Q   =  	   �  �  �  	   �  �  �    	   �        �  �    	   �     (   �  �  A     �  �  �  >  �  �       W      =     �  �  A     �  �  �  =  	   �  �  =  	   �  �  �  	   �  �  �  >  �  �  �  �  �  �       T      =     �  �  �     �  �  d  >  �  �  �  �  �  �       Y      =  	   �  �  �  	   �  `   �  >  �  �       Z      >  �  8   �  �  �  �       Z      �  �  �      �  �  �  �       Z      =     �  �  �  �   �  �  Y  �  �  �  �  �  �       \      =     �  �  =  	   �  �  A     �  �  �  =  	   �  �  �  	   �  �  �  A     �  �  �  >  �  �  �  �  �  �       Z      =     �  �  �     �  �  d  >  �  �  �  �  �  �       _      A     �  �  [   [   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �       _      A     �  �  [   d  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  @  V  �  �  �  �  =     �  "  W  
   �  �  �  A  0   �  �  [   >  �  �  �  �  �  �       _      A     �  �  [   d  =     �  �  >  �  �  9  
   �     �  A  0   �  �  [   >  �  �  �  �  �  �       `      A     �  �  d  [   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �       `      A     �  �  d  d  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  @  V  �  �  �  �  =     �  "  W  
   �  �  �  A  0   �  �  d  >  �  �  �  �  �  �       `      A     �  �  d  d  =     �  �  >  �  �  9  
   �     �  A  0   �  �  d  >  �  �  �  �  �  �       a      A     �  �  {   [   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �       a      A     �  �  {   d  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  @  V  �  �  �  �  =     �  "  W  
   �  �  �  A  0   �  �  {   >  �  �  �  �  �  �       a      A     �  �  {   d  =     �  �  >  �  �  9  
   �     �  A  0   �  �  {   >  �  �  �  �  �  �       b      A     �  �  s  [   =        �  �          �  �  �       �  �        �        �         b      A       �  s  d  =         A  �    �    =  �      =  �  	  @  V  �  
    	  =       "  W  
     
    A  0     �  s  >      �    �         b      A       �  s  d  =         >      9  
          A  0     �  s  >      �    �         d      A       �  [   [   =         �           �  �         �        �      %  �         d      A       �  [   s  =         A  �    �    =  �      =  �     @  V  �  !       =     "  "  W  
   #  !  "  A  0   $    [   >  $  #  �    �  %       d      A     '  �  [   s  =     (  '  >  &  (  9  
   )     &  A  0   *    [   >  *  )  �    �         e      A     +  �  d  [   =     ,  +  �     -  ,    �  �   .  -    �  0      �  .  /  :  �  /       e      A     1  �  d  s  =     2  1  A  �  3  �  2  =  �  4  3  =  �  5  @  V  �  6  4  5  =     7  "  W  
   8  6  7  A  0   9    d  >  9  8  �  0  �  :       e      A     <  �  d  s  =     =  <  >  ;  =  9  
   >     ;  A  0   ?    d  >  ?  >  �  0  �  0       f      A     @  �  {   [   =     A  @  �     B  A    �  �   C  B    �  E      �  C  D  O  �  D       f      A     F  �  {   s  =     G  F  A  �  H  �  G  =  �  I  H  =  �  J  @  V  �  K  I  J  =     L  "  W  
   M  K  L  A  0   N    {   >  N  M  �  E  �  O       f      A     Q  �  {   s  =     R  Q  >  P  R  9  
   S     P  A  0   T    {   >  T  S  �  E  �  E       g      A     U  �  s  [   =     V  U  �     W  V    �  �   X  W    �  Z      �  X  Y  d  �  Y       g      A     [  �  s  s  =     \  [  A  �  ]  �  \  =  �  ^  ]  =  �  _  @  V  �  `  ^  _  =     a  "  W  
   b  `  a  A  0   c    s  >  c  b  �  Z  �  d       g      A     f  �  s  s  =     g  f  >  e  g  9  
   h     e  A  0   i    s  >  i  h  �  Z  �  Z       h      =     n  {  =     o  �  A     p  m  n  [   >  p  o       i      =     q  {  A  �  r  C  �  =     s  r  A  �  t  �  s  =  �  u  t  =  �  w  v  V  �  x  u  w  =     y  {  A  %   z  \  y  =     {  z  W  
   |  x  {  Q  	   }  |      A     ~  m  q  d  >  ~  }       j      =       {  A  0   �  �  [   =  
   �  �  O     �  �  �            A     �  �  [   =  	   �  �  �     �  �  �       k      A  0   �  �  d  =  
   �  �  O     �  �  �            A     �  �  d  =  	   �  �  �     �  �  �  �     �  �  �       l      A  0   �  �  {   =  
   �  �  O     �  �  �            A     �  �  {   =  	   �  �  �     �  �  �  �     �  �  �       m      A  0   �  �  s  =  
   �  �  O     �  �  �            A     �  �  s  =  	   �  �  �     �  �  �  �     �  �  �       j      A     �  m    {   >  �  �       n      =     �  {  A     �  �  [   Q   =  	   �  �  A     �  �  [   =  	   �  �  �  	   �  �  �       o      A     �  �  d  Q   =  	   �  �  A     �  �  d  =  	   �  �  �  	   �  �  �  �  	   �  �  �       p      A     �  �  {   Q   =  	   �  �  A     �  �  {   =  	   �  �  �  	   �  �  �  �  	   �  �  �       q      A     �  �  s  Q   =  	   �  �  A     �  �  s  =  	   �  �  �  	   �  �  �  �  	   �  �  �       n      A     �  m  �  s  >  �  �       r      =     �  {  A  0   �  ;  [   =  
   �  �  O     �  �  �            A     �  �  [   =  	   �  �  �     �  �  �       s      A  0   �  ;  d  =  
   �  �  O     �  �  �            A     �  �  d  =  	   �  �  �     �  �  �  �     �  �  �       t      A  0   �  ;  {   =  
   �  �  O     �  �  �            A     �  �  {   =  	   �  �  �     �  �  �  �     �  �  �       u      A  0   �  ;  s  =  
   �  �  O     �  �  �            A     �  �  s  =  	   �  �  �     �  �  �  �     �  �  �       r      A     �  m  �  �  >  �  �       v      =     �  {  A     �  ;  [   Q   =  	   �  �  A     �  �  [   =  	   �  �  �  	   �  �  �       w      A     �  ;  d  Q   =  	   �  �  A     �  �  d  =  	   �  �  �  	   �  �  �  �  	   �  �  �       x      A     �  ;  {   Q   =  	   �  �  A     �  �  {   =  	   �  �  �  	   �  �  �  �  	   �  �  �       y      A     �  ;  s  Q   =  	   �  �  A     �  �  s  =  	   �  �  �  	   �  �  �  �  	   �  �  �       v      A     �  m  �  �  >  �  �       z      =     �  {  A  0   �    [   =  
   �  �  A     �  �  [   =  	   �  �  �  
   �  �  �       {      A  0   �    d  =  
   �  �  A     �  �  d  =  	   �  �  �  
   �  �  �  �  
   �  �  �       |      A  0   �    {   =  
   �  �  A     �  �  {   =  	   �  �  �  
   �  �  �  �  
   �  �  �       }      A  0   �    s  =  
   �  �  A     �  �  s  =  	      �  �  
     �     �  
     �         z      A  0     m  �  �  >      �    �         B      =       {  �         d  >  {    �  |  �  ~             A  >    =  8   =  	       A  E  	  C  D  =  	   
  	  �  	       
    	        
     >           �      A  >    =  A   =  	       A  E    C  D  =  	       �  	           	        
     >           �      =  	       �  	     `     =  	       �  	     `     �  	         A         [   >      A         [   >           �      =  	       �  	     `     =  	       �  	          A     !    d  >  !     A     "    d  >  "          �      =  	   #    =  	   $    �  	   %  `   $  �  	   &  #  %  A     '    {   >  '  &  A     (    {   >  (  &       �      =  	   )    =  	   *    �  	   +  )  *  A     ,    s  >  ,  +  A     -    s  >  -  +       �      >  .  �        �      >  /  �        �      >  0  8   �  1  �  1       �      �  3  4      �  5  �  5       �      =     6  0  �  �   7  6  Y  �  7  2  3  �  2       �      =     8  0  =     9  0  A     :    9  =  	   ;  :  =     <  0  A     =  m  <  �  =  	   >  =  �  	   ?  ;  >    	   @        ?  �    	   A     (   @  �  A     B    8  >  B  A       �      =     C  0  A     D    C  =  	   E  D  =  	   F  .  �  	   G  F  E  >  .  G       �      =     H  0  A     I    H  =  	   J  I  =  	   K  /  �  	   L  K  J  >  /  L  �  4  �  4       �      =     M  0  �     N  M  d  >  0  N  �  1  �  3       �      =  	   P  .  �  	   Q  `   P  >  O  Q       �      =  	   S  /  �  	   T  `   S  >  R  T       �      >  U  8   �  V  �  V       �      �  X  Y      �  Z  �  Z       �      =     [  U  �  �   \  [  Y  �  \  W  X  �  W       �      =     ]  U  =  	   ^  O  A     _    ]  =  	   `  _  �  	   a  `  ^  A     b    ]  >  b  a       �      =     c  U  =  	   d  R  A     e    c  =  	   f  e  �  	   g  f  d  A     h    c  >  h  g  �  Y  �  Y       �      =     i  U  �     j  i  d  >  U  j  �  V  �  X       �      A  k  m  m  [   =  j  n  m  >  l  n       �      A     o  m  [   [   =     p  o  A     q    [   =  	   r  q  �     s  p  r       �      A     t  m  d  [   =     u  t  A     v    d  =  	   w  v  �     x  u  w  �     y  s  x       �      A     z  m  {   [   =     {  z  A     |    {   =  	   }  |  �     ~  {  }  �       y  ~       �      A     �  m  s  [   =     �  �  A     �    s  =  	   �  �  �     �  �  �  �     �    �       �      A     �  l  [   >  �  �       �      A     �  m  [   d  =  	   �  �  A     �    [   =  	   �  �  �  	   �  �  �       �      A     �  m  d  d  =  	   �  �  A     �    d  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  m  {   d  =  	   �  �  A     �    {   =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  m  s  d  =  	   �  �  A     �    s  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  l  d  >  �  �       �      A     �  m  [   {   =     �  �  A     �    [   =  	   �  �  �     �  �  �       �      A     �  m  d  {   =     �  �  A     �    d  =  	   �  �  �     �  �  �  �     �  �  �       �      A     �  m  {   {   =     �  �  A     �    {   =  	   �  �  �     �  �  �  �     �  �  �       �      A     �  m  s  {   =     �  �  A     �    s  =  	   �  �  �     �  �  �  �     �  �  �       �      A     �  l  {   >  �  �       �      A     �  m  [   s  =  	   �  �  A     �    [   =  	   �  �  �  	   �  �  �       �      A     �  m  d  s  =  	   �  �  A     �    d  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  m  {   s  =  	   �  �  A     �    {   =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  m  s  s  =  	   �  �  A     �    s  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  l  s  >  �  �       �      A     �  m  [   �  =     �  �  A     �    [   =  	   �  �  �     �  �  �       �      A     �  m  d  �  =     �  �  A     �    d  =  	   �  �  �     �  �  �  �     �  �  �       �      A     �  m  {   �  =     �  �  A     �    {   =  	   �  �  �     �  �  �  �     �  �  �       �      A     �  m  s  �  =     �  �  A     �    s  =  	   �  �  �     �  �  �  �     �  �  �       �      A     �  l  �  >  �  �       �      A     �  m  [   �  =  	   �  �  A     �    [   =  	   �  �  �  	   �  �  �       �      A     �  m  d  �  =  	   �  �  A     �    d  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  m  {   �  =  	   �  �  A     �    {   =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  m  s  �  =  	   �  �  A     �    s  =  	   �  �  �  	   �  �  �  �  	   �  �  �       �      A     �  l  �  >  �  �       �      A  0   �  m  [   �  =  
      �  A         [   =  	       �  
               �      A  0     m  d  �  =  
       A         d  =  	       �  
         �  
   	           �      A  0   
  m  {   �  =  
     
  A         {   =  	       �  
         �  
     	         �      A  0     m  s  �  =  
       A         s  =  	       �  
         �  
              �      A  0     l  �  >           �      A       l  [   =         >      >      9       #       >           �      =          A     !  l  �  =     "  !  �     #  "  �  P     $  `   `   `   �     %  #  $       &     E   %  �     '     &       (     E   '  >    (       �      =     +  $  >  *  +  9     ,     *  =     .  $  >  -  .  9     /     -  �     0  ,  /  A  1  2  Z   �  =     3  2  �     4  0  3  >  )  4       �      A     7  l  {   =     8  7  A     9  l  s  =  	   :  9  Q  	   ;  8      Q  	   <  8     Q  	   =  8     P  
   >  ;  <  =  :  >  6  >       �      =     @    A  >  C  B  I   =  	   D  C  Q  	   E  @      Q  	   F  @     Q  	   G  @     P  
   H  E  F  G  D  >  ?  H       �      A  0   J  l  �  =  
   K  J  >  I  K       �      =     M  )  Q  	   N  M      Q  	   O  M     P  
   P  N  O  �   �   >  L  P  �  8             6  
             7        �     ;  0   1                 =     2      �     4   2   3   p  	   5   4   �  	   7   5   6   A     9   1   8   >  9   7              =     :      �     =   :   <   �     >   =   3   p  	   ?   >   �  	   @   ?   6   A     B   1   A   >  B   @              =     C      �     E   C   D   �     F   E   3   p  	   G   F   �  	   H   G   6   A     J   1   I   >  J   H              =     K      �     M   K   L   �     N   M   3   p  	   O   N   �  	   P   O   6   A     R   1   Q   >  R   P        	      =  
   S   1   �  S   8       P  :   6               7        �     ;  0   V      ;     f           R      A  \   ]   Z   [   =  W   ^   ]   =     _      Q  	   a   _       Q  	   b   _      Q  	   c   _      P  
   d   a   b   c   `   �  
   e   ^   d   >  V   e        S      A     g   V   Q   =  	   h   g   �  	   i   `   h   >  f   i        T      =  	   j   f   =  
   k   V   O     l   k   k          �     m   l   j   A     n   V   8   Q  	   o   m       >  n   o   A     p   V   A   Q  	   q   m      >  p   q        V      =  
   r   V   O     s   r   r          �     u   s   t   P     v   t   t   �     w   u   v   �  w   8       \  ;   6               7        �     ;  0   z      ;     �           ^      A  \   |   Z   {   =  W   }   |   =     ~      Q  	      ~       Q  	   �   ~      Q  	   �   ~      P  
   �      �   �   `   �  
   �   }   �   >  z   �        _      A     �   z   Q   =  	   �   �   �  	   �   `   �   >  �   �        `      =  	   �   �   =  
   �   z   O     �   �   �          �     �   �   �   A     �   z   8   Q  	   �   �       >  �   �   A     �   z   A   Q  	   �   �      >  �   �        b      =  
   �   z   O     �   �   �          �     �   �   t   P     �   t   t   �     �   �   �   �  �   8       w  #   6               7        7        �     ;     �      ;     �      ;     �           y      =  	   �        	   �         �   >  �   �        z      =  	   �        	   �         �   >  �   �        |      A     �      8   =  	   �   �   =  	   �   �   �  	   �   �   �   A     �      I   =  	   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   >  �   �        }      A     �      8   =  	   �   �     	   �   �   =  	   �   �   �  	   �   �   �   A     �      I   =  	   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   A     �      I   >  �   �        ~      =  	   �   �   A     �      8   >  �   �        �      =     �      �  �   8       �  F   6     #           7     !   7     "   �  $   ;     �      ;     �           �      =     �   "   =     �   !   =     �   "   =     �   !   �  	   �   �   �   �     �   �   �   �     �   �   �        �      E   �   >  �   �        �      =     �   !   =     �   �        �      D   �   �   >  �   �        �      =     �   �   =     �   �   =     �   !   Q  	   �   �       Q  	   �   �      Q  	   �   �      Q  	   �   �       Q  	   �   �      Q  	   �   �      Q  	   �   �       Q  	   �   �      Q  	   �   �      P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   �  �   8       �  "   6  	   (       &   7  %   '   �  )        �      =     �   '   �  	   �   �   �     	   �         �   �  	   �   �   �     	   �      
   �   �  �   8       �  Z   6     .       *   7     +   7     ,   7  %   -   �  /   ;     �      ;  �   �      ;  �   �      ;  %        ;          ;  %        ;     -     ;     .                =     �   ,        �         �   >  �   �              A     �   �   8   =  	   �   �   A     �   �   A   =  	   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �              A     �   �   8   =  	   �   �   A     �   �   I   =  	   �   �   �  �   �   �   �   �  �   �  �   �  �   �   �   /   �   �              >  �   �              A     �   �   I   =  	   �   �   A     �   �   8   =  	   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �              A     �   �   I   =  	   �   �   A     �   �   A   =  	      �   �  �     �      �  �   �  �   �  �     �   �     �              >  �                =       +   O                 =  �     �   �  	       `   �   �           =     	  +   O     
  	  	         =  �     �   �  	       `   �   �       
    �           =       +   O                  =  �     �   �  �       =  �     �   �  �       �  �         �  	       `   �   �           �           >           
      =         �         t   >                 =       -   >      9  	      (       	   !     .          >    !             =     "  -   =     #    �     $  #  "  >    $             =     %    P     &  t   t   �     '  %  &  >    '             A     (    8   =  	   )  (  A     *    A   =  	   +  *  P     ,  )  �   +  >  -  ,  =  	   /    >  .  /  9     0     -  .  O     1  0  0         >    1             =     2    P     3  t   t   �     4  2  3  >    4             =     5  -   =     6    �     7  6  5  >    7             =     8    �  8  8                          ��KT��D	                               ��v)��%�9����Y��G� �z��.v<                                I�%i/)�