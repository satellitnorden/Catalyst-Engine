�nX/J1���+S�RayTracedVolumetricLighting_RenderPipeline                         �      #     �        �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main    @  G        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedVolumetricLighting_RayGeneration.glsl      �     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
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

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
};

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 3) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 4) uniform sampler SAMPLER;

layout (set = 1, binding = 5, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 6, rgba32f) uniform image2D VolumetricLighting; 

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
	*	Maximum world position for box lights.
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
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

/*
*	Returns the extinction at the given position.
*/
float GetExtinctionAtPosition(vec3 position)
{
	#define BASE_EXTINCTION (0.00125f)

	return mix(BASE_EXTINCTION, BASE_EXTINCTION * 0.125f, Square(clamp(position.y / 512.0f, 0.0f, 1.0f)));

	#undef BASE_EXTINCTION
}

/*
*	Calculates the attenuation in the given direction.
*/
float CalculateAttenuationInDirection(vec3 position, vec3 direction, float distance)
{
	#define NUMBER_OF_SAMPLES (4)

	float attenuation = 1.0f;
	float step_size = distance / float(NUMBER_OF_SAMPLES);

	for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
	{
		vec3 sample_position = position + direction * float(i) * step_size;
		attenuation *= exp(-GetExtinctionAtPosition(sample_position) * step_size);
	}

	return attenuation;
	
	#undef NUMBER_OF_SAMPLES
}

/*
*	The Henyey-Greenstein phase function.
*/
float HenyeyGreensteinPhaseFunction(vec3 outgoing_direction, vec3 incoming_direction)
{
	float G = 0.25f;
	float dot_product = dot(outgoing_direction, -incoming_direction);

	return (1.0f - G * G) / (4.0f * PI * pow(1.0 + G * G - 2.0f * G * dot_product, 3.0f / 2.0f));
}

/*
*	Calculates the scattering with the given properties.
*/
vec3 CalculateScattering(vec3 ray_origin, vec3 ray_direction)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

layout (set = 2, binding = 0) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 2, binding = 1) buffer OpaqueModels_VERTEX_DATA_BUFFER { vec4 OpaqueModels_VERTEX_DATA[]; } OpaqueModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 2) buffer OpaqueModels_INDEX_DATA_BUFFER { uint OpaqueModels_INDEX_DATA[]; } OpaqueModels_INDEX_BUFFERS[];
layout (set = 2, binding = 3) buffer OpaqueModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] OpaqueModels_MATERIAL_INDICES; };
layout (set = 2, binding = 4) buffer MaskedModels_VERTEX_DATA_BUFFER { vec4 MaskedModels_VERTEX_DATA[]; } MaskedModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 5) buffer MaskedModels_INDEX_DATA_BUFFER { uint MaskedModels_INDEX_DATA[]; } MaskedModels_INDEX_BUFFERS[];
layout (set = 2, binding = 6) buffer MaskedModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] MaskedModels_MATERIAL_INDICES; };
layout (location = 0) rayPayloadNV float VISIBILITY;

void main()
{
    #define SCATTERING (vec3(0.8f, 0.9f, 1.0f) * 0.125f * 0.125f)
    #define NUMBER_OF_SAMPLES (8)
    #define SAMPLE_RECIPROCAL (1.0f / NUMBER_OF_SAMPLES)
    #define HALF_SAMPLE_RECIPROCAL (SAMPLE_RECIPROCAL / 2)
    vec3 start_position = CAMERA_WORLD_POSITION;
    vec2 screen_coordinate = (vec2(gl_LaunchIDNV.xy) + vec2(0.5f)) / vec2(gl_LaunchSizeNV.xy);
    vec4 scene_features_2 = imageLoad(SceneFeatures2Half, ivec2(gl_LaunchIDNV.xy));
    float depth = scene_features_2.w;
    vec3 world_position = CalculateWorldPosition(screen_coordinate, depth);
    float hit_distance = length(world_position - start_position);
	float hit_distance_reciprocal = 1.0f / hit_distance;
	vec3 ray_direction = (world_position - start_position) * hit_distance_reciprocal;
    float offsets[NUMBER_OF_SAMPLES];
    for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        offsets[i] = HALF_SAMPLE_RECIPROCAL + SAMPLE_RECIPROCAL * float(i);
    }
    for (uint i = 0; i < NUMBER_OF_SAMPLES; i += 4)
    {
        vec4 blue_noise_texture_sample = (SampleBlueNoiseTexture(uvec2(gl_LaunchIDNV.xy), i / 4) - 0.5f) * (SAMPLE_RECIPROCAL - FLOAT32_EPSILON);
        offsets[i * 4 + 0] += blue_noise_texture_sample.x;
        offsets[i * 4 + 1] += blue_noise_texture_sample.y;
        offsets[i * 4 + 2] += blue_noise_texture_sample.z;
        offsets[i * 4 + 3] += blue_noise_texture_sample.w;
    }
    for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        offsets[i] *= offsets[i];
    }
	vec3 volumetric_lighting = vec3(0.0f);
    float transmittance = 1.0f;
    for (uint sample_index = 0; sample_index < NUMBER_OF_SAMPLES; ++sample_index)
    {
        float previous_offset = sample_index > 0 ? offsets[sample_index - 1] : 0.0f;
        float current_offset = offsets[sample_index];
        vec3 sample_position = mix(start_position, world_position, current_offset);
        float sample_hit_distance = (hit_distance * current_offset) - (hit_distance * previous_offset);
        float extinction = GetExtinctionAtPosition(sample_position);
        float attenuation_factor = exp(-extinction * sample_hit_distance);
        {
            float ambient_attenuation = mix(CalculateAttenuationInDirection(sample_position, vec3(0.0f, 1.0f, 0.0f), FAR_PLANE), CalculateAttenuationInDirection(sample_position, vec3(0.0f, -1.0f, 0.0f), FAR_PLANE), 0.5f);
            vec3 scattering = mix(UPPER_SKY_COLOR, LOWER_SKY_COLOR, 0.5f) * SCATTERING * (1.0f / (4.0f * 3.14f)) * ambient_attenuation;
            vec3 scattering_integral = (scattering - scattering * attenuation_factor) / max(extinction, FLOAT32_EPSILON);
            volumetric_lighting += transmittance * scattering_integral;
        }
        for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
        {
		    Light light = UnpackLight(i);
            if (TEST_BIT(light._LightProperties, LIGHT_PROPERTY_VOLUMETRIC_BIT))
            {
                vec3 light_radiance = light._Color * light._Intensity;
                switch (light._LightType)
                {
                    case LIGHT_TYPE_DIRECTIONAL:
                    {
                        float light_attenuation = CalculateAttenuationInDirection(sample_position, -light._TransformData1, hit_distance);
                        vec3 scattering = light_radiance * SCATTERING * HenyeyGreensteinPhaseFunction(ray_direction, light._TransformData1) * light_attenuation;
                        {
                            VISIBILITY = 0.0f;
                            uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                                        | gl_RayFlagsSkipClosestHitShaderNV;
                            vec3 direction = -light._TransformData1;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	0, /*missIndex*/
	sample_position, /*origin*/
	FLOAT32_EPSILON * 128.0f, /*Tmin*/
	direction, /*direction*/
	FLOAT32_MAXIMUM, /*Tmax*/
	0 /*payload*/
);
                        }
                        scattering *= mix(0.125f, 1.0f, VISIBILITY); //Still let a little light through.
                        vec3 scattering_integral = (scattering - scattering * attenuation_factor) / max(extinction, FLOAT32_EPSILON);
                        volumetric_lighting += transmittance * scattering_integral;
                        break;
                    }
                }
            }
        }
        transmittance *= attenuation_factor;
    }
    imageStore(VolumetricLighting, ivec2(gl_LaunchIDNV.xy), vec4(volumetric_lighting, 1.0f));
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         Square(f1;    
   X    
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size     "   UnpackLight(u1;   !   index    
 '   GetExtinctionAtPosition(vf3;      &   position      -   CalculateAttenuationInDirection(vf3;vf3;f1;   *   position      +   direction     ,   distance      2   HenyeyGreensteinPhaseFunction(vf3;vf3;    0   outgoing_direction    1   incoming_direction    9   offset_index      :   General  	 :       FULL_MAIN_RESOLUTION      :      INVERSE_FULL_MAIN_RESOLUTION     	 :      HALF_MAIN_RESOLUTION      :      INVERSE_HALF_MAIN_RESOLUTION      :      FRAME     :      VIEW_DISTANCE     <         F   offset_coordinate     _   BLUE_NOISE_TEXTURES   m   near_plane_coordinate     u   view_space_position   w   Camera   	 w       WORLD_TO_CLIP_MATRIX     	 w      WORLD_TO_CAMERA_MATRIX    w      PREVIOUS_WORLD_TO_CLIP_MATRIX     w      INVERSE_WORLD_TO_CAMERA_MATRIX    w      INVERSE_CAMERA_TO_CLIP_MATRIX    	 w      CAMERA_WORLD_POSITION    	 w      CAMERA_FORWARD_VECTOR    	 w      CURRENT_FRAME_JITTER      w      NEAR_PLANE    w   	   FAR_PLANE     y         �   inverse_view_space_position_denominator   �   world_space_position      �   light_data_1      �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   light_data_2      �   light_data_3      �   light_data_4      �   light     �   param     �   attenuation   �   step_size     �   i       sample_position     param       G        dot_product   9  start_position    =  screen_coordinate     @  gl_LaunchIDNV     G  gl_LaunchSizeNV   L  scene_features_2      O  SceneFeatures2Half    V  depth     Y  world_position    Z  param     \  param     _  hit_distance      d  hit_distance_reciprocal   g  ray_direction     m  i     x  offsets   �  i    	 �  blue_noise_texture_sample     �  param     �  param     �  i     �  volumetric_lighting   �  transmittance     �  sample_index      �  previous_offset   �  current_offset    �  sample_position   �  sample_hit_distance   �  extinction    �  param     �  attenuation_factor      ambient_attenuation     param     
  param       param       param       param       param       scattering      Wind            UPPER_SKY_COLOR        LOWER_SKY_COLOR        SKY_MODE           MAXIMUM_SKY_TEXTURE_MIP_LEVEL             -  scattering_integral   =  i     G  light     H  param     Q  light_radiance    [  light_attenuation     _  param     a  param     b  param     e  scattering    h  param     j  param     r  VISIBILITY    s  ray_tracing_flags     u  direction     {  TOP_LEVEL_ACCELERATION_STRUCTURE      �  scattering_integral   �  VolumetricLighting    �  TEXTURES     	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  SKY_TEXTURE   �  SAMPLER  
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS  
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �       
 �  MaskedModels_VERTEX_DATA_BUFFER  
 �      MaskedModels_VERTEX_DATA     	 �  MaskedModels_VERTEX_BUFFERS  
 �  MaskedModels_INDEX_DATA_BUFFER   	 �      MaskedModels_INDEX_DATA  	 �  MaskedModels_INDEX_BUFFERS   
 �  MaskedModels_MATERIAL_BUFFER      �      MaskedModels_MATERIAL_INDICES     �      H  :       #       H  :      #      H  :      #      H  :      #      H  :      #       H  :      #   $   G  :      G  <   "      G  <   !      G  _   "       G  _   !      H  w          H  w       #       H  w             H  w         H  w      #   @   H  w            H  w         H  w      #   �   H  w            H  w         H  w      #   �   H  w            H  w         H  w      #      H  w            H  w      #   @  H  w      #   P  H  w      #   `  H  w      #   h  H  w   	   #   l  G  w      G  y   "      G  y   !       H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      G  @     �  G  G     �  G  O  "      G  O  !      H        #       H       #      H       #       H       #   $   G       G    "      G    !      G  r         G  {  "      G  {  !       G  �  "      G  �  !      G  �  "       G  �  !       G  �        H  �      #       G  �     G  �  "       G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                             !  	                                                                    !                                                !               
                            !               $         !  %      $   !  )      $   $      !  /      $   $     :                        ;      :   ;  ;   <        =          +  =   >         ?         +     D      +     G       +     L       +     Q       	 [                              \   [     ]   \   L      ^       ]   ;  ^   _          a       \   +     f      B+     i       +     o      @+     q     �?   t           v           w   v   v   v   v   v                     x      w   ;  x   y         z      v   +     �      +  =   �        �           �        �   �   �      �      �   ;  �   �      +  =   �      +     �         �         +     �         �         +  =   �       +  =   �      +  =   �      +  =   �      +  =   �      +     �   
ף:+     �   
�#9+     �      D+     �     �@    +         �>+     )  �IA+     3    �?   :          >           ?     >  ;  ?  @     +     D     ?,     E  D  D  ;  ?  G      	 M                             N      M  ;  N  O        S  =      +     t       v     t     w     v  +     z    �=+     {     >+     �  ���=,     �  i   i   i   ,       i   q   i   +  =     	              +         ��,       i     i                             ;         +     $  ��L<+     %  fff<+     &    �<,     '  $  %  &  +     )  ��=+     4     4   q  �     ;  q  r  �  +     t     � y     z      y  ;  z  {      +     ~  �   +     �    �7+     �  ��;  N  �      +     �       �  [   �     �      �  ;  �  �        �     �    �  �     �     �  ;  �  �      
 �                          +     �       �  �  �    �  �     �     �  ;  �  �      	 �                             �  �     �      �  ;  �  �        �     �      �  ;  �  �        �       �  �    �  �  Q      �     �  ;  �  �       �       �  �    �  �  Q      �     �  ;  �  �       �          �  �    �  �     �     �  ;  �  �       �       �  �    �  �  Q      �     �  ;  �  �       �       �  �    �  �  Q      �     �  ;  �  �       �  �    �  �     �     �  ;  �  �          q     6               �     ;  $   9     ;     =     ;  t   L     ;     V     ;  $   Y     ;     Z     ;     \     ;     _     ;     d     ;  $   g     ;     m     ;  w  x     ;     �     ;  t   �     ;     �     ;     �     ;     �     ;  $   �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  $   �     ;     �     ;     �     ;  $   �     ;     �     ;          ;  $        ;  $   
     ;          ;  $        ;  $        ;          ;  $        ;  $   -     ;     =     ;  �   G     ;     H     ;  $   Q     ;     [     ;  $   _     ;  $   a     ;     b     ;  $   e     ;  $   h     ;  $   j     ;     s     ;  $   u     ;  $   �          w      A  :  ;  y   �   =     <  ;  >  9  <       x      =  >  A  @  O     B  A  A         p     C  B  �     F  C  E  =  >  H  G  O     I  H  H         p     J  I  �     K  F  J  >  =  K       y      =  M  P  O  =  >  Q  @  O     R  Q  Q         |  S  T  R  b     U  P  T  >  L  U       z      A     W  L  �   =     X  W  >  V  X       {      =     [  =  >  Z  [  =     ]  V  >  \  ]  9     ^     Z  \  >  Y  ^       |      =     `  Y  =     a  9  �     b  `  a       c     B   b  >  _  c       }      =     e  _  �     f  q   e  >  d  f       ~      =     h  Y  =     i  9  �     j  h  i  =     k  d  �     l  j  k  >  g  l       �      >  m  G   �  n  �  n       �      �  p  q      �  r  �  r       �      =     s  m  �    u  s  t  �  u  o  p  �  o       �      =     y  m  =     |  m  p     }  |  �     ~  {  }  �       z  ~  A     �  x  y  >  �    �  q  �  q       �      =     �  m  �     �  �  �   >  m  �  �  n  �  p       �      >  �  G   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �    �  �  t  �  �  �  �  �  �       �      =  >  �  @  O     �  �  �         Q     �  �      Q     �  �     P     �  �  �  =     �  �  �     �  �  �   >  �  �  >  �  �  9     �     �  �  P     �  D  D  D  D  �     �  �  �  �     �  �  �  >  �  �       �      =     �  �  �     �  �  �   �     �  �  G   A     �  �  G   =     �  �  A     �  x  �  =     �  �  �     �  �  �  A     �  x  �  >  �  �       �      =     �  �  �     �  �  �   �     �  �  Q   A     �  �  Q   =     �  �  A     �  x  �  =     �  �  �     �  �  �  A     �  x  �  >  �  �       �      =     �  �  �     �  �  �   �     �  �  �   A     �  �  �   =     �  �  A     �  x  �  =     �  �  �     �  �  �  A     �  x  �  >  �  �       �      =     �  �  �     �  �  �   �     �  �  �   A     �  �  �   =     �  �  A     �  x  �  =     �  �  �     �  �  �  A     �  x  �  >  �  �  �  �  �  �       �      =     �  �  �     �  �  �   >  �  �  �  �  �  �       �      >  �  G   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �    �  �  t  �  �  �  �  �  �       �      =     �  �  =     �  �  A     �  x  �  =     �  �  A     �  x  �  =     �  �  �     �  �  �  A     �  x  �  >  �  �  �  �  �  �       �      =     �  �  �     �  �  �   >  �  �  �  �  �  �       �      >  �  �       �      >  �  q        �      >  �  G   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �    �  �  t  �  �  �  �  �  �       �      =     �  �  �    �  �  G   �  �      �  �  �  �  �  �       �      =     �  �  �     �  �  Q   A     �  x  �  =     �  �  >  �  �  �  �  �  �  >  �  i   �  �  �  �  =     �  �       �      >  �  �       �      =     �  �  A     �  x  �  =     �  �  >  �  �       �      =     �  9  =     �  Y  =     �  �  P     �  �  �  �       �     .   �  �  �  >  �  �       �      =     �  _  =     �  �  �     �  �  �  =     �  _  =     �  �  �     �  �  �  �     �  �  �  >  �  �       �      =     �  �  >  �  �  9     �  '   �  >  �  �       �      =        �            =       �  �                          >  �         �      =     	  �  >    	  >  
    A      y     =         >      9       -     
    =       �  >      >      A      y     =         >      9       -                   .       D  >           �      A  :      �   =         A  :       �   =     !     P     "  D  D  D       #     .     !  "  �     (  #  '  �     *  (  )  =     +    �     ,  *  +  >    ,       �      =     .    =     /    =     0  �  �     1  /  0  �     2  .  1  =     3  �       5     (   3  4  P     6  5  5  5  �     7  2  6  >  -  7       �      =     8  �  =     9  -  �     :  9  8  =     ;  �  �     <  ;  :  >  �  <       �      >  =  G   �  >  �  >       �      �  @  A      �  B  �  B       �      =     C  =  A  ?   D  �   �   �   =     E  D  �    F  C  E  �  F  ?  @  �  ?       �      =     I  =  >  H  I  9     J  "   H  >  G  J       �      A     K  G  >   =     L  K  �     M  L  �   �    N  M  �   �  P      �  N  O  P  �  O       �      A  $   R  G  �   =     S  R  A     T  G  �   =     U  T  �     V  S  U  >  Q  V       �      A     W  G  �   =     X  W  �  Z      �  X  Z      Y  �  Y       �      A  $   \  G  �   =     ]  \       ^  ]  =     `  �  >  _  `  >  a  ^  =     c  _  >  b  c  9     d  -   _  a  b  >  [  d       �      =     f  Q  �     g  f  '  =     i  g  >  h  i  A  $   k  G  �   =     l  k  >  j  l  9     m  2   h  j  �     n  g  m  =     o  [  �     p  n  o  >  e  p       �      >  r  i        �      >  s  t       �      A  $   v  G  �   =     w  v       x  w  >  u  x       �      =  y  |  {  =     }  s  =       �  =     �  u  � |  }  ~  G   G   G     �  �  �  �        �      =     �  r       �     .   {  q   �  =     �  e  �     �  �  �  >  e  �       �      =     �  e  =     �  e  =     �  �  �     �  �  �  �     �  �  �  =     �  �       �     (   �  4  P     �  �  �  �  �     �  �  �  >  �  �       �      =     �  �  =     �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      �  Z  �  Z  �  P  �  P  �  A  �  A       �      =     �  =  �     �  �  �   >  =  �  �  >  �  @       �      =     �  �  =     �  �  �     �  �  �  >  �  �  �  �  �  �       �      =     �  �  �     �  �  �   >  �  �  �  �  �  �       �      =  M  �  �  =  >  �  @  O     �  �  �         |  S  �  �  =     �  �  Q     �  �      Q     �  �     Q     �  �     P     �  �  �  �  q   c  �  �  �  �  8       �      6            	   7     
   �          �       =     4   
   =     5   
   �     6   4   5   �  6   8       J  9   6               7        7        �     ;     9      ;     F           L      A  ?   @   <   >   =     A   @   =     B      �     C   A   B   �     E   C   D   >  9   E        P      A     H      G   =     I   H   A  ?   J   <   >   =     K   J   �     M   K   L   �     N   M   D   �     O   I   N   A     P   F   G   >  P   O        Q      A     R      Q   =     S   R   A  ?   T   <   >   =     U   T   �     V   U   L   �     W   V   L   �     X   W   D   �     Y   S   X   A     Z   F   Q   >  Z   Y        S      =     `   9   A  a   b   _   `   =  \   c   b   =     d   F   p     e   d   P     g   f   f   �     h   e   g   X     j   c   h      i   �  j   8       n  @   6               7        7        �     ;     m      ;  t   u      ;     �      ;  t   �           p      =     n      �     p   n   o   P     r   q   q   �     s   p   r   >  m   s        q      A  z   {   y   >   =  v   |   {   =     }   m   =     ~      Q        }       Q     �   }      P     �      �   ~   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   q   �     �   |   �   >  u   �        r      A     �   u   �   =     �   �   �     �   q   �   >  �   �        s      =     �   �   =     �   u   �     �   �   �   >  u   �        t      A  z   �   y   �   =  v   �   �   =     �   u   �     �   �   �   >  �   �        v      =     �   �   O     �   �   �             �  �   8       �     6     "           7     !   �  #   ;  t   �      ;  t   �      ;  t   �      ;  t   �      ;  �   �           �      =     �   !   �     �   �   �   �     �   �   G   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �   !   �     �   �   �   �     �   �   Q   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �   !   �     �   �   �   �     �   �   �   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �   !   �     �   �   �   �     �   �   �   A  �   �   �   �   �   =     �   �   >  �   �        �      A     �   �   G   =     �   �   A     �   �   Q   =     �   �   A     �   �   �   =     �   �   P     �   �   �   �   A  $   �   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   G   =     �   �   A     �   �   Q   =     �   �   P     �   �   �   �   A  $   �   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   �   =     �   �   A     �   �   G   =     �   �   P     �   �   �   �   A  $   �   �   �   >  �   �        �      A     �   �   Q   =     �   �   |     �   �   A     �   �   �   >  �   �        �      A     �   �   �   =     �   �   |     �   �   A     �   �   >   >  �   �        �      A     �   �   �   =     �   �   A     �   �   �   >  �   �        �      A     �   �   G   =     �   �   A     �   �   �   >  �   �        �      A     �   �   Q   =     �   �   A     �   �   �   >  �   �        �      =     �   �   �  �   8       7  ,   6     '       %   7  $   &   �  (   ;     �           ;      A     �   &   Q   =     �   �   �     �   �   �        �      +   �   i   q   >  �   �   9     �      �        �      .   �   �   �   �  �   8       C  T   6     -       )   7  $   *   7  $   +   7     ,   �  .   ;     �      ;     �      ;     �      ;  $        ;  $             G      >  �   q        H      =     �   ,   �     �   �   �   >  �   �        J      >  �   G   �  �   �  �        J      �          �    �         J      =       �   �        �   �         �          L      =       *   =     	  +   =     
  �   p       
  �       	    =       �   �           �           >           M      =         >      9       '              =       �   �                          =       �   �           >  �     �    �         J      =       �   �         �   >  �     �  �   �         P      =       �   �    8       X  U   6     2       /   7  $   0   7  $   1   �  3   ;          ;                Z      >           [      =     !  0   =     "  1        #  "  �     $  !  #  >     $       ]      =     %    =     &    �     '  %  &  �     (  q   '  =     *    =     +    �     ,  *  +  �     -  q   ,  =     .    �     /  o   .  =     0     �     1  /  0  �     2  -  1       4        2  3  �     5  )  4  �     6  (  5  �  6  8         �e      #     ^         �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedVolumetricLighting_RayMiss.glsl     �   �     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
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

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
};

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 3) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 4) uniform sampler SAMPLER;

layout (set = 1, binding = 5, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 6, rgba32f) uniform image2D VolumetricLighting; 

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
	*	Maximum world position for box lights.
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
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

/*
*	Returns the extinction at the given position.
*/
float GetExtinctionAtPosition(vec3 position)
{
	#define BASE_EXTINCTION (0.00125f)

	return mix(BASE_EXTINCTION, BASE_EXTINCTION * 0.125f, Square(clamp(position.y / 512.0f, 0.0f, 1.0f)));

	#undef BASE_EXTINCTION
}

/*
*	Calculates the attenuation in the given direction.
*/
float CalculateAttenuationInDirection(vec3 position, vec3 direction, float distance)
{
	#define NUMBER_OF_SAMPLES (4)

	float attenuation = 1.0f;
	float step_size = distance / float(NUMBER_OF_SAMPLES);

	for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
	{
		vec3 sample_position = position + direction * float(i) * step_size;
		attenuation *= exp(-GetExtinctionAtPosition(sample_position) * step_size);
	}

	return attenuation;
	
	#undef NUMBER_OF_SAMPLES
}

/*
*	The Henyey-Greenstein phase function.
*/
float HenyeyGreensteinPhaseFunction(vec3 outgoing_direction, vec3 incoming_direction)
{
	float G = 0.25f;
	float dot_product = dot(outgoing_direction, -incoming_direction);

	return (1.0f - G * G) / (4.0f * PI * pow(1.0 + G * G - 2.0f * G * dot_product, 3.0f / 2.0f));
}

/*
*	Calculates the scattering with the given properties.
*/
vec3 CalculateScattering(vec3 ray_origin, vec3 ray_direction)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

layout (set = 2, binding = 0) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 2, binding = 1) buffer OpaqueModels_VERTEX_DATA_BUFFER { vec4 OpaqueModels_VERTEX_DATA[]; } OpaqueModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 2) buffer OpaqueModels_INDEX_DATA_BUFFER { uint OpaqueModels_INDEX_DATA[]; } OpaqueModels_INDEX_BUFFERS[];
layout (set = 2, binding = 3) buffer OpaqueModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] OpaqueModels_MATERIAL_INDICES; };
layout (set = 2, binding = 4) buffer MaskedModels_VERTEX_DATA_BUFFER { vec4 MaskedModels_VERTEX_DATA[]; } MaskedModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 5) buffer MaskedModels_INDEX_DATA_BUFFER { uint MaskedModels_INDEX_DATA[]; } MaskedModels_INDEX_BUFFERS[];
layout (set = 2, binding = 6) buffer MaskedModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] MaskedModels_MATERIAL_INDICES; };
layout (location = 0) rayPayloadInNV float VISIBILITY;

void main()
{
    VISIBILITY = 1.0f;
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main      	   VISIBILITY       TEXTURES     	    GlobalTextureAverageValues   	        TEXTURE_AVERAGE_VALUES                Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                  BLUE_NOISE_TEXTURES   $   SKY_TEXTURE   (   Camera   	 (       WORLD_TO_CLIP_MATRIX     	 (      WORLD_TO_CAMERA_MATRIX    (      PREVIOUS_WORLD_TO_CLIP_MATRIX     (      INVERSE_WORLD_TO_CAMERA_MATRIX    (      INVERSE_CAMERA_TO_CLIP_MATRIX    	 (      CAMERA_WORLD_POSITION    	 (      CAMERA_FORWARD_VECTOR    	 (      CURRENT_FRAME_JITTER      (      NEAR_PLANE    (   	   FAR_PLANE     *         +   General  	 +       FULL_MAIN_RESOLUTION      +      INVERSE_FULL_MAIN_RESOLUTION     	 +      HALF_MAIN_RESOLUTION      +      INVERSE_HALF_MAIN_RESOLUTION      +      FRAME     +      VIEW_DISTANCE     -         .   Wind      .       UPPER_SKY_COLOR   .      LOWER_SKY_COLOR   .      SKY_MODE      .      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     0         1   LightingHeader    1       _NumberOfLights   1      _MaximumNumberOfShadowCastingLights   3   Lighting      3       LIGHTING_HEADER   3      LIGHT_DATA    5         8   SAMPLER   ;   SceneFeatures2Half    <   VolumetricLighting    ?   TOP_LEVEL_ACCELERATION_STRUCTURE     
 A   OpaqueModels_VERTEX_DATA_BUFFER  
 A       OpaqueModels_VERTEX_DATA     	 E   OpaqueModels_VERTEX_BUFFERS  
 G   OpaqueModels_INDEX_DATA_BUFFER   	 G       OpaqueModels_INDEX_DATA  	 J   OpaqueModels_INDEX_BUFFERS   
 M   OpaqueModels_MATERIAL_BUFFER      M       OpaqueModels_MATERIAL_INDICES     O        
 Q   MaskedModels_VERTEX_DATA_BUFFER  
 Q       MaskedModels_VERTEX_DATA     	 T   MaskedModels_VERTEX_BUFFERS  
 V   MaskedModels_INDEX_DATA_BUFFER   	 V       MaskedModels_INDEX_DATA  	 Y   MaskedModels_INDEX_BUFFERS   
 [   MaskedModels_MATERIAL_BUFFER      [       MaskedModels_MATERIAL_INDICES     ]       G  	          G     "       G     !       G           H         #       G        G     "       G     !      H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G      "       G      !      G  $   "       G  $   !      H  (          H  (       #       H  (             H  (         H  (      #   @   H  (            H  (         H  (      #   �   H  (            H  (         H  (      #   �   H  (            H  (         H  (      #      H  (            H  (      #   @  H  (      #   P  H  (      #   `  H  (      #   h  H  (   	   #   l  G  (      G  *   "      G  *   !       H  +       #       H  +      #      H  +      #      H  +      #      H  +      #       H  +      #   $   G  +      G  -   "      G  -   !      H  .       #       H  .      #      H  .      #       H  .      #   $   G  .      G  0   "      G  0   !      H  1       #       H  1      #      G  2         H  3       #       H  3      #      G  3      G  5   "      G  5   !      G  8   "      G  8   !      G  ;   "      G  ;   !      G  <   "      G  <   !      G  ?   "      G  ?   !       G  @         H  A       #       G  A      G  E   "      G  E   !      G  F         H  G       #       G  G      G  J   "      G  J   !      G  L         H  M       #       G  M      G  O   "      G  O   !      G  P         H  Q       #       G  Q      G  T   "      G  T   !      G  U         H  V       #       G  V      G  Y   "      G  Y   !      G  Z         H  [       #       G  [      G  ]   "      G  ]   !           !                       �     ;     	   �  +     
     �? 	                                          +                                   ;                                                      ;            
                            +                                          ;                   +                                    ;              	 !                              "   !      #       "   ;  #   $         %           &           '           (   %   %   %   %   %   &   &   '            )      (   ;  )   *        +   '   '   '   '            ,      +   ;  ,   -        .   &   &            /      .   ;  /   0        1           2        3   1   2      4      3   ;  4   5        6      7       6   ;  7   8        	 9                              :       9   ;  :   ;       ;  :   <       � =      >       =   ;  >   ?         @        A   @   +     B        C   A   B      D      C   ;  D   E        F        G   F     H   G   B      I      H   ;  I   J        K           L   K     M   L      N      M   ;  N   O        P        Q   P     R   Q   B      S      R   ;  S   T        U        V   U     W   V   B      X      W   ;  X   Y        Z   K     [   Z      \      [   ;  \   ]           q     6               �          s      >  	   
   �  8         �B�m��Q P�      #     �        �    �  
  SPV_EXT_descriptor_indexing 
  SPV_NV_ray_tracing       GLSL.std.450               �     main    F   I   �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedVolumetricLighting_RayAnyHit.glsl   M   �     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
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

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
};

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 3) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 4) uniform sampler SAMPLER;

layout (set = 1, binding = 5, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 6, rgba32f) uniform image2D VolumetricLighting; 

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
	*	Maximum world position for box lights.
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
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

/*
*	Returns the extinction at the given position.
*/
float GetExtinctionAtPosition(vec3 position)
{
	#define BASE_EXTINCTION (0.00125f)

	return mix(BASE_EXTINCTION, BASE_EXTINCTION * 0.125f, Square(clamp(position.y / 512.0f, 0.0f, 1.0f)));

	#undef BASE_EXTINCTION
}

/*
*	Calculates the attenuation in the given direction.
*/
float CalculateAttenuationInDirection(vec3 position, vec3 direction, float distance)
{
	#define NUMBER_OF_SAMPLES (4)

	float attenuation = 1.0f;
	float step_size = distance / float(NUMBER_OF_SAMPLES);

	for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
	{
		vec3 sample_position = position + direction * float(i) * step_size;
		attenuation *= exp(-GetExtinctionAtPosition(sample_position) * step_size);
	}

	return attenuation;
	
	#undef NUMBER_OF_SAMPLES
}

/*
*	The Henyey-Greenstein phase function.
*/
float HenyeyGreensteinPhaseFunction(vec3 outgoing_direction, vec3 incoming_direction)
{
	float G = 0.25f;
	float dot_product = dot(outgoing_direction, -incoming_direction);

	return (1.0f - G * G) / (4.0f * PI * pow(1.0 + G * G - 2.0f * G * dot_product, 3.0f / 2.0f));
}

/*
*	Calculates the scattering with the given properties.
*/
vec3 CalculateScattering(vec3 ray_origin, vec3 ray_direction)
{
	return vec3(0.0f, 0.0f, 0.0f);
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
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         UnpackColor(u1;      color        HitVertexInformation             _Position           _Normal         _Tangent            _TextureCoordinate   	    GetHitVertexInformation(         GetHitMaterialIndex(         unpacked      ?   vertex_index_0   
 A   MaskedModels_INDEX_DATA_BUFFER   	 A       MaskedModels_INDEX_DATA  	 D   MaskedModels_INDEX_BUFFERS   	 F   gl_InstanceCustomIndexNV      I   gl_PrimitiveID    Q   vertex_data_0_0  
 S   MaskedModels_VERTEX_DATA_BUFFER  
 S       MaskedModels_VERTEX_DATA     	 V   MaskedModels_VERTEX_BUFFERS   ^   vertex_data_0_1   e   vertex_data_0_2   l   vertex_index_1    t   vertex_data_1_0   {   vertex_data_1_1   �   vertex_data_1_2   �   vertex_index_2    �   vertex_data_2_0   �   vertex_data_2_1   �   vertex_data_2_2   �   barycentric_coordinates   �   HIT_ATTRIBUTE     �   hit_vertex_information    �   gl_ObjectToWorldNV   
 ^  MaskedModels_MATERIAL_BUFFER      ^      MaskedModels_MATERIAL_INDICES     `        j  hit_vertex_information    l  hit_material_index    n  Material      n      _Properties   n     _AlbedoThickness     	 n     _NormalMapDisplacement    n     _MaterialProperties   n     _Opacity      n     _EmissiveMultiplier   n     _Padding1     n     _Padding2     q  GlobalMaterials   q      MATERIALS     s        }  opacity   �  TEXTURES      �  SAMPLER   �  param    	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE   �  Camera   	 �      WORLD_TO_CLIP_MATRIX     	 �     WORLD_TO_CAMERA_MATRIX    �     PREVIOUS_WORLD_TO_CLIP_MATRIX     �     INVERSE_WORLD_TO_CAMERA_MATRIX    �     INVERSE_CAMERA_TO_CLIP_MATRIX    	 �     CAMERA_WORLD_POSITION    	 �     CAMERA_FORWARD_VECTOR    	 �     CURRENT_FRAME_JITTER      �     NEAR_PLANE    �  	   FAR_PLANE     �        �  General  	 �      FULL_MAIN_RESOLUTION      �     INVERSE_FULL_MAIN_RESOLUTION     	 �     HALF_MAIN_RESOLUTION      �     INVERSE_HALF_MAIN_RESOLUTION      �     FRAME     �     VIEW_DISTANCE     �        �  Wind      �      UPPER_SKY_COLOR   �     LOWER_SKY_COLOR   �     SKY_MODE      �     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     �        �  LightingHeader    �      _NumberOfLights   �     _MaximumNumberOfShadowCastingLights   �  Lighting      �      LIGHTING_HEADER   �     LIGHT_DATA    �        �  SceneFeatures2Half    �  VolumetricLighting    �  TOP_LEVEL_ACCELERATION_STRUCTURE     
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS  
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �      G  @         H  A       #       G  A      G  D   "      G  D   !      G  F      �  G  I         G  R         H  S       #       G  S      G  V   "      G  V   !      G  �      �  G  ]        H  ^      #       G  ^     G  `  "      G  `  !      H  n      #       H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      G  p         H  q      #       G  q     G  s  "       G  s  !      G  �  "       G  �  !       G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !      H  �         H  �      #       H  �            H  �        H  �     #   @   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #      H  �           H  �     #   @  H  �     #   P  H  �     #   `  H  �     #   h  H  �  	   #   l  G  �     G  �  "      G  �  !       H  �      #       H  �     #      H  �     #      H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      H  �      #       H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      H  �      #       H  �     #      G  �        H  �      #       H  �     #      G  �     G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !       G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	         
   	      !     
           	           	                       !        !                 
   +        �   +  	        C+                !      	     $          +  $   %      +     *      +  $   -      +     2      +  $   5      +     :        @        A   @     B   A      C      B   ;  C   D         E      $   ;  E   F      +  $   H       ;  E   I      +  $   K         N           R   
     S   R     T   S      U      T   ;  U   V         [      
   +  $   p      +  $   �         �         +  	   �     �?   �   �     ;  �   �   �     �   �  	      �           �            �      �   ;  �   �      +  	   
         W          \          ]  \    ^  ]     _     ^  ;  _  `     +  $   b      
 n                 	         +     o       p  n  o    q  p     r     q  ;  r  s     +     w  @     y   	 ~  	                         +            �  ~       �      �  ;  �  �         �      ~    �     �      �  ;  �  �        �  ~  +  	   �     ?  �  
       �  �     �     �  ;  �  �     +     �        �  �  �     �      �  ;  �  �       	 �  	                           �  �     �      �  ;  �  �        �  
        �  �  �  �  �  �           	   	      �     �  ;  �  �       �                 	      �     �  ;  �  �       �           	      �     �  ;  �  �       �          �  
     �  �  �     �     �  ;  �  �      	 �  	                           �      �  ;  �  �      ;  �  �      � �     �      �  ;  �  �        �  
     �  �    �  �  *      �     �  ;  �  �       �       �  �    �  �  *      �     �  ;  �  �       �  \    �  �     �     �  ;  �  �          �     6               �     ;  �   j     ;     l     ;  !   }     ;     �          �      9     k     >  j  k       �      9     m     >  l  m       �      =     t  l  A  N   u  s  H   t  H   =     v  u  �     x  v  w  �  y  z  x  w  �  |      �  z  {  �  �  {       �      =     �  l  A  N   �  s  H   �  b  =     �  �  A  �  �  �  �  =  ~  �  �  =  �  �  �  V  �  �  �  �  A  W  �  j  K   =     �  �  X  
   �  �  �     
  Q  	   �  �      >  }  �  �  |  �  �       �      =     �  l  A  N   �  s  H   �  b  =     �  �  >  �  �  9  
   �     �  Q  	   �  �      >  }  �  �  |  �  |       �      =  	   �  }  �  y  �  �  �  �  �      �  �  �  �  �  �       �      � �  �  �  �  �  8       �      6  
             7        �     ;                      =           �              p  	         �  	            A  !   "          >  "                 =     #      �     &   #   %   �     '   &      p  	   (   '   �  	   )   (      A  !   +      *   >  +   )              =     ,      �     .   ,   -   �     /   .      p  	   0   /   �  	   1   0      A  !   3      2   >  3   1              =     4      �     6   4   5   �     7   6      p  	   8   7   �  	   9   8      A  !   ;      :   >  ;   9              =  
   <      �  <   8       y  .   6               �     ;     ?      ;     Q      ;     ^      ;     e      ;     l      ;     t      ;     {      ;     �      ;     �      ;     �      ;     �      ;     �      ;  �   �      ;  �   �           |      =  $   G   F   =  $   J   I   �  $   L   J   K   �  $   M   L   H   A  N   O   D   G   H   M   =     P   O   >  ?   P        }      =  $   W   F   =     X   ?   �     Y   :   X   �     Z   Y       A  [   \   V   W   H   Z   =  
   ]   \   >  Q   ]        ~      =  $   _   F   =     `   ?   �     a   :   `   �     b   a   *   A  [   c   V   _   H   b   =  
   d   c   >  ^   d              =  $   f   F   =     g   ?   �     h   :   g   �     i   h   2   A  [   j   V   f   H   i   =  
   k   j   >  e   k        �      =  $   m   F   =  $   n   I   �  $   o   n   K   �  $   q   o   p   A  N   r   D   m   H   q   =     s   r   >  l   s        �      =  $   u   F   =     v   l   �     w   :   v   �     x   w       A  [   y   V   u   H   x   =  
   z   y   >  t   z        �      =  $   |   F   =     }   l   �     ~   :   }   �        ~   *   A  [   �   V   |   H      =  
   �   �   >  {   �        �      =  $   �   F   =     �   l   �     �   :   �   �     �   �   2   A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      =  $   �   F   =  $   �   I   �  $   �   �   K   �  $   �   �   �   A  N   �   D   �   H   �   =     �   �   >  �   �        �      =  $   �   F   =     �   �   �     �   :   �   �     �   �       A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      =  $   �   F   =     �   �   �     �   :   �   �     �   �   *   A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      =  $   �   F   =     �   �   �     �   :   �   �     �   �   2   A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      A  �   �   �       =  	   �   �   �  	   �   �   �   A  �   �   �   *   =  	   �   �   �  	   �   �   �   A  �   �   �       =  	   �   �   A  �   �   �   *   =  	   �   �   P     �   �   �   �   >  �   �        �      A  !   �   Q       =  	   �   �   A  !   �   Q   *   =  	   �   �   A  !   �   Q   2   =  	   �   �   P     �   �   �   �   A  !   �   �       =  	   �   �   �     �   �   �   A  !   �   t       =  	   �   �   A  !   �   t   *   =  	   �   �   A  !   �   t   2   =  	   �   �   P     �   �   �   �   A  !   �   �   *   =  	   �   �   �     �   �   �   �     �   �   �   A  !   �   �       =  	   �   �   A  !   �   �   *   =  	   �   �   A  !   �   �   2   =  	   �   �   P     �   �   �   �   A  !   �   �   2   =  	   �   �   �     �   �   �   �     �   �   �   A  �   �   �   H   >  �   �        �      =  �   �   �   A  �   �   �   p   =     �   �   Q  	   �   �       Q  	   �   �      Q  	   �   �      P  
   �   �   �   �   �   �     �   �   �   A  �   �   �   H   >  �   �        �      A  !   �   Q   :   =  	   �   �   A  !   �   ^       =  	   �   �   A  !   �   ^   *   =  	   �   �   P     �   �   �   �   A  !   �   �       =  	   �   �   �     �   �   �   A  !   �   t   :   =  	   �   �   A  !   �   {       =  	   �   �   A  !   �   {   *   =  	   �   �   P     �   �   �   �   A  !   �   �   *   =  	   �   �   �     �   �   �   �     �   �   �   A  !   �   �   :   =  	   �   �   A  !   �   �       =  	   �   �   A  !   �   �   *   =  	      �   P       �   �      A  !     �   2   =  	       �           �       �     A  �     �   p   >           �      =  �     �   A  �     �   p   =     	    Q  	     	      Q  	     	     Q  	     	     P  
           
  �                     E     A  �     �   p   >           �      A  !     ^   2   =  	       A  !     ^   :   =  	       A  !     e       =  	       P             A  !     �       =  	       �           A  !     {   2   =  	       A  !     {   :   =  	       A  !      �       =  	   !     P     "      !  A  !   #  �   *   =  	   $  #  �     %  "  $  �     &    %  A  !   '  �   2   =  	   (  '  A  !   )  �   :   =  	   *  )  A  !   +  �       =  	   ,  +  P     -  (  *  ,  A  !   .  �   2   =  	   /  .  �     0  -  /  �     1  &  0  A  �   2  �   �   >  2  1       �      =  �   3  �   A  �   4  �   �   =     5  4  Q  	   6  5      Q  	   7  5     Q  	   8  5     P  
   9  6  7  8  
  �     :  3  9       ;     E   :  A  �   <  �   �   >  <  ;       �      A  !   =  e   *   =  	   >  =  A  !   ?  e   2   =  	   @  ?  P     A  >  @  A  !   B  �       =  	   C  B  �     D  A  C  A  !   E  �   *   =  	   F  E  A  !   G  �   2   =  	   H  G  P     I  F  H  A  !   J  �   *   =  	   K  J  �     L  I  K  �     M  D  L  A  !   N  �   *   =  	   O  N  A  !   P  �   2   =  	   Q  P  P     R  O  Q  A  !   S  �   2   =  	   T  S  �     U  R  T  �     V  M  U  A  W  X  �   K   >  X  V       �      =     Y  �   �  Y  8       �     6               �          �      =  $   a  F   �  $   c  a  b  =  $   d  F   �  $   e  d  K   A  N   f  `  H   c  e  =     g  f  �  g  8         ��KT��D	(�D��^���D_w���       ,�'��       �]����M�_`Wv�                                                    v���L���