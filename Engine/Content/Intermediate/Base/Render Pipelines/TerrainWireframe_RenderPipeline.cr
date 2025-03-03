�nX/J1������TerrainWireframe_RenderPipeline                                  ȫ      #     e                GLSL.std.450                      main    :   >   N       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\TerrainWireframe_Vertex.glsl     }   �     // OpModuleProcessed entry-point main
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
    vec3 normal;
    {
        vec3 normal_1 = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        vec3 normal_2 = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        vec3 normal_3 = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        vec3 normal_4 = texture(sampler2D(TEXTURES[NORMAL_MAP_TEXTURE_INDEX], NORMAL_MAP_SAMPLER), height_map_coordinate + vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL).xyz;
        vec3 blend_1 = mix(normal_1, normal_2, fract(height_map_coordinate.y * MAP_RESOLUTION));
	    vec3 blend_2 = mix(normal_3, normal_4, fract(height_map_coordinate.y * MAP_RESOLUTION));
	    normal = mix(blend_1, blend_2, fract(height_map_coordinate.x * MAP_RESOLUTION));
        normal = normal * 2.0f - 1.0f;
        normal = normalize(normal);
    }
    vec2 material_texture_coordinate = world_position.xz * 0.5f;
    {
        float displacements[4];
        vec2 sample_offsets[4];
        sample_offsets[0] = vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[1] = vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[2] = vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[3] = vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        for (uint i = 0; i < 4; ++i)
        {
            vec2 height_map_texture_coordinate = height_map_coordinate + sample_offsets[i];
            vec4 index_map = texture(sampler2D(TEXTURES[INDEX_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), height_map_texture_coordinate);
            Material material_1 = MATERIALS[uint(index_map[0] * float(UINT8_MAXIMUM))];
            Material material_2 = MATERIALS[uint(index_map[1] * float(UINT8_MAXIMUM))];
            Material material_3 = MATERIALS[uint(index_map[2] * float(UINT8_MAXIMUM))];
            Material material_4 = MATERIALS[uint(index_map[3] * float(UINT8_MAXIMUM))];
            vec4 normal_map_displacement_1;
            EVALUATE_NORMAL_MAP_DISPLACEMENT(material_1, material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacement_1);
            vec4 normal_map_displacement_2;
            EVALUATE_NORMAL_MAP_DISPLACEMENT(material_2, material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacement_2);
            vec4 normal_map_displacement_3;
            EVALUATE_NORMAL_MAP_DISPLACEMENT(material_3, material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacement_3);
            vec4 normal_map_displacement_4;
            EVALUATE_NORMAL_MAP_DISPLACEMENT(material_4, material_texture_coordinate, MATERIAL_SAMPLER, normal_map_displacement_4);
            normal_map_displacement_1.w = max(normal_map_displacement_1.w, TERRAIN_MINIMUM_DISPLACEMENT);
            normal_map_displacement_2.w = max(normal_map_displacement_2.w, TERRAIN_MINIMUM_DISPLACEMENT);
            normal_map_displacement_3.w = max(normal_map_displacement_3.w, TERRAIN_MINIMUM_DISPLACEMENT);
            normal_map_displacement_4.w = max(normal_map_displacement_4.w, TERRAIN_MINIMUM_DISPLACEMENT);
            vec4 blend_map = texture(sampler2D(TEXTURES[BLEND_MAP_TEXTURE_INDEX], INDEX_BLEND_MAP_SAMPLER), height_map_texture_coordinate);
            blend_map[0] *= BIAS_DISPLACEMENT(normal_map_displacement_1.w);
            blend_map[1] *= BIAS_DISPLACEMENT(normal_map_displacement_2.w);
            blend_map[2] *= BIAS_DISPLACEMENT(normal_map_displacement_3.w);
            blend_map[3] *= BIAS_DISPLACEMENT(normal_map_displacement_4.w);
            float inverse_sum = 1.0f / (blend_map[0] + blend_map[1] + blend_map[2] + blend_map[3]);
            blend_map[0] *= inverse_sum;
            blend_map[1] *= inverse_sum;
            blend_map[2] *= inverse_sum;
            blend_map[3] *= inverse_sum;
            displacements[i] =  normal_map_displacement_1.w * blend_map[0]
                                + normal_map_displacement_2.w * blend_map[1]
                                + normal_map_displacement_3.w * blend_map[2]
                                + normal_map_displacement_4.w * blend_map[3];
        }
        float blend_1 = mix(displacements[0], displacements[1], fract(height_map_coordinate.y * MAP_RESOLUTION));
	    float blend_2 = mix(displacements[2], displacements[3], fract(height_map_coordinate.y * MAP_RESOLUTION));
	    float final_displacement = mix(blend_1, blend_2, fract(height_map_coordinate.x * MAP_RESOLUTION));
        world_position += normal * mix(-0.125f, 0.25f, final_displacement); //Slight bias for upward displacement.
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(world_position,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        unpacked      8   stitched_position     :   InPosition    <   is_left_multiplier    >   InBorders     B   PushConstantData      B       COLOR     B      WORLD_POSITION    B      MINIMUM_HEIGHT_MAP_COORDINATE     B      MAXIMUM_HEIGHT_MAP_COORDINATE     B      BORDERS  
 B      PATCH_RESOLUTION_RECIPROCAL   B      PATCH_SIZE   
 B      HEIGHT_MAP_TEXTURE_INDEX     
 B      NORMAL_MAP_TEXTURE_INDEX     	 B   	   INDEX_MAP_TEXTURE_INDEX  	 B   
   BLEND_MAP_TEXTURE_INDEX   B      MAP_RESOLUTION   
 B      MAP_RESOLUTION_RECIPROCAL     D         L   is_right_multiplier   U   vertical_offset   [   is_down_multiplier    d   is_up_multiplier      m   horizontal_offset     �   is_left_multiplier    �   is_right_multiplier   �   vertical_offset   �   is_down_multiplier    �   is_up_multiplier      �   horizontal_offset     �   height_map_coordinate     �   world_position    �   height_1      �   TEXTURES        HEIGHT_MAP_SAMPLER      height_2      #  height_3      2  height_4      A  blend_1   L  blend_2   V  final_height      b  normal_1      g  NORMAL_MAP_SAMPLER    q  normal_2        normal_3      �  normal_4      �  blend_1   �  blend_2   �  normal   	 �  material_texture_coordinate   �  sample_offsets    �  i    
 �  height_map_texture_coordinate     �  index_map     �  INDEX_BLEND_MAP_SAMPLER   �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  material_1    �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �          material_2    +  material_3    B  material_4   	 _  normal_map_displacement_1     d  MATERIAL_SAMPLER      j  param    	 t  normal_map_displacement_2     ~  param    	 �  normal_map_displacement_3     �  param    	 �  normal_map_displacement_4     �  param     �  blend_map     �  inverse_sum     displacements     !  blend_1   -  blend_2   9  final_displacement    L  gl_PerVertex      L      gl_Position   L     gl_PointSize      L     gl_ClipDistance   L     gl_CullDistance   N        P  Camera   	 P      WORLD_TO_CLIP_MATRIX     	 P     WORLD_TO_CAMERA_MATRIX    P     PREVIOUS_WORLD_TO_CLIP_MATRIX     P     INVERSE_WORLD_TO_CAMERA_MATRIX    P     INVERSE_CAMERA_TO_CLIP_MATRIX    	 P     CAMERA_WORLD_POSITION    	 P     CAMERA_FORWARD_VECTOR    	 P     CURRENT_FRAME_JITTER      P     NEAR_PLANE    P  	   FAR_PLANE     R        `  BLUE_NOISE_TEXTURES   d  SKY_TEXTURE G  :          G  >         H  B       #       H  B      #      H  B      #      H  B      #       H  B      #   (   H  B      #   ,   H  B      #   0   H  B      #   4   H  B      #   8   H  B   	   #   <   H  B   
   #   @   H  B      #   D   H  B      #   H   G  B      G  �   "       G  �   !       G    "      G    !      G  g  "      G  g  !      G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  d  "      G  d  !      H  L             H  L           H  L           H  L           G  L     H  P         H  P      #       H  P            H  P        H  P     #   @   H  P           H  P        H  P     #   �   H  P           H  P        H  P     #   �   H  P           H  P        H  P     #      H  P           H  P     #   @  H  P     #   P  H  P     #   `  H  P     #   h  H  P  	   #   l  G  P     G  R  "      G  R  !       G  `  "       G  `  !      G  d  "       G  d  !           !                                   	         
   	      !     
               
   +        �   +  	        C+                     	               +           +     !      +     $      +     )      +     ,      +     1        6   	         7      6      9      6   ;  9   :         =         ;  =   >        A   	        B   A   6   6   6      	   	               	   	      C   	   B   ;  C   D   	   +     E         F   	      +     N      +  	   Y     �?+     ]      +     f   @   +     r         s   	   	   +     �      +     �       +     �   �   +  	   �      @+     �      +     �         �      A   +     �      +     �      +  	   �      ?+  	   �        	 �   	                         +     �        �   �   �      �       �   ;  �   �       +     �               �                  ;            	  �   ,  6     �   �   +          ,  6     �   Y   ,  6   +  Y   �   ,  6   :  Y   Y   +     F     ;    g        �  6   N      �     �  +     �        �  +     �  	   ;    �       
 �                 	            �     �   
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �             �  ;    d      +  	   �  ��8+     �  
   +  	   �    �A    	   N             +  	   D     �+  	   E    �>  K  	   !     L  
   	   K  K     M     L  ;  M  N       O  
        P  O  O  O  O  O  A   A   6   	   	      Q     P  ;  Q  R        S     O     \     
     ^  	  �      _      ^  ;  _  `       	 a  	                           b  a     c      b  ;  c  d           �     6               �     ;  7   8      ;     <      ;     L      ;     U      ;     [      ;     d      ;     m      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;  7   �      ;  �   �      ;     �      ;          ;     #     ;     2     ;     A     ;     L     ;     V     ;  �   b     ;  �   q     ;  �        ;  �   �     ;  �   �     ;  �   �     ;  �   �     ;  7   �     ;  �  �     ;     �     ;  7   �     ;     �     ;  �  �     ;  �       ;  �  +     ;  �  B     ;     _     ;     j     ;     t     ;     ~     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;         ;     !     ;     -     ;     9          �      =  6   ;   :   >  8   ;        �      =     ?   >   �     @   ?   !   A  F   G   D   E   =     H   G   �     I   H   !   �     J   @   I   p  	   K   J   >  <   K        �      =     M   >   �     O   M   N   A  F   P   D   E   =     Q   P   �     R   Q   N   �     S   O   R   p  	   T   S   >  L   T        �      =  	   V   <   =  	   W   L   �  	   X   V   W     	   Z      %   X   Y   >  U   Z        �      =     \   >   �     ^   \   ]   A  F   _   D   E   =     `   _   �     a   `   ]   �     b   ^   a   p  	   c   b   >  [   c        �      =     e   >   �     g   e   f   A  F   h   D   E   =     i   h   �     j   i   f   �     k   g   j   p  	   l   k   >  d   l        �      =  	   n   [   =  	   o   d   �  	   p   n   o     	   q      %   p   Y   >  m   q        �      A  s   t   D   r   =  	   u   t   =  	   v   U   �  	   w   u   v   A     x   8   !   =  	   y   x   �  	   z   y   w   A     {   8   !   >  {   z        �      A  s   |   D   r   =  	   }   |   =  	   ~   m   �  	      }   ~   A     �   8      =  	   �   �   �  	   �   �      A     �   8      >  �   �        �      =     �   >   �     �   �   )   A  F   �   D   E   =     �   �   �     �   �   )   �     �   �   �   p  	   �   �   >  �   �        �      =     �   >   �     �   �   �   A  F   �   D   E   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        �      =  	   �   �   =  	   �   �   �  	   �   �   �     	   �      %   �   Y   >  �   �        �      =     �   >   �     �   �   �   A  F   �   D   E   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        �      =     �   >   �     �   �   �   A  F   �   D   E   =     �   �   �     �   �   �   �     �   �   �   p  	   �   �   >  �   �        �      =  	   �   �   =  	   �   �   �  	   �   �   �     	   �      %   �   Y   >  �   �        �      A  s   �   D   r   =  	   �   �   �  	   �   �   �   =  	   �   �   �  	   �   �   �   A     �   8   !   =  	   �   �   �  	   �   �   �   A     �   8   !   >  �   �        �      A  s   �   D   r   =  	   �   �   �  	   �   �   �   =  	   �   �   �  	   �   �   �   A     �   8      =  	   �   �   �  	   �   �   �   A     �   8      >  �   �        �      A  s   �   D   �      =  	   �   �   A  s   �   D   �      =  	   �   �   A     �   8      =  	   �   �     	   �      .   �   �   �   A  s   �   D   �   !   =  	   �   �   A  s   �   D   �   !   =  	   �   �   A     �   8   !   =  	   �   �     	   �      .   �   �   �   P  6   �   �   �   >  �   �        �      A  s   �   D   �      =  	   �   �   A  s   �   D   �   =  	   �   �   �  	   �   �   �     	   �   �   A  s   �   D   �   =  	   �   �   �  	   �   �   �   A     �   8      =  	   �   �     	   �      .   �   �   �   �  	   �   �   �   A     �   �      >  �   �        �      A     �   �   !   >  �   �        �      A  s   �   D   �   !   =  	   �   �   A  s   �   D   �   =  	   �   �   �  	   �   �   �     	   �   �   A  s   �   D   �   =  	   �   �   �  	   �   �   �   A     �   8   !   =  	   �   �     	   �      .   �   �   �   �  	   �   �   �   A     �   �   )   >  �   �        �      A  F      D   �   =          A      �     =  �       =        V  	  
      =  6     �   A  s     D     =  	       �  6         �  6         X  
     
       �   Q  	           >  �          �      A  F     D   �   =         A      �     =  �       =        V  	        =  6     �   A  s     D     =  	       �  6         �  6          X  
   !          �   Q  	   "  !      >    "       �      A  F   $  D   �   =     %  $  A    &  �   %  =  �   '  &  =    (    V  	  )  '  (  =  6   *  �   A  s   ,  D     =  	   -  ,  �  6   .  +  -  �  6   /  *  .  X  
   0  )  /     �   Q  	   1  0      >  #  1       �      A  F   3  D   �   =     4  3  A    5  �   4  =  �   6  5  =    7    V  	  8  6  7  =  6   9  �   A  s   ;  D     =  	   <  ;  �  6   =  :  <  �  6   >  9  =  X  
   ?  8  >     �   Q  	   @  ?      >  2  @       �      =  	   B  �   =  	   C    A     D  �   !   =  	   E  D  A  s   G  D   F  =  	   H  G  �  	   I  E  H    	   J     
   I    	   K     .   B  C  J  >  A  K       �      =  	   M  #  =  	   N  2  A     O  �   !   =  	   P  O  A  s   Q  D   F  =  	   R  Q  �  	   S  P  R    	   T     
   S    	   U     .   M  N  T  >  L  U       �      =  	   W  A  =  	   X  L  A     Y  �      =  	   Z  Y  A  s   [  D   F  =  	   \  [  �  	   ]  Z  \    	   ^     
   ]    	   _     .   W  X  ^  >  V  _       �      =  	   `  V  A     a  �   !   >  a  `       �      A  F   c  D      =     d  c  A    e  �   d  =  �   f  e  =    h  g  V  	  i  f  h  =  6   j  �   A  s   k  D     =  	   l  k  �  6   m    l  �  6   n  j  m  X  
   o  i  n     �   O  A   p  o  o            >  b  p       �      A  F   r  D      =     s  r  A    t  �   s  =  �   u  t  =    v  g  V  	  w  u  v  =  6   x  �   A  s   y  D     =  	   z  y  �  6   {    z  �  6   |  x  {  X  
   }  w  |     �   O  A   ~  }  }            >  q  ~       �      A  F   �  D      =     �  �  A    �  �   �  =  �   �  �  =    �  g  V  	  �  �  �  =  6   �  �   A  s   �  D     =  	   �  �  �  6   �  +  �  �  6   �  �  �  X  
   �  �  �     �   O  A   �  �  �            >    �       �      A  F   �  D      =     �  �  A    �  �   �  =  �   �  �  =    �  g  V  	  �  �  �  =  6   �  �   A  s   �  D     =  	   �  �  �  6   �  :  �  �  6   �  �  �  X  
   �  �  �     �   O  A   �  �  �            >  �  �       �      =  A   �  b  =  A   �  q  A     �  �   !   =  	   �  �  A  s   �  D   F  =  	   �  �  �  	   �  �  �    	   �     
   �  P  A   �  �  �  �    A   �     .   �  �  �  >  �  �       �      =  A   �    =  A   �  �  A     �  �   !   =  	   �  �  A  s   �  D   F  =  	   �  �  �  	   �  �  �    	   �     
   �  P  A   �  �  �  �    A   �     .   �  �  �  >  �  �       �      =  A   �  �  =  A   �  �  A     �  �      =  	   �  �  A  s   �  D   F  =  	   �  �  �  	   �  �  �    	   �     
   �  P  A   �  �  �  �    A   �     .   �  �  �  >  �  �       �      =  A   �  �  �  A   �  �  �   P  A   �  Y   Y   Y   �  A   �  �  �  >  �  �       �      =  A   �  �    A   �     E   �  >  �  �       �      =  A   �  �   O  6   �  �  �         �  6   �  �  �   >  �  �       �      A  s   �  D     =  	   �  �  �  6   �    �  A  7   �  �  �  >  �  �       �      A  s   �  D     =  	   �  �  �  6   �    �  A  7   �  �  �   >  �  �       �      A  s   �  D     =  	   �  �  �  6   �  +  �  A  7   �  �  �   >  �  �       �      A  s   �  D     =  	   �  �  �  6   �  :  �  A  7   �  �  �   >  �  �       �      >  �     �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �  �  �  �  N   �  �  �  �  �  �       �      =  6   �  �   =     �  �  A  7   �  �  �  =  6   �  �  �  6   �  �  �  >  �  �       �      A  F   �  D   �  =     �  �  A    �  �   �  =  �   �  �  =    �  �  V  	  �  �  �  =  6   �  �  X  
   �  �  �     �   >  �  �       �      A     �  �     =  	   �  �  �  	   �  �     m        �  A      �  �     =  �      Q             A       �  �  >      Q            A       �  �   >      Q            A     	  �  �   >  	    Q     
       A       �  �   >    
  Q            A       �  E   >      Q  	          A       �  r   >      Q            A       �  �   >      Q            A       �  �   >           �      A       �  !   =  	       �  	          m         A      �  �    =  �      Q             A         �  >      Q            A         �   >      Q            A          �   >       Q     !       A     "    �   >  "  !  Q     #       A     $    E   >  $  #  Q  	   %       A     &    r   >  &  %  Q     '       A     (    �   >  (  '  Q     )       A     *    �   >  *  )       �      A     ,  �  )   =  	   -  ,  �  	   .  -     m     /  .  A    0  �  �  /  =  �  1  0  Q     2  1      A     3  +  �  >  3  2  Q     4  1     A     5  +  �   >  5  4  Q     6  1     A     7  +  �   >  7  6  Q     8  1     A     9  +  �   >  9  8  Q     :  1     A     ;  +  E   >  ;  :  Q  	   <  1     A     =  +  r   >  =  <  Q     >  1     A     ?  +  �   >  ?  >  Q     @  1     A     A  +  �   >  A  @       �      A     C  �  1   =  	   D  C  �  	   E  D     m     F  E  A    G  �  �  F  =  �  H  G  Q     I  H      A     J  B  �  >  J  I  Q     K  H     A     L  B  �   >  L  K  Q     M  H     A     N  B  �   >  N  M  Q     O  H     A     P  B  �   >  P  O  Q     Q  H     A     R  B  E   >  R  Q  Q  	   S  H     A     T  B  r   >  T  S  Q     U  H     A     V  B  �   >  V  U  Q     W  H     A     X  B  �   >  X  W       �      A     Y  �  �  =     Z  Y  �     [  Z  ]   �  �  \  [  ]   �  ^      �  \  ]  i  �  ]       �      A     `  �  �   =     a  `  A    b  �   a  =  �   c  b  =    e  d  V  	  f  c  e  =  6   g  �  X  
   h  f  g     �   >  _  h  �  ^  �  i       �      A     k  �  �   =     l  k  >  j  l  9  
   m     j  >  _  m  �  ^  �  ^       �      A     n    �  =     o  n  �     p  o  ]   �  �  q  p  ]   �  s      �  q  r  }  �  r       �      A     u    �   =     v  u  A    w  �   v  =  �   x  w  =    y  d  V  	  z  x  y  =  6   {  �  X  
   |  z  {     �   >  t  |  �  s  �  }       �      A         �   =     �    >  ~  �  9  
   �     ~  >  t  �  �  s  �  s       �      A     �  +  �  =     �  �  �     �  �  ]   �  �  �  �  ]   �  �      �  �  �  �  �  �       �      A     �  +  �   =     �  �  A    �  �   �  =  �   �  �  =    �  d  V  	  �  �  �  =  6   �  �  X  
   �  �  �     �   >  �  �  �  �  �  �       �      A     �  +  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       �      A     �  B  �  =     �  �  �     �  �  ]   �  �  �  �  ]   �  �      �  �  �  �  �  �       �      A     �  B  �   =     �  �  A    �  �   �  =  �   �  �  =    �  d  V  	  �  �  �  =  6   �  �  X  
   �  �  �     �   >  �  �  �  �  �  �       �      A     �  B  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       �      A     �  _  1   =  	   �  �    	   �     (   �  �  A     �  _  1   >  �  �       �      A     �  t  1   =  	   �  �    	   �     (   �  �  A     �  t  1   >  �  �       �      A     �  �  1   =  	   �  �    	   �     (   �  �  A     �  �  1   >  �  �       �      A     �  �  1   =  	   �  �    	   �     (   �  �  A     �  �  1   >  �  �       �      A  F   �  D   �  =     �  �  A    �  �   �  =  �   �  �  =    �  �  V  	  �  �  �  =  6   �  �  X  
   �  �  �     �   >  �  �       �      A     �  _  1   =  	   �  �    	   �        �  �  A     �  �     =  	   �  �  �  	   �  �  �  A     �  �     >  �  �       �      A     �  t  1   =  	   �  �    	   �        �  �  A     �  �  !   =  	   �  �  �  	   �  �  �  A     �  �  !   >  �  �       �      A     �  �  1   =  	   �  �    	   �        �  �  A     �  �  )   =  	   �  �  �  	   �  �  �  A     �  �  )   >  �  �       �      A     �  �  1   =  	   �  �    	   �        �  �  A     �  �  1   =  	   �  �  �  	   �  �  �  A     �  �  1   >  �  �       �      A     �  �     =  	   �  �  A     �  �  !   =  	   �  �  �  	   �  �  �  A     �  �  )   =  	   �  �  �  	   �  �  �  A     �  �  1   =  	   �  �  �  	   �  �  �  �  	   �  Y   �  >  �  �       �      =  	   �  �  A     �  �     =  	   �  �  �  	   �  �  �  A     �  �     >  �  �              =  	   �  �  A     �  �  !   =  	   �  �  �  	   �  �  �  A     �  �  !   >  �  �             =  	   �  �  A     �  �  )   =  	   �  �  �  	   �  �  �  A     �  �  )   >  �  �             =  	   �  �  A     �  �  1   =  	      �  �  	        �  A       �  1   >                 =       �  A       _  1   =  	       A     	  �     =  	   
  	  �  	       
             A       t  1   =  	       A       �  !   =  	       �  	         �  	                    A       �  1   =  	       A       �  )   =  	       �  	         �  	                    A       �  1   =  	       A       �  1   =  	       �  	         �  	                    A           >      �  �  �  �       �      =       �  �          �   >  �     �  �  �  �             A     "    �  =  	   #  "  A     $    �   =  	   %  $  A     &  �   !   =  	   '  &  A  s   (  D   F  =  	   )  (  �  	   *  '  )    	   +     
   *    	   ,     .   #  %  +  >  !  ,       	      A     .    �   =  	   /  .  A     0    �   =  	   1  0  A     2  �   !   =  	   3  2  A  s   4  D   F  =  	   5  4  �  	   6  3  5    	   7     
   6    	   8     .   /  1  7  >  -  8       
      =  	   :  !  =  	   ;  -  A     <  �      =  	   =  <  A  s   >  D   F  =  	   ?  >  �  	   @  =  ?    	   A     
   @    	   B     .   :  ;  A  >  9  B             =  A   C  �  =  	   F  9    	   G     .   D  E  F  �  A   H  C  G  =  A   I  �   �  A   J  I  H  >  �   J             A  S  T  R  �  =  O  U  T  =  A   V  �   Q  	   W  V      Q  	   X  V     Q  	   Y  V     P  
   Z  W  X  Y  Y   �  
   [  U  Z  A  \  ]  N  �  >  ]  [  �  8       �      6  
             7        �     ;                �       =           �              p  	         �  	            A              >             �       =           �              �              p  	         �  	             A     "      !   >  "            �       =     #      �     %   #   $   �     &   %      p  	   '   &   �  	   (   '      A     *      )   >  *   (        �       =     +      �     -   +   ,   �     .   -      p  	   /   .   �  	   0   /      A     2      1   >  2   0        �       =  
   3      �  3   8  �?      #     9                 GLSL.std.450                     main    
                	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\TerrainWireframe_Fragment.glsl      �     // OpModuleProcessed entry-point main
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
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   SceneLowDynamicRange1        PushConstantData             COLOR           WORLD_POSITION          MINIMUM_HEIGHT_MAP_COORDINATE           MAXIMUM_HEIGHT_MAP_COORDINATE           BORDERS  
       PATCH_RESOLUTION_RECIPROCAL         PATCH_SIZE   
       HEIGHT_MAP_TEXTURE_INDEX     
       NORMAL_MAP_TEXTURE_INDEX     	    	   INDEX_MAP_TEXTURE_INDEX  	    
   BLEND_MAP_TEXTURE_INDEX         MAP_RESOLUTION   
       MAP_RESOLUTION_RECIPROCAL                 TEXTURES          Material              _Properties          _AlbedoThickness     	        _NormalMapDisplacement           _MaterialProperties          _Opacity             _EmissiveMultiplier          _Padding1            _Padding2     #   GlobalMaterials   #       MATERIALS     %         *   BLUE_NOISE_TEXTURES   .   SKY_TEXTURE   0   Camera   	 0       WORLD_TO_CLIP_MATRIX     	 0      WORLD_TO_CAMERA_MATRIX    0      PREVIOUS_WORLD_TO_CLIP_MATRIX     0      INVERSE_WORLD_TO_CAMERA_MATRIX    0      INVERSE_CAMERA_TO_CLIP_MATRIX    	 0      CAMERA_WORLD_POSITION    	 0      CAMERA_FORWARD_VECTOR    	 0      CURRENT_FRAME_JITTER      0      NEAR_PLANE    0   	   FAR_PLANE     2         5   HEIGHT_MAP_SAMPLER    6   NORMAL_MAP_SAMPLER    7   INDEX_BLEND_MAP_SAMPLER   8   MATERIAL_SAMPLER    G  
          H         #       H        #      H        #      H        #       H        #   (   H        #   ,   H        #   0   H        #   4   H        #   8   H     	   #   <   H     
   #   @   H        #   D   H        #   H   G        G     "       G     !       H          #       H         #      H         #      H         #      H         #      H         #      H         #      H         #      G  "          H  #       #       G  #      G  %   "       G  %   !      G  *   "       G  *   !      G  .   "       G  .   !      H  0          H  0       #       H  0             H  0         H  0      #   @   H  0            H  0         H  0      #   �   H  0            H  0         H  0      #   �   H  0            H  0         H  0      #      H  0            H  0      #   @  H  0      #   P  H  0      #   `  H  0      #   h  H  0   	   #   l  G  0      G  2   "      G  2   !       G  5   "      G  5   !      G  6   "      G  6   !      G  7   "      G  7   !      G  8   "      G  8   !           !                               	         ;  	   
                                                                                           	      ;        	               +                  	      +           ? 	                             +                                   ;             
                             +     !        "       !     #   "      $      #   ;  $   %        &      +     '         (   &   '      )       (   ;  )   *        	 +                              ,   +      -       ,   ;  -   .         /           0   /   /   /   /   /                     1      0   ;  1   2        3      4       3   ;  4   5       ;  4   6       ;  4   7       ;  4   8            �     6               �          �      A              =           Q               Q              Q              P                    >  
      �  8                          ��KT��D	                               <�A�cz                                     2z��-D<