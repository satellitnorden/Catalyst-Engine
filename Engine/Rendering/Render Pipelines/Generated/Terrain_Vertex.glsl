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

//Constants.
#define TERRAIN_MINIMUM_DISPLACEMENT (0.001f)
#define BIAS_DISPLACEMENT(X) (X * X * X * X)

/*
*   Terrain material struct definition.
*/
struct TerrainMaterial
{
    vec3 albedo;
    vec3 normal_map;
    vec4 material_properties;
};

/*
*   Blend two terrain materials.
*/
TerrainMaterial BlendTerrainMaterial(TerrainMaterial first, TerrainMaterial second, float alpha)
{
    TerrainMaterial output_material;

    output_material.albedo = mix(first.albedo, second.albedo, alpha);
    output_material.normal_map = mix(first.normal_map, second.normal_map, alpha);
    output_material.material_properties = mix(first.material_properties, second.material_properties, alpha);

    return output_material;
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
    OutHeightMapTextureCoordinate = height_map_coordinate;
    vec2 material_texture_coordinate = OutWorldPosition.xz * 0.5f;
    {
        float displacements[4];
        vec2 sample_offsets[4];
        sample_offsets[0] = vec2(0.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[1] = vec2(0.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[2] = vec2(1.0f, 0.0f) * MAP_RESOLUTION_RECIPROCAL;
        sample_offsets[3] = vec2(1.0f, 1.0f) * MAP_RESOLUTION_RECIPROCAL;
        for (uint i = 0; i < 4; ++i)
        {
            vec2 height_map_texture_coordinate = OutHeightMapTextureCoordinate + sample_offsets[i];
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
        float blend_1 = mix(displacements[0], displacements[1], fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION));
	    float blend_2 = mix(displacements[2], displacements[3], fract(OutHeightMapTextureCoordinate.y * MAP_RESOLUTION));
	    float final_displacement = mix(blend_1, blend_2, fract(OutHeightMapTextureCoordinate.x * MAP_RESOLUTION));
        OutWorldPosition += normal * mix(-0.125f, 0.25f, final_displacement); //Slight bias for upward displacement.
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition,1.0f);
}