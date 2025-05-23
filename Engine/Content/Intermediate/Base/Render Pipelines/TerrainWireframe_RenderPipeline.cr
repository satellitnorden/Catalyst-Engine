�nX/J1������TerrainWireframe_RenderPipeline                                  ��      #     z                GLSL.std.450                      main    �   �   c       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\TerrainWireframe_Vertex.glsl     �   �     // OpModuleProcessed entry-point main
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
}    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        RotateYaw(vf3;f1;        X        angle        RandomFloat(vf2;         coordinate    !   CalculateTerrainMaterialCoordinate(vf3;vf3;vf2;      world_position       normal        tile_index    $   unpacked      I   sine      L   cosine    O   temp      t   absolute_normal   y   x_dominant    �   z_dominant    �   material_coordinate   �   random_rotation   �   param     �   param     �   param     �   stitched_position     �   InPosition    �   is_left_multiplier    �   InBorders     �   PushConstantData      �       COLOR     �      WORLD_POSITION    �      MINIMUM_HEIGHT_MAP_COORDINATE     �      MAXIMUM_HEIGHT_MAP_COORDINATE     �      BORDERS  
 �      PATCH_RESOLUTION_RECIPROCAL   �      PATCH_SIZE   
 �      HEIGHT_MAP_TEXTURE_INDEX     
 �      NORMAL_MAP_TEXTURE_INDEX     	 �   	   INDEX_MAP_TEXTURE_INDEX  	 �   
   BLEND_MAP_TEXTURE_INDEX   �      MAP_RESOLUTION   
 �      MAP_RESOLUTION_RECIPROCAL     �         �   is_right_multiplier   �   vertical_offset   �   is_down_multiplier    �   is_up_multiplier        horizontal_offset       is_left_multiplier    "  is_right_multiplier   +  vertical_offset   0  is_down_multiplier    9  is_up_multiplier      B  horizontal_offset     Z  height_map_coordinate     l  world_position    �  height_1      �  TEXTURES      �  HEIGHT_MAP_SAMPLER    �  height_2      �  height_3      �  height_4      �  blend_1   �  blend_2   �  final_height      �  normals   �  NORMAL_MAP_SAMPLER    2  blend_1   ?  blend_2   L  normal    ]  displacement_multiplier   `  Camera   	 `      WORLD_TO_CLIP_MATRIX     	 `     WORLD_TO_CAMERA_MATRIX    `     PREVIOUS_WORLD_TO_CLIP_MATRIX     `     INVERSE_WORLD_TO_CAMERA_MATRIX    `     INVERSE_CAMERA_TO_CLIP_MATRIX    	 `     CAMERA_WORLD_POSITION    	 `     CAMERA_FORWARD_VECTOR    	 `     CURRENT_FRAME_JITTER      `     NEAR_PLANE    `  	   FAR_PLANE     b        p  base_coordinate   �  sample_coordinates    �  sample_index      �  terrain_normal    �  index_map     �  INDEX_BLEND_MAP_SAMPLER   �  blend_map     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  materials     �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �       	 8  material_texture_coordinate   @  param     B  param     D  param    	 N  normal_map_displacements      S  MATERIAL_SAMPLER      Z  param     o  param     �  param     �  param     �  weights_sum   �  weight_index      �  weights   �  weights_sum_reciprocal    �  weight_index      �  displacements     �  horizontal_weight     �  vertical_weight   �  weights     weights_sum     weight_index      +  weights_sum_reciprocal    .  weight_index      >  final_displacement    a  gl_PerVertex      a      gl_Position   a     gl_PointSize      a     gl_ClipDistance   a     gl_CullDistance   c       	 p  GlobalTextureAverageValues   	 p      TEXTURE_AVERAGE_VALUES    r        u  BLUE_NOISE_TEXTURES   y  SKY_TEXTURE G  �          G  �         H  �       #       H  �      #      H  �      #      H  �      #       H  �      #   (   H  �      #   ,   H  �      #   0   H  �      #   4   H  �      #   8   H  �   	   #   <   H  �   
   #   @   H  �      #   D   H  �      #   H   G  �      G  �  "       G  �  !       G  �  "      G  �  !      G  �  "      G  �  !      H  `         H  `      #       H  `            H  `        H  `     #   @   H  `           H  `        H  `     #   �   H  `           H  `        H  `     #   �   H  `           H  `        H  `     #      H  `           H  `     #   @  H  `     #   P  H  `     #   `  H  `     #   h  H  `  	   #   l  G  `     G  b  "      G  b  !       G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  S  "      G  S  !      H  a             H  a           H  a           H  a           G  a     G  o        H  p      #       G  p     G  r  "       G  r  !      G  u  "       G  u  !      G  y  "       G  y  !           !                                   	         
   	      !     
           	                           	   !                   	                  !     	      !                    #      
   +     &   �   +  	   )     C+     +         .          +  .   /      +     4      +  .   7      +     <      +  .   ?      +     D      +  	   j   9�OA+  	   k   Lw�B,     l   j   k   +  	   o   ��*G  w      x      w   +  	   �       +  	   �     �?+  	   �      ?+  	   �   �I�+  	   �   �I@   �         ;  �   �         �         ;  �   �        �                  	   	               	   	      �   	   �   ;  �   �   	   +  .   �         �   	      +     �      +     �      +     �   @   +  .           	  	   	   +     $     +     2      +     ;  �   +  	   I     @+  .   [     +  .   ^     +  .   m     +  .   p      	 �  	                         +     �       �  �  �     �      �  ;  �  �      +  .   �        �      �    �     �      �  ;  �  �        �  �  ,     �  �   �   +  .   �     ,     �  �   �   ,     �  �   �   ,     �  �   �   +  .   �       �     �      �     �  +  .   �      ;  �  �        _  
        `  _  _  _  _  _           	   	      a     `  ;  a  b        c        +  	   h    �B  �     �      �     �  +  .   �  	   ;  �  �      +  .   �  
    
 �                 	           �  �  �      �     �   
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �        �     �     �     �    L  
   �      M     L  ;  �  S        �  	   �      �     �  +  	   �     A+  	   �     4+  	   W     �+  	   X    @>  `  	   4     a  
   	   `  `     b     a  ;  b  c        d     _     m     
     o  
   �    p  o     q     p  ;  q  r       s  �  2     t      s  ;  t  u       	 v  	                           w  v     x      w  ;  x  y           �     6               �     ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;          ;          ;     "     ;     +     ;     0     ;     9     ;     B     ;     Z     ;     l     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  �  �     ;     2     ;     ?     ;     L     ;     ]     ;     p     ;  �  �     ;     �     ;     �     ;  #   �     ;  #   �     ;  �  �     ;     8     ;     @     ;     B     ;     D     ;  M  N     ;     Z     ;     o     ;     �     ;     �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;  �  �     ;          ;          ;     +     ;     .     ;     >          �      =     �   �   >  �   �        �      =     �   �   �     �   �   4   A  �   �   �   �   =     �   �   �     �   �   4   �     �   �   �   p  	   �   �   >  �   �        �      =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        �      =  	   �   �   =  	   �   �   �  	   �   �   �     	   �      %   �   �   >  �   �        �      =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        �      =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   �        �   �   �       �      p  	       >  �          �      =  	     �   =  	     �   �  	           	        %     �   >           �      A  	  
  �     =  	     
  =  	     �   �  	         A       �   4   =  	       �  	         A       �   4   >           �      A  	    �     =  	       =  	       �  	         A       �   +   =  	       �  	         A       �   +   >           �      =       �   �         <   A  �     �   �   =         �         <   �            p  	   !     >    !       �      =     #  �   �     %  #  $  A  �   &  �   �   =     '  &  �     (  '  $  �     )  %  (  p  	   *  )  >  "  *       �      =  	   ,    =  	   -  "  �  	   .  ,  -    	   /     %   .  �   >  +  /       �      =     1  �   �     3  1  2  A  �   4  �   �   =     5  4  �     6  5  2  �     7  3  6  p  	   8  7  >  0  8       �      =     :  �   �     <  :  ;  A  �   =  �   �   =     >  =  �     ?  >  ;  �     @  <  ?  p  	   A  @  >  9  A       �      =  	   C  0  =  	   D  9  �  	   E  C  D    	   F     %   E  �   >  B  F       �      A  	  G  �     =  	   H  G  �  	   J  H  I  =  	   K  +  �  	   L  J  K  A     M  �   4   =  	   N  M  �  	   O  N  L  A     P  �   4   >  P  O       �      A  	  Q  �     =  	   R  Q  �  	   S  R  I  =  	   T  B  �  	   U  S  T  A     V  �   +   =  	   W  V  �  	   X  W  U  A     Y  �   +   >  Y  X       �      A  	  \  �   [  +   =  	   ]  \  A  	  _  �   ^  +   =  	   `  _  A     a  �   +   =  	   b  a    	   c     .   ]  `  b  A  	  d  �   [  4   =  	   e  d  A  	  f  �   ^  4   =  	   g  f  A     h  �   4   =  	   i  h    	   j     .   e  g  i  P     k  c  j  >  Z  k       �      A  	  n  �   m  +   =  	   o  n  A  	  q  �   p  =  	   r  q  �  	   s  r  �     	   t  s  A  	  u  �   p  =  	   v  u  �  	   w  v  �   A     x  �   +   =  	   y  x    	   z     .   t  w  y  �  	   {  o  z  A     |  l  +   >  |  {       �      A     }  l  4   >  }  �               A  	  ~  �   m  4   =  	     ~  A  	  �  �   p  =  	   �  �  �  	   �  �  �     	   �  �  A  	  �  �   p  =  	   �  �  �  	   �  �  �   A     �  �   4   =  	   �  �    	   �     .   �  �  �  �  	   �    �  A     �  l  <   >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  Z  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  X  
   �  �  �     �   Q  	   �  �      >  �  �             =  	   �  �  =  	   �  �  A     �  Z  4   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �             =  	   �  �  =  	   �  �  A     �  Z  4   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �             =  	   �  �  =  	   �  �  A     �  Z  +   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �    	   �     .   �  �  �  >  �  �       	      =  	   �  �  A     �  l  4   >  �  �             A  �   �  �   /   =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =        Z  A  	    �   �  =  	       �       �    �            X  
     �       �   O                     A       �  �  >                 A  �     �   /   =     	    A  �  
  �  	  =  �    
  =  �    �  V  �        =       Z  A  	    �   �  =  	       �       �    �           X  
            �   O                     A       �  m  >                 A  �     �   /   =         A  �    �    =  �      =  �    �  V  �        =       Z  A  	    �   �  =  	       �       �    �            X  
   !          �   O     "  !  !            A     #  �  [  >  #  "             A  �   $  �   /   =     %  $  A  �  &  �  %  =  �  '  &  =  �  (  �  V  �  )  '  (  =     *  Z  A  	  +  �   �  =  	   ,  +  �     -  �  ,  �     .  *  -  X  
   /  )  .     �   O     0  /  /            A     1  �  ^  >  1  0             A     3  �  �  =     4  3  A     5  �  m  =     6  5  A     7  Z  4   =  	   8  7  A  	  9  �   �  =  	   :  9  �  	   ;  8  :    	   <     
   ;  P     =  <  <  <       >     .   4  6  =  >  2  >             A     @  �  [  =     A  @  A     B  �  ^  =     C  B  A     D  Z  4   =  	   E  D  A  	  F  �   �  =  	   G  F  �  	   H  E  G    	   I     
   H  P     J  I  I  I       K     .   A  C  J  >  ?  K             =     M  2  =     N  ?  A     O  Z  +   =  	   P  O  A  	  Q  �   �  =  	   R  Q  �  	   S  P  R    	   T     
   S  P     U  T  T  T       V     .   M  N  U  >  L  V             =     W  L  �     X  W  I  P     Y  �   �   �   �     Z  X  Y  >  L  Z             =     [  L       \     E   [  >  L  \             =     ^  l  A  c  d  b    =     e  d  �     f  ^  e    	   g     B   f  �  	   i  g  h    	   j     %   i  �   �  	   k  �   j  >  ]  k             =  	   l  ]  �  w   m  l  �   �  o      �  m  n  o  �  n             A     q  Z  +   =  	   r  q  A  	  s  �   �  =  	   t  s  �  	   u  r  t  m     v  u  p  	   w  v  �  	   x  w  �              A     y  Z  4   =  	   z  y  A  	  {  �   �  =  	   |  {  �  	   }  z  |  m     ~  }  p  	     ~  �  	   �    �   P     �  x  �             A  	  �  �   �  =  	   �  �  �     �  �  �             >  p  �       !      =     �  p  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  �  >  �  �       "      =     �  p  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  m  >  �  �       #      =     �  p  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  [  >  �  �       $      =     �  p  A  	  �  �   �  =  	   �  �  �     �  �  �  �     �  �  �  A     �  �  ^  >  �  �       &      >  �  +   �  �  �  �       &      �  �  �      �  �  �  �       &      =     �  �  �  w   �  �  �   �  �  �  �  �  �       (      A  �   �  �   /   =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  A     �  �  �  =     �  �  X  
   �  �  �     �   O     �  �  �            �     �  �  I  P     �  �   �   �   �     �  �  �  >  �  �       )      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  A     �  �  �  =     �  �  X  
   �  �  �     �   >  �  �       *      A  �   �  �   �  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  �  V  �  �  �  �  =     �  �  A     �  �  �  =     �  �  X  
   �  �  �     �   >  �  �       ,      A     �  �  +   =  	   �  �  �  	   �  �  )   m     �  �  A  �  �  �  �  �  =  �  �  �  A  �  �  �  �  Q     �  �      A     �  �  �  >  �  �  Q     �  �     A     �  �  m  >  �  �  Q     �  �     A     �  �  [  >  �  �  Q     �  �     A     �  �  ^  >  �  �  Q     �  �     A     �  �  �   >  �  �  Q  	   �  �     A     �  �    >  �  �  Q     �  �     A     �  �  p  >  �  �  Q     �  �     A     �  �  �  >  �  �       -      A     �  �  4   =  	   �  �  �  	   �  �  )   m     �  �  A  �  �  �  �  �  =  �  �  �  A  �  �  �  m  Q     �  �      A     �  �  �  >  �  �  Q     �  �     A     �  �  m  >  �  �  Q     �  �     A     �  �  [  >  �  �  Q        �     A       �  ^  >       Q       �     A       �  �   >      Q  	     �     A       �    >      Q       �     A       �  p  >      Q       �     A     	  �  �  >  	         .      A     
  �  <   =  	     
  �  	       )   m         A  �    �  �    =  �      A  �    �  [  Q             A         �  >      Q            A         m  >      Q            A         [  >      Q            A         ^  >      Q            A         �   >      Q  	          A           >      Q            A         p  >      Q            A          �  >            /      A     !  �  D   =  	   "  !  �  	   #  "  )   m     $  #  A  �  %  �  �  $  =  �  &  %  A  �  '  �  ^  Q     (  &      A     )  '  �  >  )  (  Q     *  &     A     +  '  m  >  +  *  Q     ,  &     A     -  '  [  >  -  ,  Q     .  &     A     /  '  ^  >  /  .  Q     0  &     A     1  '  �   >  1  0  Q  	   2  &     A     3  '    >  3  2  Q     4  &     A     5  '  p  >  5  4  Q     6  &     A     7  '  �  >  7  6       0      =     9  �  A     :  �  9  =     ;  :  A  	  <  �   �  =  	   =  <  �     >  ;  =  �     ?  >  �   =     A  l  >  @  A  =     C  �  >  B  C  >  D  ?  9     E  !   @  B  D  >  8  E       2      A     F  �  �  �  =     G  F  �     H  G  �   �  w   I  H  �   �  K      �  I  J  Y  �  J       2      A     O  �  �  [  =     P  O  A  �  Q  �  P  =  �  R  Q  =  �  T  S  V  �  U  R  T  =     V  8  X  
   W  U  V     �   A  #   X  N  �  >  X  W  �  K  �  Y       2      A     [  �  �  [  =     \  [  >  Z  \  9  
   ]     Z  A  #   ^  N  �  >  ^  ]  �  K  �  K       3      A     _  �  m  �  =     `  _  �     a  `  �   �  w   b  a  �   �  d      �  b  c  n  �  c       3      A     e  �  m  [  =     f  e  A  �  g  �  f  =  �  h  g  =  �  i  S  V  �  j  h  i  =     k  8  X  
   l  j  k     �   A  #   m  N  m  >  m  l  �  d  �  n       3      A     p  �  m  [  =     q  p  >  o  q  9  
   r     o  A  #   s  N  m  >  s  r  �  d  �  d       4      A     t  �  [  �  =     u  t  �     v  u  �   �  w   w  v  �   �  y      �  w  x  �  �  x       4      A     z  �  [  [  =     {  z  A  �  |  �  {  =  �  }  |  =  �  ~  S  V  �    }  ~  =     �  8  X  
   �    �     �   A  #   �  N  [  >  �  �  �  y  �  �       4      A     �  �  [  [  =     �  �  >  �  �  9  
   �     �  A  #   �  N  [  >  �  �  �  y  �  y       5      A     �  �  ^  �  =     �  �  �     �  �  �   �  w   �  �  �   �  �      �  �  �  �  �  �       5      A     �  �  ^  [  =     �  �  A  �  �  �  �  =  �  �  �  =  �  �  S  V  �  �  �  �  =     �  8  X  
   �  �  �     �   A  #   �  N  ^  >  �  �  �  �  �  �       5      A     �  �  ^  [  =     �  �  >  �  �  9  
   �     �  A  #   �  N  ^  >  �  �  �  �  �  �       7      >  �  �        8      >  �  +   �  �  �  �       8      �  �  �      �  �  �  �       8      =     �  �  �  w   �  �  �   �  �  �  �  �  �       :      =     �  �  =     �  �  A     �  �  �  =  	   �  �  =     �  �  A     �  N  �  D   =  	   �  �  �  	   �  �  �    	   �        �  �    	   �     (   �  �  A     �  �  �  >  �  �       ;      =     �  �  A     �  �  �  =  	   �  �  =  	   �  �  �  	   �  �  �  >  �  �  �  �  �  �       8      =     �  �  �     �  �  m  >  �  �  �  �  �  �       =      =  	   �  �  �  	   �  �   �  >  �  �       >      >  �  +   �  �  �  �       >      �  �  �      �  �  �  �       >      =     �  �  �  w   �  �  �   �  �  �  �  �  �       @      =     �  �  =  	   �  �  A     �  �  �  =  	   �  �  �  	   �  �  �  A     �  �  �  >  �  �  �  �  �  �       >      =     �  �  �     �  �  m  >  �  �  �  �  �  �       B      =     �  �  A     �  N  �  D   =  	   �  �  A     �  �  �  =  	   �  �  �  	   �  �  �       C      A     �  N  m  D   =  	   �  �  A     �  �  m  =  	   �  �  �  	   �  �  �  �  	   �  �  �       D      A     �  N  [  D   =  	   �  �  A     �  �  [  =  	   �  �  �  	   �  �  �  �  	   �  �  �       E      A     �  N  ^  D   =  	   �  �  A     �  �  ^  =  	   �  �  �  	   �  �  �  �  	   �  �  �       B      A     �  �  �  >  �  �  �  �  �  �       &      =     �  �  �     �  �  m  >  �  �  �  �  �  �       G      A     �  Z  +   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  >  �  �       H      A     �  Z  4   =  	   �  �  A  	  �  �   �  =  	   �  �  �  	   �  �  �    	   �     
   �  >  �  �       K      =  	   �  �  �  	   �  �   �  =  	   �  �  �  	   �  �   �  �  	      �  �  A       �  �  >            L      =  	     �  �  	     �     =  	     �  �  	         A       �  m  >           M      =  	     �  =  	     �  �  	   	  �     �  	   
    	  A       �  [  >    
       N      =  	     �  =  	     �  �  	         A       �  ^  >           O      >    �        P      >    +   �    �         P      �          �    �         P      =         �  w       �   �        �         R      =         =         A       �    =  	       =         A       �    =  	       �  	            	   !           �    	   "     (   !  �  A     #  �    >  #  "       S      =     $    A     %  �  $  =  	   &  %  =  	   '    �  	   (  '  &  >    (  �    �         P      =     )    �     *  )  m  >    *  �    �         U      =  	   ,    �  	   -  �   ,  >  +  -       V      >  .  +   �  /  �  /       V      �  1  2      �  3  �  3       V      =     4  .  �  w   5  4  �   �  5  0  1  �  0       X      =     6  .  =  	   7  +  A     8  �  6  =  	   9  8  �  	   :  9  7  A     ;  �  6  >  ;  :  �  2  �  2       V      =     <  .  �     =  <  m  >  .  =  �  /  �  1       [      A     ?  �  �  =  	   @  ?  A     A  �  �  =  	   B  A  �  	   C  @  B       \      A     D  �  m  =  	   E  D  A     F  �  m  =  	   G  F  �  	   H  E  G  �  	   I  C  H       ]      A     J  �  [  =  	   K  J  A     L  �  [  =  	   M  L  �  	   N  K  M  �  	   O  I  N       ^      A     P  �  ^  =  	   Q  P  A     R  �  ^  =  	   S  R  �  	   T  Q  S  �  	   U  O  T       [      >  >  U       _      =     V  L  =  	   Y  >    	   Z     .   W  X  Y  �     [  V  Z  =  	   \  ]  �     ]  [  \  =     ^  l  �     _  ^  ]  >  l  _  �  o  �  o       a      A  d  e  b  �  =  _  f  e  =     g  l  Q  	   h  g      Q  	   i  g     Q  	   j  g     P  
   k  h  i  j  �   �  
   l  f  k  A  m  n  c  �  >  n  l  �  8       �      6  
             7        �     ;  #   $                 =     %      �     '   %   &   p  	   (   '   �  	   *   (   )   A     ,   $   +   >  ,   *              =     -      �     0   -   /   �     1   0   &   p  	   2   1   �  	   3   2   )   A     5   $   4   >  5   3              =     6      �     8   6   7   �     9   8   &   p  	   :   9   �  	   ;   :   )   A     =   $   <   >  =   ;              =     >      �     @   >   ?   �     A   @   &   p  	   B   A   �  	   C   B   )   A     E   $   D   >  E   C              =  
   F   $   �  F   8       +  #   6               7        7        �     ;     I      ;     L      ;     O           -      =  	   J        	   K         J   >  I   K        .      =  	   M        	   N         M   >  L   N        0      A     P      +   =  	   Q   P   =  	   R   L   �  	   S   Q   R   A     T      <   =  	   U   T   =  	   V   I   �  	   W   U   V   �  	   X   S   W   >  O   X        1      A     Y      +   =  	   Z   Y     	   [   Z   =  	   \   I   �  	   ]   [   \   A     ^      <   =  	   _   ^   =  	   `   L   �  	   a   _   `   �  	   b   ]   a   A     c      <   >  c   b        2      =  	   d   O   A     e      +   >  e   d        4      =     f      �  f   8       ~  "   6  	             7        �          �      =     i      �  	   m   i   l     	   n         m   �  	   p   n   o     	   q      
   p   �  q   8       �  Z   6     !          7        7        7         �  "   ;     t      ;  x   y      ;  x   �      ;     �      ;     �      ;     �      ;     �      ;     �           �      =     u           v         u   >  t   v        �      A     z   t   +   =  	   {   z   A     |   t   4   =  	   }   |   �  w   ~   {   }   �  �       �  ~      �   �          �      A     �   t   +   =  	   �   �   A     �   t   <   =  	   �   �   �  w   �   �   �   �  �   �  �   �  w   �   ~   "   �           �      >  y   �        �      A     �   t   <   =  	   �   �   A     �   t   +   =  	   �   �   �  w   �   �   �   �  �       �  �   �   �   �  �        �      A     �   t   <   =  	   �   �   A     �   t   4   =  	   �   �   �  w   �   �   �   �  �   �  �   �  w   �   �   �   �   �        �      >  �   �        �      =     �      O     �   �   �         =  w   �   y   �  	   �   �   �   �   �     �   �   �   =     �      O     �   �   �          =  w   �   �   �  	   �   �   �   �   �     �   �   �   �     �   �   �   =     �      O     �   �   �          =  w   �   y   �  w   �   �   =  w   �   �   �  w   �   �   �  w   �   �   �   �  	   �   �   �   �   �     �   �   �   �     �   �   �   >  �   �        �      =     �   �   �     �   �   �   >  �   �        �      =     �       >  �   �   9  	   �      �     	   �      .   �   �   �   >  �   �        �      =     �       =     �   �   �     �   �   �   >  �   �        �      =     �   �   P     �   �   �   �     �   �   �   >  �   �        �      A     �   �   +   =  	   �   �   A     �   �   4   =  	   �   �   P     �   �   �   �   >  �   �   =  	   �   �   >  �   �   9     �      �   �   O     �   �   �          >  �   �        �      =     �   �   P     �   �   �   �     �   �   �   >  �   �        �      =     �       =     �   �   �     �   �   �   >  �   �        �      =     �   �   �  �   8  �K      #     =                 GLSL.std.450                     main    
                	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\TerrainWireframe_Fragment.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (location = 0) out vec4 SceneLowDynamicRange1;

void main()
{
	SceneLowDynamicRange1 = vec4(COLOR,0.5f);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   SceneLowDynamicRange1        PushConstantData             COLOR           WORLD_POSITION          MINIMUM_HEIGHT_MAP_COORDINATE           MAXIMUM_HEIGHT_MAP_COORDINATE           BORDERS  
       PATCH_RESOLUTION_RECIPROCAL         PATCH_SIZE   
       HEIGHT_MAP_TEXTURE_INDEX     
       NORMAL_MAP_TEXTURE_INDEX     	    	   INDEX_MAP_TEXTURE_INDEX  	    
   BLEND_MAP_TEXTURE_INDEX         MAP_RESOLUTION   
       MAP_RESOLUTION_RECIPROCAL                 TEXTURES     	 !   GlobalTextureAverageValues   	 !       TEXTURE_AVERAGE_VALUES    #         $   Material      $       _Properties   $      _AlbedoThickness     	 $      _NormalMapDisplacement    $      _MaterialProperties   $      _Opacity      $      _EmissiveMultiplier   $      _Padding1     $      _Padding2     '   GlobalMaterials   '       MATERIALS     )         .   BLUE_NOISE_TEXTURES   2   SKY_TEXTURE   4   Camera   	 4       WORLD_TO_CLIP_MATRIX     	 4      WORLD_TO_CAMERA_MATRIX    4      PREVIOUS_WORLD_TO_CLIP_MATRIX     4      INVERSE_WORLD_TO_CAMERA_MATRIX    4      INVERSE_CAMERA_TO_CLIP_MATRIX    	 4      CAMERA_WORLD_POSITION    	 4      CAMERA_FORWARD_VECTOR    	 4      CURRENT_FRAME_JITTER      4      NEAR_PLANE    4   	   FAR_PLANE     6         9   HEIGHT_MAP_SAMPLER    :   NORMAL_MAP_SAMPLER    ;   INDEX_BLEND_MAP_SAMPLER   <   MATERIAL_SAMPLER    G  
          H         #       H        #      H        #      H        #       H        #   (   H        #   ,   H        #   0   H        #   4   H        #   8   H     	   #   <   H     
   #   @   H        #   D   H        #   H   G        G     "       G     !       G            H  !       #       G  !      G  #   "       G  #   !      H  $       #       H  $      #      H  $      #      H  $      #      H  $      #      H  $      #      H  $      #      H  $      #      G  &          H  '       #       G  '      G  )   "       G  )   !      G  .   "       G  .   !      G  2   "       G  2   !      H  4          H  4       #       H  4             H  4         H  4      #   @   H  4            H  4         H  4      #   �   H  4            H  4         H  4      #   �   H  4            H  4         H  4      #      H  4            H  4      #   @  H  4      #   P  H  4      #   `  H  4      #   h  H  4   	   #   l  G  4      G  6   "      G  6   !       G  9   "      G  9   !      G  :   "      G  :   !      G  ;   "      G  ;   !      G  <   "      G  <   !           !                               	         ;  	   
                                                                                           	      ;        	               +                  	      +           ? 	                             +                                   ;                          !          "      !   ;  "   #       
 $                           +     %        &   $   %     '   &      (      '   ;  (   )        *      +     +         ,   *   +      -       ,   ;  -   .        	 /                              0   /      1       0   ;  1   2         3           4   3   3   3   3   3                     5      4   ;  5   6        7      8       7   ;  8   9       ;  8   :       ;  8   ;       ;  8   <            �     6               �          �      A              =           Q               Q              Q              P                    >  
      �  8                          ��KT��D	                               <�A�cz                                     2z��-D<