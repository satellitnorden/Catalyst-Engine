�nX/J1��7Q݈�SRayTracedSpecularIrradiance_RenderPipeline                         ��      #     �        �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main    �   �         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedSpecularIrradiance_RayGeneration.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2; 
layout (set = 1, binding = 8, rgba32f) uniform image2D SceneFeatures3; 
layout (set = 1, binding = 9, rgba32f) uniform image2D SpecularIrradiance; 

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
layout (location = 0) rayPayloadNV vec3 RADIANCE;

void main()
{
    #define NUMBER_OF_SAMPLES (1)
    vec2 screen_coordinate = (vec2(gl_LaunchIDNV.xy) + vec2(0.5f)) / vec2(gl_LaunchSizeNV.xy);
    vec4 scene_features_2 = imageLoad(SceneFeatures2, ivec2(gl_LaunchIDNV.xy));
    vec4 scene_features_3 = imageLoad(SceneFeatures3, ivec2(gl_LaunchIDNV.xy));
    vec3 normal = scene_features_2.xyz;
    float depth = scene_features_2.w;
    float roughness = scene_features_3.x;
    vec3 world_position = CalculateWorldPosition(screen_coordinate, depth);
    vec3 view_direction = normalize(world_position - CAMERA_WORLD_POSITION);
    vec3 accumulated_radiance = vec3(0.0f);
    for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        vec3 ray_direction;
        vec4 noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_LaunchIDNV.xy), i);
        mat3 random_rotation = CalculateGramSchmidtRotationMatrix(normal, noise_texture_sample.xyz * 2.0f - 1.0f);
        uint random_hemisphere_sample_index = uint(noise_texture_sample.w * 64.0f) & 63;
        vec3 random_hemisphere_direction = IRRADIANCE_HEMISPHERE_SAMPLES[random_hemisphere_sample_index].xyz;
        ray_direction = random_rotation * random_hemisphere_direction;
        ray_direction = dot(ray_direction, normal) >= 0.0f ? ray_direction : -ray_direction;
        ray_direction = normalize(normal + ray_direction);
        vec3 specular_direction = reflect(view_direction, normal);
        ray_direction = normalize(mix(specular_direction, ray_direction, roughness));
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	0, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	0, /*missIndex*/
	world_position+normal*FLOAT32_EPSILON*4096.0f, /*origin*/
	FLOAT32_EPSILON * 128.0f, /*Tmin*/
	ray_direction, /*direction*/
	FLOAT32_MAXIMUM, /*Tmax*/
	0 /*payload*/
);
        accumulated_radiance += RADIANCE;
    }
    imageStore(SpecularIrradiance, ivec2(gl_LaunchIDNV.xy), vec4(accumulated_radiance / NUMBER_OF_SAMPLES, 1.0f));
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main     
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth         CalculateGramSchmidtRotationMatrix(vf3;vf3;      normal       random_tilt   "   offset_index      #   General  	 #       FULL_MAIN_RESOLUTION      #      INVERSE_FULL_MAIN_RESOLUTION     	 #      HALF_MAIN_RESOLUTION      #      INVERSE_HALF_MAIN_RESOLUTION      #      FRAME     #      VIEW_DISTANCE     %         /   offset_coordinate     H   BLUE_NOISE_TEXTURES   V   near_plane_coordinate     ^   view_space_position   `   Camera   	 `       WORLD_TO_CLIP_MATRIX     	 `      WORLD_TO_CAMERA_MATRIX    `      PREVIOUS_WORLD_TO_CLIP_MATRIX     `      INVERSE_WORLD_TO_CAMERA_MATRIX    `      INVERSE_CAMERA_TO_CLIP_MATRIX    	 `      CAMERA_WORLD_POSITION    	 `      CAMERA_FORWARD_VECTOR    	 `      CURRENT_FRAME_JITTER      `      NEAR_PLANE    `   	   FAR_PLANE     b         p   inverse_view_space_position_denominator   x   world_space_position      �   random_tangent    �   random_bitangent      �   screen_coordinate     �   gl_LaunchIDNV     �   gl_LaunchSizeNV   �   scene_features_2      �   SceneFeatures2    �   scene_features_3      �   SceneFeatures3    �   normal    �   depth     �   roughness     �   world_position    �   param     �   param     �   view_direction    �   accumulated_radiance      �   i     �   noise_texture_sample      �   param     �   param     �   random_rotation   �   param     �   param    
 �   random_hemisphere_sample_index   	    random_hemisphere_direction     Irradiance          IRRADIANCE_HEMISPHERE_SAMPLES               ray_direction        specular_direction    ,  TOP_LEVEL_ACCELERATION_STRUCTURE      :  RADIANCE      A  SpecularIrradiance    P  TEXTURES     	 R  GlobalTextureAverageValues   	 R      TEXTURE_AVERAGE_VALUES    T        U  Material      U      _Properties   U     _AlbedoThickness     	 U     _NormalMapDisplacement    U     _MaterialProperties   U     _Opacity      U     _EmissiveMultiplier   U     _Padding1     U     _Padding2     X  GlobalMaterials   X      MATERIALS     Z        ^  SKY_TEXTURE  	 a  HammersleyHemisphereSamples  
 a      HAMMERSLEY_COSINUS_SAMPLES   
 a     HAMMERSLEY_UNIFORM_SAMPLES    c        d  Wind      d      UPPER_SKY_COLOR   d     LOWER_SKY_COLOR   d     SKY_MODE      d     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     f        g  LightingHeader    g      _NumberOfLights   g     _MaximumNumberOfShadowCastingLights   i  Lighting      i      LIGHTING_HEADER   i     LIGHT_DATA    k        n  SAMPLER  
 p  OpaqueModels_VERTEX_DATA_BUFFER  
 p      OpaqueModels_VERTEX_DATA     	 s  OpaqueModels_VERTEX_BUFFERS  
 u  OpaqueModels_INDEX_DATA_BUFFER   	 u      OpaqueModels_INDEX_DATA  	 x  OpaqueModels_INDEX_BUFFERS   
 {  OpaqueModels_MATERIAL_BUFFER      {      OpaqueModels_MATERIAL_INDICES     }       
   MaskedModels_VERTEX_DATA_BUFFER  
       MaskedModels_VERTEX_DATA     	 �  MaskedModels_VERTEX_BUFFERS  
 �  MaskedModels_INDEX_DATA_BUFFER   	 �      MaskedModels_INDEX_DATA  	 �  MaskedModels_INDEX_BUFFERS   
 �  MaskedModels_MATERIAL_BUFFER      �      MaskedModels_MATERIAL_INDICES     �      H  #       #       H  #      #      H  #      #      H  #      #      H  #      #       H  #      #   $   G  #      G  %   "      G  %   !      G  H   "       G  H   !      H  `          H  `       #       H  `             H  `         H  `      #   @   H  `            H  `         H  `      #   �   H  `            H  `         H  `      #   �   H  `            H  `         H  `      #      H  `            H  `      #   @  H  `      #   P  H  `      #   `  H  `      #   h  H  `   	   #   l  G  `      G  b   "      G  b   !       G  �      �  G  �      �  G  �   "      G  �   !      G  �   "      G  �   !      G          H        #       G       G    "      G    !      G  ,  "      G  ,  !       G  :         G  A  "      G  A  !   	   G  P  "       G  P  !       G  Q        H  R      #       G  R     G  T  "       G  T  !      H  U      #       H  U     #      H  U     #      H  U     #      H  U     #      H  U     #      H  U     #      H  U     #      G  W         H  X      #       G  X     G  Z  "       G  Z  !      G  ^  "       G  ^  !      G  _        G  `        H  a      #       H  a     #      G  a     G  c  "      G  c  !      H  d      #       H  d     #      H  d     #       H  d     #   $   G  d     G  f  "      G  f  !      H  g      #       H  g     #      G  h        H  i      #       H  i     #      G  i     G  k  "      G  k  !      G  n  "      G  n  !      G  o        H  p      #       G  p     G  s  "      G  s  !      G  t        H  u      #       G  u     G  x  "      G  x  !      G  z        H  {      #       G  {     G  }  "      G  }  !      G  ~        H        #       G       G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	            
                             !        	   
                                                 !                                     !                #                        $      #   ;  $   %        &          +  &   '         (         +     -      +     0       +     5       +     :       	 D                              E   D     F   E   5      G       F   ;  G   H          J       E   +     O      B+     R       +     X      @+     Z     �?   ]           _           `   _   _   _   _   _                     a      `   ;  a   b         c      _   +     q      +  &   y        �            �      �   ;  �   �      +     �      ?,     �   �   �   ;  �   �       	 �                              �       �   ;  �   �         �   &      ;  �   �       +  &   �         �         ,     �   R   R   R     �      �         +     �     �B+     �   ?   +       @                            ;         +  &                    � *     +      *  ;  +  ,      +     .  �   +     1     4+     3    �E+     6    �7+     8  ��   9  �     ;  9  :  �  +  &   ?     ;  �   A      +     M       N  D   M     O      N  ;  O  P        Q     M    R  Q     S     R  ;  S  T      
 U                          +     V       W  U  V    X  W     Y     X  ;  Y  Z      	 [                             \  [     ]      \  ;  ]  ^        _         `         a  _  `     b     a  ;  b  c       d                 e     d  ;  e  f       g          h       i  g  h     j     i  ;  j  k       l     m      l  ;  m  n        o       p  o    q  p  :      r     q  ;  r  s       t       u  t    v  u  :      w     v  ;  w  x       y          z  y    {  z     |     {  ;  |  }       ~         ~    �    :      �     �  ;  �  �       �       �  �    �  �  :      �     �  ;  �  �       �  y    �  �     �     �  ;  �  �          L     6               �     ;     �      ;  ]   �      ;  ]   �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;  
   �      ;  ]   �      ;  	   �      ;  
   �      ;  �   �      ;     �      ;     �      ;  
   �      ;           ;          ;          ;                O      =  �   �   �   O     �   �   �          p     �   �   �     �   �   �   =  �   �   �   O     �   �   �          p     �   �   �     �   �   �   >  �   �        P      =  �   �   �   =  �   �   �   O     �   �   �          |  �   �   �   b     �   �   �   >  �   �        Q      =  �   �   �   =  �   �   �   O     �   �   �          |  �   �   �   b     �   �   �   >  �   �        R      =     �   �   O     �   �   �             >  �   �        S      A     �   �   q   =     �   �   >  �   �        T      A     �   �   0   =     �   �   >  �   �        U      =     �   �   >  �   �   =     �   �   >  �   �   9     �      �   �   >  �   �        V      =     �   �   A  �   �   b   �   =     �   �   �     �   �   �        �      E   �   >  �   �        W      >  �   �        X      >  �   0   �  �   �  �        X      �  �   �       �  �   �  �        X      =     �   �   �  �   �   �   :   �  �   �   �   �  �        [      =  �   �   �   O     �   �   �          Q     �   �       Q     �   �      P     �   �   �   >  �   �   =     �   �   >  �   �   9     �      �   �   >  �   �        \      =     �   �   O     �   �   �             �     �   �   X   P     �   Z   Z   Z   �     �   �   �   =     �   �   >  �   �   >  �   �   9     �       �   �   >  �   �        ]      A     �   �   q   =     �   �   �     �   �   �   m     �   �   �     �   �   �   >  �   �        ^      =       �   A    	        =     
  	  O       
  
            >            _      =       �   =          �           >           `      =         =       �   �           �  �       R   �        �        �         `      =         >      �    �         `      =                  >      �    �    =              `      >           a      =       �   =         �                     E     >           b      =     !  �   =     "  �        #     G   !  "  >     #       c      =     $     =     %    =     &  �   P     '  &  &  &       (     .   $  %  '       )     E   (  >    )       q      =  *  -  ,       l      =     /  �   =     0  �   �     2  0  1  �     4  2  3  �     5  /  4       q      =     7    � -  0   .  0   0   0   5  6  7  8         r      =     ;  :  =     <  �   �     =  <  ;  >  �   =  �  �   �  �        X      =     >  �   �     @  >  ?  >  �   @  �  �   �  �        t      =  �   B  A  =  �   C  �   O     D  C  C         |  �   E  D  =     F  �   P     G  Z   Z   Z   �     H  F  G  Q     I  H      Q     J  H     Q     K  H     P     L  I  J  K  Z   c  B  E  L  �  8       V  9   6               7  	      7  
      �     ;  
   "      ;  	   /           X      A  (   )   %   '   =     *   )   =     +      �     ,   *   +   �     .   ,   -   >  "   .        \      A  
   1      0   =     2   1   A  (   3   %   '   =     4   3   �     6   4   5   �     7   6   -   �     8   2   7   A  
   9   /   0   >  9   8        ]      A  
   ;      :   =     <   ;   A  (   =   %   '   =     >   =   �     ?   >   5   �     @   ?   5   �     A   @   -   �     B   <   A   A  
   C   /   :   >  C   B        _      =     I   "   A  J   K   H   I   =  E   L   K   =     M   /   p     N   M   P     P   O   O   �     Q   N   P   X     S   L   Q      R   �  S   8       z  @   6               7        7        �     ;     V      ;  ]   ^      ;     p      ;  ]   x           |      =     W      �     Y   W   X   P     [   Z   Z   �     \   Y   [   >  V   \        }      A  c   d   b   '   =  _   e   d   =     f   V   =     g      Q     h   f       Q     i   f      P     j   h   i   g   Q     k   j       Q     l   j      Q     m   j      P     n   k   l   m   Z   �     o   e   n   >  ^   o        ~      A     r   ^   q   =     s   r   �     t   Z   s   >  p   t              =     u   p   =     v   ^   �     w   v   u   >  ^   w        �      A  c   z   b   y   =  _   {   z   =     |   ^   �     }   {   |   >  x   }        �      =     ~   x   O        ~   ~             �     8       �  F   6                7        7        �  !   ;     �      ;     �           �      =     �      =     �      =     �      =     �      �     �   �   �   �     �   �   �   �     �   �   �        �      E   �   >  �   �        �      =     �      =     �   �        �      D   �   �   >  �   �        �      =     �   �   =     �   �   =     �      Q     �   �       Q     �   �      Q     �   �      Q     �   �       Q     �   �      Q     �   �      Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   �  �   8         ��      #     �         �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main    ]        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedSpecularIrradiance_RayMiss.glsl     �   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2; 
layout (set = 1, binding = 8, rgba32f) uniform image2D SceneFeatures3; 
layout (set = 1, binding = 9, rgba32f) uniform image2D SpecularIrradiance; 

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
layout (location = 0) rayPayloadInNV vec3 RADIANCE;

void main()
{
    RADIANCE = SampleSky(gl_WorldRayDirectionNV, MAXIMUM_SKY_TEXTURE_MIP_LEVEL);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         SampleSky(vf3;f1;        direction        mip_level     ,   Wind      ,       UPPER_SKY_COLOR   ,      LOWER_SKY_COLOR   ,      SKY_MODE      ,      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     .         O   SKY_TEXTURE   [   RADIANCE      ]   gl_WorldRayDirectionNV    _   param     a   param     j   TEXTURES     	 l   GlobalTextureAverageValues   	 l       TEXTURE_AVERAGE_VALUES    n         o   Material      o       _Properties   o      _AlbedoThickness     	 o      _NormalMapDisplacement    o      _MaterialProperties   o      _Opacity      o      _EmissiveMultiplier   o      _Padding1     o      _Padding2     r   GlobalMaterials   r       MATERIALS     t         y   BLUE_NOISE_TEXTURES   |   Camera   	 |       WORLD_TO_CLIP_MATRIX     	 |      WORLD_TO_CAMERA_MATRIX    |      PREVIOUS_WORLD_TO_CLIP_MATRIX     |      INVERSE_WORLD_TO_CAMERA_MATRIX    |      INVERSE_CAMERA_TO_CLIP_MATRIX    	 |      CAMERA_WORLD_POSITION    	 |      CAMERA_FORWARD_VECTOR    	 |      CURRENT_FRAME_JITTER      |      NEAR_PLANE    |   	   FAR_PLANE     ~            General  	        FULL_MAIN_RESOLUTION            INVERSE_FULL_MAIN_RESOLUTION     	       HALF_MAIN_RESOLUTION            INVERSE_HALF_MAIN_RESOLUTION            FRAME           VIEW_DISTANCE     �        	 �   HammersleyHemisphereSamples  
 �       HAMMERSLEY_COSINUS_SAMPLES   
 �      HAMMERSLEY_UNIFORM_SAMPLES    �         �   Irradiance    �       IRRADIANCE_HEMISPHERE_SAMPLES     �         �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   SAMPLER   �   SceneFeatures2    �   SceneFeatures3    �   SpecularIrradiance    �   TOP_LEVEL_ACCELERATION_STRUCTURE     
 �   OpaqueModels_VERTEX_DATA_BUFFER  
 �       OpaqueModels_VERTEX_DATA     	 �   OpaqueModels_VERTEX_BUFFERS  
 �   OpaqueModels_INDEX_DATA_BUFFER   	 �       OpaqueModels_INDEX_DATA  	 �   OpaqueModels_INDEX_BUFFERS   
 �   OpaqueModels_MATERIAL_BUFFER      �       OpaqueModels_MATERIAL_INDICES     �        
 �   MaskedModels_VERTEX_DATA_BUFFER  
 �       MaskedModels_VERTEX_DATA     	 �   MaskedModels_VERTEX_BUFFERS  
 �   MaskedModels_INDEX_DATA_BUFFER   	 �       MaskedModels_INDEX_DATA  	 �   MaskedModels_INDEX_BUFFERS   
 �   MaskedModels_MATERIAL_BUFFER      �       MaskedModels_MATERIAL_INDICES     �       H  ,       #       H  ,      #      H  ,      #       H  ,      #   $   G  ,      G  .   "      G  .   !      G  O   "       G  O   !      G  [          G  ]      �  G  j   "       G  j   !       G  k         H  l       #       G  l      G  n   "       G  n   !      H  o       #       H  o      #      H  o      #      H  o      #      H  o      #      H  o      #      H  o      #      H  o      #      G  q          H  r       #       G  r      G  t   "       G  t   !      G  y   "       G  y   !      H  |          H  |       #       H  |             H  |         H  |      #   @   H  |            H  |         H  |      #   �   H  |            H  |         H  |      #   �   H  |            H  |         H  |      #      H  |            H  |      #   @  H  |      #   P  H  |      #   `  H  |      #   h  H  |   	   #   l  G  |      G  ~   "      G  ~   !       H         #       H        #      H        #      H        #      H        #       H        #   $   G        G  �   "      G  �   !      G  �         G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !      H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !   	   G  �   "      G  �   !       G  �         H  �       #       G  �      G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !           !                               	            
         !        	   
                     +            +           +     !      +     (     �B+     )       ,     *   (   )   )     ,                  -      ,   ;  -   .        /          +  /   0         1         ,     9   )   )   )   +  /   ;         <         +  /   ?       +     C     �?,     D   )   C   )   +     F      ? 	 L                              M   L      N       M   ;  N   O         S            Z   �     ;  Z   [   �     \         ;  \   ]      +  /   ^         b          	 f                            +     g        h   f   g      i       h   ;  i   j         k   S   g     l   k      m      l   ;  m   n       
 o                           +     p        q   o   p     r   q      s      r   ;  s   t        u   f   +     v         w   u   v      x       w   ;  x   y         z   S        {           |   z   z   z   z   z         {            }      |   ;  }   ~           {   {   {   {            �         ;  �   �      +     �   @     �   S   �     �   S   �     �   �   �      �      �   ;  �   �        �   S   �     �   �      �      �   ;  �   �        �           �   S     �   �   �      �      �   ;  �   �        �      �       �   ;  �   �        	 �                              �       �   ;  �   �       ;  �   �       ;  �   �       � �      �       �   ;  �   �         �   S     �   �     �   �         �      �   ;  �   �        �        �   �     �   �         �      �   ;  �   �        �           �   �     �   �      �      �   ;  �   �        �   S     �   �     �   �         �      �   ;  �   �        �        �   �     �   �         �      �   ;  �   �        �   �     �   �      �      �   ;  �   �           L     6               �     ;  	   _      ;  
   a           N      =     `   ]   >  _   `   A  b   c   .   ^   =     d   c   >  a   d   9     e      _   a   >  [   e   �  8       !  /   6               7  	      7  
      �          $      A  
            =           �           �           �         �           �          $      A  
            =           �           �     �     �                    �           �          �            �          $      A  
   "      !   =     #   "   �     $   #   �      �      �     %         $      �  '       �  %   &   '   �  &        &      �  *   �  '        )      A  1   2   .   0   =     3   2   �  8       � 	 3   7       4      5      6   �  7        >      �  9   �  4        .      �  9   �  5        3      A  <   =   .   ;   =     >   =   A  <   @   .   ?   =     A   @   =     B      �     E   B   D   �     G   E   F   �     H   G   F   P     I   H   H   H        J      .   >   A   I   �  J   �  6        8      =  M   P   O   =     Q      =     R      X  S   T   P   Q      R   O     U   T   T             �  U   �  8   �  8  ��      #     i         �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedSpecularIrradiance_RayMiss.glsl     �   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2; 
layout (set = 1, binding = 8, rgba32f) uniform image2D SceneFeatures3; 
layout (set = 1, binding = 9, rgba32f) uniform image2D SpecularIrradiance; 

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
layout (location = 1) rayPayloadInNV float VISIBILITY;

void main()
{
    VISIBILITY = 1.0f;
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main      	   VISIBILITY       TEXTURES     	    GlobalTextureAverageValues   	        TEXTURE_AVERAGE_VALUES                Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                  BLUE_NOISE_TEXTURES   $   SKY_TEXTURE   (   Camera   	 (       WORLD_TO_CLIP_MATRIX     	 (      WORLD_TO_CAMERA_MATRIX    (      PREVIOUS_WORLD_TO_CLIP_MATRIX     (      INVERSE_WORLD_TO_CAMERA_MATRIX    (      INVERSE_CAMERA_TO_CLIP_MATRIX    	 (      CAMERA_WORLD_POSITION    	 (      CAMERA_FORWARD_VECTOR    	 (      CURRENT_FRAME_JITTER      (      NEAR_PLANE    (   	   FAR_PLANE     *         +   General  	 +       FULL_MAIN_RESOLUTION      +      INVERSE_FULL_MAIN_RESOLUTION     	 +      HALF_MAIN_RESOLUTION      +      INVERSE_HALF_MAIN_RESOLUTION      +      FRAME     +      VIEW_DISTANCE     -        	 1   HammersleyHemisphereSamples  
 1       HAMMERSLEY_COSINUS_SAMPLES   
 1      HAMMERSLEY_UNIFORM_SAMPLES    3         5   Irradiance    5       IRRADIANCE_HEMISPHERE_SAMPLES     7         8   Wind      8       UPPER_SKY_COLOR   8      LOWER_SKY_COLOR   8      SKY_MODE      8      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     :         ;   LightingHeader    ;       _NumberOfLights   ;      _MaximumNumberOfShadowCastingLights   =   Lighting      =       LIGHTING_HEADER   =      LIGHT_DATA    ?         B   SAMPLER   E   SceneFeatures2    F   SceneFeatures3    G   SpecularIrradiance    J   TOP_LEVEL_ACCELERATION_STRUCTURE     
 L   OpaqueModels_VERTEX_DATA_BUFFER  
 L       OpaqueModels_VERTEX_DATA     	 P   OpaqueModels_VERTEX_BUFFERS  
 R   OpaqueModels_INDEX_DATA_BUFFER   	 R       OpaqueModels_INDEX_DATA  	 U   OpaqueModels_INDEX_BUFFERS   
 X   OpaqueModels_MATERIAL_BUFFER      X       OpaqueModels_MATERIAL_INDICES     Z        
 \   MaskedModels_VERTEX_DATA_BUFFER  
 \       MaskedModels_VERTEX_DATA     	 _   MaskedModels_VERTEX_BUFFERS  
 a   MaskedModels_INDEX_DATA_BUFFER   	 a       MaskedModels_INDEX_DATA  	 d   MaskedModels_INDEX_BUFFERS   
 f   MaskedModels_MATERIAL_BUFFER      f       MaskedModels_MATERIAL_INDICES     h       G  	         G     "       G     !       G           H         #       G        G     "       G     !      H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G      "       G      !      G  $   "       G  $   !      H  (          H  (       #       H  (             H  (         H  (      #   @   H  (            H  (         H  (      #   �   H  (            H  (         H  (      #   �   H  (            H  (         H  (      #      H  (            H  (      #   @  H  (      #   P  H  (      #   `  H  (      #   h  H  (   	   #   l  G  (      G  *   "      G  *   !       H  +       #       H  +      #      H  +      #      H  +      #      H  +      #       H  +      #   $   G  +      G  -   "      G  -   !      G  /         G  0         H  1       #       H  1      #      G  1      G  3   "      G  3   !      G  4         H  5       #       G  5      G  7   "      G  7   !      H  8       #       H  8      #      H  8      #       H  8      #   $   G  8      G  :   "      G  :   !      H  ;       #       H  ;      #      G  <         H  =       #       H  =      #      G  =      G  ?   "      G  ?   !      G  B   "      G  B   !      G  E   "      G  E   !      G  F   "      G  F   !      G  G   "      G  G   !   	   G  J   "      G  J   !       G  K         H  L       #       G  L      G  P   "      G  P   !      G  Q         H  R       #       G  R      G  U   "      G  U   !      G  W         H  X       #       G  X      G  Z   "      G  Z   !      G  [         H  \       #       G  \      G  _   "      G  _   !      G  `         H  a       #       G  a      G  d   "      G  d   !      G  e         H  f       #       G  f      G  h   "      G  h   !           !                       �     ;     	   �  +     
     �? 	                                          +                                   ;                                                      ;            
                            +                                          ;                   +                                    ;              	 !                              "   !      #       "   ;  #   $         %           &           '           (   %   %   %   %   %   &   &   '            )      (   ;  )   *        +   '   '   '   '            ,      +   ;  ,   -      +     .   @     /      .     0      .     1   /   0      2      1   ;  2   3        4      .     5   4      6      5   ;  6   7        8   &   &            9      8   ;  9   :        ;           <        =   ;   <      >      =   ;  >   ?        @      A       @   ;  A   B        	 C                              D       C   ;  D   E       ;  D   F       ;  D   G       � H      I       H   ;  I   J         K        L   K   +     M        N   L   M      O      N   ;  O   P        Q        R   Q     S   R   M      T      S   ;  T   U        V           W   V     X   W      Y      X   ;  Y   Z        [        \   [     ]   \   M      ^      ]   ;  ^   _        `        a   `     b   a   M      c      b   ;  c   d        e   V     f   e      g      f   ;  g   h           L     6               �          N      >  	   
   �  8         ��%H���     #     �        �    �  
  SPV_EXT_descriptor_indexing 
  SPV_NV_ray_tracing       GLSL.std.450               �     main    �  �    �  �  �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedSpecularIrradiance_RayClosestHit.glsl   �!   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2; 
layout (set = 1, binding = 8, rgba32f) uniform image2D SceneFeatures3; 
layout (set = 1, binding = 9, rgba32f) uniform image2D SpecularIrradiance; 

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
	uint vertex_index_0 = OpaqueModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_INDEX_DATA[gl_PrimitiveID * 3 + 0];
	vec4 vertex_data_0_0 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_0 + 0];
	vec4 vertex_data_0_1 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_0 + 1];
	vec4 vertex_data_0_2 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_0 + 2];
	uint vertex_index_1 = OpaqueModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_INDEX_DATA[gl_PrimitiveID * 3 + 1];
	vec4 vertex_data_1_0 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_1 + 0];
	vec4 vertex_data_1_1 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_1 + 1];
	vec4 vertex_data_1_2 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_1 + 2];
	uint vertex_index_2 = OpaqueModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_INDEX_DATA[gl_PrimitiveID * 3 + 2];
	vec4 vertex_data_2_0 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_2 + 0];
	vec4 vertex_data_2_1 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_2 + 1];
	vec4 vertex_data_2_2 = OpaqueModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].OpaqueModels_VERTEX_DATA[3 * vertex_index_2 + 2];
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
	return OpaqueModels_MATERIAL_INDICES[gl_InstanceCustomIndexNV / 4][gl_InstanceCustomIndexNV & 3];
}

layout (location = 0) rayPayloadInNV vec3 RADIANCE; 

layout (location = 1) rayPayloadNV float VISIBILITY; 

void main()
{
    HitVertexInformation hit_vertex_information = GetHitVertexInformation();
    uint hit_material_index = GetHitMaterialIndex();
    vec4 albedo_thickness;
    EVALUATE_AVERAGE_ALBEDO_THICKNESS(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, albedo_thickness);
    vec4 normal_map_displacement;
    EVALUATE_AVERAGE_NORMAL_MAP_DISPLACEMENT(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, normal_map_displacement);
    vec4 material_properties;
    EVALUATE_AVERAGE_MATERIAL_PROPERTIES(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, material_properties);
    vec3 hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV;
    RADIANCE = vec3(0.0f, 0.0f, 0.0f);
    RADIANCE += albedo_thickness.rgb * material_properties.w * MATERIALS[hit_material_index]._EmissiveMultiplier;
    {
        vec3 incoming_diffuse_irradiance;
        {
            VISIBILITY = 0.0f;
            uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                        | gl_RayFlagsSkipClosestHitShaderNV;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	1, /*missIndex*/
	hit_position, /*origin*/
	FLOAT32_EPSILON * 8.0f, /*Tmin*/
	hit_vertex_information._Normal, /*direction*/
	FLOAT32_MAXIMUM, /*Tmax*/
	1 /*payload*/
);
            incoming_diffuse_irradiance = SampleSky(hit_vertex_information._Normal, MAXIMUM_SKY_TEXTURE_MIP_LEVEL) * VISIBILITY;
        }
        RADIANCE += CalculateIndirectLighting
        (
            -gl_WorldRayDirectionNV,
            albedo_thickness.rgb,
            hit_vertex_information._Normal,
            material_properties.x,
            material_properties.y,
            material_properties.z,
            albedo_thickness.w,
            incoming_diffuse_irradiance,
            vec3(0.0f)
        );
    }
    {
        for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
        {
	        Light light = UnpackLight(i);
            vec3 light_radiance = light._Color * light._Intensity;
            switch (light._LightType)
            {
                case LIGHT_TYPE_DIRECTIONAL:
                {
                    VISIBILITY = 0.0f;
                    uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                                | gl_RayFlagsSkipClosestHitShaderNV;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	1, /*missIndex*/
	hit_position, /*origin*/
	FLOAT32_EPSILON * 8.0f, /*Tmin*/
	-light._TransformData1, /*direction*/
	FLOAT32_MAXIMUM, /*Tmax*/
	1 /*payload*/
);
                    RADIANCE += BidirectionalReflectanceDistribution
                    (
                        -gl_WorldRayDirectionNV,
                        albedo_thickness.rgb,
                        hit_vertex_information._Normal,
                        material_properties.x,
                        material_properties.y,
                        albedo_thickness.w,
                        light._TransformData1
                    ) * light_radiance * VISIBILITY;
                    break;
                }
            }
        }
    }
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         UnpackColor(u1;      color        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index        Distribution(f1;f1;      roughness        microsurface_angle   	 !   Geometry(vf3;vf3;vf3;f1;         normal       outgoing_direction       radiance_direction        roughness     &   Fresnel(vf3;f1;   $   surface_color     %   difference_angle     
 .   DisneyDiffuse(vf3;f1;f1;f1;f1;    )   albedo    *   roughness     +   difference_angle      ,   radiance_angle    -   outgoing_angle    8   BidirectionalReflectanceDistribution(vf3;vf3;vf3;f1;f1;f1;vf3;    1   outgoing_direction    2   albedo    3   normal    4   roughness     5   metallic      6   thickness     7   radiance_direction    <   GeometryIndirect(f1;f1;   :   roughness     ;   outgoing_angle    @   FresnelIndirect(vf3;f1;   >   surface_color     ?   outgoing_angle    L   CalculateIndirectLighting(vf3;vf3;vf3;f1;f1;f1;f1;vf3;vf3;    C   outgoing_direction    D   albedo    E   normal    F   roughness     G   metallic      H   ambient_occlusion     I   thickness     J   diffuse_irradiance    K   specular_irradiance   P   SampleSky(vf3;f1;     N   direction     O   mip_level     S   HitVertexInformation      S       _Position     S      _Normal   S      _Tangent      S      _TextureCoordinate   	 U   GetHitVertexInformation(      X   GetHitMaterialIndex(      [   unpacked      �   light_data_1      �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   light_data_2      �   light_data_3      �   light_data_4      �   light     �   roughness_squared    	 �   microsurface_angle_squared    �   nominator     �   denominator  
 �   outgoing_direction_coefficient    �   irradiance_direction_coefficient      �   roughness_coefficient       nominator       denominator     first_coefficient       nominator       denominator   "  second_coefficient    >  FD90      H  term_1    L  term_2    U  term_3    e  microsurface_normal   k  surface_color     r  outgoing_angle    w  radiance_angle    }  microsurface_angle    �  difference_angle      �  distribution      �  param     �  param     �  geometry      �  param     �  param     �  param     �  param     �  fresnel   �  param     �  param     �  diffuse_component     �  param     �  param     �  param     �  param     �  param     �  nominator     �  denominator   �  specular_component    �  weakening_factor      �  roughness_coefficient     �  nominator     �  denominator   �  coefficient     surface_color       outgoing_angle      geometry        param       param       fresnel     param       param       diffuse_component     $  nominator     )  denominator   ,  specular_component    [  Wind      [      UPPER_SKY_COLOR   [     LOWER_SKY_COLOR   [     SKY_MODE      [     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     ]        w  SKY_TEXTURE   �  vertex_index_0   
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   	 �  gl_InstanceCustomIndexNV      �  gl_PrimitiveID    �  vertex_data_0_0  
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS   �  vertex_data_0_1   �  vertex_data_0_2   �  vertex_index_1    �  vertex_data_1_0   �  vertex_data_1_1   �  vertex_data_1_2   �  vertex_index_2    �  vertex_data_2_0   �  vertex_data_2_1   �  vertex_data_2_2   �  barycentric_coordinates   �  HIT_ATTRIBUTE     �  hit_vertex_information      gl_ObjectToWorldNV   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �        �  hit_vertex_information    �  hit_material_index    �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  albedo_thickness     	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  param     �  normal_map_displacement   �  param     �  material_properties   �  param     �  hit_position      �  gl_WorldRayOriginNV   �  gl_WorldRayDirectionNV    �  gl_HitTNV     �  RADIANCE        VISIBILITY      ray_tracing_flags       TOP_LEVEL_ACCELERATION_STRUCTURE     	   incoming_diffuse_irradiance     param       param        param     !  param     $  param     '  param     *  param     -  param     0  param     3  param     5  param     9  i     C  light     D  param     G  light_radiance    Q  ray_tracing_flags     Z  param     [  param     ^  param     a  param     d  param     g  param     j  param     {  TEXTURES        BLUE_NOISE_TEXTURES   �  Camera   	 �      WORLD_TO_CLIP_MATRIX     	 �     WORLD_TO_CAMERA_MATRIX    �     PREVIOUS_WORLD_TO_CLIP_MATRIX     �     INVERSE_WORLD_TO_CAMERA_MATRIX    �     INVERSE_CAMERA_TO_CLIP_MATRIX    	 �     CAMERA_WORLD_POSITION    	 �     CAMERA_FORWARD_VECTOR    	 �     CURRENT_FRAME_JITTER      �     NEAR_PLANE    �  	   FAR_PLANE     �        �  General  	 �      FULL_MAIN_RESOLUTION      �     INVERSE_FULL_MAIN_RESOLUTION     	 �     HALF_MAIN_RESOLUTION      �     INVERSE_HALF_MAIN_RESOLUTION      �     FRAME     �     VIEW_DISTANCE     �       	 �  HammersleyHemisphereSamples  
 �      HAMMERSLEY_COSINUS_SAMPLES   
 �     HAMMERSLEY_UNIFORM_SAMPLES    �        �  Irradiance    �      IRRADIANCE_HEMISPHERE_SAMPLES     �        �  SAMPLER   �  SceneFeatures2    �  SceneFeatures3    �  SpecularIrradiance   
 �  MaskedModels_VERTEX_DATA_BUFFER  
 �      MaskedModels_VERTEX_DATA     	 �  MaskedModels_VERTEX_BUFFERS  
 �  MaskedModels_INDEX_DATA_BUFFER   	 �      MaskedModels_INDEX_DATA  	 �  MaskedModels_INDEX_BUFFERS   
 �  MaskedModels_MATERIAL_BUFFER      �      MaskedModels_MATERIAL_INDICES     �      H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      H  [      #       H  [     #      H  [     #       H  [     #   $   G  [     G  ]  "      G  ]  !      G  w  "       G  w  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �     �  G  �        G  �        H  �      #       G  �     G  �  "      G  �  !      G       �  G  �        H  �      #       G  �     G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �        H  �      #       G  �     G  �  "       G  �  !      G  �     �  G  �     �  G  �     �  G  �         G          G    "      G    !       G  {  "       G  {  !       G    "       G    !      H  �         H  �      #       H  �            H  �        H  �     #   @   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #      H  �           H  �     #   @  H  �     #   P  H  �     #   `  H  �     #   h  H  �  	   #   l  G  �     G  �  "      G  �  !       H  �      #       H  �     #      H  �     #      H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      G  �        G  �        H  �      #       H  �     #      G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !   	   G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	         
   	      !     
           	       
                   	   	   	   !                    	   !     	                     !     	               !  #            !  (                     ! 
 0                           !  B                                   R   	        S            R   !  T   S   !  W         Z      
   +     ]   �   +  	   `     C+     b         e          +  e   f      +     k      +  e   n      +     s      +  e   v      +     {        �           �   
     �   �   �      �      �   ;  �   �      +  e   �      +     �         �      
      �         +  e   �       +  e   �      +  e   �      +  e   �      +  e   �      +  e   �      +  e   �      +  	   �     �@+  	   �     �?+  	   �   �I@+  	   �       +  	        A    ,     3  �   �   �   +  	   8    �@+  	   ?     ?+  	   @     @+  	   l  
�#=,     m  l  l  l  ,     �  �   �   �   +  	   X    �B,     Y  X  �   �     [           	      \     [  ;  \  ]        ^           g        ,     m  �   �   �    	 t  	                           u  t     v      u  ;  v  w        �       �  �    �  �     �     �  ;  �  �        �     e   ;  �  �     ;  �  �       �  
     �  �    �  �     �     �  ;  �  �        �  �     ;  �  �  �     �  �  	      �     S                       ;            �     R     �          �  �    �  �     �     �  ;  �  �      
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �     +     �     +     �       �  
   �    �  �     �     �  ;  �  �     +     �     +     �         �        ;  �  �     ;  �  �        �     	   ;  �  �        �  �     ;  �  �  �           	        �  	   ;      �  +     	     � 
           
  ;          +  	       �5+  	     �� 	 x  	                           y  x  �     z      y  ;  z  {        |  x    }  |  �     ~      }  ;  ~          �  
        �  �  �  �  �  �        R   	   	      �     �  ;  �  �       �  R   R   R   R      	      �     �  ;  �  �     +     �  @     �  
   �    �  
   �    �  �  �     �     �  ;  �  �       �  
   �    �  �     �     �  ;  �  �       �     �      �  ;  �  �       	 �  	                           �      �  ;  �  �      ;  �  �      ;  �  �        �  
     �  �    �  �  k      �     �  ;  �  �       �       �  �    �  �  k      �     �  ;  �  �       �  �    �  �     �     �  ;  �  �          w     6               �     ;  �  �     ;     �     ;  Z   �     ;     �     ;  Z   �     ;     �     ;  Z   �     ;     �     ;     �     ;          ;          ;          ;          ;           ;     !     ;     $     ;     '     ;     *     ;     -     ;     0     ;     3     ;     5     ;     9     ;  �   C     ;     D     ;     G     ;     Q     ;     Z     ;     [     ;     ^     ;     a     ;     d     ;     g     ;     j          y      9  S   �  U   >  �  �       z      9     �  X   >  �  �       |      =     �  �  A  ^  �  �  �   �  �   =     �  �  �     �  �  �  �    �  �  �  �  �      �  �  �  �  �  �       |      =     �  �  A  ^  �  �  �   �  �   =     �  �  A  �   �  �  �   �  =  
   �  �  >  �  �  �  �  �  �       |      =     �  �  A  ^  �  �  �   �  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       ~      =     �  �  A  ^  �  �  �   �  �   =     �  �  �     �  �  �  �    �  �  �  �  �      �  �  �  �  �  �       ~      =     �  �  A  ^  �  �  �   �  �   =     �  �  A  �   �  �  �   �  =  
   �  �  >  �  �  �  �  �  �       ~      =     �  �  A  ^  �  �  �   �  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       �      =     �  �  A  ^  �  �  �   �  �   =     �  �  �     �  �  �  �    �  �  �  �  �      �  �  �  �  �  �       �      =     �  �  A  ^  �  �  �   �  �   =     �  �  A  �   �  �  �   �  =  
   �  �  >  �  �  �  �  �  �       �      =     �  �  A  ^  �  �  �   �  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       �      =     �  �  =     �  �  =  	   �  �  �     �  �  �  �     �  �  �  >  �  �       �      >  �  �       �      =  
   �  �  O     �  �  �            A     �  �  {   =  	   �  �  �     �  �  �  =     �  �  A       �  �   �  �   =  	       �       �    =       �  �           >  �         �      >    �        �      >    	       �      =  
      =         =       �  A       �  �   =         �     ]   b   b   k           �        �      A       �  �   =         >      A       ]  �   =  	       >      9       P       =  	       �           >           �      =       �                �      >       =  
   "  �  O     #  "  "            >  !  #  A     %  �  �   =     &  %  >  $  &  A     (  �  b   =  	   )  (  >  '  )  A     +  �  k   =  	   ,  +  >  *  ,  A     .  �  s   =  	   /  .  >  -  /  A     1  �  {   =  	   2  1  >  0  2  =     4    >  3  4  >  5  �  9     6  L      !  $  '  *  -  0  3  5       �      =     7  �  �     8  7  6  >  �  8       �      >  9  b   �  :  �  :       �      �  <  =      �  >  �  >       �      =     ?  9  A  ^  @  �   �   �   =     A  @  �    B  ?  A  �  B  ;  <  �  ;       �      =     E  9  >  D  E  9     F     D  >  C  F       �      A     H  C  �   =     I  H  A     J  C  �   =  	   K  J  �     L  I  K  >  G  L       �      A     M  C  �   =     N  M  �  P      �  N  P      O  �  O       �      >    �        �      >  Q  	       �      =  
  R    =     S  Q  =     T  �       �      A     U  C  �   =     V  U       W  V       �      � R  S  ]   b   b   k   T    W    �        �      =     X  �       Y  X       �      >  Z  Y  =  
   \  �  O     ]  \  \            >  [  ]  A     _  �  �   =     `  _  >  ^  `  A     b  �  b   =  	   c  b  >  a  c  A     e  �  k   =  	   f  e  >  d  f  A     h  �  {   =  	   i  h  >  g  i  A     k  C  �   =     l  k  >  j  l  9     m  8   Z  [  ^  a  d  g  j       �      =     n  G  �     o  m  n  =  	   p    �     q  o  p       �      =     r  �  �     s  r  q  >  �  s       �      �  P  �  P  �  =  �  =       �      =     v  9  �     w  v  �   >  9  w  �  :  �  <  �  8       �      6  
             7        �     ;  Z   [                 =     \      �     ^   \   ]   p  	   _   ^   �  	   a   _   `   A     c   [   b   >  c   a              =     d      �     g   d   f   �     h   g   ]   p  	   i   h   �  	   j   i   `   A     l   [   k   >  l   j              =     m      �     o   m   n   �     p   o   ]   p  	   q   p   �  	   r   q   `   A     t   [   s   >  t   r              =     u      �     w   u   v   �     x   w   ]   p  	   y   x   �  	   z   y   `   A     |   [   {   >  |   z              =  
   }   [   �  }   8       �     6               7        �     ;  Z   �      ;  Z   �      ;  Z   �      ;  Z   �      ;  �   �           �      =     �      �     �   �   �   �     �   �   b   A  �   �   �   �   �   =  
   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   k   A  �   �   �   �   �   =  
   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   s   A  �   �   �   �   �   =  
   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   {   A  �   �   �   �   �   =  
   �   �   >  �   �        �      A     �   �   b   =  	   �   �   A     �   �   k   =  	   �   �   A     �   �   s   =  	   �   �   P     �   �   �   �   A     �   �   �   >  �   �        �      A     �   �   {   =  	   �   �   A     �   �   b   =  	   �   �   A     �   �   k   =  	   �   �   P     �   �   �   �   A     �   �   �   >  �   �        �      A     �   �   s   =  	   �   �   A     �   �   {   =  	   �   �   A     �   �   b   =  	   �   �   P     �   �   �   �   A     �   �   �   >  �   �        �      A     �   �   k   =  	   �   �   |     �   �   A     �   �   �   >  �   �        �      A     �   �   s   =  	   �   �   |     �   �   A     �   �   �   >  �   �        �      A     �   �   {   =  	   �   �   A     �   �   �   >  �   �        �      A     �   �   b   =  	   �   �   A     �   �   �   >  �   �        �      A     �   �   k   =  	   �   �   A     �   �   �   >  �   �        �      =     �   �   �  �   8       
  =   6  	             7        7        �     ;     �      ;     �      ;     �      ;     �                 =  	   �        	   �         �   �   >  �   �              =  	   �      =  	   �      �  	   �   �   �   >  �   �              =  	   �   �   >  �   �              =  	   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   �  	   �   �   �   >  �   �              =  	   �   �   �  	   �   �   �   =  	   �   �   �  	   �   �   �   >  �   �              =  	   �   �   =  	   �   �   �  	   �   �   �   �  �   8         ^   6  	   !          7        7        7        7         �  "   ;     �      ;     �      ;     �      ;          ;          ;          ;          ;          ;          ;     "     ;     %                =     �      =     �      �  	   �   �   �     	   �      (   �   �   >  �   �        "      =     �      =     �           �   �   �  	   �   �   �     	   �      (   �   �   >  �   �        %      =  	   �       �  	   �   �   �   >  �   �        &      =  	   �   �   =  	      �   �  	     �      �  	         >  �          -      =  	     �   >           0      =  	     �   =  	     �   �  	   	  �     �  	   
    	  =  	     �   �  	     
    >           2      =  	       �        �   �        �        �         2      =  	       =  	       �  	         >      �    �    >    �   �    �    =  	            2      >           :      =  	     �   >           =      =  	     �   =  	     �   �  	     �     �  	         =  	      �   �  	   !       >    !       ?      =  	   #    �    $  #  �   �  '      �  $  &  +  �  &       ?      =  	   (    =  	   )    �  	   *  (  )  >  %  *  �  '  �  +  >  %  �   �  '  �  '  =  	   ,  %       ?      >  "  ,       C      =  	   -    =  	   .  "  �  	   /  -  .  �  /  8       J  8   6     &       #   7     $   7     %   �  '        M      =     2  $   =     4  $   �     5  3  4  =  	   6  %   �  	   7  �   6    	   9        7  8  �     :  5  9  �     ;  2  :  �  ;  8       \  t   6     .       (   7     )   7     *   7     +   7     ,   7     -   �  /   ;     >     ;     H     ;     L     ;     U          _      =  	   A  *   �  	   B  @  A  =  	   C  +     	   D        C    	   E        D  @  �  	   F  B  E  �  	   G  ?  F  >  >  G       b      =     I  )   P     J  �   �   �   �     K  I  J  >  H  K       c      =  	   M  >  �  	   N  M  �   =  	   O  ,     	   P        O  �  	   Q  �   P    	   R        Q  8  �  	   S  N  R  �  	   T  �   S  >  L  T       d      =  	   V  >  �  	   W  V  �   =  	   X  -     	   Y        X  �  	   Z  �   Y    	   [        Z  8  �  	   \  W  [  �  	   ]  �   \  >  U  ]       f      =     ^  H  =  	   _  L  �     `  ^  _  =  	   a  U  �     b  `  a  �  b  8       �     6     8       0   7     1   7     2   7     3   7     4   7     5   7     6   7     7   �  9   ;     e     ;     k     ;     r     ;     w     ;     }     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �          �      =     f  1   =     g  7        h  g  �     i  f  h       j     E   i  >  e  j       �      =     n  2   =  	   o  5   P     p  o  o  o       q     .   m  n  p  >  k  q       �      =     s  3   =     t  1   �  	   u  s  t    	   v     (   u  �   >  r  v       �      =     x  3   =     y  7        z  y  �  	   {  x  z    	   |     (   {  �   >  w  |       �      =     ~  3   =       e  �  	   �  ~      	   �     (   �  �   >  }  �       �      =     �  7        �  �  =     �  e  �  	   �  �  �    	   �     (   �  �   >  �  �       �      =  	   �  4   >  �  �  =  	   �  }  >  �  �  9  	   �     �  �  >  �  �       �      =     �  3   >  �  �  =     �  1   >  �  �  =     �  7   >  �  �  =  	   �  4   >  �  �  9  	   �  !   �  �  �  �  >  �  �       �      =     �  k  >  �  �  =  	   �  �  >  �  �  9     �  &   �  �  >  �  �       �      =     �  2   >  �  �  =  	   �  4   >  �  �  =  	   �  �  >  �  �  =  	   �  w  >  �  �  =  	   �  r  >  �  �  9 	    �  .   �  �  �  �  �  >  �  �       �      =     �  �  P     �  �   �   �   �     �  �  �  =  	   �  5   �  	   �  �   �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      =  	   �  �  P     �  �  �  �  =  	   �  �  P     �  �  �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      =  	   �  r  �  	   �  �   �  =  	   �  w  �  	   �  �  �  >  �  �       �      =  	   �  �  �    �  �  �   �  �      �  �  �  �  �  �       �      =     �  �  =  	   �  �  P     �  �  �  �  �     �  �  �  >  �  �  �  �  �  �  >  �  �  �  �  �  �  =     �  �       �      >  �  �       �      =     �  3   =     �  7        �  �  �  	   �  �  �  >  �  �       �      =  	   �  �  �  	   �  �  ?  �  	   �  �  ?    	   �     +   �  �   �   =  	   �  �    	   �     (   �  �   =  	   �  6     	   �     .   �  �  �  >  �  �       �      =     �  �  =     �  �  �     �  �  �  =  	   �  �  �     �  �  �  �  �  8       �  =   6  	   <          7     :   7     ;   �  =   ;     �     ;     �     ;     �     ;     �     ;     �          �      =  	   �  :   =  	   �  :   �  	   �  �  �  �  	   �  �  ?  >  �  �       �      =  	   �  ;   >  �  �       �      =  	   �  ;   =  	   �  �  �  	   �  �   �  �  	   �  �  �  =  	   �  �  �  	   �  �  �  >  �  �       �      =  	   �  �  �    �  �  �   �  �      �  �  �  �  �  �       �      =  	   �  �  =  	   �  �  �  	   �  �  �  >  �  �  �  �  �  �  >  �  �   �  �  �  �  =  	   �  �       �      >  �  �       �      =  	   �  �  �  �  8       �  >   6     @       #   7     >   7     ?   �  A        �      =     �  >   =     �  >   �     �  3  �  =  	   �  ?   �  	      �   �    	              8  �       �    �       �    �    8       �     6     L       B   7     C   7     D   7     E   7     F   7     G   7     H   7     I   7     J   7     K   �  M   ;          ;          ;          ;          ;          ;          ;          ;          ;          ;     $     ;     )     ;     ,     ;     /          �      =       D   =  	     G   P     	             
     .   m    	  >    
       �      =       E   =       C   �  	           	        (     �   >           �      =  	     F   >      =  	       >      9  	     <       >                 =         >      =  	       >      9       @       >                 =         �       3    =  	     G   �  	      �     �     !       =     "  D   �     #  !  "  >    #             =  	   %    P     &  %  %  %  =     '    �     (  &  '  >  $  (             =  	   *    �  	   +  �   *  >  )  +             =  	   -  )  �    .  -  �   �  1      �  .  0  6  �  0             =     2  $  =  	   3  )  P     4  3  3  3  �     5  2  4  >  /  5  �  1  �  6  >  /  �  �  1  �  1  =     7  /             >  ,  7             =     8    =     9  J   �     :  8  9  =  	   ;  H   �     <  :  ;  =     =  ,  =     >  K   �     ?  =  >  =  	   @  H   �     A  ?  @  �     B  <  A  �  B  8       !  /   6     P       #   7     N   7     O   �  Q        $      A     E  N   b   =  	   F  E  �    G  F  �    H  G  �  J      �  H  I  J  �  I       $      A     K  N   k   =  	   L  K  �    M  L  �  J  �  J  �    N  G  Q   M  I  �    O  N  �  Q      �  O  P  Q  �  P       $      A     R  N   s   =  	   S  R  �    T  S  �  Q  �  Q  �    U  N  J  T  P  �  W      �  U  V  W  �  V       &      �  Y  �  W       )      A  ^  _  ]  �   =     `  _  �  e      � 	 `  d      a     b     c  �  d       >      �  �  �  a       .      �  �  �  b       3      A  g  h  ]  �   =     i  h  A  g  j  ]  �   =     k  j  =     l  N   �  	   n  l  m  �  	   o  n  ?  �  	   p  o  ?  P     q  p  p  p       r     .   i  k  q  �  r  �  c       8      =  u  x  w  =     y  N   =  	   z  O   X  
   {  x  y     z  O     |  {  {            �  |  �  e  �  8       T  .   6  S   U       T   �  V   ;     �     ;  Z   �     ;  Z   �     ;  Z   �     ;     �     ;  Z   �     ;  Z   �     ;  Z   �     ;     �     ;  Z   �     ;  Z   �     ;  Z   �     ;     �     ;  �  �          W      =  e   �  �  =  e   �  �  �  e   �  �  �   �  e   �  �  �   A  ^  �  �  �  �   �  =     �  �  >  �  �       X      =  e   �  �  =     �  �  �     �  {   �  �     �  �  b   A  �   �  �  �  �   �  =  
   �  �  >  �  �       Y      =  e   �  �  =     �  �  �     �  {   �  �     �  �  k   A  �   �  �  �  �   �  =  
   �  �  >  �  �       Z      =  e   �  �  =     �  �  �     �  {   �  �     �  �  s   A  �   �  �  �  �   �  =  
   �  �  >  �  �       [      =  e   �  �  =  e   �  �  �  e   �  �  �   �  e   �  �  �   A  ^  �  �  �  �   �  =     �  �  >  �  �       \      =  e   �  �  =     �  �  �     �  {   �  �     �  �  b   A  �   �  �  �  �   �  =  
   �  �  >  �  �       ]      =  e   �  �  =     �  �  �     �  {   �  �     �  �  k   A  �   �  �  �  �   �  =  
   �  �  >  �  �       ^      =  e   �  �  =     �  �  �     �  {   �  �     �  �  s   A  �   �  �  �  �   �  =  
   �  �  >  �  �       _      =  e   �  �  =  e   �  �  �  e   �  �  �   �  e   �  �  �   A  ^  �  �  �  �   �  =     �  �  >  �  �       `      =  e   �  �  =     �  �  �     �  {   �  �     �  �  b   A  �   �  �  �  �   �  =  
   �  �  >  �  �       a      =  e   �  �  =     �  �  �     �  {   �  �     �  �  k   A  �   �  �  �  �   �  =  
   �  �  >  �  �       b      =  e   �  �  =     �  �  �     �  {   �  �     �  �  s   A  �   �  �  �  �   �  =  
   �  �  >  �  �       c      A  �  �  �  b   =  	   �  �  �  	   �  �   �  A  �  �  �  k   =  	   �  �  �  	   �  �  �  A  �  �  �  b   =  	   �  �  A  �  �  �  k   =  	   �  �  P     �  �  �  �  >  �  �       d      A     �  �  b   =  	   �  �  A     �  �  k   =  	   �  �  A     �  �  s   =  	   �  �  P     �  �  �  �  A     �  �  b   =  	   �  �  �     �  �  �  A     �  �  b   =  	   �  �  A     �  �  k   =  	      �  A       �  s   =  	       P       �       A       �  k   =  	       �           �       �    A       �  b   =  	   	    A     
  �  k   =  	     
  A       �  s   =  	       P       	      A       �  s   =  	       �           �           A       �  �   >           e      =        A       �  �   =         Q  	           Q  	          Q  	          P  
           �   �           A       �  �   >           f      A        �  {   =  	   !     A     "  �  b   =  	   #  "  A     $  �  k   =  	   %  $  P     &  !  #  %  A     '  �  b   =  	   (  '  �     )  &  (  A     *  �  {   =  	   +  *  A     ,  �  b   =  	   -  ,  A     .  �  k   =  	   /  .  P     0  +  -  /  A     1  �  k   =  	   2  1  �     3  0  2  �     4  )  3  A     5  �  {   =  	   6  5  A     7  �  b   =  	   8  7  A     9  �  k   =  	   :  9  P     ;  6  8  :  A     <  �  s   =  	   =  <  �     >  ;  =  �     ?  4  >  A     @  �  �   >  @  ?       g      =    A    A     B  �  �   =     C  B  Q  	   D  C      Q  	   E  C     Q  	   F  C     P  
   G  D  E  F  �   �     H  A  G       I     E   H  A     J  �  �   >  J  I       h      A     K  �  s   =  	   L  K  A     M  �  {   =  	   N  M  A     O  �  b   =  	   P  O  P     Q  L  N  P  A     R  �  b   =  	   S  R  �     T  Q  S  A     U  �  s   =  	   V  U  A     W  �  {   =  	   X  W  A     Y  �  b   =  	   Z  Y  P     [  V  X  Z  A     \  �  k   =  	   ]  \  �     ^  [  ]  �     _  T  ^  A     `  �  s   =  	   a  `  A     b  �  {   =  	   c  b  A     d  �  b   =  	   e  d  P     f  a  c  e  A     g  �  s   =  	   h  g  �     i  f  h  �     j  _  i  A     k  �  �   >  k  j       i      =    l    A     m  �  �   =     n  m  Q  	   o  n      Q  	   p  n     Q  	   q  n     P  
   r  o  p  q  �   �     s  l  r       t     E   s  A     u  �  �   >  u  t       j      A     v  �  k   =  	   w  v  A     x  �  s   =  	   y  x  P  R   z  w  y  A     {  �  b   =  	   |  {  �  R   }  z  |  A     ~  �  k   =  	     ~  A     �  �  s   =  	   �  �  P  R   �    �  A     �  �  k   =  	   �  �  �  R   �  �  �  �  R   �  }  �  A     �  �  k   =  	   �  �  A     �  �  s   =  	   �  �  P  R   �  �  �  A     �  �  s   =  	   �  �  �  R   �  �  �  �  R   �  �  �  A  �  �  �  �   >  �  �       k      =  S   �  �  �  �  8       n     6     X       W   �  Y        p      =  e   �  �  �  e   �  �  �   =  e   �  �  �  e   �  �  �   A  ^  �  �  �   �  �  =     �  �  �  �  8   �B�m��Q�     #     �        �    �  
  SPV_EXT_descriptor_indexing 
  SPV_NV_ray_tracing       GLSL.std.450               �     main    �  �    �  �  �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedSpecularIrradiance_RayClosestHit.glsl   �!   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2; 
layout (set = 1, binding = 8, rgba32f) uniform image2D SceneFeatures3; 
layout (set = 1, binding = 9, rgba32f) uniform image2D SpecularIrradiance; 

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

layout (location = 0) rayPayloadInNV vec3 RADIANCE; 

layout (location = 1) rayPayloadNV float VISIBILITY; 

void main()
{
    HitVertexInformation hit_vertex_information = GetHitVertexInformation();
    uint hit_material_index = GetHitMaterialIndex();
    vec4 albedo_thickness;
    EVALUATE_AVERAGE_ALBEDO_THICKNESS(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, albedo_thickness);
    vec4 normal_map_displacement;
    EVALUATE_AVERAGE_NORMAL_MAP_DISPLACEMENT(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, normal_map_displacement);
    vec4 material_properties;
    EVALUATE_AVERAGE_MATERIAL_PROPERTIES(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, material_properties);
    vec3 hit_position = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV;
    RADIANCE = vec3(0.0f, 0.0f, 0.0f);
    RADIANCE += albedo_thickness.rgb * material_properties.w * MATERIALS[hit_material_index]._EmissiveMultiplier;
    {
        vec3 incoming_diffuse_irradiance;
        {
            VISIBILITY = 0.0f;
            uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                        | gl_RayFlagsSkipClosestHitShaderNV;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	1, /*missIndex*/
	hit_position, /*origin*/
	FLOAT32_EPSILON * 8.0f, /*Tmin*/
	hit_vertex_information._Normal, /*direction*/
	FLOAT32_MAXIMUM, /*Tmax*/
	1 /*payload*/
);
            incoming_diffuse_irradiance = SampleSky(hit_vertex_information._Normal, MAXIMUM_SKY_TEXTURE_MIP_LEVEL) * VISIBILITY;
        }
        RADIANCE += CalculateIndirectLighting
        (
            -gl_WorldRayDirectionNV,
            albedo_thickness.rgb,
            hit_vertex_information._Normal,
            material_properties.x,
            material_properties.y,
            material_properties.z,
            albedo_thickness.w,
            incoming_diffuse_irradiance,
            vec3(0.0f)
        );
    }
    {
        for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
        {
	        Light light = UnpackLight(i);
            vec3 light_radiance = light._Color * light._Intensity;
            switch (light._LightType)
            {
                case LIGHT_TYPE_DIRECTIONAL:
                {
                    VISIBILITY = 0.0f;
                    uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                                | gl_RayFlagsSkipClosestHitShaderNV;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	1, /*missIndex*/
	hit_position, /*origin*/
	FLOAT32_EPSILON * 8.0f, /*Tmin*/
	-light._TransformData1, /*direction*/
	FLOAT32_MAXIMUM, /*Tmax*/
	1 /*payload*/
);
                    RADIANCE += BidirectionalReflectanceDistribution
                    (
                        -gl_WorldRayDirectionNV,
                        albedo_thickness.rgb,
                        hit_vertex_information._Normal,
                        material_properties.x,
                        material_properties.y,
                        albedo_thickness.w,
                        light._TransformData1
                    ) * light_radiance * VISIBILITY;
                    break;
                }
            }
        }
    }
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         UnpackColor(u1;      color        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index        Distribution(f1;f1;      roughness        microsurface_angle   	 !   Geometry(vf3;vf3;vf3;f1;         normal       outgoing_direction       radiance_direction        roughness     &   Fresnel(vf3;f1;   $   surface_color     %   difference_angle     
 .   DisneyDiffuse(vf3;f1;f1;f1;f1;    )   albedo    *   roughness     +   difference_angle      ,   radiance_angle    -   outgoing_angle    8   BidirectionalReflectanceDistribution(vf3;vf3;vf3;f1;f1;f1;vf3;    1   outgoing_direction    2   albedo    3   normal    4   roughness     5   metallic      6   thickness     7   radiance_direction    <   GeometryIndirect(f1;f1;   :   roughness     ;   outgoing_angle    @   FresnelIndirect(vf3;f1;   >   surface_color     ?   outgoing_angle    L   CalculateIndirectLighting(vf3;vf3;vf3;f1;f1;f1;f1;vf3;vf3;    C   outgoing_direction    D   albedo    E   normal    F   roughness     G   metallic      H   ambient_occlusion     I   thickness     J   diffuse_irradiance    K   specular_irradiance   P   SampleSky(vf3;f1;     N   direction     O   mip_level     S   HitVertexInformation      S       _Position     S      _Normal   S      _Tangent      S      _TextureCoordinate   	 U   GetHitVertexInformation(      X   GetHitMaterialIndex(      [   unpacked      �   light_data_1      �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   light_data_2      �   light_data_3      �   light_data_4      �   light     �   roughness_squared    	 �   microsurface_angle_squared    �   nominator     �   denominator  
 �   outgoing_direction_coefficient    �   irradiance_direction_coefficient      �   roughness_coefficient       nominator       denominator     first_coefficient       nominator       denominator   "  second_coefficient    >  FD90      H  term_1    L  term_2    U  term_3    e  microsurface_normal   k  surface_color     r  outgoing_angle    w  radiance_angle    }  microsurface_angle    �  difference_angle      �  distribution      �  param     �  param     �  geometry      �  param     �  param     �  param     �  param     �  fresnel   �  param     �  param     �  diffuse_component     �  param     �  param     �  param     �  param     �  param     �  nominator     �  denominator   �  specular_component    �  weakening_factor      �  roughness_coefficient     �  nominator     �  denominator   �  coefficient     surface_color       outgoing_angle      geometry        param       param       fresnel     param       param       diffuse_component     $  nominator     )  denominator   ,  specular_component    [  Wind      [      UPPER_SKY_COLOR   [     LOWER_SKY_COLOR   [     SKY_MODE      [     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     ]        w  SKY_TEXTURE   �  vertex_index_0   
 �  MaskedModels_INDEX_DATA_BUFFER   	 �      MaskedModels_INDEX_DATA  	 �  MaskedModels_INDEX_BUFFERS   	 �  gl_InstanceCustomIndexNV      �  gl_PrimitiveID    �  vertex_data_0_0  
 �  MaskedModels_VERTEX_DATA_BUFFER  
 �      MaskedModels_VERTEX_DATA     	 �  MaskedModels_VERTEX_BUFFERS   �  vertex_data_0_1   �  vertex_data_0_2   �  vertex_index_1    �  vertex_data_1_0   �  vertex_data_1_1   �  vertex_data_1_2   �  vertex_index_2    �  vertex_data_2_0   �  vertex_data_2_1   �  vertex_data_2_2   �  barycentric_coordinates   �  HIT_ATTRIBUTE     �  hit_vertex_information      gl_ObjectToWorldNV   
 �  MaskedModels_MATERIAL_BUFFER      �      MaskedModels_MATERIAL_INDICES     �        �  hit_vertex_information    �  hit_material_index    �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  albedo_thickness     	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  param     �  normal_map_displacement   �  param     �  material_properties   �  param     �  hit_position      �  gl_WorldRayOriginNV   �  gl_WorldRayDirectionNV    �  gl_HitTNV     �  RADIANCE        VISIBILITY      ray_tracing_flags       TOP_LEVEL_ACCELERATION_STRUCTURE     	   incoming_diffuse_irradiance     param       param        param     !  param     $  param     '  param     *  param     -  param     0  param     3  param     5  param     9  i     C  light     D  param     G  light_radiance    Q  ray_tracing_flags     Z  param     [  param     ^  param     a  param     d  param     g  param     j  param     {  TEXTURES        BLUE_NOISE_TEXTURES   �  Camera   	 �      WORLD_TO_CLIP_MATRIX     	 �     WORLD_TO_CAMERA_MATRIX    �     PREVIOUS_WORLD_TO_CLIP_MATRIX     �     INVERSE_WORLD_TO_CAMERA_MATRIX    �     INVERSE_CAMERA_TO_CLIP_MATRIX    	 �     CAMERA_WORLD_POSITION    	 �     CAMERA_FORWARD_VECTOR    	 �     CURRENT_FRAME_JITTER      �     NEAR_PLANE    �  	   FAR_PLANE     �        �  General  	 �      FULL_MAIN_RESOLUTION      �     INVERSE_FULL_MAIN_RESOLUTION     	 �     HALF_MAIN_RESOLUTION      �     INVERSE_HALF_MAIN_RESOLUTION      �     FRAME     �     VIEW_DISTANCE     �       	 �  HammersleyHemisphereSamples  
 �      HAMMERSLEY_COSINUS_SAMPLES   
 �     HAMMERSLEY_UNIFORM_SAMPLES    �        �  Irradiance    �      IRRADIANCE_HEMISPHERE_SAMPLES     �        �  SAMPLER   �  SceneFeatures2    �  SceneFeatures3    �  SpecularIrradiance   
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS  
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �      H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      H  [      #       H  [     #      H  [     #       H  [     #   $   G  [     G  ]  "      G  ]  !      G  w  "       G  w  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �     �  G  �        G  �        H  �      #       G  �     G  �  "      G  �  !      G       �  G  �        H  �      #       G  �     G  �  "      G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �        H  �      #       G  �     G  �  "       G  �  !      G  �     �  G  �     �  G  �     �  G  �         G          G    "      G    !       G  {  "       G  {  !       G    "       G    !      H  �         H  �      #       H  �            H  �        H  �     #   @   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #      H  �           H  �     #   @  H  �     #   P  H  �     #   `  H  �     #   h  H  �  	   #   l  G  �     G  �  "      G  �  !       H  �      #       H  �     #      H  �     #      H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      G  �        G  �        H  �      #       H  �     #      G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !   	   G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	         
   	      !     
           	       
                   	   	   	   !                    	   !     	                     !     	               !  #            !  (                     ! 
 0                           !  B                                   R   	        S            R   !  T   S   !  W         Z      
   +     ]   �   +  	   `     C+     b         e          +  e   f      +     k      +  e   n      +     s      +  e   v      +     {        �           �   
     �   �   �      �      �   ;  �   �      +  e   �      +     �         �      
      �         +  e   �       +  e   �      +  e   �      +  e   �      +  e   �      +  e   �      +  e   �      +  	   �     �@+  	   �     �?+  	   �   �I@+  	   �       +  	        A    ,     3  �   �   �   +  	   8    �@+  	   ?     ?+  	   @     @+  	   l  
�#=,     m  l  l  l  ,     �  �   �   �   +  	   X    �B,     Y  X  �   �     [           	      \     [  ;  \  ]        ^           g        ,     m  �   �   �    	 t  	                           u  t     v      u  ;  v  w        �       �  �    �  �     �     �  ;  �  �        �     e   ;  �  �     ;  �  �       �  
     �  �    �  �     �     �  ;  �  �        �  �     ;  �  �  �     �  �  	      �     S                       ;            �     R     �          �  �    �  �     �     �  ;  �  �      
 �                 	         +     �       �  �  �    �  �     �     �  ;  �  �     +     �     +     �       �  
   �    �  �     �     �  ;  �  �     +     �     +     �         �        ;  �  �     ;  �  �        �     	   ;  �  �        �  �     ;  �  �  �           	        �  	   ;      �  +     	     � 
           
  ;          +  	       �5+  	     �� 	 x  	                           y  x  �     z      y  ;  z  {        |  x    }  |  �     ~      }  ;  ~          �  
        �  �  �  �  �  �        R   	   	      �     �  ;  �  �       �  R   R   R   R      	      �     �  ;  �  �     +     �  @     �  
   �    �  
   �    �  �  �     �     �  ;  �  �       �  
   �    �  �     �     �  ;  �  �       �     �      �  ;  �  �       	 �  	                           �      �  ;  �  �      ;  �  �      ;  �  �        �  
     �  �    �  �  k      �     �  ;  �  �       �       �  �    �  �  k      �     �  ;  �  �       �  �    �  �     �     �  ;  �  �          w     6               �     ;  �  �     ;     �     ;  Z   �     ;     �     ;  Z   �     ;     �     ;  Z   �     ;     �     ;     �     ;          ;          ;          ;          ;           ;     !     ;     $     ;     '     ;     *     ;     -     ;     0     ;     3     ;     5     ;     9     ;  �   C     ;     D     ;     G     ;     Q     ;     Z     ;     [     ;     ^     ;     a     ;     d     ;     g     ;     j          y      9  S   �  U   >  �  �       z      9     �  X   >  �  �       |      =     �  �  A  ^  �  �  �   �  �   =     �  �  �     �  �  �  �    �  �  �  �  �      �  �  �  �  �  �       |      =     �  �  A  ^  �  �  �   �  �   =     �  �  A  �   �  �  �   �  =  
   �  �  >  �  �  �  �  �  �       |      =     �  �  A  ^  �  �  �   �  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       ~      =     �  �  A  ^  �  �  �   �  �   =     �  �  �     �  �  �  �    �  �  �  �  �      �  �  �  �  �  �       ~      =     �  �  A  ^  �  �  �   �  �   =     �  �  A  �   �  �  �   �  =  
   �  �  >  �  �  �  �  �  �       ~      =     �  �  A  ^  �  �  �   �  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       �      =     �  �  A  ^  �  �  �   �  �   =     �  �  �     �  �  �  �    �  �  �  �  �      �  �  �  �  �  �       �      =     �  �  A  ^  �  �  �   �  �   =     �  �  A  �   �  �  �   �  =  
   �  �  >  �  �  �  �  �  �       �      =     �  �  A  ^  �  �  �   �  �   =     �  �  >  �  �  9  
   �     �  >  �  �  �  �  �  �       �      =     �  �  =     �  �  =  	   �  �  �     �  �  �  �     �  �  �  >  �  �       �      >  �  �       �      =  
   �  �  O     �  �  �            A     �  �  {   =  	   �  �  �     �  �  �  =     �  �  A       �  �   �  �   =  	       �       �    =       �  �           >  �         �      >    �        �      >    	       �      =  
      =         =       �  A       �  �   =         �     ]   b   b   k           �        �      A       �  �   =         >      A       ]  �   =  	       >      9       P       =  	       �           >           �      =       �                �      >       =  
   "  �  O     #  "  "            >  !  #  A     %  �  �   =     &  %  >  $  &  A     (  �  b   =  	   )  (  >  '  )  A     +  �  k   =  	   ,  +  >  *  ,  A     .  �  s   =  	   /  .  >  -  /  A     1  �  {   =  	   2  1  >  0  2  =     4    >  3  4  >  5  �  9     6  L      !  $  '  *  -  0  3  5       �      =     7  �  �     8  7  6  >  �  8       �      >  9  b   �  :  �  :       �      �  <  =      �  >  �  >       �      =     ?  9  A  ^  @  �   �   �   =     A  @  �    B  ?  A  �  B  ;  <  �  ;       �      =     E  9  >  D  E  9     F     D  >  C  F       �      A     H  C  �   =     I  H  A     J  C  �   =  	   K  J  �     L  I  K  >  G  L       �      A     M  C  �   =     N  M  �  P      �  N  P      O  �  O       �      >    �        �      >  Q  	       �      =  
  R    =     S  Q  =     T  �       �      A     U  C  �   =     V  U       W  V       �      � R  S  ]   b   b   k   T    W    �        �      =     X  �       Y  X       �      >  Z  Y  =  
   \  �  O     ]  \  \            >  [  ]  A     _  �  �   =     `  _  >  ^  `  A     b  �  b   =  	   c  b  >  a  c  A     e  �  k   =  	   f  e  >  d  f  A     h  �  {   =  	   i  h  >  g  i  A     k  C  �   =     l  k  >  j  l  9     m  8   Z  [  ^  a  d  g  j       �      =     n  G  �     o  m  n  =  	   p    �     q  o  p       �      =     r  �  �     s  r  q  >  �  s       �      �  P  �  P  �  =  �  =       �      =     v  9  �     w  v  �   >  9  w  �  :  �  <  �  8       �      6  
             7        �     ;  Z   [                 =     \      �     ^   \   ]   p  	   _   ^   �  	   a   _   `   A     c   [   b   >  c   a              =     d      �     g   d   f   �     h   g   ]   p  	   i   h   �  	   j   i   `   A     l   [   k   >  l   j              =     m      �     o   m   n   �     p   o   ]   p  	   q   p   �  	   r   q   `   A     t   [   s   >  t   r              =     u      �     w   u   v   �     x   w   ]   p  	   y   x   �  	   z   y   `   A     |   [   {   >  |   z              =  
   }   [   �  }   8       �     6               7        �     ;  Z   �      ;  Z   �      ;  Z   �      ;  Z   �      ;  �   �           �      =     �      �     �   �   �   �     �   �   b   A  �   �   �   �   �   =  
   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   k   A  �   �   �   �   �   =  
   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   s   A  �   �   �   �   �   =  
   �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   {   A  �   �   �   �   �   =  
   �   �   >  �   �        �      A     �   �   b   =  	   �   �   A     �   �   k   =  	   �   �   A     �   �   s   =  	   �   �   P     �   �   �   �   A     �   �   �   >  �   �        �      A     �   �   {   =  	   �   �   A     �   �   b   =  	   �   �   A     �   �   k   =  	   �   �   P     �   �   �   �   A     �   �   �   >  �   �        �      A     �   �   s   =  	   �   �   A     �   �   {   =  	   �   �   A     �   �   b   =  	   �   �   P     �   �   �   �   A     �   �   �   >  �   �        �      A     �   �   k   =  	   �   �   |     �   �   A     �   �   �   >  �   �        �      A     �   �   s   =  	   �   �   |     �   �   A     �   �   �   >  �   �        �      A     �   �   {   =  	   �   �   A     �   �   �   >  �   �        �      A     �   �   b   =  	   �   �   A     �   �   �   >  �   �        �      A     �   �   k   =  	   �   �   A     �   �   �   >  �   �        �      =     �   �   �  �   8       
  =   6  	             7        7        �     ;     �      ;     �      ;     �      ;     �                 =  	   �        	   �         �   �   >  �   �              =  	   �      =  	   �      �  	   �   �   �   >  �   �              =  	   �   �   >  �   �              =  	   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   �  	   �   �   �   >  �   �              =  	   �   �   �  	   �   �   �   =  	   �   �   �  	   �   �   �   >  �   �              =  	   �   �   =  	   �   �   �  	   �   �   �   �  �   8         ^   6  	   !          7        7        7        7         �  "   ;     �      ;     �      ;     �      ;          ;          ;          ;          ;          ;          ;     "     ;     %                =     �      =     �      �  	   �   �   �     	   �      (   �   �   >  �   �        "      =     �      =     �           �   �   �  	   �   �   �     	   �      (   �   �   >  �   �        %      =  	   �       �  	   �   �   �   >  �   �        &      =  	   �   �   =  	      �   �  	     �      �  	         >  �          -      =  	     �   >           0      =  	     �   =  	     �   �  	   	  �     �  	   
    	  =  	     �   �  	     
    >           2      =  	       �        �   �        �        �         2      =  	       =  	       �  	         >      �    �    >    �   �    �    =  	            2      >           :      =  	     �   >           =      =  	     �   =  	     �   �  	     �     �  	         =  	      �   �  	   !       >    !       ?      =  	   #    �    $  #  �   �  '      �  $  &  +  �  &       ?      =  	   (    =  	   )    �  	   *  (  )  >  %  *  �  '  �  +  >  %  �   �  '  �  '  =  	   ,  %       ?      >  "  ,       C      =  	   -    =  	   .  "  �  	   /  -  .  �  /  8       J  8   6     &       #   7     $   7     %   �  '        M      =     2  $   =     4  $   �     5  3  4  =  	   6  %   �  	   7  �   6    	   9        7  8  �     :  5  9  �     ;  2  :  �  ;  8       \  t   6     .       (   7     )   7     *   7     +   7     ,   7     -   �  /   ;     >     ;     H     ;     L     ;     U          _      =  	   A  *   �  	   B  @  A  =  	   C  +     	   D        C    	   E        D  @  �  	   F  B  E  �  	   G  ?  F  >  >  G       b      =     I  )   P     J  �   �   �   �     K  I  J  >  H  K       c      =  	   M  >  �  	   N  M  �   =  	   O  ,     	   P        O  �  	   Q  �   P    	   R        Q  8  �  	   S  N  R  �  	   T  �   S  >  L  T       d      =  	   V  >  �  	   W  V  �   =  	   X  -     	   Y        X  �  	   Z  �   Y    	   [        Z  8  �  	   \  W  [  �  	   ]  �   \  >  U  ]       f      =     ^  H  =  	   _  L  �     `  ^  _  =  	   a  U  �     b  `  a  �  b  8       �     6     8       0   7     1   7     2   7     3   7     4   7     5   7     6   7     7   �  9   ;     e     ;     k     ;     r     ;     w     ;     }     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �          �      =     f  1   =     g  7        h  g  �     i  f  h       j     E   i  >  e  j       �      =     n  2   =  	   o  5   P     p  o  o  o       q     .   m  n  p  >  k  q       �      =     s  3   =     t  1   �  	   u  s  t    	   v     (   u  �   >  r  v       �      =     x  3   =     y  7        z  y  �  	   {  x  z    	   |     (   {  �   >  w  |       �      =     ~  3   =       e  �  	   �  ~      	   �     (   �  �   >  }  �       �      =     �  7        �  �  =     �  e  �  	   �  �  �    	   �     (   �  �   >  �  �       �      =  	   �  4   >  �  �  =  	   �  }  >  �  �  9  	   �     �  �  >  �  �       �      =     �  3   >  �  �  =     �  1   >  �  �  =     �  7   >  �  �  =  	   �  4   >  �  �  9  	   �  !   �  �  �  �  >  �  �       �      =     �  k  >  �  �  =  	   �  �  >  �  �  9     �  &   �  �  >  �  �       �      =     �  2   >  �  �  =  	   �  4   >  �  �  =  	   �  �  >  �  �  =  	   �  w  >  �  �  =  	   �  r  >  �  �  9 	    �  .   �  �  �  �  �  >  �  �       �      =     �  �  P     �  �   �   �   �     �  �  �  =  	   �  5   �  	   �  �   �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      =  	   �  �  P     �  �  �  �  =  	   �  �  P     �  �  �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      =  	   �  r  �  	   �  �   �  =  	   �  w  �  	   �  �  �  >  �  �       �      =  	   �  �  �    �  �  �   �  �      �  �  �  �  �  �       �      =     �  �  =  	   �  �  P     �  �  �  �  �     �  �  �  >  �  �  �  �  �  �  >  �  �  �  �  �  �  =     �  �       �      >  �  �       �      =     �  3   =     �  7        �  �  �  	   �  �  �  >  �  �       �      =  	   �  �  �  	   �  �  ?  �  	   �  �  ?    	   �     +   �  �   �   =  	   �  �    	   �     (   �  �   =  	   �  6     	   �     .   �  �  �  >  �  �       �      =     �  �  =     �  �  �     �  �  �  =  	   �  �  �     �  �  �  �  �  8       �  =   6  	   <          7     :   7     ;   �  =   ;     �     ;     �     ;     �     ;     �     ;     �          �      =  	   �  :   =  	   �  :   �  	   �  �  �  �  	   �  �  ?  >  �  �       �      =  	   �  ;   >  �  �       �      =  	   �  ;   =  	   �  �  �  	   �  �   �  �  	   �  �  �  =  	   �  �  �  	   �  �  �  >  �  �       �      =  	   �  �  �    �  �  �   �  �      �  �  �  �  �  �       �      =  	   �  �  =  	   �  �  �  	   �  �  �  >  �  �  �  �  �  �  >  �  �   �  �  �  �  =  	   �  �       �      >  �  �       �      =  	   �  �  �  �  8       �  >   6     @       #   7     >   7     ?   �  A        �      =     �  >   =     �  >   �     �  3  �  =  	   �  ?   �  	      �   �    	              8  �       �    �       �    �    8       �     6     L       B   7     C   7     D   7     E   7     F   7     G   7     H   7     I   7     J   7     K   �  M   ;          ;          ;          ;          ;          ;          ;          ;          ;          ;     $     ;     )     ;     ,     ;     /          �      =       D   =  	     G   P     	             
     .   m    	  >    
       �      =       E   =       C   �  	           	        (     �   >           �      =  	     F   >      =  	       >      9  	     <       >                 =         >      =  	       >      9       @       >                 =         �       3    =  	     G   �  	      �     �     !       =     "  D   �     #  !  "  >    #             =  	   %    P     &  %  %  %  =     '    �     (  &  '  >  $  (             =  	   *    �  	   +  �   *  >  )  +             =  	   -  )  �    .  -  �   �  1      �  .  0  6  �  0             =     2  $  =  	   3  )  P     4  3  3  3  �     5  2  4  >  /  5  �  1  �  6  >  /  �  �  1  �  1  =     7  /             >  ,  7             =     8    =     9  J   �     :  8  9  =  	   ;  H   �     <  :  ;  =     =  ,  =     >  K   �     ?  =  >  =  	   @  H   �     A  ?  @  �     B  <  A  �  B  8       !  /   6     P       #   7     N   7     O   �  Q        $      A     E  N   b   =  	   F  E  �    G  F  �    H  G  �  J      �  H  I  J  �  I       $      A     K  N   k   =  	   L  K  �    M  L  �  J  �  J  �    N  G  Q   M  I  �    O  N  �  Q      �  O  P  Q  �  P       $      A     R  N   s   =  	   S  R  �    T  S  �  Q  �  Q  �    U  N  J  T  P  �  W      �  U  V  W  �  V       &      �  Y  �  W       )      A  ^  _  ]  �   =     `  _  �  e      � 	 `  d      a     b     c  �  d       >      �  �  �  a       .      �  �  �  b       3      A  g  h  ]  �   =     i  h  A  g  j  ]  �   =     k  j  =     l  N   �  	   n  l  m  �  	   o  n  ?  �  	   p  o  ?  P     q  p  p  p       r     .   i  k  q  �  r  �  c       8      =  u  x  w  =     y  N   =  	   z  O   X  
   {  x  y     z  O     |  {  {            �  |  �  e  �  8       T  .   6  S   U       T   �  V   ;     �     ;  Z   �     ;  Z   �     ;  Z   �     ;     �     ;  Z   �     ;  Z   �     ;  Z   �     ;     �     ;  Z   �     ;  Z   �     ;  Z   �     ;     �     ;  �  �          W      =  e   �  �  =  e   �  �  �  e   �  �  �   �  e   �  �  �   A  ^  �  �  �  �   �  =     �  �  >  �  �       X      =  e   �  �  =     �  �  �     �  {   �  �     �  �  b   A  �   �  �  �  �   �  =  
   �  �  >  �  �       Y      =  e   �  �  =     �  �  �     �  {   �  �     �  �  k   A  �   �  �  �  �   �  =  
   �  �  >  �  �       Z      =  e   �  �  =     �  �  �     �  {   �  �     �  �  s   A  �   �  �  �  �   �  =  
   �  �  >  �  �       [      =  e   �  �  =  e   �  �  �  e   �  �  �   �  e   �  �  �   A  ^  �  �  �  �   �  =     �  �  >  �  �       \      =  e   �  �  =     �  �  �     �  {   �  �     �  �  b   A  �   �  �  �  �   �  =  
   �  �  >  �  �       ]      =  e   �  �  =     �  �  �     �  {   �  �     �  �  k   A  �   �  �  �  �   �  =  
   �  �  >  �  �       ^      =  e   �  �  =     �  �  �     �  {   �  �     �  �  s   A  �   �  �  �  �   �  =  
   �  �  >  �  �       _      =  e   �  �  =  e   �  �  �  e   �  �  �   �  e   �  �  �   A  ^  �  �  �  �   �  =     �  �  >  �  �       `      =  e   �  �  =     �  �  �     �  {   �  �     �  �  b   A  �   �  �  �  �   �  =  
   �  �  >  �  �       a      =  e   �  �  =     �  �  �     �  {   �  �     �  �  k   A  �   �  �  �  �   �  =  
   �  �  >  �  �       b      =  e   �  �  =     �  �  �     �  {   �  �     �  �  s   A  �   �  �  �  �   �  =  
   �  �  >  �  �       c      A  �  �  �  b   =  	   �  �  �  	   �  �   �  A  �  �  �  k   =  	   �  �  �  	   �  �  �  A  �  �  �  b   =  	   �  �  A  �  �  �  k   =  	   �  �  P     �  �  �  �  >  �  �       d      A     �  �  b   =  	   �  �  A     �  �  k   =  	   �  �  A     �  �  s   =  	   �  �  P     �  �  �  �  A     �  �  b   =  	   �  �  �     �  �  �  A     �  �  b   =  	   �  �  A     �  �  k   =  	      �  A       �  s   =  	       P       �       A       �  k   =  	       �           �       �    A       �  b   =  	   	    A     
  �  k   =  	     
  A       �  s   =  	       P       	      A       �  s   =  	       �           �           A       �  �   >           e      =        A       �  �   =         Q  	           Q  	          Q  	          P  
           �   �           A       �  �   >           f      A        �  {   =  	   !     A     "  �  b   =  	   #  "  A     $  �  k   =  	   %  $  P     &  !  #  %  A     '  �  b   =  	   (  '  �     )  &  (  A     *  �  {   =  	   +  *  A     ,  �  b   =  	   -  ,  A     .  �  k   =  	   /  .  P     0  +  -  /  A     1  �  k   =  	   2  1  �     3  0  2  �     4  )  3  A     5  �  {   =  	   6  5  A     7  �  b   =  	   8  7  A     9  �  k   =  	   :  9  P     ;  6  8  :  A     <  �  s   =  	   =  <  �     >  ;  =  �     ?  4  >  A     @  �  �   >  @  ?       g      =    A    A     B  �  �   =     C  B  Q  	   D  C      Q  	   E  C     Q  	   F  C     P  
   G  D  E  F  �   �     H  A  G       I     E   H  A     J  �  �   >  J  I       h      A     K  �  s   =  	   L  K  A     M  �  {   =  	   N  M  A     O  �  b   =  	   P  O  P     Q  L  N  P  A     R  �  b   =  	   S  R  �     T  Q  S  A     U  �  s   =  	   V  U  A     W  �  {   =  	   X  W  A     Y  �  b   =  	   Z  Y  P     [  V  X  Z  A     \  �  k   =  	   ]  \  �     ^  [  ]  �     _  T  ^  A     `  �  s   =  	   a  `  A     b  �  {   =  	   c  b  A     d  �  b   =  	   e  d  P     f  a  c  e  A     g  �  s   =  	   h  g  �     i  f  h  �     j  _  i  A     k  �  �   >  k  j       i      =    l    A     m  �  �   =     n  m  Q  	   o  n      Q  	   p  n     Q  	   q  n     P  
   r  o  p  q  �   �     s  l  r       t     E   s  A     u  �  �   >  u  t       j      A     v  �  k   =  	   w  v  A     x  �  s   =  	   y  x  P  R   z  w  y  A     {  �  b   =  	   |  {  �  R   }  z  |  A     ~  �  k   =  	     ~  A     �  �  s   =  	   �  �  P  R   �    �  A     �  �  k   =  	   �  �  �  R   �  �  �  �  R   �  }  �  A     �  �  k   =  	   �  �  A     �  �  s   =  	   �  �  P  R   �  �  �  A     �  �  s   =  	   �  �  �  R   �  �  �  �  R   �  �  �  A  �  �  �  �   >  �  �       k      =  S   �  �  �  �  8       n     6     X       W   �  Y        p      =  e   �  �  �  e   �  �  �   =  e   �  �  �  e   �  �  �   A  ^  �  �  �   �  �  =     �  �  �  �  8  �      #     �        �    �  
  SPV_EXT_descriptor_indexing 
  SPV_NV_ray_tracing       GLSL.std.450               �     main    F   I   �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedSpecularIrradiance_RayAnyHit.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 7, rgba32f) uniform image2D SceneFeatures2; 
layout (set = 1, binding = 8, rgba32f) uniform image2D SceneFeatures3; 
layout (set = 1, binding = 9, rgba32f) uniform image2D SpecularIrradiance; 

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
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         UnpackColor(u1;      color        HitVertexInformation             _Position           _Normal         _Tangent            _TextureCoordinate   	    GetHitVertexInformation(         GetHitMaterialIndex(         unpacked      ?   vertex_index_0   
 A   MaskedModels_INDEX_DATA_BUFFER   	 A       MaskedModels_INDEX_DATA  	 D   MaskedModels_INDEX_BUFFERS   	 F   gl_InstanceCustomIndexNV      I   gl_PrimitiveID    Q   vertex_data_0_0  
 S   MaskedModels_VERTEX_DATA_BUFFER  
 S       MaskedModels_VERTEX_DATA     	 V   MaskedModels_VERTEX_BUFFERS   ^   vertex_data_0_1   e   vertex_data_0_2   l   vertex_index_1    t   vertex_data_1_0   {   vertex_data_1_1   �   vertex_data_1_2   �   vertex_index_2    �   vertex_data_2_0   �   vertex_data_2_1   �   vertex_data_2_2   �   barycentric_coordinates   �   HIT_ATTRIBUTE     �   hit_vertex_information    �   gl_ObjectToWorldNV   
 ^  MaskedModels_MATERIAL_BUFFER      ^      MaskedModels_MATERIAL_INDICES     `        j  hit_vertex_information    l  hit_material_index    n  Material      n      _Properties   n     _AlbedoThickness     	 n     _NormalMapDisplacement    n     _MaterialProperties   n     _Opacity      n     _EmissiveMultiplier   n     _Padding1     n     _Padding2     q  GlobalMaterials   q      MATERIALS     s        }  opacity   �  TEXTURES      �  SAMPLER   �  param    	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE   �  Camera   	 �      WORLD_TO_CLIP_MATRIX     	 �     WORLD_TO_CAMERA_MATRIX    �     PREVIOUS_WORLD_TO_CLIP_MATRIX     �     INVERSE_WORLD_TO_CAMERA_MATRIX    �     INVERSE_CAMERA_TO_CLIP_MATRIX    	 �     CAMERA_WORLD_POSITION    	 �     CAMERA_FORWARD_VECTOR    	 �     CURRENT_FRAME_JITTER      �     NEAR_PLANE    �  	   FAR_PLANE     �        �  General  	 �      FULL_MAIN_RESOLUTION      �     INVERSE_FULL_MAIN_RESOLUTION     	 �     HALF_MAIN_RESOLUTION      �     INVERSE_HALF_MAIN_RESOLUTION      �     FRAME     �     VIEW_DISTANCE     �       	 �  HammersleyHemisphereSamples  
 �      HAMMERSLEY_COSINUS_SAMPLES   
 �     HAMMERSLEY_UNIFORM_SAMPLES    �        �  Irradiance    �      IRRADIANCE_HEMISPHERE_SAMPLES     �        �  Wind      �      UPPER_SKY_COLOR   �     LOWER_SKY_COLOR   �     SKY_MODE      �     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     �        �  LightingHeader    �      _NumberOfLights   �     _MaximumNumberOfShadowCastingLights   �  Lighting      �      LIGHTING_HEADER   �     LIGHT_DATA    �        �  SceneFeatures2    �  SceneFeatures3    �  SpecularIrradiance    �  TOP_LEVEL_ACCELERATION_STRUCTURE     
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS  
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �      G  @         H  A       #       G  A      G  D   "      G  D   !      G  F      �  G  I         G  R         H  S       #       G  S      G  V   "      G  V   !      G  �      �  G  ]        H  ^      #       G  ^     G  `  "      G  `  !      H  n      #       H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      G  p         H  q      #       G  q     G  s  "       G  s  !      G  �  "       G  �  !       G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !      H  �         H  �      #       H  �            H  �        H  �     #   @   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #      H  �           H  �     #   @  H  �     #   P  H  �     #   `  H  �     #   h  H  �  	   #   l  G  �     G  �  "      G  �  !       H  �      #       H  �     #      H  �     #      H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      G  �        G  �        H  �      #       H  �     #      G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      H  �      #       H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      H  �      #       H  �     #      G  �        H  �      #       H  �     #      G  �     G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !   	   G  �  "      G  �  !       G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	         
   	      !     
           	           	                       !        !                 
   +        �   +  	        C+                !      	     $          +  $   %      +     *      +  $   -      +     2      +  $   5      +     :        @        A   @     B   A      C      B   ;  C   D         E      $   ;  E   F      +  $   H       ;  E   I      +  $   K         N           R   
     S   R     T   S      U      T   ;  U   V         [      
   +  $   p      +  $   �         �         +  	   �     �?   �   �     ;  �   �   �     �   �  	      �           �            �      �   ;  �   �      +  	   
         W          \          ]  \    ^  ]     _     ^  ;  _  `     +  $   b      
 n                 	         +     o       p  n  o    q  p     r     q  ;  r  s     +     w  @     y   	 ~  	                         +            �  ~       �      �  ;  �  �         �      ~    �     �      �  ;  �  �        �  ~  +  	   �     ?  �  
       �  �     �     �  ;  �  �     +     �        �  �  �     �      �  ;  �  �       	 �  	                           �  �     �      �  ;  �  �        �  
        �  �  �  �  �  �           	   	      �     �  ;  �  �       �                 	      �     �  ;  �  �       �  
   w    �  
   w    �  �  �     �     �  ;  �  �       �  
   w    �  �     �     �  ;  �  �       �           	      �     �  ;  �  �       �          �  
     �  �  �     �     �  ;  �  �      	 �  	                           �      �  ;  �  �      ;  �  �      ;  �  �      � �     �      �  ;  �  �        �  
     �  �    �  �  *      �     �  ;  �  �       �       �  �    �  �  *      �     �  ;  �  �       �  \    �  �     �     �  ;  �  �          s     6               �     ;  �   j     ;     l     ;  !   }     ;     �          u      9     k     >  j  k       v      9     m     >  l  m       x      =     t  l  A  N   u  s  H   t  H   =     v  u  �     x  v  w  �  y  z  x  w  �  |      �  z  {  �  �  {       x      =     �  l  A  N   �  s  H   �  b  =     �  �  A  �  �  �  �  =  ~  �  �  =  �  �  �  V  �  �  �  �  A  W  �  j  K   =     �  �  X  
   �  �  �     
  Q  	   �  �      >  }  �  �  |  �  �       x      =     �  l  A  N   �  s  H   �  b  =     �  �  >  �  �  9  
   �     �  Q  	   �  �      >  }  �  �  |  �  |       y      =  	   �  }  �  y  �  �  �  �  �      �  �  �  �  �  �       {      � �  �  �  �  �  8       �      6  
             7        �     ;                      =           �              p  	         �  	            A  !   "          >  "                 =     #      �     &   #   %   �     '   &      p  	   (   '   �  	   )   (      A  !   +      *   >  +   )              =     ,      �     .   ,   -   �     /   .      p  	   0   /   �  	   1   0      A  !   3      2   >  3   1              =     4      �     6   4   5   �     7   6      p  	   8   7   �  	   9   8      A  !   ;      :   >  ;   9              =  
   <      �  <   8       T  .   6               �     ;     ?      ;     Q      ;     ^      ;     e      ;     l      ;     t      ;     {      ;     �      ;     �      ;     �      ;     �      ;     �      ;  �   �      ;  �   �           W      =  $   G   F   =  $   J   I   �  $   L   J   K   �  $   M   L   H   A  N   O   D   G   H   M   =     P   O   >  ?   P        X      =  $   W   F   =     X   ?   �     Y   :   X   �     Z   Y       A  [   \   V   W   H   Z   =  
   ]   \   >  Q   ]        Y      =  $   _   F   =     `   ?   �     a   :   `   �     b   a   *   A  [   c   V   _   H   b   =  
   d   c   >  ^   d        Z      =  $   f   F   =     g   ?   �     h   :   g   �     i   h   2   A  [   j   V   f   H   i   =  
   k   j   >  e   k        [      =  $   m   F   =  $   n   I   �  $   o   n   K   �  $   q   o   p   A  N   r   D   m   H   q   =     s   r   >  l   s        \      =  $   u   F   =     v   l   �     w   :   v   �     x   w       A  [   y   V   u   H   x   =  
   z   y   >  t   z        ]      =  $   |   F   =     }   l   �     ~   :   }   �        ~   *   A  [   �   V   |   H      =  
   �   �   >  {   �        ^      =  $   �   F   =     �   l   �     �   :   �   �     �   �   2   A  [   �   V   �   H   �   =  
   �   �   >  �   �        _      =  $   �   F   =  $   �   I   �  $   �   �   K   �  $   �   �   �   A  N   �   D   �   H   �   =     �   �   >  �   �        `      =  $   �   F   =     �   �   �     �   :   �   �     �   �       A  [   �   V   �   H   �   =  
   �   �   >  �   �        a      =  $   �   F   =     �   �   �     �   :   �   �     �   �   *   A  [   �   V   �   H   �   =  
   �   �   >  �   �        b      =  $   �   F   =     �   �   �     �   :   �   �     �   �   2   A  [   �   V   �   H   �   =  
   �   �   >  �   �        c      A  �   �   �       =  	   �   �   �  	   �   �   �   A  �   �   �   *   =  	   �   �   �  	   �   �   �   A  �   �   �       =  	   �   �   A  �   �   �   *   =  	   �   �   P     �   �   �   �   >  �   �        d      A  !   �   Q       =  	   �   �   A  !   �   Q   *   =  	   �   �   A  !   �   Q   2   =  	   �   �   P     �   �   �   �   A  !   �   �       =  	   �   �   �     �   �   �   A  !   �   t       =  	   �   �   A  !   �   t   *   =  	   �   �   A  !   �   t   2   =  	   �   �   P     �   �   �   �   A  !   �   �   *   =  	   �   �   �     �   �   �   �     �   �   �   A  !   �   �       =  	   �   �   A  !   �   �   *   =  	   �   �   A  !   �   �   2   =  	   �   �   P     �   �   �   �   A  !   �   �   2   =  	   �   �   �     �   �   �   �     �   �   �   A  �   �   �   H   >  �   �        e      =  �   �   �   A  �   �   �   p   =     �   �   Q  	   �   �       Q  	   �   �      Q  	   �   �      P  
   �   �   �   �   �   �     �   �   �   A  �   �   �   H   >  �   �        f      A  !   �   Q   :   =  	   �   �   A  !   �   ^       =  	   �   �   A  !   �   ^   *   =  	   �   �   P     �   �   �   �   A  !   �   �       =  	   �   �   �     �   �   �   A  !   �   t   :   =  	   �   �   A  !   �   {       =  	   �   �   A  !   �   {   *   =  	   �   �   P     �   �   �   �   A  !   �   �   *   =  	   �   �   �     �   �   �   �     �   �   �   A  !   �   �   :   =  	   �   �   A  !   �   �       =  	   �   �   A  !   �   �   *   =  	      �   P       �   �      A  !     �   2   =  	       �           �       �     A  �     �   p   >           g      =  �     �   A  �     �   p   =     	    Q  	     	      Q  	     	     Q  	     	     P  
           
  �                     E     A  �     �   p   >           h      A  !     ^   2   =  	       A  !     ^   :   =  	       A  !     e       =  	       P             A  !     �       =  	       �           A  !     {   2   =  	       A  !     {   :   =  	       A  !      �       =  	   !     P     "      !  A  !   #  �   *   =  	   $  #  �     %  "  $  �     &    %  A  !   '  �   2   =  	   (  '  A  !   )  �   :   =  	   *  )  A  !   +  �       =  	   ,  +  P     -  (  *  ,  A  !   .  �   2   =  	   /  .  �     0  -  /  �     1  &  0  A  �   2  �   �   >  2  1       i      =  �   3  �   A  �   4  �   �   =     5  4  Q  	   6  5      Q  	   7  5     Q  	   8  5     P  
   9  6  7  8  
  �     :  3  9       ;     E   :  A  �   <  �   �   >  <  ;       j      A  !   =  e   *   =  	   >  =  A  !   ?  e   2   =  	   @  ?  P     A  >  @  A  !   B  �       =  	   C  B  �     D  A  C  A  !   E  �   *   =  	   F  E  A  !   G  �   2   =  	   H  G  P     I  F  H  A  !   J  �   *   =  	   K  J  �     L  I  K  �     M  D  L  A  !   N  �   *   =  	   O  N  A  !   P  �   2   =  	   Q  P  P     R  O  Q  A  !   S  �   2   =  	   T  S  �     U  R  T  �     V  M  U  A  W  X  �   K   >  X  V       k      =     Y  �   �  Y  8       n     6               �          p      =  $   a  F   �  $   c  a  b  =  $   d  F   �  $   e  d  K   A  N   f  `  H   c  e  =     g  f  �  g  8         ��KT��D	(�D��^���l�a�B��XDi�D_w���       ,�'��       �%�9����Y��G�]�V��|1                                                    |g�[;�