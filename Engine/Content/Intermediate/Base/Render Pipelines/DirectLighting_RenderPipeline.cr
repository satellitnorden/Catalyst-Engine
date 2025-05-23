�nX/J14�O��:�DirectLighting_RenderPipeline                                    |      #     f                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\DirectLighting_Vertex.glsl   �   �     // OpModuleProcessed entry-point main
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

////////////
// COMMON //
////////////

/*
*	The distribution function.
*	This approximates the amount the surface's microfacets are aligned to the halfway vector,
*	influenced by the roughness of the surface; this is the primary function approximating the microfacets.
*	Trowbridge-Reitz GGX.
*/
float Distribution(float roughness, float microsurface_angle)
{
	float roughness_squared = pow(roughness, 4.0f);
	float microsurface_angle_squared = microsurface_angle * microsurface_angle;

	float nominator = roughness_squared;
	float denominator = microsurface_angle_squared * (roughness_squared - 1.0f) + 1.0f;
	denominator = PI * denominator * denominator;

	return nominator / denominator;
}

/*
*	The geometry function.
*	This describes the self-shadowing property of the microfacets.
*	When a surface is relatively rough, the surface's microfacets can overshadow other microfacets reducing the light the surface reflects.
*	Schlick-GGX.
*/
float Geometry(vec3 normal, vec3 outgoing_direction, vec3 radiance_direction, float roughness)
{
	//Calculate the outgoing direction coefficient.
	float outgoing_direction_coefficient = max(dot(normal, outgoing_direction), 0.0f);

	//Calculate the irradiance direction coefficient.
	float irradiance_direction_coefficient = max(dot(normal, -radiance_direction), 0.0f);

	//Calculate the roughness coefficient.
	float roughness_coefficient = roughness + 1.0f;
	roughness_coefficient = (roughness_coefficient * roughness_coefficient) / 8.0f;

	//Calculate the first coefficient.
	float first_coefficient;

	{
		//Calculate the nominator.
		float nominator = outgoing_direction_coefficient;

		//Calculate the denominator.
		float denominator = outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

		first_coefficient = denominator > 0.0f ? nominator / denominator : 0.0f;
	}

	//Calculate the second coefficient.
	float second_coefficient;

	{
		//Calculate the nominator.
		float nominator = irradiance_direction_coefficient;

		//Calculate the denominator.
		float denominator = irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

		second_coefficient = denominator > 0.0f ? nominator / denominator : 0.0f;
	}

	//Calculate the geometry.
	return first_coefficient * second_coefficient;
}

/*
*	The fresnel function for direct lighting.
*	The Fresnel equation describes the ratio of surface reflection at different surface angles.
*/
vec3 Fresnel(vec3 surface_color, float difference_angle)
{
	//Calculate the fresnel.
	return surface_color + (vec3(1.0f) - surface_color) * pow(1.0f - difference_angle, 5.0f);
}

/*
*	The lambert diffuse function.
*/
vec3 LambertDiffuse(vec3 albedo)
{
	return albedo / PI;
}

/*
*	The disney diffuse function.
*	Inspired by: https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
*/
vec3 DisneyDiffuse(vec3 albedo, float roughness, float difference_angle, float radiance_angle, float outgoing_angle)
{
	//Calculate some stuff.
	float FD90 = 0.5f + 2.0f * roughness * pow(cos(difference_angle), 2.0f);

	//Set up terms.
	vec3 term_1 = albedo / PI;
	float term_2 = 1.0f + (FD90 - 1.0f) * pow(1.0f - cos(radiance_angle), 5.0f);
	float term_3 = 1.0f + (FD90 - 1.0f) * pow(1.0f - cos(outgoing_angle), 5.0f);

	return term_1 * term_2 * term_3;
}

////////////////////////////////////////////
// BIDIRECTIONAL REFLECTANCE DISTRIBUTION //
////////////////////////////////////////////
/*
*	Samples the bidirectional reflectance distribution with the given parameters.
*	
*	Parameters;
*
*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
*	- albedo: The albedo of the surface point being shaded.
*	- normal: The normal of the surface point being shaded.
*	- roughness: The roughness of the surface point being shaded.
*	- metallic: The metallic of the surface point being shaded.
*	- thickness: The thickness of the surface point being shaded.
*	- radiance_direction: A direction vector going from the entity emitting irradiance toward the surface point being shaded.
*/
vec3 BidirectionalReflectanceDistribution
(
	vec3 outgoing_direction,
	vec3 albedo,
	vec3 normal,
	float roughness,
	float metallic,
	float thickness,
	vec3 radiance_direction
)
{
	//Calculate the microsurface normal.
	vec3 microsurface_normal = normalize(outgoing_direction + -radiance_direction);

	//Calculate the surface color.
	vec3 surface_color = mix(vec3(0.04f), albedo, metallic);

	//Calculate the angle values.
	float outgoing_angle = max(dot(normal, outgoing_direction), 0.0f);
	float radiance_angle = max(dot(normal, -radiance_direction), 0.0f);
	float microsurface_angle = max(dot(normal, microsurface_normal), 0.0f);
	float difference_angle = max(dot(-radiance_direction, microsurface_normal), 0.0f);

	//Calculate the normal distribution.
	float distribution = Distribution(roughness, microsurface_angle);

	//Calculate the geometry.
	float geometry = Geometry(normal, outgoing_direction, radiance_direction, roughness);

	//Calculate the fresnel.
	vec3 fresnel = Fresnel(surface_color, difference_angle);

	//Calculate the diffuse component.
	vec3 diffuse_component;

	{
		diffuse_component = DisneyDiffuse(albedo, roughness, difference_angle, radiance_angle, outgoing_angle);

		diffuse_component *= (1.0f - fresnel) * (1.0f - metallic);
	}

	//Calculate the specular component.
	vec3 specular_component;

	{
		vec3 nominator = vec3(distribution) * vec3(geometry) * fresnel;
		float denominator = 4.0f * outgoing_angle * radiance_angle;

		specular_component = denominator > 0.0f ? nominator / denominator : vec3(0.0f);
	}

	//Calculate the weakening factor.
	float weakening_factor = dot(normal, -radiance_direction);
	weakening_factor = mix(clamp(weakening_factor * 0.5f + 0.5f, 0.0f, 1.0f), max(weakening_factor, 0.0f), thickness);

	return (diffuse_component + specular_component) * weakening_factor;
}

///////////////////////
// INDIRECT LIGHTING //
///////////////////////

/*
*	The geometry function for indirect lighting.
*	This describes the self-shadowing property of the microfacets.
*	When a surface is relatively rough, the surface's microfacets can overshadow other microfacets reducing the light the surface reflects.
*	Schlick-GGX.
*/
float GeometryIndirect(float roughness, float outgoing_angle)
{
	//Calculate the roughness coefficient.
	float roughness_coefficient = (roughness * roughness) * 0.5f;

	//Calculate the coefficient.
	float coefficient;

	{
		//Calculate the nominator.
		float nominator = outgoing_angle;

		//Calculate the denominator.
		float denominator = outgoing_angle * (1.0f - roughness_coefficient) + roughness_coefficient;

		coefficient = denominator > 0.0f ? nominator / denominator : 0.0f;
	}

	//Calculate the geometry.
	return coefficient;
}

/*
*	The fresnel function for indirect lighting.
*	The Fresnel equation describes the ratio of surface reflection at different surface angles.
*/
vec3 FresnelIndirect(vec3 surface_color, float outgoing_angle)
{
	//Calculate the fresnel.
	return surface_color + (vec3(1.0f) - surface_color) * pow(1.0f - outgoing_angle, 5.0f);
}

/*
*	Calculates inddirect lighting. Returns the radiance transmitted from the surface in the outgoing direction.
*
*	Arguments;
*	
*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
*	- albedo: The albedo of the surface point being shaded.
*	- normal: The normal of the surface point being shaded.
*	- roughness: The roughness of the surface point being shaded.
*	- metallic: The metallic of the surface point being shaded.
*	- ambient_occlusion: The ambient occlusion of the surface point being shaded.
*	- thickness: The thickness of the surface point being shaded.
*	- radiance: The incoming irradiance towards the surface point being shaded.
*/
vec3 CalculateIndirectLighting
(
	vec3 outgoing_direction,
	vec3 albedo,
	vec3 normal,
	float roughness,
	float metallic,
	float ambient_occlusion,
	float thickness,
	vec3 diffuse_irradiance,
	vec3 specular_irradiance
)
{
	//Calculate the surface color.
	vec3 surface_color = mix(vec3(0.04f), albedo, metallic);

	//Calculate the angle values.
	float outgoing_angle = max(dot(normal, outgoing_direction), 0.0f);

	//Calculate the geometry.
	float geometry = GeometryIndirect(roughness, outgoing_angle);

	//Calculate the fresnel.
	vec3 fresnel = FresnelIndirect(surface_color, outgoing_angle);

	//Calculate the diffuse component.
	vec3 diffuse_component;

	{
		diffuse_component = (vec3(1.0f) - fresnel) * (1.0f - metallic) * albedo;
	}

	//Calculate the specular component.
	vec3 specular_component;

	{
		vec3 nominator = vec3(geometry) * fresnel;
		float denominator = 4.0f * outgoing_angle;

		specular_component = denominator > 0.0f ? nominator / denominator : vec3(0.0f);
	}

	return (diffuse_component * diffuse_irradiance * ambient_occlusion) + (specular_component * specular_irradiance * ambient_occlusion);
}

layout (set = 1, binding = 4) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 6) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 7) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 8) uniform sampler2D INTERMEDIATE_RGBA_FLOAT32_HALF_1;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   R   Camera   	 R       WORLD_TO_CLIP_MATRIX     	 R      WORLD_TO_CAMERA_MATRIX    R      PREVIOUS_WORLD_TO_CLIP_MATRIX     R      INVERSE_WORLD_TO_CAMERA_MATRIX    R      INVERSE_CAMERA_TO_CLIP_MATRIX    	 R      CAMERA_WORLD_POSITION    	 R      CAMERA_FORWARD_VECTOR    	 R      CURRENT_FRAME_JITTER      R      NEAR_PLANE    R   	   FAR_PLANE     T         U   General  	 U       FULL_MAIN_RESOLUTION      U      INVERSE_FULL_MAIN_RESOLUTION     	 U      HALF_MAIN_RESOLUTION      U      INVERSE_HALF_MAIN_RESOLUTION      U      FRAME     U      VIEW_DISTANCE     W         X   Wind      X       UPPER_SKY_COLOR   X      LOWER_SKY_COLOR   X      SKY_MODE      X      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     Z         [   LightingHeader    [       _NumberOfLights   [      _MaximumNumberOfShadowCastingLights   ]   Lighting      ]       LIGHTING_HEADER   ]      LIGHT_DATA    _         a   SceneFeatures1    b   SceneFeatures2    c   SceneFeatures3    d   SceneFeatures2Half    e   INTERMEDIATE_RGBA_FLOAT32_HALF_1    G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  R          H  R       #       H  R             H  R         H  R      #   @   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #      H  R            H  R      #   @  H  R      #   P  H  R      #   `  H  R      #   h  H  R   	   #   l  G  R      G  T   "      G  T   !       H  U       #       H  U      #      H  U      #      H  U      #      H  U      #       H  U      #   $   G  U      G  W   "      G  W   !      H  X       #       H  X      #      H  X      #       H  X      #   $   G  X      G  Z   "      G  Z   !      H  [       #       H  [      #      G  \         H  ]       #       H  ]      #      G  ]      G  _   "      G  _   !      G  a   "      G  a   !      G  b   "      G  b   !      G  c   "      G  c   !      G  d   "      G  d   !      G  e   "      G  e   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P   ,        Q           R   P   P   P   P   P   Q   Q               S      R   ;  S   T        U               #         V      U   ;  V   W        X   Q   Q   #         Y      X   ;  Y   Z        [   #   #     \   ,     ]   [   \      ^      ]   ;  ^   _         `       G   ;  `   a       ;  `   b       ;  `   c       ;  `   d       ;  `   e            3     6               �     ;     	      ;                5      =           �              �              o           �        
      >  	           6      =           �              �              o           �        
      >             7      =        	   �               �     "       !   A  %   &      $   >  &   "        8      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        9      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  ��      #     c                GLSL.std.450                     main    �  D               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\DirectLighting_Fragment.glsl     �   �     // OpModuleProcessed entry-point main
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

////////////
// COMMON //
////////////

/*
*	The distribution function.
*	This approximates the amount the surface's microfacets are aligned to the halfway vector,
*	influenced by the roughness of the surface; this is the primary function approximating the microfacets.
*	Trowbridge-Reitz GGX.
*/
float Distribution(float roughness, float microsurface_angle)
{
	float roughness_squared = pow(roughness, 4.0f);
	float microsurface_angle_squared = microsurface_angle * microsurface_angle;

	float nominator = roughness_squared;
	float denominator = microsurface_angle_squared * (roughness_squared - 1.0f) + 1.0f;
	denominator = PI * denominator * denominator;

	return nominator / denominator;
}

/*
*	The geometry function.
*	This describes the self-shadowing property of the microfacets.
*	When a surface is relatively rough, the surface's microfacets can overshadow other microfacets reducing the light the surface reflects.
*	Schlick-GGX.
*/
float Geometry(vec3 normal, vec3 outgoing_direction, vec3 radiance_direction, float roughness)
{
	//Calculate the outgoing direction coefficient.
	float outgoing_direction_coefficient = max(dot(normal, outgoing_direction), 0.0f);

	//Calculate the irradiance direction coefficient.
	float irradiance_direction_coefficient = max(dot(normal, -radiance_direction), 0.0f);

	//Calculate the roughness coefficient.
	float roughness_coefficient = roughness + 1.0f;
	roughness_coefficient = (roughness_coefficient * roughness_coefficient) / 8.0f;

	//Calculate the first coefficient.
	float first_coefficient;

	{
		//Calculate the nominator.
		float nominator = outgoing_direction_coefficient;

		//Calculate the denominator.
		float denominator = outgoing_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

		first_coefficient = denominator > 0.0f ? nominator / denominator : 0.0f;
	}

	//Calculate the second coefficient.
	float second_coefficient;

	{
		//Calculate the nominator.
		float nominator = irradiance_direction_coefficient;

		//Calculate the denominator.
		float denominator = irradiance_direction_coefficient * (1.0f - roughness_coefficient) + roughness_coefficient;

		second_coefficient = denominator > 0.0f ? nominator / denominator : 0.0f;
	}

	//Calculate the geometry.
	return first_coefficient * second_coefficient;
}

/*
*	The fresnel function for direct lighting.
*	The Fresnel equation describes the ratio of surface reflection at different surface angles.
*/
vec3 Fresnel(vec3 surface_color, float difference_angle)
{
	//Calculate the fresnel.
	return surface_color + (vec3(1.0f) - surface_color) * pow(1.0f - difference_angle, 5.0f);
}

/*
*	The lambert diffuse function.
*/
vec3 LambertDiffuse(vec3 albedo)
{
	return albedo / PI;
}

/*
*	The disney diffuse function.
*	Inspired by: https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
*/
vec3 DisneyDiffuse(vec3 albedo, float roughness, float difference_angle, float radiance_angle, float outgoing_angle)
{
	//Calculate some stuff.
	float FD90 = 0.5f + 2.0f * roughness * pow(cos(difference_angle), 2.0f);

	//Set up terms.
	vec3 term_1 = albedo / PI;
	float term_2 = 1.0f + (FD90 - 1.0f) * pow(1.0f - cos(radiance_angle), 5.0f);
	float term_3 = 1.0f + (FD90 - 1.0f) * pow(1.0f - cos(outgoing_angle), 5.0f);

	return term_1 * term_2 * term_3;
}

////////////////////////////////////////////
// BIDIRECTIONAL REFLECTANCE DISTRIBUTION //
////////////////////////////////////////////
/*
*	Samples the bidirectional reflectance distribution with the given parameters.
*	
*	Parameters;
*
*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
*	- albedo: The albedo of the surface point being shaded.
*	- normal: The normal of the surface point being shaded.
*	- roughness: The roughness of the surface point being shaded.
*	- metallic: The metallic of the surface point being shaded.
*	- thickness: The thickness of the surface point being shaded.
*	- radiance_direction: A direction vector going from the entity emitting irradiance toward the surface point being shaded.
*/
vec3 BidirectionalReflectanceDistribution
(
	vec3 outgoing_direction,
	vec3 albedo,
	vec3 normal,
	float roughness,
	float metallic,
	float thickness,
	vec3 radiance_direction
)
{
	//Calculate the microsurface normal.
	vec3 microsurface_normal = normalize(outgoing_direction + -radiance_direction);

	//Calculate the surface color.
	vec3 surface_color = mix(vec3(0.04f), albedo, metallic);

	//Calculate the angle values.
	float outgoing_angle = max(dot(normal, outgoing_direction), 0.0f);
	float radiance_angle = max(dot(normal, -radiance_direction), 0.0f);
	float microsurface_angle = max(dot(normal, microsurface_normal), 0.0f);
	float difference_angle = max(dot(-radiance_direction, microsurface_normal), 0.0f);

	//Calculate the normal distribution.
	float distribution = Distribution(roughness, microsurface_angle);

	//Calculate the geometry.
	float geometry = Geometry(normal, outgoing_direction, radiance_direction, roughness);

	//Calculate the fresnel.
	vec3 fresnel = Fresnel(surface_color, difference_angle);

	//Calculate the diffuse component.
	vec3 diffuse_component;

	{
		diffuse_component = DisneyDiffuse(albedo, roughness, difference_angle, radiance_angle, outgoing_angle);

		diffuse_component *= (1.0f - fresnel) * (1.0f - metallic);
	}

	//Calculate the specular component.
	vec3 specular_component;

	{
		vec3 nominator = vec3(distribution) * vec3(geometry) * fresnel;
		float denominator = 4.0f * outgoing_angle * radiance_angle;

		specular_component = denominator > 0.0f ? nominator / denominator : vec3(0.0f);
	}

	//Calculate the weakening factor.
	float weakening_factor = dot(normal, -radiance_direction);
	weakening_factor = mix(clamp(weakening_factor * 0.5f + 0.5f, 0.0f, 1.0f), max(weakening_factor, 0.0f), thickness);

	return (diffuse_component + specular_component) * weakening_factor;
}

///////////////////////
// INDIRECT LIGHTING //
///////////////////////

/*
*	The geometry function for indirect lighting.
*	This describes the self-shadowing property of the microfacets.
*	When a surface is relatively rough, the surface's microfacets can overshadow other microfacets reducing the light the surface reflects.
*	Schlick-GGX.
*/
float GeometryIndirect(float roughness, float outgoing_angle)
{
	//Calculate the roughness coefficient.
	float roughness_coefficient = (roughness * roughness) * 0.5f;

	//Calculate the coefficient.
	float coefficient;

	{
		//Calculate the nominator.
		float nominator = outgoing_angle;

		//Calculate the denominator.
		float denominator = outgoing_angle * (1.0f - roughness_coefficient) + roughness_coefficient;

		coefficient = denominator > 0.0f ? nominator / denominator : 0.0f;
	}

	//Calculate the geometry.
	return coefficient;
}

/*
*	The fresnel function for indirect lighting.
*	The Fresnel equation describes the ratio of surface reflection at different surface angles.
*/
vec3 FresnelIndirect(vec3 surface_color, float outgoing_angle)
{
	//Calculate the fresnel.
	return surface_color + (vec3(1.0f) - surface_color) * pow(1.0f - outgoing_angle, 5.0f);
}

/*
*	Calculates inddirect lighting. Returns the radiance transmitted from the surface in the outgoing direction.
*
*	Arguments;
*	
*	- outgoing_direction: A direction vector from the point on the surface being shaded in the outgoing direction.
*	- albedo: The albedo of the surface point being shaded.
*	- normal: The normal of the surface point being shaded.
*	- roughness: The roughness of the surface point being shaded.
*	- metallic: The metallic of the surface point being shaded.
*	- ambient_occlusion: The ambient occlusion of the surface point being shaded.
*	- thickness: The thickness of the surface point being shaded.
*	- radiance: The incoming irradiance towards the surface point being shaded.
*/
vec3 CalculateIndirectLighting
(
	vec3 outgoing_direction,
	vec3 albedo,
	vec3 normal,
	float roughness,
	float metallic,
	float ambient_occlusion,
	float thickness,
	vec3 diffuse_irradiance,
	vec3 specular_irradiance
)
{
	//Calculate the surface color.
	vec3 surface_color = mix(vec3(0.04f), albedo, metallic);

	//Calculate the angle values.
	float outgoing_angle = max(dot(normal, outgoing_direction), 0.0f);

	//Calculate the geometry.
	float geometry = GeometryIndirect(roughness, outgoing_angle);

	//Calculate the fresnel.
	vec3 fresnel = FresnelIndirect(surface_color, outgoing_angle);

	//Calculate the diffuse component.
	vec3 diffuse_component;

	{
		diffuse_component = (vec3(1.0f) - fresnel) * (1.0f - metallic) * albedo;
	}

	//Calculate the specular component.
	vec3 specular_component;

	{
		vec3 nominator = vec3(geometry) * fresnel;
		float denominator = 4.0f * outgoing_angle;

		specular_component = denominator > 0.0f ? nominator / denominator : vec3(0.0f);
	}

	return (diffuse_component * diffuse_irradiance * ambient_occlusion) + (specular_component * specular_irradiance * ambient_occlusion);
}

layout (set = 1, binding = 4) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 6) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 7) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 8) uniform sampler2D INTERMEDIATE_RGBA_FLOAT32_HALF_1;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 Scene;

void main()
{
    vec4 scene_features_1 = texture(SceneFeatures1, InScreenCoordinate);
    vec4 scene_features_2 = texture(SceneFeatures2, InScreenCoordinate);
    vec4 scene_features_3 = texture(SceneFeatures3, InScreenCoordinate);
    vec3 albedo = scene_features_1.rgb;
    float thickness = scene_features_1.w;
    vec3 normal = scene_features_2.xyz;
    float depth = scene_features_2.w;
    float roughness = scene_features_3.x;
    float metallic = scene_features_3.y;
    float ambient_occlusion = scene_features_3.z;
    float emissive = scene_features_3.w;
    vec3 world_position = CalculateWorldPosition(InScreenCoordinate, depth);
    vec3 view_direction = normalize(CAMERA_WORLD_POSITION - world_position);
    float linearized_depth = LinearizeDepth(depth);
    vec4 shadows;
    {
#if 0
        shadows = texture(INTERMEDIATE_RGBA_FLOAT32_HALF_1, InScreenCoordinate);
#else
        vec2 sample_coordinate_1 = InScreenCoordinate;
        vec2 sample_coordinate_2 = InScreenCoordinate + vec2(0.0f, 1.0f) * INVERSE_HALF_MAIN_RESOLUTION;
        vec2 sample_coordinate_3 = InScreenCoordinate + vec2(1.0f, 0.0f) * INVERSE_HALF_MAIN_RESOLUTION;
        vec2 sample_coordinate_4 = InScreenCoordinate + vec2(1.0f, 1.0f) * INVERSE_HALF_MAIN_RESOLUTION;
        vec4 shadows_1 = texture(INTERMEDIATE_RGBA_FLOAT32_HALF_1, sample_coordinate_1);
        vec4 shadows_2 = texture(INTERMEDIATE_RGBA_FLOAT32_HALF_1, sample_coordinate_2);
        vec4 shadows_3 = texture(INTERMEDIATE_RGBA_FLOAT32_HALF_1, sample_coordinate_3);
        vec4 shadows_4 = texture(INTERMEDIATE_RGBA_FLOAT32_HALF_1, sample_coordinate_4);
        float linearized_depth_1 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_1).w);
        float linearized_depth_2 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_2).w);
        float linearized_depth_3 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_3).w);
        float linearized_depth_4 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_4).w);
        float horizontal_weight = fract(InScreenCoordinate.x * HALF_MAIN_RESOLUTION.x);
        float vertical_weight = fract(InScreenCoordinate.y * HALF_MAIN_RESOLUTION.y);
        float weight_1 = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
	    float weight_2 = (1.0f - horizontal_weight) * vertical_weight;
	    float weight_3 = horizontal_weight * (1.0f - vertical_weight);
	    float weight_4 = horizontal_weight * vertical_weight;
        weight_1 = max(weight_1 * exp(-abs(linearized_depth - linearized_depth_1)), FLOAT32_EPSILON * 1.0f);
        weight_2 = max(weight_2 * exp(-abs(linearized_depth - linearized_depth_2)), FLOAT32_EPSILON * 1.0f);
        weight_3 = max(weight_3 * exp(-abs(linearized_depth - linearized_depth_3)), FLOAT32_EPSILON * 1.0f);
        weight_4 = max(weight_4 * exp(-abs(linearized_depth - linearized_depth_4)), FLOAT32_EPSILON * 1.0f);
        float total_weight_reciprocal = 1.0f / (weight_1 + weight_2 + weight_3 + weight_4);
	    weight_1 *= total_weight_reciprocal;
	    weight_2 *= total_weight_reciprocal;
	    weight_3 *= total_weight_reciprocal;
        weight_4 *= total_weight_reciprocal;
        shadows =   shadows_1 * weight_1
                    + shadows_2 * weight_2
                    + shadows_3 * weight_3
                    + shadows_4 * weight_4;
#endif
        /*
        for (uint i = 0; i < 4; ++i)
        {
            float noise_weight = InverseSquare(1.0f - (abs(shadows[i] - 0.5f) * 2.0f));
            shadows[i] = mix(shadows[i], InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME), noise_weight);
        }
        */
    }
    vec3 lighting = vec3(0.0f);
    for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
    {
	    Light light = UnpackLight(i);
        vec3 light_radiance = light._Color * light._Intensity;
        switch (light._LightType)
        {
            case LIGHT_TYPE_DIRECTIONAL:
            {
                float shadow_factor = shadows[0];
                lighting += BidirectionalReflectanceDistribution
                (
                    view_direction,
                    albedo,
                    normal,
                    roughness,
                    metallic,
                    thickness,
                    light._TransformData1
                ) * light_radiance * shadow_factor * mix(0.125f, 1.0f, ambient_occlusion);
                break;
            }
            case LIGHT_TYPE_POINT:
            {
                float distance_from_light = length(world_position - light._TransformData1);
                vec3 direction_from_light = (world_position - light._TransformData1) * (1.0f / distance_from_light);
                float attenuation = distance_from_light > 0.0f ? 1.0f / distance_from_light : 1.0f;
                lighting += BidirectionalReflectanceDistribution
                (
                    view_direction,
                    albedo,
                    normal,
                    roughness,
                    metallic,
                    thickness,
                    direction_from_light
                ) * light_radiance * attenuation * mix(0.5f, 1.0f, ambient_occlusion);
                break;
            }
        }
    }
	Scene = vec4(lighting,1.0f);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         LinearizeDepth(f1;    
   depth    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index        Distribution(f1;f1;      roughness        microsurface_angle   	 '   Geometry(vf3;vf3;vf3;f1;      #   normal    $   outgoing_direction    %   radiance_direction    &   roughness     ,   Fresnel(vf3;f1;   *   surface_color     +   difference_angle     
 4   DisneyDiffuse(vf3;f1;f1;f1;f1;    /   albedo    0   roughness     1   difference_angle      2   radiance_angle    3   outgoing_angle    >   BidirectionalReflectanceDistribution(vf3;vf3;vf3;f1;f1;f1;vf3;    7   outgoing_direction    8   albedo    9   normal    :   roughness     ;   metallic      <   thickness     =   radiance_direction    B   Camera   	 B       WORLD_TO_CLIP_MATRIX     	 B      WORLD_TO_CAMERA_MATRIX    B      PREVIOUS_WORLD_TO_CLIP_MATRIX     B      INVERSE_WORLD_TO_CAMERA_MATRIX    B      INVERSE_CAMERA_TO_CLIP_MATRIX    	 B      CAMERA_WORLD_POSITION    	 B      CAMERA_FORWARD_VECTOR    	 B      CURRENT_FRAME_JITTER      B      NEAR_PLANE    B   	   FAR_PLANE     D         [   near_plane_coordinate     c   view_space_position   r   inverse_view_space_position_denominator   z   world_space_position      �   light_data_1      �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   light_data_2      �   light_data_3      �   light_data_4      �   light     �   roughness_squared    	 �   microsurface_angle_squared    �   nominator     �   denominator  
 �   outgoing_direction_coefficient    �   irradiance_direction_coefficient         roughness_coefficient       nominator     
  denominator     first_coefficient       nominator       denominator   &  second_coefficient    B  FD90      K  term_1    O  term_2    X  term_3    h  microsurface_normal   n  surface_color     u  outgoing_angle    z  radiance_angle    �  microsurface_angle    �  difference_angle      �  distribution      �  param     �  param     �  geometry      �  param     �  param     �  param     �  param     �  fresnel   �  param     �  param     �  diffuse_component     �  param     �  param     �  param     �  param     �  param     �  nominator     �  denominator   �  specular_component    �  weakening_factor      �  scene_features_1      �  SceneFeatures1    �  InScreenCoordinate    �  scene_features_2      �  SceneFeatures2    �  scene_features_3      �  SceneFeatures3    �  albedo    �  thickness     �  normal       depth       roughness       metallic      	  ambient_occlusion       emissive        world_position      param       param       view_direction      linearized_depth        param        sample_coordinate_1   "  sample_coordinate_2   %  General  	 %      FULL_MAIN_RESOLUTION      %     INVERSE_FULL_MAIN_RESOLUTION     	 %     HALF_MAIN_RESOLUTION      %     INVERSE_HALF_MAIN_RESOLUTION      %     FRAME     %     VIEW_DISTANCE     '        -  sample_coordinate_3   4  sample_coordinate_4   ;  shadows_1     <  INTERMEDIATE_RGBA_FLOAT32_HALF_1      @  shadows_2     D  shadows_3     H  shadows_4     L  linearized_depth_1    M  SceneFeatures2Half    Q  param     T  linearized_depth_2    X  param     [  linearized_depth_3    _  param     b  linearized_depth_4    f  param     i  horizontal_weight     q  vertical_weight   x  weight_1      ~  weight_2      �  weight_3      �  weight_4      �  total_weight_reciprocal   �  shadows   �  lighting      �  i     �  light     �  param     �  light_radiance    �  shadow_factor     �  param     �  param     �  param     �  param     �  param     �  param        param       distance_from_light     direction_from_light        attenuation   '  param     )  param     +  param     -  param     /  param     1  param     3  param     D  Scene     M  TEXTURES     	 O  GlobalTextureAverageValues   	 O      TEXTURE_AVERAGE_VALUES    Q        R  Material      R      _Properties   R     _AlbedoThickness     	 R     _NormalMapDisplacement    R     _MaterialProperties   R     _Opacity      R     _EmissiveMultiplier   R     _Padding1     R     _Padding2     U  GlobalMaterials   U      MATERIALS     W        [  BLUE_NOISE_TEXTURES   _  SKY_TEXTURE   `  Wind      `      UPPER_SKY_COLOR   `     LOWER_SKY_COLOR   `     SKY_MODE      `     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     b      H  B          H  B       #       H  B             H  B         H  B      #   @   H  B            H  B         H  B      #   �   H  B            H  B         H  B      #   �   H  B            H  B         H  B      #      H  B            H  B      #   @  H  B      #   P  H  B      #   `  H  B      #   h  H  B   	   #   l  G  B      G  D   "      G  D   !       H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      G  �  "      G  �  !      G  �         G  �  "      G  �  !      G  �  "      G  �  !      H  %      #       H  %     #      H  %     #      H  %     #      H  %     #       H  %     #   $   G  %     G  '  "      G  '  !      G  <  "      G  <  !      G  M  "      G  M  !      G  D         G  M  "       G  M  !       G  N        H  O      #       G  O     G  Q  "       G  Q  !      H  R      #       H  R     #      H  R     #      H  R     #      H  R     #      H  R     #      H  R     #      H  R     #      G  T         H  U      #       G  U     G  W  "       G  W  !      G  [  "       G  [  !      G  _  "       G  _  !      H  `      #       H  `     #      H  `     #       H  `     #   $   G  `     G  b  "      G  b  !           !                             !  	                                           !                                        
                            !           !                 !         !  "      !   !   !      !  )      !      !  .      !               ! 
 6      !   !   !            !     @           A   @        B   A   A   A   A   A                     C      B   ;  C   D        E          +  E   F   	      G         +  E   J      +     ]      @+     _     �?   b      @   +  E   d         e      A   +     s      +  E   {        �           �   @     �   �   �      �      �   ;  �   �      +  E   �      +     �      +     �          �      @   +     �      +     �         �         +  E   �       +  E   �      +  E   �      +  E   �      +  E   �      +     �     �@+     �   �I@+     �       +          A    ,     7  _   _   _   +     <    �@+     C     ?+     o  
�#=,     p  o  o  o  ,     �  �   �   �    	 �                             �  �     �      �  ;  �  �         �        ;  �  �     ;  �  �      ;  �  �                 ,     $  �   _     %                       &     %  ;  &  '        (        ,     /  _   �   ,     6  _   _   ;  �  <      ;  �  M         j        +     �     4   �        +          >   C     @   ;  C  D     +     J       K  �  J     L      K  ;  L  M        N  @   J    O  N     P     O  ;  P  Q      
 R                          +     S       T  R  S    U  T     V     U  ;  V  W     +     X        Y  �  X     Z      Y  ;  Z  [       	 \                             ]  \     ^      ]  ;  ^  _        `                 a     `  ;  a  b          7     6               �     ;  b   �     ;  b   �     ;  b   �     ;  !   �     ;     �     ;  !   �     ;           ;          ;          ;     	     ;          ;  !        ;          ;          ;  !        ;          ;          ;           ;     "     ;     -     ;     4     ;  b   ;     ;  b   @     ;  b   D     ;  b   H     ;     L     ;     Q     ;     T     ;     X     ;     [     ;     _     ;     b     ;     f     ;     i     ;     q     ;     x     ;     ~     ;     �     ;     �     ;     �     ;  b   �     ;  !   �     ;     �     ;  �   �     ;     �     ;  !   �     ;     �     ;  !   �     ;  !   �     ;  !   �     ;     �     ;     �     ;     �     ;  !         ;          ;  !        ;          ;           ;  !   '     ;  !   )     ;  !   +     ;     -     ;     /     ;     1     ;  !   3          9      =  �  �  �  =     �  �  W  @   �  �  �  >  �  �       :      =  �  �  �  =     �  �  W  @   �  �  �  >  �  �       ;      =  �  �  �  =     �  �  W  @   �  �  �  >  �  �       <      =  @   �  �  O     �  �  �            >  �  �       =      A     �  �  s   =     �  �  >  �  �       >      =  @   �  �  O     �  �  �            >  �  �       ?      A       �  s   =         >            @      A       �  �   =         >           A      A       �  �   =         >           B      A     
  �  �   =       
  >  	         C      A       �  s   =         >           D      =       �  >      =          >      9              >           E      A      D   �   =         =         �                     E     >           F      =          >      9            >           L      =     !  �  >     !       M      =     #  �  A  (  )  '  {   =     *  )  �     +  $  *  �     ,  #  +  >  "  ,       N      =     .  �  A  (  0  '  {   =     1  0  �     2  /  1  �     3  .  2  >  -  3       O      =     5  �  A  (  7  '  {   =     8  7  �     9  6  8  �     :  5  9  >  4  :       P      =  �  =  <  =     >     W  @   ?  =  >  >  ;  ?       Q      =  �  A  <  =     B  "  W  @   C  A  B  >  @  C       R      =  �  E  <  =     F  -  W  @   G  E  F  >  D  G       S      =  �  I  <  =     J  4  W  @   K  I  J  >  H  K       T      =  �  N  M  =     O     W  @   P  N  O  Q     R  P     >  Q  R  9     S     Q  >  L  S       U      =  �  U  M  =     V  "  W  @   W  U  V  Q     Y  W     >  X  Y  9     Z     X  >  T  Z       V      =  �  \  M  =     ]  -  W  @   ^  \  ]  Q     `  ^     >  _  `  9     a     _  >  [  a       W      =  �  c  M  =     d  4  W  @   e  c  d  Q     g  e     >  f  g  9     h     f  >  b  h       X      A  j  k  �  �   =     l  k  A  G   m  '  �   �   =     n  m  �     o  l  n       p     
   o  >  i  p       Y      A  j  r  �  �   =     s  r  A  G   t  '  �   �   =     u  t  �     v  s  u       w     
   v  >  q  w       Z      =     y  i  �     z  _   y  =     {  q  �     |  _   {  �     }  z  |  >  x  }       [      =       i  �     �  _     =     �  q  �     �  �  �  >  ~  �       \      =     �  i  =     �  q  �     �  _   �  �     �  �  �  >  �  �       ]      =     �  i  =     �  q  �     �  �  �  >  �  �       ^      =     �  x  =     �    =     �  L  �     �  �  �       �        �       �  �       �        �  �     �  �  �       �     (   �  �  >  x  �       _      =     �  ~  =     �    =     �  T  �     �  �  �       �        �       �  �       �        �  �     �  �  �       �     (   �  �  >  ~  �       `      =     �  �  =     �    =     �  [  �     �  �  �       �        �       �  �       �        �  �     �  �  �       �     (   �  �  >  �  �       a      =     �  �  =     �    =     �  b  �     �  �  �       �        �       �  �       �        �  �     �  �  �       �     (   �  �  >  �  �       b      =     �  x  =     �  ~  �     �  �  �  =     �  �  �     �  �  �  =     �  �  �     �  �  �  �     �  _   �  >  �  �       c      =     �  �  =     �  x  �     �  �  �  >  x  �       d      =     �  �  =     �  ~  �     �  �  �  >  ~  �       e      =     �  �  =     �  �  �     �  �  �  >  �  �       f      =     �  �  =     �  �  �     �  �  �  >  �  �       g      =  @   �  ;  =     �  x  �  @   �  �  �       h      =  @   �  @  =     �  ~  �  @   �  �  �  �  @   �  �  �       i      =  @   �  D  =     �  �  �  @   �  �  �  �  @   �  �  �       j      =  @   �  H  =     �  �  �  @   �  �  �  �  @   �  �  �       g      >  �  �       t      >  �  �       u      >  �  �   �  �  �  �       u      �  �  �      �  �  �  �       u      =     �  �  A  �  �  �   �   �   =     �  �  �    �  �  �  �  �  �  �  �  �       w      =     �  �  >  �  �  9     �     �  >  �  �       x      A  !   �  �  �   =     �  �  A     �  �  �   =     �  �  �     �  �  �  >  �  �       y      A     �  �  {   =     �  �  �  �      �  �  �      �     �  �  �       }      A     �  �  �   =     �  �  >  �  �       �      =     �    >  �  �  =     �  �  >  �  �  =     �  �  >  �  �  =     �    >  �  �  =     �    >  �  �  =     �  �  >  �  �  A  !     �  �   =         >       9       >   �  �  �  �  �  �          �      =       �  �           =       �  �           =     	  	       
     .     _   	  �         
       ~      =       �  �           >  �         �      �  �  �  �       �      =         A  !     �  �   =         �                     B     >           �      =         A  !     �  �   =         �           =         �       _     �           >           �      =         �        �   �  "      �    !  %  �  !       �      =     #    �     $  _   #  >     $  �  "  �  %  >     _   �  "  �  "  =     &          �      >    &       �      =     (    >  '  (  =     *  �  >  )  *  =     ,  �  >  +  ,  =     .    >  -  .  =     0    >  /  0  =     2  �  >  1  2  =     4    >  3  4  9     5  >   '  )  +  -  /  1  3       �      =     6  �  �     7  5  6  =     8    �     9  7  8  =     :  	       ;     .   C  _   :  �     <  9  ;       �      =     =  �  �     >  =  <  >  �  >       �      �  �  �  �  �  �  �  �       u      =     A  �  �     B  A  �   >  �  B  �  �  �  �       �      =     E  �  Q     F  E      Q     G  E     Q     H  E     P  @   I  F  G  H  _   >  D  I  �  8       F  !   6            	   7     
   �          H      A  G   H   D   F   =     I   H   A  G   K   D   J   =     L   K   �     M   I   L   =     N   
   A  G   O   D   F   =     P   O   A  G   Q   D   J   =     R   Q   �     S   P   R   �     T   N   S   A  G   U   D   J   =     V   U   �     W   T   V   �     X   M   W   �  X   8       [  @   6               7        7        �     ;     [      ;  b   c      ;     r      ;  b   z           ]      =     \      �     ^   \   ]   P     `   _   _   �     a   ^   `   >  [   a        ^      A  e   f   D   d   =  A   g   f   =     h   [   =     i      Q     j   h       Q     k   h      P     l   j   k   i   Q     m   l       Q     n   l      Q     o   l      P  @   p   m   n   o   _   �  @   q   g   p   >  c   q        _      A     t   c   s   =     u   t   �     v   _   u   >  r   v        `      =     w   r   =  @   x   c   �  @   y   x   w   >  c   y        a      A  e   |   D   {   =  A   }   |   =  @   ~   c   �  @      }   ~   >  z           c      =  @   �   z   O     �   �   �             �  �   8       �     6               7        �     ;  b   �      ;  b   �      ;  b   �      ;  b   �      ;  �   �           �      =     �      �     �   �   �   �     �   �   �   A  �   �   �   �   �   =  @   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   �   A  �   �   �   �   �   =  @   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   �   A  �   �   �   �   �   =  @   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   s   A  �   �   �   �   �   =  @   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   �   =     �   �   A     �   �   �   =     �   �   P     �   �   �   �   A  !   �   �   �   >  �   �        �      A     �   �   s   =     �   �   A     �   �   �   =     �   �   A     �   �   �   =     �   �   P     �   �   �   �   A  !   �   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   s   =     �   �   A     �   �   �   =     �   �   P     �   �   �   �   A  !   �   �   �   >  �   �        �      A     �   �   �   =     �   �   |     �   �   A     �   �   {   >  �   �        �      A     �   �   �   =     �   �   |     �   �   A     �   �   d   >  �   �        �      A     �   �   s   =     �   �   A     �   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   �   >  �   �        �      =     �   �   �  �   8         =   6               7        7        �      ;     �      ;     �      ;     �      ;     �           !      =     �           �         �   �   >  �   �        "      =     �      =     �      �     �   �   �   >  �   �        $      =     �   �   >  �   �        %      =     �   �   =     �   �   �     �   �   _   �     �   �   �   �     �   �   _   >  �   �        &      =     �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �        (      =     �   �   =     �   �   �     �   �   �   �  �   8       1  ^   6     '       "   7  !   #   7  !   $   7  !   %   7     &   �  (   ;     �      ;     �      ;           ;          ;     
     ;          ;          ;          ;          ;     &     ;     )          4      =     �   #   =     �   $   �     �   �   �        �      (   �   �   >  �   �        7      =     �   #   =     �   %        �   �   �     �   �   �        �      (   �   �   >  �   �        :      =       &   �         _   >            ;      =          =          �           �           >            B      =     	  �   >    	       E      =       �   =          �       _     �           =          �           >  
         G      =       
  �        �   �        �        �         G      =         =       
  �           >      �    �    >    �   �    �    =              G      >           O      =       �   >           R      =        �   =     !     �     "  _   !  �     #     "  =     $     �     %  #  $  >    %       T      =     '    �    (  '  �   �  +      �  (  *  /  �  *       T      =     ,    =     -    �     .  ,  -  >  )  .  �  +  �  /  >  )  �   �  +  �  +  =     0  )       T      >  &  0       X      =     1    =     2  &  �     3  1  2  �  3  8       _  8   6     ,       )   7  !   *   7     +   �  -        b      =     6  *   =     8  *   �     9  7  8  =     :  +   �     ;  _   :       =        ;  <  �     >  9  =  �     ?  6  >  �  ?  8       q  t   6     4       .   7  !   /   7     0   7     1   7     2   7     3   �  5   ;     B     ;  !   K     ;     O     ;     X          t      =     D  0   �     E  ]   D  =     F  1        G        F       H        G  ]   �     I  E  H  �     J  C  I  >  B  J       w      =     L  /   P     M  �   �   �   �     N  L  M  >  K  N       x      =     P  B  �     Q  P  _   =     R  2        S        R  �     T  _   S       U        T  <  �     V  Q  U  �     W  _   V  >  O  W       y      =     Y  B  �     Z  Y  _   =     [  3        \        [  �     ]  _   \       ^        ]  <  �     _  Z  ^  �     `  _   _  >  X  `       {      =     a  K  =     b  O  �     c  a  b  =     d  X  �     e  c  d  �  e  8       �     6     >       6   7  !   7   7  !   8   7  !   9   7     :   7     ;   7     <   7  !   =   �  ?   ;  !   h     ;  !   n     ;     u     ;     z     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  !   �     ;  !   �     ;  !   �     ;     �     ;  !   �     ;  !   �     ;     �     ;  !   �     ;  !   �     ;     �     ;     �     ;     �     ;     �     ;  !   �     ;     �     ;  !   �     ;  !   �     ;     �          �      =     i  7   =     j  =        k  j  �     l  i  k       m     E   l  >  h  m       �      =     q  8   =     r  ;   P     s  r  r  r       t     .   p  q  s  >  n  t       �      =     v  9   =     w  7   �     x  v  w       y     (   x  �   >  u  y       �      =     {  9   =     |  =        }  |  �     ~  {  }            (   ~  �   >  z         �      =     �  9   =     �  h  �     �  �  �       �     (   �  �   >  �  �       �      =     �  =        �  �  =     �  h  �     �  �  �       �     (   �  �   >  �  �       �      =     �  :   >  �  �  =     �  �  >  �  �  9     �     �  �  >  �  �       �      =     �  9   >  �  �  =     �  7   >  �  �  =     �  =   >  �  �  =     �  :   >  �  �  9     �  '   �  �  �  �  >  �  �       �      =     �  n  >  �  �  =     �  �  >  �  �  9     �  ,   �  �  >  �  �       �      =     �  8   >  �  �  =     �  :   >  �  �  =     �  �  >  �  �  =     �  z  >  �  �  =     �  u  >  �  �  9 	    �  4   �  �  �  �  �  >  �  �       �      =     �  �  P     �  _   _   _   �     �  �  �  =     �  ;   �     �  _   �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      =     �  �  P     �  �  �  �  =     �  �  P     �  �  �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      =     �  u  �     �  �   �  =     �  z  �     �  �  �  >  �  �       �      =     �  �  �    �  �  �   �  �      �  �  �  �  �  �       �      =     �  �  =     �  �  P     �  �  �  �  �     �  �  �  >  �  �  �  �  �  �  >  �  �  �  �  �  �  =     �  �       �      >  �  �       �      =     �  9   =     �  =        �  �  �     �  �  �  >  �  �       �      =     �  �  �     �  �  C  �     �  �  C       �     +   �  �   _   =     �  �       �     (   �  �   =     �  <        �     .   �  �  �  >  �  �       �      =     �  �  =     �  �  �     �  �  �  =     �  �  �     �  �  �  �  �  8                          ��KT��D	(�D��^���D_w���       ,�'��               ��v)� Y  �%�9��     ��Y��G�     �]����M     ��%��]j	            N1�8��;                               \�5s����