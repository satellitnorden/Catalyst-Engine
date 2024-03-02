#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require
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

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
};

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 2) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

layout (set = 1, binding = 4, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 5, rgba32f) uniform image2D Shadows; 

/*
*   Samples the current blue noise texture at the given coordinate and index.
*/
vec4 SampleBlueNoiseTexture(uvec2 coordinate, uint index)
{
    uint offset_index = (FRAME + index) & (NUMBER_OF_BLUE_NOISE_TEXTURES - 1);

    uvec2 offset_coordinate;

    offset_coordinate.x = coordinate.x + ((FRAME / NUMBER_OF_BLUE_NOISE_TEXTURES) & (BLUE_NOISE_TEXTURE_RESOLUTION - 1));
    offset_coordinate.y = coordinate.y + ((FRAME / NUMBER_OF_BLUE_NOISE_TEXTURES / NUMBER_OF_BLUE_NOISE_TEXTURES) & (BLUE_NOISE_TEXTURE_RESOLUTION - 1));

    return texture(BLUE_NOISE_TEXTURES[offset_index], vec2(offset_coordinate) / float(BLUE_NOISE_TEXTURE_RESOLUTION));
}

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

//Constants.
#define LIGHT_TYPE_DIRECTIONAL (0)
#define LIGHT_TYPE_POINT (1)
#define LIGHT_TYPE_BOX (2)

#define LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT (BIT(0))
#define LIGHT_PROPERTY_VOLUMETRIC_BIT (BIT(1))
#define LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT (BIT(2))

/*
*	Light struct definition.
*/
struct Light
{
	/*
	*	First transform data.
	*	Direction for directional lights, position for point lights, minimum world position for box lights.
	*/
	vec3 _TransformData1;

	/*
	*	Second transform data.
	*	Maximum word position for box lights.
	*/
	vec3 _TransformData2;
	vec3 _Color;
	uint _LightType;
	uint _LightProperties;
	float _Intensity;
	float _Radius;
	float _Size;
};

/*
*	Unpacks the light at the given index.
*   Requies the Lighting storage buffer to be included.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 light_data_1 = LIGHT_DATA[index * 4 + 0];
  	vec4 light_data_2 = LIGHT_DATA[index * 4 + 1];
  	vec4 light_data_3 = LIGHT_DATA[index * 4 + 2];
  	vec4 light_data_4 = LIGHT_DATA[index * 4 + 3];

  	light._TransformData1 = vec3(light_data_1.x, light_data_1.y, light_data_1.z);
  	light._TransformData2 = vec3(light_data_1.w, light_data_2.x, light_data_2.y);
  	light._Color = vec3(light_data_2.z, light_data_2.w, light_data_3.x);
  	light._LightType = floatBitsToUint(light_data_3.y);
  	light._LightProperties = floatBitsToUint(light_data_3.z);
  	light._Intensity = light_data_3.w;
  	light._Radius = light_data_4.x;
  	light._Size = light_data_4.y;

	return light;
}

layout (set = 2, binding = 0) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 2, binding = 1) buffer OpaqueModels_VERTEX_DATA_BUFFER { vec4 OpaqueModels_VERTEX_DATA[]; } OpaqueModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 2) buffer OpaqueModels_INDEX_DATA_BUFFER { uint OpaqueModels_INDEX_DATA[]; } OpaqueModels_INDEX_BUFFERS[];
layout (set = 2, binding = 3) buffer OpaqueModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] OpaqueModels_MATERIAL_INDICES; };
layout (set = 2, binding = 4) buffer MaskedModels_VERTEX_DATA_BUFFER { vec4 MaskedModels_VERTEX_DATA[]; } MaskedModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 5) buffer MaskedModels_INDEX_DATA_BUFFER { uint MaskedModels_INDEX_DATA[]; } MaskedModels_INDEX_BUFFERS[];
layout (set = 2, binding = 6) buffer MaskedModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] MaskedModels_MATERIAL_INDICES; };
hitAttributeNV vec3 HIT_ATTRIBUTE;

struct HitVertexInformation
{
	vec3 _Position;
	vec3 _Normal;
	vec3 _Tangent;
	vec2 _TextureCoordinate;
};

HitVertexInformation GetHitVertexInformation()
{
	HitVertexInformation hit_vertex_information;
	uint vertex_index_0 = MaskedModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_INDEX_DATA[gl_PrimitiveID * 3 + 0];
	vec4 vertex_data_0_0 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_0 + 0];
	vec4 vertex_data_0_1 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_0 + 1];
	vec4 vertex_data_0_2 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_0 + 2];
	uint vertex_index_1 = MaskedModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_INDEX_DATA[gl_PrimitiveID * 3 + 1];
	vec4 vertex_data_1_0 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_1 + 0];
	vec4 vertex_data_1_1 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_1 + 1];
	vec4 vertex_data_1_2 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_1 + 2];
	uint vertex_index_2 = MaskedModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_INDEX_DATA[gl_PrimitiveID * 3 + 2];
	vec4 vertex_data_2_0 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_2 + 0];
	vec4 vertex_data_2_1 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_2 + 1];
	vec4 vertex_data_2_2 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_2 + 2];
	vec3 barycentric_coordinates = vec3(1.0f - HIT_ATTRIBUTE.x - HIT_ATTRIBUTE.y, HIT_ATTRIBUTE.x, HIT_ATTRIBUTE.y);
	hit_vertex_information._Position = vec3(vertex_data_0_0.x, vertex_data_0_0.y, vertex_data_0_0.z) * barycentric_coordinates[0] + vec3(vertex_data_1_0.x, vertex_data_1_0.y, vertex_data_1_0.z) * barycentric_coordinates[1] + vec3(vertex_data_2_0.x, vertex_data_2_0.y, vertex_data_2_0.z) * barycentric_coordinates[2];
	hit_vertex_information._Position = gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 1.0f);
	hit_vertex_information._Normal = vec3(vertex_data_0_0.w, vertex_data_0_1.x, vertex_data_0_1.y) * barycentric_coordinates[0] + vec3(vertex_data_1_0.w, vertex_data_1_1.x, vertex_data_1_1.y) * barycentric_coordinates[1] + vec3(vertex_data_2_0.w, vertex_data_2_1.x, vertex_data_2_1.y) * barycentric_coordinates[2];
	hit_vertex_information._Normal = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 0.0f));
	hit_vertex_information._Tangent = vec3(vertex_data_0_1.z, vertex_data_0_1.w, vertex_data_0_2.x) * barycentric_coordinates[0] + vec3(vertex_data_1_1.z, vertex_data_1_1.w, vertex_data_1_2.x) * barycentric_coordinates[1] + vec3(vertex_data_2_1.z, vertex_data_2_1.w, vertex_data_2_2.x) * barycentric_coordinates[2];
	hit_vertex_information._Tangent = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Tangent, 0.0f));
	hit_vertex_information._TextureCoordinate = vec2(vertex_data_0_2.y, vertex_data_0_2.z) * barycentric_coordinates[0] + vec2(vertex_data_1_2.y, vertex_data_1_2.z) * barycentric_coordinates[1] + vec2(vertex_data_2_2.y, vertex_data_2_2.z) * barycentric_coordinates[2];
	return hit_vertex_information;
}

uint GetHitMaterialIndex()
{
	return MaskedModels_MATERIAL_INDICES[gl_InstanceCustomIndexNV / 4][gl_InstanceCustomIndexNV & 3];
}

void main()
{
    HitVertexInformation hit_vertex_information = GetHitVertexInformation();
    uint hit_material_index = GetHitMaterialIndex();
    float opacity;
    EVALUATE_OPACITY(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, opacity);
    if (opacity < 0.5f)
    {
        ignoreIntersectionNV();
    }
}
