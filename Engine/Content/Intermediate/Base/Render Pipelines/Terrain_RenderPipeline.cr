�nX/J1�7�L~�|Terrain_RenderPipeline                                           L�      #     =                GLSL.std.450              
        main    �   �   �    *       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Terrain_Vertex.glsl     �     // OpModuleProcessed entry-point main
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

//The sky texture.
layout (set = 0, binding = 3) uniform samplerCube SKY_TEXTURE;

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
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
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
#define TERRAIN_MINIMUM_DISPLACEMENT (0.0001f)
#define BIAS_DISPLACEMENT(X) (pow(X, 16.0f))
#define TERRAIN_MATERIAL_SCALE (0.5f)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    vec3 albedo;
    vec4 normal_map_displacement;
    vec4 material_properties;
};

/*
*   Calculates the material coordinate for the given world position.
*/
vec2 CalculateTerrainMaterialCoordinate(vec3 world_position, vec2 tile_index, vec3 normal)
{
#if 1
    //Take the absolute of the normal.
    vec3 absolute_normal = abs(normal);

    //Calculate the tile.
    bool x_dominant = absolute_normal.x > absolute_normal.y && absolute_normal.x > absolute_normal.z;
    bool z_dominant = absolute_normal.z > absolute_normal.x && absolute_normal.z > absolute_normal.y;
    vec2 tile = world_position.yz * float(x_dominant) + world_position.xy * float(z_dominant) + world_position.xz * float(!x_dominant && !z_dominant);

    tile *= TERRAIN_MATERIAL_SCALE;

    //Calculate the random rotation.
    float random_rotation = mix(-PI, PI, RandomFloat(tile_index));

    //Randomly rotate the tile.
    tile -= tile_index;
    tile -= 0.5f;
    tile = RotateYaw(vec3(tile.x, 0.0f, tile.y), random_rotation).xz;
    tile += 0.5f;
    tile += tile_index;

    //Return the tile!
    return tile;
#else
    return vec2(world_position.x, world_position.z) * 0.5f;
#endif
}

/*
*   Returns the highest index.
*/
uint HighestIndex(float value_1, float value_2, float value_3, float value_4)
{
    uint highest_index = 0;
    float highest_value = value_1;

    if (highest_value < value_2)
    {
        highest_index = 1;
        highest_value = value_2;
    }

    if (highest_value < value_3)
    {
        highest_index = 2;
        highest_value = value_3;
    }

    if (highest_value < value_4)
    {
        highest_index = 3;
        highest_value = value_4;
    }

    return highest_index;
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
        float displacements[4];
        vec2 sample_offsets[4];
        sample_offsets[0] = vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[1] = vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[2] = vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[3] = vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        for (uint i = 0; i < 4; ++i)
        {
            vec2 material_texture_coordinate = CalculateTerrainMaterialCoordinate(OutWorldPosition, (floor(OutHeightMapTextureCoordinate * MAP_RESOLUTION) + sample_offsets[i] * MAP_RESOLUTION), normals[i]);
            vec2 height_map_texture_coordinate = OutHeightMapTextureCoordinate + sample_offsets[i];
            vec4 index_map = texture(sampler2D(TEXTURES[INDEX_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), height_map_texture_coordinate);
            vec4 blend_map = texture(sampler2D(TEXTURES[BLEND_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), height_map_texture_coordinate);
            Material materials[4];
            materials[0] = MATERIALS[uint(index_map[0] * float(UINT8_MAXIMUM))];
            materials[1] = MATERIALS[uint(index_map[1] * float(UINT8_MAXIMUM))];
            materials[2] = MATERIALS[uint(index_map[2] * float(UINT8_MAXIMUM))];
            materials[3] = MATERIALS[uint(index_map[3] * float(UINT8_MAXIMUM))];
            vec4 normal_map_displacements[4];
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[0], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[0]);
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[1], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[1]);
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[2], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[2]);
            EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[3], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[3]);
            uint highest_index = HighestIndex
            (
                normal_map_displacements[0].w * blend_map[0],
                normal_map_displacements[1].w * blend_map[1],
                normal_map_displacements[2].w * blend_map[2],
                normal_map_displacements[3].w * blend_map[3]
            );
            displacements[i] =  normal_map_displacements[highest_index].w;
        }
        vec4 blends;
        blends[0] = (1.0f - fract(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION)) * (1.0f - fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION));
        blends[1] = (1.0f - fract(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION)) * (fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION));
        blends[2] = (fract(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION)) * (1.0f - fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION));
        blends[3] = (fract(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION)) * (fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION));
        uint highest_index = HighestIndex
        (
            displacements[0] * blends[0],
            displacements[1] * blends[1],
            displacements[2] * blends[2],
            displacements[3] * blends[3]
        );
	    float final_displacement = displacements[highest_index];
        OutWorldPosition += normal * mix(-0.125f, 0.1875f, final_displacement) * displacement_multiplier; //Slight bias for upward displacement.
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition,1.0f);
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        RotateYaw(vf3;f1;        X        angle        RandomFloat(vf2;         coordinate    !   CalculateTerrainMaterialCoordinate(vf3;vf2;vf3;      world_position       tile_index        normal   	 (   HighestIndex(f1;f1;f1;f1;     $   value_1   %   value_2   &   value_3   '   value_4   +   unpacked      P   sine      S   cosine    V   temp      {   absolute_normal   �   x_dominant    �   z_dominant    �   tile      �   random_rotation   �   param     �   param     �   param     �   highest_index     �   highest_value     �   stitched_position     �   InPosition    �   is_left_multiplier    �   InBorders     �   PushConstantData      �       WORLD_POSITION    �      MINIMUM_HEIGHT_MAP_COORDINATE     �      MAXIMUM_HEIGHT_MAP_COORDINATE     �      BORDERS  
 �      PATCH_RESOLUTION_RECIPROCAL   �      PATCH_SIZE   
 �      HEIGHT_MAP_TEXTURE_INDEX     
 �      NORMAL_MAP_TEXTURE_INDEX     	 �      INDEX_MAP_TEXTURE_INDEX  	 �   	   BLEND_MAP_TEXTURE_INDEX   �   
   MAP_RESOLUTION   
 �      MAP_RESOLUTION_RECIPROCAL     �           is_right_multiplier     vertical_offset     is_down_multiplier      is_up_multiplier      "  horizontal_offset     9  is_left_multiplier    A  is_right_multiplier   J  vertical_offset   O  is_down_multiplier    X  is_up_multiplier      a  horizontal_offset     y  height_map_coordinate     �  OutWorldPosition      �  height_1      �  TEXTURES      �  HEIGHT_MAP_SAMPLER    �  height_2      �  height_3      �  height_4      �  blend_1      blend_2   
  final_height        normals     NORMAL_MAP_SAMPLER    S  blend_1   `  blend_2   m  normal   
   OutHeightMapTextureCoordinate     �  displacement_multiplier   �  Camera   	 �      WORLD_TO_CLIP_MATRIX     	 �     WORLD_TO_CAMERA_MATRIX    �     PREVIOUS_WORLD_TO_CLIP_MATRIX     �     INVERSE_WORLD_TO_CAMERA_MATRIX    �     INVERSE_CAMERA_TO_CLIP_MATRIX    	 �     CAMERA_WORLD_POSITION    	 �     CAMERA_FORWARD_VECTOR    	 �     CURRENT_FRAME_JITTER      �     NEAR_PLANE    �  	   FAR_PLANE     �        �  sample_offsets    �  i    	 �  material_texture_coordinate   �  param     �  param     �  param    
 �  height_map_texture_coordinate     �  index_map     �  INDEX_BLEND_MAP_SAMPLER   �  blend_map     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  materials     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �       	 N  normal_map_displacements      S  MATERIAL_SAMPLER      Z  param     o  param     �  param     �  param     �  highest_index     �  param     �  param     �  param     �  param     �  displacements     �  blends    �  highest_index       param       param       param       param       final_displacement    (  gl_PerVertex      (      gl_Position   (     gl_PointSize      (     gl_ClipDistance   (     gl_CullDistance   *        8  BLUE_NOISE_TEXTURES   <  SKY_TEXTURE G  �          G  �         H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   H  �      #   (   H  �      #   ,   H  �   	   #   0   H  �   
   #   4   H  �      #   8   G  �      G  �         G  �  "       G  �  !       G  �  "      G  �  !      G    "      G    !      G          H  �         H  �      #       H  �            H  �        H  �     #   @   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #      H  �           H  �     #   @  H  �     #   P  H  �     #   `  H  �     #   h  H  �  	   #   l  G  �     G  �  "      G  �  !       G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  S  "      G  S  !      H  (             H  (           H  (           H  (           G  (     G  8  "       G  8  !      G  <  "       G  <  !           !                                   	         
   	      !     
           	                           	   !                   	                  !     	      !                 !  #                     *      
   +     -   �   +  	   0     C+     2         5          +  5   6      +     ;      +  5   >      +     C      +  5   F      +     K      +  	   q   9�OA+  	   r   Lw�B,     s   q   r   +  	   v   ��*G  ~            ~   +  	   �       +  	   �     �?+  	   �      ?+  	   �   �I�+  	   �   �I@   �         ;  �   �         �         ;  �   �        �               	   	               	   	      �   	   �   ;  �   �   	   +  5   �         �   	      +          +          +       @   +  5   '        (  	   	   +     C     +     Q      +     Z  �   +  	   h     @+  5   z     +  5   }        �        ;  �  �     +  5   �      +  5   �        �     	    	 �  	                         +     �       �  �  �     �      �  ;  �  �      +  5   �        �      �    �     �      �  ;  �  �        �  �  ,     �  �   �   +  5   �     ,     �  �   �   ,     �  �   �   ,     �  �   �   +  5   �  
                      +  5        ;  �           ~        ;  ~         �  
        �  �  �  �  �  �           	   	      �     �  ;  �  �        �        +  	   �    �B  �          �     �  ;  �  �      +  5   �  	    
 �                 	           �  �       �     �   
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �        �     �     �     �    L  
        M     L  ;  �  S        �  	        �     �  +  	        �+  	       @>  '  	   ;     (  
   	   '  '     )     (  ;  )  *        +     �     4     
     6  �  Q     7      6  ;  7  8       	 9  	                           :  9     ;      :  ;  ;  <           �     6               �     ;     �      ;     �      ;          ;          ;          ;          ;     "     ;     9     ;     A     ;     J     ;     O     ;     X     ;     a     ;     y     ;     �     ;     �     ;     �     ;     �     ;     �     ;           ;     
     ;         ;     S     ;     `     ;     m     ;     �     ;  �  �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  *   �     ;  *   �     ;  �  �     ;  M  N     ;     Z     ;     o     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  �  �     ;  *   �     ;     �     ;          ;          ;          ;          ;               �      =     �   �   >  �   �        �      =     �   �   �     �   �   ;   A  �   �   �   �   =     �   �   �     �   �   ;   �        �   �   p  	        >  �          �      =       �   �           A  �     �   �   =         �           �     	      p  	   
  	  >    
       �      =  	     �   =  	       �  	           	        %     �   >           �      =       �   �           A  �     �   �   =         �           �           p  	       >           �      =       �   �           A  �     �   �   =         �           �            p  	   !     >    !              =  	   #    =  	   $    �  	   %  #  $    	   &     %   %  �   >  "  &             A  (  )  �   '  =  	   *  )  =  	   +    �  	   ,  *  +  A     -  �   ;   =  	   .  -  �  	   /  .  ,  A     0  �   ;   >  0  /             A  (  1  �   '  =  	   2  1  =  	   3  "  �  	   4  2  3  A     5  �   2   =  	   6  5  �  	   7  6  4  A     8  �   2   >  8  7             =     :  �   �     ;  :  C   A  �   <  �   �   =     =  <  �     >  =  C   �     ?  ;  >  p  	   @  ?  >  9  @             =     B  �   �     D  B  C  A  �   E  �   �   =     F  E  �     G  F  C  �     H  D  G  p  	   I  H  >  A  I             =  	   K  9  =  	   L  A  �  	   M  K  L    	   N     %   M  �   >  J  N             =     P  �   �     R  P  Q  A  �   S  �   �   =     T  S  �     U  T  Q  �     V  R  U  p  	   W  V  >  O  W       	      =     Y  �   �     [  Y  Z  A  �   \  �   �   =     ]  \  �     ^  ]  Z  �     _  [  ^  p  	   `  _  >  X  `       
      =  	   b  O  =  	   c  X  �  	   d  b  c    	   e     %   d  �   >  a  e             A  (  f  �   '  =  	   g  f  �  	   i  g  h  =  	   j  J  �  	   k  i  j  A     l  �   ;   =  	   m  l  �  	   n  m  k  A     o  �   ;   >  o  n             A  (  p  �   '  =  	   q  p  �  	   r  q  h  =  	   s  a  �  	   t  r  s  A     u  �   2   =  	   v  u  �  	   w  v  t  A     x  �   2   >  x  w             A  (  {  �   z  2   =  	   |  {  A  (  ~  �   }  2   =  	     ~  A     �  �   2   =  	   �  �    	   �     .   |    �  A  (  �  �   z  ;   =  	   �  �  A  (  �  �   }  ;   =  	   �  �  A     �  �   ;   =  	   �  �    	   �     .   �  �  �  P     �  �  �  >  y  �             A  (  �  �   �  2   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �     	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �   A     �  �   2   =  	   �  �    	   �     .   �  �  �  �  	   �  �  �  A  �  �  �  2   >  �  �             A  �  �  �  ;   >  �  �              A  (  �  �   �  ;   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �     	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �   A     �  �   ;   =  	   �  �    	   �     .   �  �  �  �  	   �  �  �  A  �  �  �  C   >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  y  A  (  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  y  A  (  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  y  A  (  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  y  A  (  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             =  	   �  �  =  	   �  �  A     �  y  ;   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �             =  	     �  =  	     �  A       y  ;   =  	       A  (    �   �  =  	       �  	           	        
       	   	     .         >     	             =  	     �  =  	        A       y  2   =  	       A  (    �   �  =  	       �  	           	        
       	        .         >  
               =  	     
  A  �    �  ;   >                 A  �     �     =         A  �    �    =  �      =  �      V  �         =     !  y  A  (  "  �   �  =  	   #  "  �     $  �  #  �     %  !  $  X  
   &     %     �   O     '  &  &            A     (    �  >  (  '              A  �   )  �     =     *  )  A  �  +  �  *  =  �  ,  +  =  �  -    V  �  .  ,  -  =     /  y  A  (  0  �   �  =  	   1  0  �     2  �  1  �     3  /  2  X  
   4  .  3     �   O     5  4  4            A     6    z  >  6  5       !      A  �   7  �     =     8  7  A  �  9  �  8  =  �  :  9  =  �  ;    V  �  <  :  ;  =     =  y  A  (  >  �   �  =  	   ?  >  �     @  �  ?  �     A  =  @  X  
   B  <  A     �   O     C  B  B            A     D    }  >  D  C       "      A  �   E  �     =     F  E  A  �  G  �  F  =  �  H  G  =  �  I    V  �  J  H  I  =     K  y  A  (  L  �   �  =  	   M  L  �     N  �  M  �     O  K  N  X  
   P  J  O     �   O     Q  P  P            A     R    �   >  R  Q       #      A     T    �  =     U  T  A     V    z  =     W  V  A     X  y  ;   =  	   Y  X  A  (  Z  �   �  =  	   [  Z  �  	   \  Y  [    	   ]     
   \  P     ^  ]  ]  ]       _     .   U  W  ^  >  S  _       $      A     a    }  =     b  a  A     c    �   =     d  c  A     e  y  ;   =  	   f  e  A  (  g  �   �  =  	   h  g  �  	   i  f  h    	   j     
   i  P     k  j  j  j       l     .   b  d  k  >  `  l       %      =     n  S  =     o  `  A     p  y  2   =  	   q  p  A  (  r  �   �  =  	   s  r  �  	   t  q  s    	   u     
   t  P     v  u  u  u       w     .   n  o  v  >  m  w       &      =     x  m  �     y  x  h  P     z  �   �   �   �     {  y  z  >  m  {       '      =     |  m       }     E   |  >  m  }       )      =     �  y  >    �       *      =     �  �  A  �  �  �  �  =     �  �  �     �  �  �    	   �     B   �  �  	   �  �  �    	   �     %   �  �   �  	   �  �   �  >  �  �       +      =  	   �  �  �  ~   �  �  �   �  �      �  �  �  �  �  �       /      A  (  �  �   �  =  	   �  �  �     �  �  �  A     �  �  �  >  �  �       0      A  (  �  �   �  =  	   �  �  �     �  �  �  A     �  �  z  >  �  �       1      A  (  �  �   �  =  	   �  �  �     �  �  �  A     �  �  }  >  �  �       2      A  (  �  �   �  =  	   �  �  �     �  �  �  A     �  �  �   >  �  �       3      >  �  2   �  �  �  �       3      �  �  �      �  �  �  �       3      =     �  �  �  ~   �  �    �  �  �  �  �  �       5      =     �    A  (  �  �   �  =  	   �  �  �     �  �  �       �        �  =     �  �  A     �  �  �  =     �  �  A  (  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  =     �  �  =     �  �  >  �  �  >  �  �  A     �    �  =     �  �  >  �  �  9     �  !   �  �  �  >  �  �       6      =     �    =     �  �  A     �  �  �  =     �  �  �     �  �  �  >  �  �       7      A  �   �  �   6   =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  X  
   �  �  �     �   >  �  �       8      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  X  
   �  �  �     �   >  �  �       :      A     �  �  2   =  	   �  �  �  	   �  �  0   m     �  �  A  �  �  �  �  �  =  �  �  �  A  �  �  �  �  Q     �  �      A     �  �  �  >  �  �  Q     �  �     A     �  �  z  >  �  �  Q     �  �     A     �  �  }  >  �  �  Q     �  �     A     �  �  �   >  �  �  Q     �  �     A     �  �  '  >  �  �  Q  	   �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �  >  �  �  Q     �  �     A        �    >     �       ;      A       �  ;   =  	       �  	       0   m         A  �    �  �    =  �      A  �    �  z  Q             A     	    �  >  	    Q     
       A         z  >    
  Q            A         }  >      Q            A         �   >      Q            A         '  >      Q  	          A         �  >      Q            A         �  >      Q            A           >           <      A       �  C   =  	       �  	       0   m         A  �    �  �    =  �      A  �    �  }  Q             A          �  >       Q     !       A     "    z  >  "  !  Q     #       A     $    }  >  $  #  Q     %       A     &    �   >  &  %  Q     '       A     (    '  >  (  '  Q  	   )       A     *    �  >  *  )  Q     +       A     ,    �  >  ,  +  Q     -       A     .      >  .  -       =      A     /  �  K   =  	   0  /  �  	   1  0  0   m     2  1  A  �  3  �  �  2  =  �  4  3  A  �  5  �  �   Q     6  4      A     7  5  �  >  7  6  Q     8  4     A     9  5  z  >  9  8  Q     :  4     A     ;  5  }  >  ;  :  Q     <  4     A     =  5  �   >  =  <  Q     >  4     A     ?  5  '  >  ?  >  Q  	   @  4     A     A  5  �  >  A  @  Q     B  4     A     C  5  �  >  C  B  Q     D  4     A     E  5    >  E  D       ?      A     F  �  �  �  =     G  F  �     H  G    �  ~   I  H    �  K      �  I  J  Y  �  J       ?      A     O  �  �  }  =     P  O  A  �  Q  �  P  =  �  R  Q  =  �  T  S  V  �  U  R  T  =     V  �  X  
   W  U  V     �   A  *   X  N  �  >  X  W  �  K  �  Y       ?      A     [  �  �  }  =     \  [  >  Z  \  9  
   ]     Z  A  *   ^  N  �  >  ^  ]  �  K  �  K       @      A     _  �  z  �  =     `  _  �     a  `    �  ~   b  a    �  d      �  b  c  n  �  c       @      A     e  �  z  }  =     f  e  A  �  g  �  f  =  �  h  g  =  �  i  S  V  �  j  h  i  =     k  �  X  
   l  j  k     �   A  *   m  N  z  >  m  l  �  d  �  n       @      A     p  �  z  }  =     q  p  >  o  q  9  
   r     o  A  *   s  N  z  >  s  r  �  d  �  d       A      A     t  �  }  �  =     u  t  �     v  u    �  ~   w  v    �  y      �  w  x  �  �  x       A      A     z  �  }  }  =     {  z  A  �  |  �  {  =  �  }  |  =  �  ~  S  V  �    }  ~  =     �  �  X  
   �    �     �   A  *   �  N  }  >  �  �  �  y  �  �       A      A     �  �  }  }  =     �  �  >  �  �  9  
   �     �  A  *   �  N  }  >  �  �  �  y  �  y       B      A     �  �  �   �  =     �  �  �     �  �    �  ~   �  �    �  �      �  �  �  �  �  �       B      A     �  �  �   }  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  S  V  �  �  �  �  =     �  �  X  
   �  �  �     �   A  *   �  N  �   >  �  �  �  �  �  �       B      A     �  �  �   }  =     �  �  >  �  �  9  
   �     �  A  *   �  N  �   >  �  �  �  �  �  �       E      A     �  N  �  K   =  	   �  �  A     �  �  2   =  	   �  �  �  	   �  �  �       F      A     �  N  z  K   =  	   �  �  A     �  �  ;   =  	   �  �  �  	   �  �  �       G      A     �  N  }  K   =  	   �  �  A     �  �  C   =  	   �  �  �  	   �  �  �       H      A     �  N  �   K   =  	   �  �  A     �  �  K   =  	   �  �  �  	   �  �  �  >  �  �  >  �  �  >  �  �  >  �  �  9     �  (   �  �  �  �       C      >  �  �       J      =     �  �  =     �  �  A     �  N  �  K   =  	   �  �  A     �  �  �  >  �  �  �  �  �  �       3      =     �  �  �     �  �  z  >  �  �  �  �  �  �       M      A  �  �    2   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  �  	   �  �   �  A  �  �    ;   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  �  	   �  �   �  �  	   �  �  �  A     �  �  2   >  �  �       N      A  �  �    2   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  �  	   �  �   �  A  �  �    ;   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  �  	   �  �  �  A     �  �  ;   >  �  �       O      A  �  �    2   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  A  �  �    ;   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  �  	   �  �   �  �  	   �  �  �  A     �  �  C   >  �  �       P      A  �  �    2   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  A  �  �    ;   =  	   �  �  A  (  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  �  	   �  �  �  A     �  �  K   >  �  �       S      A        �  �  =  	        A       �  2   =  	       �  	              T      A       �  z  =  	       A       �  ;   =  	       �  	   	           U      A     
  �  }  =  	     
  A       �  C   =  	       �  	              V      A       �  �   =  	       A       �  K   =  	       �  	         >      >    	  >      >      9       (                Q      >  �         X      =       �  A       �    =  	       >           Y      =       m  =  	          	   !     .          �     "    !  =  	   #  �  �     $  "  #  =     %  �  �     &  %  $  >  �  &  �  �  �  �       [      A  +  ,  �  �  =  �  -  ,  =     .  �  Q  	   /  .      Q  	   0  .     Q  	   1  .     P  
   2  /  0  1  �   �  
   3  -  2  A  4  5  *  �  >  5  3  �  8       �      6  
             7        �     ;  *   +           �       =     ,      �     .   ,   -   p  	   /   .   �  	   1   /   0   A     3   +   2   >  3   1        �       =     4      �     7   4   6   �     8   7   -   p  	   9   8   �  	   :   9   0   A     <   +   ;   >  <   :        �       =     =      �     ?   =   >   �     @   ?   -   p  	   A   @   �  	   B   A   0   A     D   +   C   >  D   B        �       =     E      �     G   E   F   �     H   G   -   p  	   I   H   �  	   J   I   0   A     L   +   K   >  L   J        �       =  
   M   +   �  M   8       '  #   6               7        7        �     ;     P      ;     S      ;     V           )      =  	   Q        	   R         Q   >  P   R        *      =  	   T        	   U         T   >  S   U        ,      A     W      2   =  	   X   W   =  	   Y   S   �  	   Z   X   Y   A     [      C   =  	   \   [   =  	   ]   P   �  	   ^   \   ]   �  	   _   Z   ^   >  V   _        -      A     `      2   =  	   a   `     	   b   a   =  	   c   P   �  	   d   b   c   A     e      C   =  	   f   e   =  	   g   S   �  	   h   f   g   �  	   i   d   h   A     j      C   >  j   i        .      =  	   k   V   A     l      2   >  l   k        0      =     m      �  m   8       �  "   6  	             7        �          �      =     p      �  	   t   p   s     	   u         t   �  	   w   u   v     	   x      
   w   �  x   8       �  Z   6     !          7        7        7         �  "   ;     {      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �           �      =     |            }         |   >  {   }        �      A     �   {   2   =  	   �   �   A     �   {   ;   =  	   �   �   �  ~   �   �   �   �  �       �  �   �   �   �  �        �      A     �   {   2   =  	   �   �   A     �   {   C   =  	   �   �   �  ~   �   �   �   �  �   �  �   �  ~   �   �   "   �   �        �      >  �   �        �      A     �   {   C   =  	   �   �   A     �   {   2   =  	   �   �   �  ~   �   �   �   �  �       �  �   �   �   �  �        �      A     �   {   C   =  	   �   �   A     �   {   ;   =  	   �   �   �  ~   �   �   �   �  �   �  �   �  ~   �   �   �   �   �        �      >  �   �        �      =     �      O     �   �   �         =  ~   �   �   �  	   �   �   �   �   �     �   �   �   =     �      O     �   �   �          =  ~   �   �   �  	   �   �   �   �   �     �   �   �   �     �   �   �   =     �      O     �   �   �          =  ~   �   �   �  ~   �   �   =  ~   �   �   �  ~   �   �   �  ~   �   �   �   �  	   �   �   �   �   �     �   �   �   �     �   �   �   >  �   �        �      =     �   �   �     �   �   �   >  �   �        �      =     �      >  �   �   9  	   �      �     	   �      .   �   �   �   >  �   �        �      =     �      =     �   �   �     �   �   �   >  �   �        �      =     �   �   P     �   �   �   �     �   �   �   >  �   �        �      A     �   �   2   =  	   �   �   A     �   �   ;   =  	   �   �   P     �   �   �   �   >  �   �   =  	   �   �   >  �   �   9     �      �   �   O     �   �   �          >  �   �        �      =     �   �   P     �   �   �   �     �   �   �   >  �   �        �      =     �      =     �   �   �     �   �   �   >  �   �        �      =     �   �   �  �   8       �  M   6     (       #   7     $   7     %   7     &   7     '   �  )   ;     �      ;     �           �      >  �   2        �      =  	   �   $   >  �   �        �      =  	   �   �   =  	   �   %   �  ~   �   �   �   �  �       �  �   �   �   �  �        �      >  �   ;        �      =  	   �   %   >  �   �   �  �   �  �        �      =  	   �   �   =  	   �   &   �  ~   �   �   �   �  �       �  �   �   �   �  �        �      >  �   C        �      =  	   �   &   >  �   �   �  �   �  �        �      =  	   �   �   =  	   �   '   �  ~   �   �   �   �  �       �  �   �   �   �  �        �      >  �   K        �      =  	   �   '   >  �   �   �  �   �  �        �      =     �   �   �  �   8  ؾ      #     �                GLSL.std.450                     main    t    �  �  �  �  �               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Terrain_Fragment.glsl    2   �     // OpModuleProcessed entry-point main
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

//The sky texture.
layout (set = 0, binding = 3) uniform samplerCube SKY_TEXTURE;

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
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
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
#define TERRAIN_MINIMUM_DISPLACEMENT (0.0001f)
#define BIAS_DISPLACEMENT(X) (pow(X, 16.0f))
#define TERRAIN_MATERIAL_SCALE (0.5f)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    vec3 albedo;
    vec4 normal_map_displacement;
    vec4 material_properties;
};

/*
*   Calculates the material coordinate for the given world position.
*/
vec2 CalculateTerrainMaterialCoordinate(vec3 world_position, vec2 tile_index, vec3 normal)
{
#if 1
    //Take the absolute of the normal.
    vec3 absolute_normal = abs(normal);

    //Calculate the tile.
    bool x_dominant = absolute_normal.x > absolute_normal.y && absolute_normal.x > absolute_normal.z;
    bool z_dominant = absolute_normal.z > absolute_normal.x && absolute_normal.z > absolute_normal.y;
    vec2 tile = world_position.yz * float(x_dominant) + world_position.xy * float(z_dominant) + world_position.xz * float(!x_dominant && !z_dominant);

    tile *= TERRAIN_MATERIAL_SCALE;

    //Calculate the random rotation.
    float random_rotation = mix(-PI, PI, RandomFloat(tile_index));

    //Randomly rotate the tile.
    tile -= tile_index;
    tile -= 0.5f;
    tile = RotateYaw(vec3(tile.x, 0.0f, tile.y), random_rotation).xz;
    tile += 0.5f;
    tile += tile_index;

    //Return the tile!
    return tile;
#else
    return vec2(world_position.x, world_position.z) * 0.5f;
#endif
}

/*
*   Returns the highest index.
*/
uint HighestIndex(float value_1, float value_2, float value_3, float value_4)
{
    uint highest_index = 0;
    float highest_value = value_1;

    if (highest_value < value_2)
    {
        highest_index = 1;
        highest_value = value_2;
    }

    if (highest_value < value_3)
    {
        highest_index = 2;
        highest_value = value_3;
    }

    if (highest_value < value_4)
    {
        highest_index = 3;
        highest_value = value_4;
    }

    return highest_index;
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
    vec3 normals[4];
    vec3 normal;
    {
        normals[0] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), InHeightMapTextureCoordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        normals[1] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), InHeightMapTextureCoordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        normals[2] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), InHeightMapTextureCoordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        normals[3] = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), InHeightMapTextureCoordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        vec3 blend_1 = mix(normals[0], normals[1], fract(InHeightMapTextureCoordinate.y * MAP_RESOLUTION));
	    vec3 blend_2 = mix(normals[2], normals[3], fract(InHeightMapTextureCoordinate.y * MAP_RESOLUTION));
	    normal = mix(blend_1, blend_2, fract(InHeightMapTextureCoordinate.x * MAP_RESOLUTION));
        normal = normal * 2.0f - 1.0f;
        normal = normalize(normal);
    }
    mat3 tangent_space_matrix = CalculateGramSchmidtRotationMatrix(normal, vec3(0.0f, 0.0f, 1.0f));
    TerrainMaterial terrain_materials[4];
    vec2 sample_offsets[4];
    sample_offsets[0] = vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
    sample_offsets[1] = vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
    sample_offsets[2] = vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
    sample_offsets[3] = vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
    for (uint i = 0; i < 4; ++i)
    {
        vec2 material_texture_coordinate = CalculateTerrainMaterialCoordinate(InWorldPosition, floor(InHeightMapTextureCoordinate * MAP_RESOLUTION) + sample_offsets[i] * MAP_RESOLUTION, normals[i]);
        vec2 height_map_texture_coordinate = InHeightMapTextureCoordinate + sample_offsets[i];
        vec4 index_map = texture(sampler2D(TEXTURES[INDEX_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), height_map_texture_coordinate);
        vec4 blend_map = texture(sampler2D(TEXTURES[BLEND_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), height_map_texture_coordinate);
        Material materials[4];
        materials[0] = MATERIALS[uint(index_map[0] * float(UINT8_MAXIMUM))];
        materials[1] = MATERIALS[uint(index_map[1] * float(UINT8_MAXIMUM))];
        materials[2] = MATERIALS[uint(index_map[2] * float(UINT8_MAXIMUM))];
        materials[3] = MATERIALS[uint(index_map[3] * float(UINT8_MAXIMUM))];
        vec4 normal_map_displacements[4];
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[0], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[0]);
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[1], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[1]);
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[2], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[2]);
        EVALUATE_NORMAL_MAP_DISPLACEMENT(materials[3], material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacements[3]);
        uint highest_index = HighestIndex
        (
            normal_map_displacements[0].w * blend_map[0],
            normal_map_displacements[1].w * blend_map[1],
            normal_map_displacements[2].w * blend_map[2],
            normal_map_displacements[3].w * blend_map[3]
        );
        vec4 albedo_thickness;
        EVALUATE_ALBEDO_THICKNESS(materials[highest_index], material_texture_coordinate, MATERIAL_SAMPLER, albedo_thickness);
        vec4 material_properties;
        EVALUATE_MATERIAL_PROPERTIES(materials[highest_index], material_texture_coordinate, MATERIAL_SAMPLER, material_properties);
        terrain_materials[i].albedo = albedo_thickness.rgb;
        terrain_materials[i].normal_map_displacement = normal_map_displacements[highest_index];
        terrain_materials[i].material_properties = material_properties;
    }
    vec4 blends;
    float fractions[2];
    fractions[0] = fract(InHeightMapTextureCoordinate.x * MAP_RESOLUTION);
    fractions[1] = fract(InHeightMapTextureCoordinate.y * MAP_RESOLUTION);
    blends[0] = (1.0f - fractions[0]) * (1.0f - fractions[1]);
    blends[1] = (1.0f - fractions[0]) * (fractions[1]);
    blends[2] = (fractions[0]) * (1.0f - fractions[1]);
    blends[3] = (fractions[0]) * (fractions[1]);
    uint highest_index = HighestIndex
    (
        terrain_materials[0].normal_map_displacement.w * blends[0],
        terrain_materials[1].normal_map_displacement.w * blends[1],
        terrain_materials[2].normal_map_displacement.w * blends[2],
        terrain_materials[3].normal_map_displacement.w * blends[3]
    );
    TerrainMaterial final_material;
    final_material.albedo = terrain_materials[highest_index].albedo;
    final_material.normal_map_displacement = terrain_materials[highest_index].normal_map_displacement;
    final_material.material_properties = terrain_materials[highest_index].material_properties;
    final_material.normal_map_displacement.xyz = final_material.normal_map_displacement.xyz * 2.0f - 1.0f;
	final_material.normal_map_displacement.xyz = normalize(final_material.normal_map_displacement.xyz);
	vec3 shading_normal = normalize(tangent_space_matrix * final_material.normal_map_displacement.xyz);
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(final_material.albedo,1.0f);
	SceneFeatures2 = vec4(shading_normal,gl_FragCoord.z);
	SceneFeatures3 = final_material.material_properties;
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       RotateYaw(vf3;f1;        X        angle     #   CalculateGramSchmidtRotationMatrix(vf3;vf3;   !   normal    "   random_tilt   (   RandomFloat(vf2;      '   coordinate    .   CalculateTerrainMaterialCoordinate(vf3;vf2;vf3;   +   world_position    ,   tile_index    -   normal   	 5   HighestIndex(f1;f1;f1;f1;     1   value_1   2   value_2   3   value_3   4   value_4   8   unpacked      ]   view_space_position   _   Camera   	 _       WORLD_TO_CLIP_MATRIX     	 _      WORLD_TO_CAMERA_MATRIX    _      PREVIOUS_WORLD_TO_CLIP_MATRIX     _      INVERSE_WORLD_TO_CAMERA_MATRIX    _      INVERSE_CAMERA_TO_CLIP_MATRIX    	 _      CAMERA_WORLD_POSITION    	 _      CAMERA_FORWARD_VECTOR    	 _      CURRENT_FRAME_JITTER      _      NEAR_PLANE    _   	   FAR_PLANE     a         m   denominator   �   view_space_position   �   denominator   �   sine      �   cosine    �   temp      �   random_tangent    �   random_bitangent      �   absolute_normal   �   x_dominant    �   z_dominant    
  tile      "  random_rotation   %  param     4  param     5  param     B  highest_index     C  highest_value     ]  normals   b  TEXTURES      c  PushConstantData      c      WORLD_POSITION    c     MINIMUM_HEIGHT_MAP_COORDINATE     c     MAXIMUM_HEIGHT_MAP_COORDINATE     c     BORDERS  
 c     PATCH_RESOLUTION_RECIPROCAL   c     PATCH_SIZE   
 c     HEIGHT_MAP_TEXTURE_INDEX     
 c     NORMAL_MAP_TEXTURE_INDEX     	 c     INDEX_MAP_TEXTURE_INDEX  	 c  	   BLEND_MAP_TEXTURE_INDEX   c  
   MAP_RESOLUTION   
 c     MAP_RESOLUTION_RECIPROCAL     e        o  NORMAL_MAP_SAMPLER   
 t  InHeightMapTextureCoordinate      �  blend_1   �  blend_2   �  normal    �  tangent_space_matrix      �  param     �  param     �  sample_offsets    �  i    	 �  material_texture_coordinate     InWorldPosition     param       param       param    
   height_map_texture_coordinate       index_map     !  INDEX_BLEND_MAP_SAMPLER   &  blend_map     0  Material      0      _Properties   0     _AlbedoThickness     	 0     _NormalMapDisplacement    0     _MaterialProperties   0     _Opacity      0     _EmissiveMultiplier   0     _Padding1     0     _Padding2     3  materials     4  Material      4      _Properties   4     _AlbedoThickness     	 4     _NormalMapDisplacement    4     _MaterialProperties   4     _Opacity      4     _EmissiveMultiplier   4     _Padding1     4     _Padding2     7  GlobalMaterials   7      MATERIALS     9       	 �  normal_map_displacements      �  MATERIAL_SAMPLER      �  param     �  param     �  param     �  param     �  highest_index     	  param     
  param       param       param       albedo_thickness      "  param     .  material_properties   :  param     >  TerrainMaterial   >      albedo   	 >     normal_map_displacement   >     material_properties   A  terrain_materials     R  fractions     a  blends    ~  highest_index     �  param     �  param     �  param     �  param     �  final_material    �  shading_normal    �  velocity      �  param     �  param     �  SceneFeatures1    �  SceneFeatures2    �  gl_FragCoord      �  SceneFeatures3    �  SceneFeatures4    �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE   �  HEIGHT_MAP_SAMPLER  H  _          H  _       #       H  _             H  _         H  _      #   @   H  _            H  _         H  _      #   �   H  _            H  _         H  _      #   �   H  _            H  _         H  _      #      H  _            H  _      #   @  H  _      #   P  H  _      #   `  H  _      #   h  H  _   	   #   l  G  _      G  a   "      G  a   !       G  b  "       G  b  !       H  c      #       H  c     #      H  c     #      H  c     #      H  c     #      H  c     #       H  c     #   $   H  c     #   (   H  c     #   ,   H  c  	   #   0   H  c  
   #   4   H  c     #   8   G  c     G  o  "      G  o  !      G  t        G           G  !  "      G  !  !      H  4      #       H  4     #      H  4     #      H  4     #      H  4     #      H  4     #      H  4     #      H  4     #      G  6         H  7      #       G  7     G  9  "       G  9  !      G  �  "      G  �  !      G  �         G  �        G  �        G  �        G  �        G  �  "       G  �  !      G  �  "       G  �  !      G  �  "      G  �  !           !                                   	         
   	      !     
           	                       	      !                    	   !                         !                  %         !  &   	   %   !  *         %      !  0                     7      
   +     :   �   +  	   =     C+     ?         B          +  B   C      +     H      +  B   K      +     P      +  B   S      +     X        ^   
        _   ^   ^   ^   ^   ^            	   	      `      _   ;  `   a      +  B   b          c      ^   +  	   g     �?+  	   {      ?+  B   �      +  	   �       +  	   �   9�OA+  	   �   Lw�B,     �   �   �   +  	   �   ��*G  �      �      �   +  	   #  �I�+  	   $  �I@+     Z       [     Z     \     [   	 ^  	                         +     _       `  ^  _     a      `  ;  a  b        c              	   	               	   	      d  	   c  ;  d  e  	   +  B   f        g  	         j      ^    m     n      m  ;  n  o        q  ^     s        ;  s  t     ,     v  �   �   +  B   w        x  	   	   +  B   �     ,     �  �   g   ,     �  g   �   +  B   �     ,     �  g   g      �     	   +  B   �  
   +  	   �     @   �        ,     �  �   �   g     �     Z     �     �              ;          ;  n  !      +  B   '  	    
 0                 	           1  0  Z     2     1   
 4                 	         +     5       6  4  5    7  6     8     7  ;  8  9        >     4     A     0  +  B   L     +  B   O     +  B   R     +     �       �  
   Z     �     �  ;  n  �      +          +     )        >     
   
     ?  >  Z     @     ?    P  	   P      Q     P     �     >     �           �     
   ;  �  �     ;  �  �        �     
   ;  �  �     ;  �  �     ;  �  �       �  q  )     �      �  ;  �  �       	 �  	                           �  �     �      �  ;  �  �      ;  n  �           �     6               �     ;  \  ]     ;     �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;  �  �     ;     �     ;  %   �     ;          ;  %        ;          ;  %        ;  7        ;  7   &     ;  2  3     ;  �  �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     	     ;     
     ;          ;          ;  7        ;     "     ;  7   .     ;     :     ;  @  A     ;  Q  R     ;  7   a     ;     ~     ;     �     ;     �     ;     �     ;     �     ;  �  �     ;     �     ;  %   �     ;     �     ;     �                 A  g  h  e  f  =     i  h  A  j  k  b  i  =  ^  l  k  =  m  p  o  V  q  r  l  p  =     u  t  A  x  y  e  w  =  	   z  y  �     {  v  z  �     |  u  {  W  
   }  r  |  O     ~  }  }            A       ]  b   >    ~             A  g  �  e  f  =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  o  V  q  �  �  �  =     �  t  A  x  �  e  w  =  	   �  �  �     �  �  �  �     �  �  �  W  
   �  �  �  O     �  �  �            A     �  ]  �  >  �  �             A  g  �  e  f  =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  o  V  q  �  �  �  =     �  t  A  x  �  e  w  =  	   �  �  �     �  �  �  �     �  �  �  W  
   �  �  �  O     �  �  �            A     �  ]  �   >  �  �             A  g  �  e  f  =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  o  V  q  �  �  �  =     �  t  A  x  �  e  w  =  	   �  �  �     �  �  �  �     �  �  �  W  
   �  �  �  O     �  �  �            A     �  ]  �  >  �  �             A     �  ]  b   =     �  �  A     �  ]  �  =     �  �  A  �  �  t  H   =  	   �  �  A  x  �  e  �  =  	   �  �  �  	   �  �  �    	   �     
   �  P     �  �  �  �       �     .   �  �  �  >  �  �             A     �  ]  �   =     �  �  A     �  ]  �  =     �  �  A  �  �  t  H   =  	   �  �  A  x  �  e  �  =  	   �  �  �  	   �  �  �    	   �     
   �  P     �  �  �  �       �     .   �  �  �  >  �  �             =     �  �  =     �  �  A  �  �  t  ?   =  	   �  �  A  x  �  e  �  =  	   �  �  �  	   �  �  �    	   �     
   �  P     �  �  �  �       �     .   �  �  �  >  �  �             =     �  �  �     �  �  �  P     �  g   g   g   �     �  �  �  >  �  �             =     �  �       �     E   �  >  �  �       
      =     �  �  >  �  �  >  �  �  9     �  #   �  �  >  �  �             A  x  �  e  w  =  	   �  �  �     �  v  �  A  %   �  �  b   >  �  �             A  x  �  e  w  =  	   �  �  �     �  �  �  A  %   �  �  �  >  �  �             A  x  �  e  w  =  	   �  �  �     �  �  �  A  %   �  �  �   >  �  �             A  x  �  e  w  =  	   �  �  �     �  �  �  A  %   �  �  �  >  �  �             >  �  ?   �  �  �  �             �  �  �      �  �  �  �             =     �  �  �  �   �  �  Z  �  �  �  �  �  �             =       t  A  x    e  �  =  	       �                          =       �  A  %     �    =     	    A  x  
  e  �  =  	     
  �       	    �           =       �  =         >      >      A       ]    =         >      9       .         >  �               =       t  =       �  A  %     �    =         �           >                 A  g    e  C   =         A  j    b    =  ^       =  m  "  !  V  q  #     "  =     $    W  
   %  #  $  >    %             A  g  (  e  '  =     )  (  A  j  *  b  )  =  ^  +  *  =  m  ,  !  V  q  -  +  ,  =     .    W  
   /  -  .  >  &  /             A     :    ?   =  	   ;  :  �  	   <  ;  =   m     =  <  A  >  ?  9  b   =  =  4  @  ?  A  A  B  3  b   Q     C  @      A     D  B  b   >  D  C  Q     E  @     A     F  B  �  >  F  E  Q     G  @     A     H  B  �   >  H  G  Q     I  @     A     J  B  �  >  J  I  Q     K  @     A     M  B  L  >  M  K  Q  	   N  @     A     P  B  O  >  P  N  Q     Q  @     A     S  B  R  >  S  Q  Q     T  @     A     U  B  f  >  U  T             A     V    H   =  	   W  V  �  	   X  W  =   m     Y  X  A  >  Z  9  b   Y  =  4  [  Z  A  A  \  3  �  Q     ]  [      A     ^  \  b   >  ^  ]  Q     _  [     A     `  \  �  >  `  _  Q     a  [     A     b  \  �   >  b  a  Q     c  [     A     d  \  �  >  d  c  Q     e  [     A     f  \  L  >  f  e  Q  	   g  [     A     h  \  O  >  h  g  Q     i  [     A     j  \  R  >  j  i  Q     k  [     A     l  \  f  >  l  k             A     m    P   =  	   n  m  �  	   o  n  =   m     p  o  A  >  q  9  b   p  =  4  r  q  A  A  s  3  �   Q     t  r      A     u  s  b   >  u  t  Q     v  r     A     w  s  �  >  w  v  Q     x  r     A     y  s  �   >  y  x  Q     z  r     A     {  s  �  >  {  z  Q     |  r     A     }  s  L  >  }  |  Q  	   ~  r     A       s  O  >    ~  Q     �  r     A     �  s  R  >  �  �  Q     �  r     A     �  s  f  >  �  �             A     �    X   =  	   �  �  �  	   �  �  =   m     �  �  A  >  �  9  b   �  =  4  �  �  A  A  �  3  �  Q     �  �      A     �  �  b   >  �  �  Q     �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  �   >  �  �  Q     �  �     A     �  �  �  >  �  �  Q     �  �     A     �  �  L  >  �  �  Q  	   �  �     A     �  �  O  >  �  �  Q     �  �     A     �  �  R  >  �  �  Q     �  �     A     �  �  f  >  �  �             A     �  3  b   b   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �             A     �  3  b   �   =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  �  V  q  �  �  �  =     �  �  W  
   �  �  �  A  7   �  �  b   >  �  �  �  �  �  �             A     �  3  b   �   =     �  �  >  �  �  9  
   �     �  A  7   �  �  b   >  �  �  �  �  �  �             A     �  3  �  b   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �             A     �  3  �  �   =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  �  V  q  �  �  �  =     �  �  W  
   �  �  �  A  7   �  �  �  >  �  �  �  �  �  �             A     �  3  �  �   =     �  �  >  �  �  9  
   �     �  A  7   �  �  �  >  �  �  �  �  �  �             A     �  3  �   b   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �             A     �  3  �   �   =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  �  V  q  �  �  �  =     �  �  W  
   �  �  �  A  7   �  �  �   >  �  �  �  �  �  �             A     �  3  �   �   =     �  �  >  �  �  9  
   �     �  A  7   �  �  �   >  �  �  �  �  �  �              A     �  3  �  b   =     �  �  �     �  �  �  �  �   �  �  �  �  �      �  �  �  �  �  �              A     �  3  �  �   =     �  �  A  j  �  b  �  =  ^  �  �  =  m  �  �  V  q  �  �  �  =     �  �  W  
   �  �  �  A  7   �  �  �  >  �  �  �  �  �  �              A     �  3  �  �   =     �  �  >  �  �  9  
   �     �  A  7   �  �  �  >  �  �  �  �  �  �       #      A     �  �  b   X   =  	   �  �  A     �  &  ?   =  	   �  �  �  	   �  �  �       $      A     �  �  �  X   =  	   �  �  A     �  &  H   =  	   �  �  �  	   �  �  �       %      A     �  �  �   X   =  	      �  A       &  P   =  	       �  	               &      A       �  �  X   =  	       A       &  X   =  	       �  	         >  	  �  >  
  �  >      >      9       5   	  
           !      >  �         )      =       �  A       3    b   =         �           �  �         �        �         �         )      =       �  A       3    �  =         A  j    b    =  ^      =  m    �  V  q        =       �  W  
         >      �    �          )      =     !  �  A     #  3  !  �  =     $  #  >  "  $  9  
   %     "  >    %  �    �         +      =     &  �  A     '  3  &  b   =     (  '  �     *  (  )  �  �   +  *  )  �  -      �  +  ,  8  �  ,       +      =     /  �  A     0  3  /  �  =     1  0  A  j  2  b  1  =  ^  3  2  =  m  4  �  V  q  5  3  4  =     6  �  W  
   7  5  6  >  .  7  �  -  �  8       +      =     9  �  A     ;  3  9  �  =     <  ;  >  :  <  9  
   =     :  >  .  =  �  -  �  -       ,      =     B  �  =  
   C    O     D  C  C            A     E  A  B  b   >  E  D       -      =     F  �  =     G  �  A  7   H  �  G  =  
   I  H  A  7   J  A  F  �  >  J  I       .      =     K  �  =  
   L  .  A  7   M  A  K  �   >  M  L  �  �  �  �             =     N  �  �     O  N  �  >  �  O  �  �  �  �       2      A  �  S  t  ?   =  	   T  S  A  x  U  e  �  =  	   V  U  �  	   W  T  V    	   X     
   W  A     Y  R  b   >  Y  X       3      A  �  Z  t  H   =  	   [  Z  A  x  \  e  �  =  	   ]  \  �  	   ^  [  ]    	   _     
   ^  A     `  R  �  >  `  _       4      A     b  R  b   =  	   c  b  �  	   d  g   c  A     e  R  �  =  	   f  e  �  	   g  g   f  �  	   h  d  g  A     i  a  ?   >  i  h       5      A     j  R  b   =  	   k  j  �  	   l  g   k  A     m  R  �  =  	   n  m  �  	   o  l  n  A     p  a  H   >  p  o       6      A     q  R  b   =  	   r  q  A     s  R  �  =  	   t  s  �  	   u  g   t  �  	   v  r  u  A     w  a  P   >  w  v       7      A     x  R  b   =  	   y  x  A     z  R  �  =  	   {  z  �  	   |  y  {  A     }  a  X   >  }  |       :      A       A  b   �  X   =  	   �    A     �  a  ?   =  	   �  �  �  	   �  �  �       ;      A     �  A  �  �  X   =  	   �  �  A     �  a  H   =  	   �  �  �  	   �  �  �       <      A     �  A  �   �  X   =  	   �  �  A     �  a  P   =  	   �  �  �  	   �  �  �       =      A     �  A  �  �  X   =  	   �  �  A     �  a  X   =  	   �  �  �  	   �  �  �  >  �  �  >  �  �  >  �  �  >  �  �  9     �  5   �  �  �  �       8      >  ~  �       @      =     �  ~  A     �  A  �  b   =     �  �  A     �  �  b   >  �  �       A      =     �  ~  A  7   �  A  �  �  =  
   �  �  A  7   �  �  �  >  �  �       B      =     �  ~  A  7   �  A  �  �   =  
   �  �  A  7   �  �  �   >  �  �       C      A  7   �  �  �  =  
   �  �  O     �  �  �            �     �  �  �  P     �  g   g   g   �     �  �  �  A     �  �  �  ?   Q  	   �  �      >  �  �  A     �  �  �  H   Q  	   �  �     >  �  �  A     �  �  �  P   Q  	   �  �     >  �  �       D      A  7   �  �  �  =  
   �  �  O     �  �  �                 �     E   �  A     �  �  �  ?   Q  	   �  �      >  �  �  A     �  �  �  H   Q  	   �  �     >  �  �  A     �  �  �  P   Q  	   �  �     >  �  �       E      =     �  �  A  7   �  �  �  =  
   �  �  O     �  �  �            �     �  �  �       �     E   �  >  �  �       F      =     �    >  �  �  9     �     �  =     �    >  �  �  9     �     �  �     �  �  �  A  �  �  a   f  =     �  �  �     �  �  �  >  �  �       G      A     �  �  b   =     �  �  Q  	   �  �      Q  	   �  �     Q  	   �  �     P  
   �  �  �  �  g   >  �  �       H      =     �  �  A  �  �  �  P   =  	   �  �  Q  	   �  �      Q  	   �  �     Q  	   �  �     P  
   �  �  �  �  �  >  �  �       I      A  7   �  �  �   =  
   �  �  >  �  �       J      =     �  �  Q  	   �  �      Q  	   �  �     P  
   �  �  �  �   �   >  �  �  �  8       �      6  
             7        �     ;  7   8           �       =     9      �     ;   9   :   p  	   <   ;   �  	   >   <   =   A     @   8   ?   >  @   >        �       =     A      �     D   A   C   �     E   D   :   p  	   F   E   �  	   G   F   =   A     I   8   H   >  I   G        �       =     J      �     L   J   K   �     M   L   :   p  	   N   M   �  	   O   N   =   A     Q   8   P   >  Q   O        �       =     R      �     T   R   S   �     U   T   :   p  	   V   U   �  	   W   V   =   A     Y   8   X   >  Y   W        �       =  
   Z   8   �  Z   8         :   6               7        �     ;  7   ]      ;     m                 A  c   d   a   b   =  ^   e   d   =     f      Q  	   h   f       Q  	   i   f      Q  	   j   f      P  
   k   h   i   j   g   �  
   l   e   k   >  ]   l              A     n   ]   X   =  	   o   n   �  	   p   g   o   >  m   p              =  	   q   m   =  
   r   ]   O     s   r   r          �     t   s   q   A     u   ]   ?   Q  	   v   t       >  u   v   A     w   ]   H   Q  	   x   t      >  w   x              =  
   y   ]   O     z   y   y          �     |   z   {   P     }   {   {   �     ~   |   }   �  ~   8         ;   6               7        �     ;  7   �      ;     �                 A  c   �   a   �   =  ^   �   �   =     �      Q  	   �   �       Q  	   �   �      Q  	   �   �      P  
   �   �   �   �   g   �  
   �   �   �   >  �   �              A     �   �   X   =  	   �   �   �  	   �   g   �   >  �   �              =  	   �   �   =  
   �   �   O     �   �   �          �     �   �   �   A     �   �   ?   Q  	   �   �       >  �   �   A     �   �   H   Q  	   �   �      >  �   �              =  
   �   �   O     �   �   �          �     �   �   {   P     �   {   {   �     �   �   �   �  �   8       )  #   6               7        7        �     ;     �      ;     �      ;     �           +      =  	   �        	   �         �   >  �   �        ,      =  	   �        	   �         �   >  �   �        .      A     �      ?   =  	   �   �   =  	   �   �   �  	   �   �   �   A     �      P   =  	   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   >  �   �        /      A     �      ?   =  	   �   �     	   �   �   =  	   �   �   �  	   �   �   �   A     �      P   =  	   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   A     �      P   >  �   �        0      =  	   �   �   A     �      ?   >  �   �        2      =     �      �  �   8       8  F   6     #           7     !   7     "   �  $   ;     �      ;     �           :      =     �   "   =     �   !   =     �   "   =     �   !   �  	   �   �   �   �     �   �   �   �     �   �   �        �      E   �   >  �   �        ;      =     �   !   =     �   �        �      D   �   �   >  �   �        =      =     �   �   =     �   �   =     �   !   Q  	   �   �       Q  	   �   �      Q  	   �   �      Q  	   �   �       Q  	   �   �      Q  	   �   �      Q  	   �   �       Q  	   �   �      Q  	   �   �      P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   �  �   8       �  "   6  	   (       &   7  %   '   �  )        �      =     �   '   �  	   �   �   �     	   �         �   �  	   �   �   �     	   �      
   �   �  �   8       �  Z   6     .       *   7     +   7  %   ,   7     -   �  /   ;     �      ;  �   �      ;  �   �      ;  %   
     ;     "     ;  %   %     ;     4     ;     5          �      =     �   -        �         �   >  �   �        �      A     �   �   ?   =  	   �   �   A     �   �   H   =  	   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        �      A     �   �   ?   =  	   �   �   A     �   �   P   =  	   �   �   �  �   �   �   �   �  �   �  �   �  �   �   �   /   �   �        �      >  �   �        �      A     �   �   P   =  	   �   �   A     �   �   ?   =  	      �   �  �     �      �        �        �         �      A       �   P   =  	       A       �   H   =  	       �  �         �    �    �  �   	    �            �      >  �   	       �      =       +   O                 =  �     �   �  	       g   �   �           =       +   O                  =  �     �   �  	       g   �   �           �           =       +   O                  =  �     �   �  �       =  �     �   �  �       �  �         �  	       g   �   �           �           >  
         �      =        
  �     !     {   >  
  !       �      =     &  ,   >  %  &  9  	   '  (   %    	   (     .   #  $  '  >  "  (       �      =     )  ,   =     *  
  �     +  *  )  >  
  +       �      =     ,  
  P     -  {   {   �     .  ,  -  >  
  .       �      A     /  
  ?   =  	   0  /  A     1  
  H   =  	   2  1  P     3  0  �   2  >  4  3  =  	   6  "  >  5  6  9     7     4  5  O     8  7  7         >  
  8       �      =     9  
  P     :  {   {   �     ;  9  :  >  
  ;       �      =     <  ,   =     =  
  �     >  =  <  >  
  >       �      =     ?  
  �  ?  8       �  M   6     5       0   7     1   7     2   7     3   7     4   �  6   ;     B     ;     C          �      >  B  ?        �      =  	   D  1   >  C  D       �      =  	   E  C  =  	   F  2   �  �   G  E  F  �  I      �  G  H  I  �  H       �      >  B  H        �      =  	   J  2   >  C  J  �  I  �  I       �      =  	   K  C  =  	   L  3   �  �   M  K  L  �  O      �  M  N  O  �  N       �      >  B  P        �      =  	   P  3   >  C  P  �  O  �  O       �      =  	   Q  C  =  	   R  4   �  �   S  Q  R  �  U      �  S  T  U  �  T       �      >  B  X        �      =  	   V  4   >  C  V  �  U  �  U       �      =     W  B  �  W  8                          ��KT��D	                               ��v)��%�9����Y��G� �z��.v<                                I�%i/)�