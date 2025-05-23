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
	layout (offset = 0) vec3 COLOR;
	layout (offset = 16) vec2 WORLD_POSITION;
	layout (offset = 24) vec2 MINIMUM_HEIGHT_MAP_COORDINATE;
	layout (offset = 32) vec2 MAXIMUM_HEIGHT_MAP_COORDINATE;
	layout (offset = 40) uint BORDERS;
	layout (offset = 44) float PATCH_RESOLUTION_RECIPROCAL;
	layout (offset = 48) float PATCH_SIZE;
	layout (offset = 52) uint HEIGHT_MAP_TEXTURE_INDEX;
	layout (offset = 56) uint NORMAL_MAP_TEXTURE_INDEX;
	layout (offset = 60) uint INDEX_MAP_TEXTURE_INDEX;
	layout (offset = 64) uint BLEND_MAP_TEXTURE_INDEX;
	layout (offset = 68) float MAP_RESOLUTION;
	layout (offset = 72) float MAP_RESOLUTION_RECIPROCAL;
};

layout (location = 0) in vec2 InPosition;
layout (location = 1) in uint InBorders;

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
    vec3 world_position;
    world_position.x = WORLD_POSITION.x + mix(-(PATCH_SIZE * 0.5f), (PATCH_SIZE * 0.5f), stitched_position.x);
    world_position.y = 0.0f;
    world_position.z = WORLD_POSITION.y + mix(-(PATCH_SIZE * 0.5f), (PATCH_SIZE * 0.5f), stitched_position.y);
    {
        float height_1 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float height_2 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float height_3 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float height_4 = texture(sampler2D(TEXTURES[HEIGHT_MAP_TEXTURE_INDEX], HEIGHT_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).x;
        float blend_1 = mix(height_1, height_2, fract(height_map_coordinate.y * MAP_RESOLUTION));
	    float blend_2 = mix(height_3, height_4, fract(height_map_coordinate.y * MAP_RESOLUTION));
	    float final_height = mix(blend_1, blend_2, fract(height_map_coordinate.x * MAP_RESOLUTION));
        world_position.y = final_height;
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
    float displacement_multiplier = 1.0f - min(length(world_position - CAMERA_WORLD_POSITION) / 64.0f, 1.0f);
    if (displacement_multiplier > 0.0f)
    {
        vec2 base_coordinate = vec2
        (
            float(uint(height_map_coordinate.x * MAP_RESOLUTION)) + 0.5f,
            float(uint(height_map_coordinate.y * MAP_RESOLUTION)) + 0.5f
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
            vec2 material_texture_coordinate = CalculateTerrainMaterialCoordinate(world_position, terrain_normal, sample_coordinates[sample_index] * MAP_RESOLUTION * TERRAIN_MATERIAL_SCALE);
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
        float horizontal_weight = fract(height_map_coordinate.x * MAP_RESOLUTION);
        float vertical_weight = fract(height_map_coordinate.y * MAP_RESOLUTION);
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
        world_position += normal * mix(-0.125f, 0.1875f, final_displacement) * displacement_multiplier; //Slight bias for upward displacement.
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(world_position,1.0f);
}