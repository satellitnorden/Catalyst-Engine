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

layout (std140, set = 1, binding = 2) uniform HammersleyHemisphereSamples
{
	layout (offset = 0) vec4 HAMMERSLEY_COSINUS_SAMPLES[64];
	layout (offset = 1024) vec4 HAMMERSLEY_UNIFORM_SAMPLES[64];
};

layout (std140, set = 1, binding = 3) uniform Irradiance
{
	layout (offset = 0) vec4 IRRADIANCE_HEMISPHERE_SAMPLES[64];
};

layout (std140, set = 1, binding = 4) uniform Wind
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
layout (std430, set = 1, binding = 5) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 6) uniform sampler SAMPLER;

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 8, rgba32f) uniform image2D DiffuseIrradiance; 

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
