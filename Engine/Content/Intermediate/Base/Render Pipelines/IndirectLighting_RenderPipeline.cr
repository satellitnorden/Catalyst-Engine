�nX/J1"�����IndirectLighting_RenderPipeline                                  �z      #     e                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\IndirectLighting_Vertex.glsl     ;   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform RenderingConfiguration
{
	layout (offset = 0) uint DIFFUSE_IRRADIANCE_MODE;
	layout (offset = 4) uint SPECULAR_IRRADIANCE_MODE;
	layout (offset = 8) uint VOLUMETRIC_SHADOWS_MODE;
};

layout (std140, set = 1, binding = 3) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
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

//Constants.
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

//Constants.
#define SKY_MODE_ATMOSPHERIC_SCATTERING (0)
#define SKY_MODE_GRADIENT (1)
#define SKY_MODE_TEXTURE (2)

/*
*	Samples the sky.
*	Requires the "World" uniform buffer to be bound.
*/
vec3 SampleSky(vec3 direction, float mip_level)
{
	//Here because ray tracing sometines passes in invalid stuff...
	if (isnan(direction.x) || isnan(direction.y) || isnan(direction.z))
	{
		return vec3(100.0f, 0.0f, 0.0f);
	}

	switch (SKY_MODE)
	{
		case SKY_MODE_ATMOSPHERIC_SCATTERING:
		{
			//Oh no!
			return vec3(0.0f);
		}

		case SKY_MODE_GRADIENT:
		{
			return mix(LOWER_SKY_COLOR, UPPER_SKY_COLOR, dot(direction, vec3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f);
		}

		case SKY_MODE_TEXTURE:
		{
			return textureLod(SKY_TEXTURE, direction, mip_level).rgb;
		}
	
		default:
		{
			//Oh no!
			return vec3(0.0f);
		}
	}
}

layout (set = 1, binding = 4) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 6) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 7) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 8) uniform sampler2D DiffuseIrradiance;
layout (set = 1, binding = 9) uniform sampler2D SpecularIrradiance;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   R   Camera   	 R       WORLD_TO_CLIP_MATRIX     	 R      WORLD_TO_CAMERA_MATRIX    R      PREVIOUS_WORLD_TO_CLIP_MATRIX     R      INVERSE_WORLD_TO_CAMERA_MATRIX    R      INVERSE_CAMERA_TO_CLIP_MATRIX    	 R      CAMERA_WORLD_POSITION    	 R      CAMERA_FORWARD_VECTOR    	 R      CURRENT_FRAME_JITTER      R      NEAR_PLANE    R   	   FAR_PLANE     T         U   General  	 U       FULL_MAIN_RESOLUTION      U      INVERSE_FULL_MAIN_RESOLUTION     	 U      HALF_MAIN_RESOLUTION      U      INVERSE_HALF_MAIN_RESOLUTION      U      FRAME     U      VIEW_DISTANCE     W         X   RenderingConfiguration   	 X       DIFFUSE_IRRADIANCE_MODE  
 X      SPECULAR_IRRADIANCE_MODE     	 X      VOLUMETRIC_SHADOWS_MODE   Z         [   Wind      [       UPPER_SKY_COLOR   [      LOWER_SKY_COLOR   [      SKY_MODE      [      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     ]         _   SceneFeatures1    `   SceneFeatures2    a   SceneFeatures3    b   SceneFeatures2Half    c   DiffuseIrradiance     d   SpecularIrradiance  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  R          H  R       #       H  R             H  R         H  R      #   @   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #      H  R            H  R      #   @  H  R      #   P  H  R      #   `  H  R      #   h  H  R   	   #   l  G  R      G  T   "      G  T   !       H  U       #       H  U      #      H  U      #      H  U      #      H  U      #       H  U      #   $   G  U      G  W   "      G  W   !      H  X       #       H  X      #      H  X      #      G  X      G  Z   "      G  Z   !      H  [       #       H  [      #      H  [      #       H  [      #   $   G  [      G  ]   "      G  ]   !      G  _   "      G  _   !      G  `   "      G  `   !      G  a   "      G  a   !      G  b   "      G  b   !      G  c   "      G  c   !      G  d   "      G  d   !   	        !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P   ,        Q           R   P   P   P   P   P   Q   Q               S      R   ;  S   T        U               #         V      U   ;  V   W        X   #   #   #      Y      X   ;  Y   Z        [   Q   Q   #         \      [   ;  \   ]         ^       G   ;  ^   _       ;  ^   `       ;  ^   a       ;  ^   b       ;  ^   c       ;  ^   d            ,     6               �     ;     	      ;                .      =           �              �              o           �        
      >  	           /      =           �              �              o           �        
      >             0      =        	   �               �     "       !   A  %   &      $   >  &   "        1      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        2      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  (�      #     �                GLSL.std.450                     main    U  S  �               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\IndirectLighting_Fragment.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform RenderingConfiguration
{
	layout (offset = 0) uint DIFFUSE_IRRADIANCE_MODE;
	layout (offset = 4) uint SPECULAR_IRRADIANCE_MODE;
	layout (offset = 8) uint VOLUMETRIC_SHADOWS_MODE;
};

layout (std140, set = 1, binding = 3) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
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

//Constants.
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

//Constants.
#define SKY_MODE_ATMOSPHERIC_SCATTERING (0)
#define SKY_MODE_GRADIENT (1)
#define SKY_MODE_TEXTURE (2)

/*
*	Samples the sky.
*	Requires the "World" uniform buffer to be bound.
*/
vec3 SampleSky(vec3 direction, float mip_level)
{
	//Here because ray tracing sometines passes in invalid stuff...
	if (isnan(direction.x) || isnan(direction.y) || isnan(direction.z))
	{
		return vec3(100.0f, 0.0f, 0.0f);
	}

	switch (SKY_MODE)
	{
		case SKY_MODE_ATMOSPHERIC_SCATTERING:
		{
			//Oh no!
			return vec3(0.0f);
		}

		case SKY_MODE_GRADIENT:
		{
			return mix(LOWER_SKY_COLOR, UPPER_SKY_COLOR, dot(direction, vec3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f);
		}

		case SKY_MODE_TEXTURE:
		{
			return textureLod(SKY_TEXTURE, direction, mip_level).rgb;
		}
	
		default:
		{
			//Oh no!
			return vec3(0.0f);
		}
	}
}

layout (set = 1, binding = 4) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 6) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 7) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 8) uniform sampler2D DiffuseIrradiance;
layout (set = 1, binding = 9) uniform sampler2D SpecularIrradiance;

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
    vec3 incoming_diffuse_irradiance = vec3(0.0f, 0.0f, 0.0f);
    switch (DIFFUSE_IRRADIANCE_MODE)
    {
        case DIFFUSE_IRRADIANCE_MODE_NONE:
        {
            incoming_diffuse_irradiance = SampleSky(normal, MAXIMUM_SKY_TEXTURE_MIP_LEVEL);
            break;
        }
        case DIFFUSE_IRRADIANCE_MODE_RAY_TRACED:
        {
            vec2 sample_coordinates[4];
            sample_coordinates[0] = InScreenCoordinate;
            sample_coordinates[1] = InScreenCoordinate + vec2(INVERSE_HALF_MAIN_RESOLUTION.x, 0.0f);
            sample_coordinates[2] = InScreenCoordinate + vec2(0.0f, INVERSE_HALF_MAIN_RESOLUTION.y);
            sample_coordinates[3] = InScreenCoordinate + vec2(INVERSE_HALF_MAIN_RESOLUTION.x, INVERSE_HALF_MAIN_RESOLUTION.y);
            vec3 diffuse_irradiance_samples[4];
            float depth_samples[4];
            for (uint i = 0; i < 4; ++i)
            {
                diffuse_irradiance_samples[i] = texture(DiffuseIrradiance, sample_coordinates[i]).rgb;
                depth_samples[i] = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinates[i]).w);
            }
            vec2 fractions = fract(InScreenCoordinate * HALF_MAIN_RESOLUTION);
            float weights[4];
            weights[0] = (1.0f - fractions.x) * (1.0f - fractions.y);
            weights[1] = fractions.x * (1.0f - fractions.y);
            weights[2] = (1.0f - fractions.x) * fractions.y;
            weights[3] = fractions.x * fractions.y;
            float center_depth = LinearizeDepth(depth);
            for (uint i = 0; i < 4; ++i)
            {
                weights[i] *= exp(-abs(center_depth - depth_samples[i]));
            }
            float weight_sum = weights[0] + weights[1] + weights[2] + weights[3];
            float inverse_weight_sum = weight_sum > 0.0f ? 1.0f / weight_sum : 1.0f;
            for (uint i = 0; i < 4; ++i)
            {
                weights[i] *= inverse_weight_sum;
            }
            for (uint i = 0; i < 4; ++i)
            {
                incoming_diffuse_irradiance += diffuse_irradiance_samples[i] * weights[i];
            }
            incoming_diffuse_irradiance *= mix(0.875f, 1.125f, InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME));
            break;
        }
    }
    vec3 incoming_specular_irradiance = vec3(0.0f, 0.0f, 0.0f);
    switch (SPECULAR_IRRADIANCE_MODE)
    {
        case SPECULAR_IRRADIANCE_MODE_NONE:
        {
            incoming_specular_irradiance = SampleSky(reflect(-view_direction, normal), roughness * MAXIMUM_SKY_TEXTURE_MIP_LEVEL);
            break;
        }
        case SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE:
        {
            vec4 specular_irradiance_sample = texture(SpecularIrradiance, InScreenCoordinate);
            incoming_specular_irradiance = specular_irradiance_sample.rgb;
            incoming_specular_irradiance *= mix(0.875f, 1.125f, InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME));
            vec3 sky_sample = SampleSky(reflect(-view_direction, normal), roughness * MAXIMUM_SKY_TEXTURE_MIP_LEVEL);
            incoming_specular_irradiance = mix(sky_sample, incoming_specular_irradiance, specular_irradiance_sample.a);
            break;
        }
        case SPECULAR_IRRADIANCE_MODE_RAY_TRACED:
        {
            vec4 specular_irradiance_sample = texture(SpecularIrradiance, InScreenCoordinate);
            incoming_specular_irradiance = specular_irradiance_sample.rgb;
            vec3 sky_sample = SampleSky(reflect(-view_direction, normal), roughness * MAXIMUM_SKY_TEXTURE_MIP_LEVEL);
            incoming_specular_irradiance = mix(sky_sample, incoming_specular_irradiance, specular_irradiance_sample.a);
            break;
        }
    }
    vec3 indirect_lighting = CalculateIndirectLighting
    (
        view_direction,
        albedo,
        normal,
        roughness,
        metallic,
        ambient_occlusion,
        thickness,
        incoming_diffuse_irradiance,
        incoming_specular_irradiance
    );
	Scene = vec4(indirect_lighting,1.0f);
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         LinearizeDepth(f1;    
   depth    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        InterleavedGradientNoise(vu2;u1;         coordinate       frame     !   GeometryIndirect(f1;f1;      roughness         outgoing_angle    '   FresnelIndirect(vf3;f1;   %   surface_color     &   outgoing_angle    3   CalculateIndirectLighting(vf3;vf3;vf3;f1;f1;f1;f1;vf3;vf3;    *   outgoing_direction    +   albedo    ,   normal    -   roughness     .   metallic      /   ambient_occlusion     0   thickness     1   diffuse_irradiance    2   specular_irradiance   7   SampleSky(vf3;f1;     5   direction     6   mip_level     ;   Camera   	 ;       WORLD_TO_CLIP_MATRIX     	 ;      WORLD_TO_CAMERA_MATRIX    ;      PREVIOUS_WORLD_TO_CLIP_MATRIX     ;      INVERSE_WORLD_TO_CAMERA_MATRIX    ;      INVERSE_CAMERA_TO_CLIP_MATRIX    	 ;      CAMERA_WORLD_POSITION    	 ;      CAMERA_FORWARD_VECTOR    	 ;      CURRENT_FRAME_JITTER      ;      NEAR_PLANE    ;   	   FAR_PLANE     =         T   near_plane_coordinate     \   view_space_position   k   inverse_view_space_position_denominator   s   world_space_position      �   x     �   y     �   roughness_coefficient     �   nominator     �   denominator   �   coefficient   �   surface_color     �   outgoing_angle    �   geometry      �   param     �   param     �   fresnel   �   param     �   param     �   diffuse_component     �   nominator     �   denominator   �   specular_component    %  Wind      %      UPPER_SKY_COLOR   %     LOWER_SKY_COLOR   %     SKY_MODE      %     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     '        D  SKY_TEXTURE   N  scene_features_1      R  SceneFeatures1    U  InScreenCoordinate    X  scene_features_2      Y  SceneFeatures2    ]  scene_features_3      ^  SceneFeatures3    b  albedo    e  thickness     h  normal    k  depth     n  roughness     q  metallic      t  ambient_occlusion     w  emissive      z  world_position    {  param     }  param     �  view_direction   	 �  incoming_diffuse_irradiance   �  RenderingConfiguration   	 �      DIFFUSE_IRRADIANCE_MODE  
 �     SPECULAR_IRRADIANCE_MODE     	 �     VOLUMETRIC_SHADOWS_MODE   �        �  param     �  param     �  sample_coordinates    �  General  	 �      FULL_MAIN_RESOLUTION      �     INVERSE_FULL_MAIN_RESOLUTION     	 �     HALF_MAIN_RESOLUTION      �     INVERSE_HALF_MAIN_RESOLUTION      �     FRAME     �     VIEW_DISTANCE     �        �  i    	 �  diffuse_irradiance_samples    �  DiffuseIrradiance     �  depth_samples     �  SceneFeatures2Half    �  param     �  fractions     �  weights   �  center_depth      �  param        i       weight_sum    #  inverse_weight_sum    -  i     =  i     S  gl_FragCoord      W  param     X  param    
 a  incoming_specular_irradiance      p  param     q  param    	 t  specular_irradiance_sample    u  SpecularIrradiance    ~  param       param     �  sky_sample    �  param     �  param    	 �  specular_irradiance_sample    �  sky_sample    �  param     �  param     �  indirect_lighting     �  param     �  param     �  param     �  param     �  param     �  param     �  param     �  param     �  param     �  Scene     �  TEXTURES     	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  BLUE_NOISE_TEXTURES H  ;          H  ;       #       H  ;             H  ;         H  ;      #   @   H  ;            H  ;         H  ;      #   �   H  ;            H  ;         H  ;      #   �   H  ;            H  ;         H  ;      #      H  ;            H  ;      #   @  H  ;      #   P  H  ;      #   `  H  ;      #   h  H  ;   	   #   l  G  ;      G  =   "      G  =   !       H  %      #       H  %     #      H  %     #       H  %     #   $   G  %     G  '  "      G  '  !      G  D  "       G  D  !      G  R  "      G  R  !      G  U         G  Y  "      G  Y  !      G  ^  "      G  ^  !      H  �      #       H  �     #      H  �     #      G  �     G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  S        G  u  "      G  u  !   	   G  �         G  �  "       G  �  !       G  �        H  �      #       G  �     G  �  "       G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !           !                             !  	                                           !                                                              !              !                 #         !  $      #      !  )      #   #   #               #   #     9           :   9        ;   :   :   :   :   :                     <      ;   ;  <   =        >          +  >   ?   	      @         +  >   C      +     V      @+     X     �?   [      9   +  >   ]         ^      :   +     l      +  >   t      +     ~   @   +     �       +     �   �Ҳ@+     �      +     �   ��SB+     �   Cq�=+     �   �E�;+     �      ?+     �         �   ,     �   X   X   X   +     �     �@+     �   
�#=,     �   �   �   �   +     �     �@,     �   �   �   �   +          +     "    �B,     #  "  �   �     %                 &     %  ;  &  '     +  >   (        )        +  >   2        3        +  >   6      ,     :  �   X   �    	 A                             B  A     C      B  ;  C  D       	 O                             P  O     Q      P  ;  Q  R         T        ;  T  U     ;  Q  Y      ;  Q  ^      +  >   �       �              �     �  ;  �  �     +     �       �     �     �     �    �                       �     �  ;  �  �       �     �     �     �  ;  Q  �        �     �     �     �  ;  Q  �         �        +     P    `?+     Q    �?   R     9   ;  R  S     ;  Q  u         �     9   ;  �  �     +     �       �  O  �     �      �  ;  �  �        �  9   �    �  �     �     �  ;  �  �      
 �                          +     �       �  �  �    �  �     �     �  ;  �  �     +     �        �  P  �     �      �  ;  �  �           0     6               �     ;  [   N     ;  [   X     ;  [   ]     ;  #   b     ;     e     ;  #   h     ;     k     ;     n     ;     q     ;     t     ;     w     ;  #   z     ;     {     ;     }     ;  #   �     ;  #   �     ;  #   �     ;     �     ;  �  �     ;     �     ;  �  �     ;  �  �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;           ;          ;     #     ;     &     ;     -     ;     =     ;     W     ;     X     ;  #   a     ;  #   p     ;     q     ;  [   t     ;     ~     ;          ;  #   �     ;  #   �     ;     �     ;  [   �     ;  #   �     ;  #   �     ;     �     ;  #   �     ;  #   �     ;  #   �     ;  #   �     ;     �     ;     �     ;     �     ;     �     ;  #   �     ;  #   �          2      =  P  S  R  =     V  U  W  9   W  S  V  >  N  W       3      =  P  Z  Y  =     [  U  W  9   \  Z  [  >  X  \       4      =  P  _  ^  =     `  U  W  9   a  _  `  >  ]  a       5      =  9   c  N  O     d  c  c            >  b  d       6      A     f  N  l   =     g  f  >  e  g       7      =  9   i  X  O     j  i  i            >  h  j       8      A     l  X  l   =     m  l  >  k  m       9      A     o  ]  �   =     p  o  >  n  p       :      A     r  ]  �   =     s  r  >  q  s       ;      A     u  ]    =     v  u  >  t  v       <      A     x  ]  l   =     y  x  >  w  y       =      =     |  U  >  {  |  =     ~  k  >  }  ~  9          {  }  >  z         >      A  3  �  =   �  =     �  �  =     �  z  �     �  �  �       �     E   �  >  �  �       ?      >  �  �        @      A  )  �  �  6  =     �  �  �  �      �  �  �      �     �  �  �       D      =     �  h  >  �  �  A  @   �  '  t   =     �  �  >  �  �  9     �  7   �  �  >  �  �       E      �  �  �  �       J      =     �  U  A     �  �  6  >  �  �       K      =     �  U  A  @   �  �  t   �   =     �  �  P     �  �  �   �     �  �  �  A     �  �  2  >  �  �       L      =     �  U  A  @   �  �  t   �   =     �  �  P     �  �   �  �     �  �  �  A     �  �  (  >  �  �       M      =     �  U  A  @   �  �  t   �   =     �  �  A  @   �  �  t   �   =     �  �  P     �  �  �  �     �  �  �  A     �  �  t   >  �  �       P      >  �  �   �  �  �  �       P      �  �  �      �  �  �  �       P      =     �  �  �  �   �  �  �  �  �  �  �  �  �       R      =     �  �  =  P  �  �  =     �  �  A     �  �  �  =     �  �  W  9   �  �  �  O     �  �  �            A  #   �  �  �  >  �  �       S      =     �  �  =  P  �  �  =     �  �  A     �  �  �  =     �  �  W  9   �  �  �  Q     �  �     >  �  �  9     �     �  A     �  �  �  >  �  �  �  �  �  �       P      =     �  �  �     �  �  2  >  �  �  �  �  �  �       U      =     �  U  A  �  �  �  (  =     �  �  �     �  �  �       �     
   �  >  �  �       W      A     �  �  �   =     �  �  �     �  X   �  A     �  �  �   =     �  �  �     �  X   �  �     �  �  �  A     �  �  6  >  �  �       X      A     �  �  �   =     �  �  A     �  �  �   =     �  �  �     �  X   �  �     �  �  �  A     �  �  2  >  �  �       Y      A     �  �  �   =     �  �  �     �  X   �  A     �  �  �   =     �  �  �     �  �  �  A     �  �  (  >  �  �       Z      A     �  �  �   =     �  �  A     �  �  �   =     �  �  �     �  �  �  A     �  �  t   >  �  �       [      =     �  k  >  �  �  9     �     �  >  �  �       \      >     �   �    �         \      �          �    �         \      =          �  �       �  �        �         ^      =          =     	  �  =     
     A       �  
  =         �       	                                           A       �    =         �           A       �    >      �    �         \      =          �         2  >       �    �         `      A       �  6  =         A       �  2  =         �           A       �  (  =         �           A        �  t   =     !     �     "    !  >    "       a      =     $    �  �   %  $  �   �  (      �  %  '  +  �  '       a      =     )    �     *  X   )  >  &  *  �  (  �  +  >  &  X   �  (  �  (  =     ,  &       a      >  #  ,       b      >  -  �   �  .  �  .       b      �  0  1      �  2  �  2       b      =     3  -  �  �   4  3  �  �  4  /  0  �  /       d      =     5  -  =     6  #  A     7  �  5  =     8  7  �     9  8  6  A     :  �  5  >  :  9  �  1  �  1       b      =     ;  -  �     <  ;  2  >  -  <  �  .  �  0       f      >  =  �   �  >  �  >       f      �  @  A      �  B  �  B       f      =     C  =  �  �   D  C  �  �  D  ?  @  �  ?       h      =     E  =  A  #   F  �  E  =     G  F  =     H  =  A     I  �  H  =     J  I  �     K  G  J  =     L  �  �     M  L  K  >  �  M  �  A  �  A       f      =     N  =  �     O  N  2  >  =  O  �  >  �  @       j      =  9   T  S  O     U  T  T         m     V  U  >  W  V  A  )  Y  �  ]   =     Z  Y  >  X  Z  9     [     W  X       \     .   P  Q  [  =     ]  �  �     ^  ]  \  >  �  ^       k      �  �  �  �       n      >  a  �        o      A  )  b  �  2  =     c  b  �  g      � 	 c  g      d     e     f  �  d       s      =     h  �       i  h  =     j  h       k     G   i  j  =     l  n  A  @   m  '  t   =     n  m  �     o  l  n  >  p  k  >  q  o  9     r  7   p  q  >  a  r       t      �  g  �  e       x      =  P  v  u  =     w  U  W  9   x  v  w  >  t  x       y      =  9   y  t  O     z  y  y            >  a  z       z      =  9   {  S  O     |  {  {         m     }  |  >  ~  }  A  )  �  �  ]   =     �  �  >    �  9     �     ~         �     .   P  Q  �  =     �  a  �     �  �  �  >  a  �       {      =     �  �       �  �  =     �  h       �     G   �  �  =     �  n  A  @   �  '  t   =     �  �  �     �  �  �  >  �  �  >  �  �  9     �  7   �  �  >  �  �       |      =     �  �  =     �  a  A     �  t  l   =     �  �  P     �  �  �  �       �     .   �  �  �  >  a  �       }      �  g  �  f       �      =  P  �  u  =     �  U  W  9   �  �  �  >  �  �       �      =  9   �  �  O     �  �  �            >  a  �       �      =     �  �       �  �  =     �  h       �     G   �  �  =     �  n  A  @   �  '  t   =     �  �  �     �  �  �  >  �  �  >  �  �  9     �  7   �  �  >  �  �       �      =     �  �  =     �  a  A     �  �  l   =     �  �  P     �  �  �  �       �     .   �  �  �  >  a  �       �      �  g  �  g       �      =     �  �  >  �  �  =     �  b  >  �  �  =     �  h  >  �  �  =     �  n  >  �  �  =     �  q  >  �  �  =     �  t  >  �  �  =     �  e  >  �  �  =     �  �  >  �  �  =     �  a  >  �  �  9     �  3   �  �  �  �  �  �  �  �  �       �      >  �  �       �      =     �  �  Q     �  �      Q     �  �     Q     �  �     P  9   �  �  �  �  X   >  �  �  �  8       A  !   6            	   7     
   �          C      A  @   A   =   ?   =     B   A   A  @   D   =   C   =     E   D   �     F   B   E   =     G   
   A  @   H   =   ?   =     I   H   A  @   J   =   C   =     K   J   �     L   I   K   �     M   G   L   A  @   N   =   C   =     O   N   �     P   M   O   �     Q   F   P   �  Q   8       V  @   6               7        7        �     ;     T      ;  [   \      ;     k      ;  [   s           X      =     U      �     W   U   V   P     Y   X   X   �     Z   W   Y   >  T   Z        Y      A  ^   _   =   ]   =  :   `   _   =     a   T   =     b      Q     c   a       Q     d   a      P     e   c   d   b   Q     f   e       Q     g   e      Q     h   e      P  9   i   f   g   h   X   �  9   j   `   i   >  \   j        Z      A     m   \   l   =     n   m   �     o   X   n   >  k   o        [      =     p   k   =  9   q   \   �  9   r   q   p   >  \   r        \      A  ^   u   =   t   =  :   v   u   =  9   w   \   �  9   x   v   w   >  s   x        ^      =  9   y   s   O     z   y   y             �  z   8       �  <   6               7        7        �     ;     �      ;     �           �      =     }      �        }   ~   >             �      A     �      �   =     �   �   p     �   �   =     �      p     �   �   �     �   �   �   �     �   �   �   >  �   �        �      A     �      �   =     �   �   p     �   �   =     �      p     �   �   �     �   �   �   �     �   �   �   >  �   �        �      =     �   �   �     �   �   �   =     �   �   �     �   �   �   �     �   �   �   �     �   �   X   �     �   �   �   �     �   �   X   �  �   8       �  =   6     !          7        7         �  "   ;     �      ;     �      ;     �      ;     �      ;     �           �      =     �      =     �      �     �   �   �   �     �   �   �   >  �   �        �      =     �       >  �   �        �      =     �       =     �   �   �     �   X   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �        �      =     �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        �      =     �   �   =     �   �   �     �   �   �   >  �   �   �  �   �  �   >  �   �   �  �   �  �   =     �   �        �      >  �   �        �      =     �   �   �  �   8       �  >   6     '       $   7  #   %   7     &   �  (        �      =     �   %   =     �   %   �     �   �   �   =     �   &   �     �   X   �        �         �   �   �     �   �   �   �     �   �   �   �  �   8       �     6     3       )   7  #   *   7  #   +   7  #   ,   7     -   7     .   7     /   7     0   7  #   1   7  #   2   �  4   ;  #   �      ;     �      ;     �      ;     �      ;     �      ;  #   �      ;  #   �      ;     �      ;  #   �      ;  #   �      ;     �      ;  #   �      ;  #   �           �      =     �   +   =     �   .   P     �   �   �   �        �      .   �   �   �   >  �   �        �      =     �   ,   =     �   *   �     �   �   �        �      (   �   �   >  �   �        �      =     �   -   >  �   �   =     �   �   >  �   �   9     �   !   �   �   >  �   �        �      =     �   �   >  �   �   =     �   �   >  �   �   9     �   '   �   �   >  �   �        �      =     �   �   �     �   �   �   =     �   .   �     �   X   �   �     �   �   �   =     �   +   �     �   �   �   >  �   �        �      =     �   �   P     �   �   �   �   =     �   �   �     �   �   �   >  �   �        �      =     �   �   �     �   �   �   >  �   �        �      =     �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        �      =     �   �   =     �   �   P     �   �   �   �   �     �   �   �   >  �   �   �  �   �  �   >  �   �   �  �   �  �   =        �        �      >  �           �      =       �   =       1   �           =       /   �           =       �   =       2   �           =     	  /   �     
    	  �         
  �    8         /   6     7       $   7  #   5   7     6   �  8              A       5   �   =         �  �       �  �       �        �        �               A       5   �   =         �  �       �    �    �  �       8       �  �       �        �        �               A       5     =         �  �       �    �    �  �             �  !      �       !  �                �  #  �  !             A  )  *  '  (  =     +  *  �  0      � 	 +  /      ,     -     .  �  /              �  �   �  ,             �  �   �  -             A  3  4  '  2  =     5  4  A  3  7  '  6  =     8  7  =     9  5   �     ;  9  :  �     <  ;  �   �     =  <  �   P     >  =  =  =       ?     .   5  8  >  �  ?  �  .             =  B  E  D  =     F  5   =     G  6   X  9   H  E  F     G  O     I  H  H            �  I  �  0  �  8                          ��KT��D	(�D��^������Vb�>�D_w���                       ��v)� inco�%�9�� _irr��Y��G� mple�]����M MAXIE��}�+ URE_]�V��|1            N1�8��;                               \�5s����