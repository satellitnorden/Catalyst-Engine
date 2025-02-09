�nX/J14�O��:�DirectLighting_RenderPipeline                                    �d      #     ^                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\DirectLighting_Vertex.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 1) uniform Wind
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
layout (std430, set = 1, binding = 2) buffer Lighting
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
		float denominator = max(4.0f * max(dot(normal, outgoing_direction), 0.0f), 0.00001f);

		specular_component = nominator / denominator;
	}

	return (diffuse_component * diffuse_irradiance * ambient_occlusion) + (specular_component * specular_irradiance * ambient_occlusion);
}

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 6) uniform sampler2D INTERMEDIATE_RGBA_FLOAT32_HALF_1;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES      =   Material      =       _Properties   =      _AlbedoThickness     	 =      _NormalMapDisplacement    =      _MaterialProperties   =      _Opacity      =      _EmissiveMultiplier   =      _Padding1     =      _Padding2     @   GlobalMaterials   @       MATERIALS     B         G   BLUE_NOISE_TEXTURES   K   SKY_TEXTURE   N   Camera   	 N       WORLD_TO_CLIP_MATRIX     	 N      WORLD_TO_CAMERA_MATRIX    N      PREVIOUS_WORLD_TO_CLIP_MATRIX     N      INVERSE_WORLD_TO_CAMERA_MATRIX    N      INVERSE_CAMERA_TO_CLIP_MATRIX    	 N      CAMERA_WORLD_POSITION    	 N      CAMERA_FORWARD_VECTOR    	 N      CURRENT_FRAME_JITTER      N      NEAR_PLANE    N   	   FAR_PLANE     P         Q   Wind      Q       UPPER_SKY_COLOR   Q      LOWER_SKY_COLOR   Q      SKY_MODE      Q      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     S         T   LightingHeader    T       _NumberOfLights   T      _MaximumNumberOfShadowCastingLights   V   Lighting      V       LIGHTING_HEADER   V      LIGHT_DATA    X         Z   SceneFeatures1    [   SceneFeatures2    \   SceneFeatures3    ]   INTERMEDIATE_RGBA_FLOAT32_HALF_1    G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       H  =       #       H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      G  ?          H  @       #       G  @      G  B   "       G  B   !      G  G   "       G  G   !      G  K   "       G  K   !      H  N          H  N       #       H  N             H  N         H  N      #   @   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #      H  N            H  N      #   @  H  N      #   P  H  N      #   `  H  N      #   h  H  N   	   #   l  G  N      G  P   "      G  P   !       H  Q       #       H  Q      #      H  Q      #       H  Q      #   $   G  Q      G  S   "      G  S   !      H  T       #       H  T      #      G  U         H  V       #       H  V      #      G  V      G  X   "      G  X   !      G  Z   "      G  Z   !      G  [   "      G  [   !      G  \   "      G  \   !      G  ]   "      G  ]   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <        
 =   #   #   #   #   #      #   #   +  #   >        ?   =   >     @   ?      A      @   ;  A   B        C   8   +  #   D         E   C   D      F       E   ;  F   G        	 H                              I   H      J       I   ;  J   K         L   ,        M           N   L   L   L   L   L   M   M               O      N   ;  O   P        Q   M   M   #         R      Q   ;  R   S        T   #   #     U   ,     V   T   U      W      V   ;  W   X         Y       C   ;  Y   Z       ;  Y   [       ;  Y   \       ;  Y   ]            �     6               �     ;     	      ;                �      =           �              �              o           �        
      >  	           �      =           �              �              o           �        
      >             �      =        	   �               �     "       !   A  %   &      $   >  &   "        �      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        �      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  ت      #     �                GLSL.std.450                     main    �  t               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\DirectLighting_Fragment.glsl     �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 1) uniform Wind
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
layout (std430, set = 1, binding = 2) buffer Lighting
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
		float denominator = max(4.0f * max(dot(normal, outgoing_direction), 0.0f), 0.00001f);

		specular_component = nominator / denominator;
	}

	return (diffuse_component * diffuse_irradiance * ambient_occlusion) + (specular_component * specular_irradiance * ambient_occlusion);
}

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 6) uniform sampler2D INTERMEDIATE_RGBA_FLOAT32_HALF_1;

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
    vec3 lighting = vec3(0.0f);
    for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
    {
	    Light light = UnpackLight(i);
        vec3 light_radiance = light._Color * light._Intensity;
        switch (light._LightType)
        {
            case LIGHT_TYPE_DIRECTIONAL:
            {
                float shadow_factor = texture(INTERMEDIATE_RGBA_FLOAT32_HALF_1, InScreenCoordinate).x;
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
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index        Distribution(f1;f1;      roughness        microsurface_angle   	 #   Geometry(vf3;vf3;vf3;f1;         normal        outgoing_direction    !   radiance_direction    "   roughness     (   Fresnel(vf3;f1;   &   surface_color     '   difference_angle     
 0   DisneyDiffuse(vf3;f1;f1;f1;f1;    +   albedo    ,   roughness     -   difference_angle      .   radiance_angle    /   outgoing_angle    :   BidirectionalReflectanceDistribution(vf3;vf3;vf3;f1;f1;f1;vf3;    3   outgoing_direction    4   albedo    5   normal    6   roughness     7   metallic      8   thickness     9   radiance_direction    <   near_plane_coordinate     E   view_space_position   G   Camera   	 G       WORLD_TO_CLIP_MATRIX     	 G      WORLD_TO_CAMERA_MATRIX    G      PREVIOUS_WORLD_TO_CLIP_MATRIX     G      INVERSE_WORLD_TO_CAMERA_MATRIX    G      INVERSE_CAMERA_TO_CLIP_MATRIX    	 G      CAMERA_WORLD_POSITION    	 G      CAMERA_FORWARD_VECTOR    	 G      CURRENT_FRAME_JITTER      G      NEAR_PLANE    G   	   FAR_PLANE     I         Y   inverse_view_space_position_denominator   a   world_space_position      k   light_data_1      l   LightingHeader    l       _NumberOfLights   l      _MaximumNumberOfShadowCastingLights   n   Lighting      n       LIGHTING_HEADER   n      LIGHT_DATA    p         z   light_data_2      �   light_data_3      �   light_data_4      �   light     �   roughness_squared    	 �   microsurface_angle_squared    �   nominator     �   denominator  
 �   outgoing_direction_coefficient    �   irradiance_direction_coefficient      �   roughness_coefficient     �   nominator     �   denominator   �   first_coefficient       nominator       denominator     second_coefficient    )  FD90      2  term_1    6  term_2    ?  term_3    O  microsurface_normal   U  surface_color     \  outgoing_angle    a  radiance_angle    g  microsurface_angle    l  difference_angle      r  distribution      s  param     u  param     x  geometry      y  param     {  param     }  param       param     �  fresnel   �  param     �  param     �  diffuse_component     �  param     �  param     �  param     �  param     �  param     �  nominator     �  denominator   �  specular_component    �  weakening_factor      �  scene_features_1      �  SceneFeatures1    �  InScreenCoordinate    �  scene_features_2      �  SceneFeatures2    �  scene_features_3      �  SceneFeatures3    �  albedo    �  thickness     �  normal    �  depth     �  roughness     �  metallic      �  ambient_occlusion     �  emissive      �  world_position    �  param     �  param     �  view_direction      lighting        i       light       param       light_radiance      shadow_factor       INTERMEDIATE_RGBA_FLOAT32_HALF_1      $  param     &  param     (  param     *  param     ,  param     .  param     0  param     ?  distance_from_light   E  direction_from_light      M  attenuation   W  param     Y  param     [  param     ]  param     _  param     a  param     c  param     t  Scene     }  TEXTURES      ~  Material      ~      _Properties   ~     _AlbedoThickness     	 ~     _NormalMapDisplacement    ~     _MaterialProperties   ~     _Opacity      ~     _EmissiveMultiplier   ~     _Padding1     ~     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE   �  Wind      �      UPPER_SKY_COLOR   �     LOWER_SKY_COLOR   �     SKY_MODE      �     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     �      H  G          H  G       #       H  G             H  G         H  G      #   @   H  G            H  G         H  G      #   �   H  G            H  G         H  G      #   �   H  G            H  G         H  G      #      H  G            H  G      #   @  H  G      #   P  H  G      #   `  H  G      #   h  H  G   	   #   l  G  G      G  I   "      G  I   !       H  l       #       H  l      #      G  m         H  n       #       H  n      #      G  n      G  p   "      G  p   !      G  �  "      G  �  !      G  �         G  �  "      G  �  !      G  �  "      G  �  !      G    "      G    !      G  t         G  }  "       G  }  !       H  ~      #       H  ~     #      H  ~     #      H  ~     #      H  ~     #      H  ~     #      H  ~     #      H  ~     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !      H  �      #       H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !           !                               	            
                    !        	   
                             
                            !           !        
   
               !                 
   !  %         
   !  *         
   
   
   
   ! 
 2               
   
   
      +     >      @+     @     �?  C            D      C     F   C        G   F   F   F   F   F                     H      G   ;  H   I        J          +  J   K         L      F   +     Z      +  J   b        l           m   C     n   l   m      o      n   ;  o   p      +  J   q      +     s      +     u          w      C   +     }      +     �         �         +  J   �       +  J   �      +  J   �      +  J   �      +  J   �      +     �     �@+     �   �I@+     �       +     �      A  �   ,       @   @   @   +     #    �@+     *     ?+     V  
�#=,     W  V  V  V  ,     �  �   �   �    	 �                             �  �     �      �  ;  �  �         �        ;  �  �     ;  �  �      ;  �  �         �                   ;  �        +     8     >   s     C   ;  s  t     +     z       {  �  z     |      {  ;  |  }       
 ~                          +            �  ~      �  �     �     �  ;  �  �     +     �        �  �  �     �      �  ;  �  �       	 �                             �  �     �      �  ;  �  �        �                 �     �  ;  �  �          �     6               �     ;  D   �     ;  D   �     ;  D   �     ;     �     ;  
   �     ;     �     ;  
   �     ;  
   �     ;  
   �     ;  
   �     ;  
   �     ;     �     ;  	   �     ;  
   �     ;     �     ;          ;          ;  �        ;          ;          ;  
        ;     $     ;     &     ;     (     ;  
   *     ;  
   ,     ;  
   .     ;     0     ;  
   ?     ;     E     ;  
   M     ;  
   P     ;     W     ;     Y     ;     [     ;  
   ]     ;  
   _     ;  
   a     ;     c          �      =  �  �  �  =     �  �  W  C   �  �  �  >  �  �       �      =  �  �  �  =     �  �  W  C   �  �  �  >  �  �       �      =  �  �  �  =     �  �  W  C   �  �  �  >  �  �       �      =  C   �  �  O     �  �  �            >  �  �       �      A  
   �  �  Z   =     �  �  >  �  �       �      =  C   �  �  O     �  �  �            >  �  �       �      A  
   �  �  Z   =     �  �  >  �  �       �      A  
   �  �  u   =     �  �  >  �  �       �      A  
   �  �  }   =     �  �  >  �  �       �      A  
   �  �  �   =     �  �  >  �  �       �      A  
   �  �  Z   =     �  �  >  �  �       �      =     �  �  >  �  �  =     �  �  >  �  �  9     �     �  �  >  �  �       �      A  �  �  I   �   =     �  �  =        �  �       �               E     >  �         �      >    �       �      >    u   �    �         �      �          �  	  �  	       �      =     
    A      p   �   �   =         �  �     
    �        �         �      =         >      9            >           �      A         �   =         A  
       �   =         �           >           �      A         b   =         �        �                 �         �      =  �       =     !  �  W  C   "     !  Q     #  "      >    #       �      =     %  �  >  $  %  =     '  �  >  &  '  =     )  �  >  (  )  =     +  �  >  *  +  =     -  �  >  ,  -  =     /  �  >  .  /  A     1    �   =     2  1  >  0  2  9     3  :   $  &  (  *  ,  .  0       �      =     4    �     5  3  4  =     6    �     7  5  6  =     9  �       :     .   8  @   9  �     ;  7  :       �      =     <    �     =  <  ;  >    =       �      �    �         �      =     @  �  A     A    �   =     B  A  �     C  @  B       D     B   C  >  ?  D       �      =     F  �  A     G    �   =     H  G  �     I  F  H  =     J  ?  �     K  @   J  �     L  I  K  >  E  L       �      =     N  ?  �  �   O  N  �   �  R      �  O  Q  U  �  Q       �      =     S  ?  �     T  @   S  >  P  T  �  R  �  U  >  P  @   �  R  �  R  =     V  P       �      >  M  V       �      =     X  �  >  W  X  =     Z  �  >  Y  Z  =     \  �  >  [  \  =     ^  �  >  ]  ^  =     `  �  >  _  `  =     b  �  >  a  b  =     d  E  >  c  d  9     e  :   W  Y  [  ]  _  a  c       �      =     f    �     g  e  f  =     h  M  �     i  g  h  =     j  �       k     .   *  @   j  �     l  i  k       �      =     m    �     n  m  l  >    n       �      �    �    �    �         �      =     q    �     r  q  q   >    r  �    �         �      =     u    Q     v  u      Q     w  u     Q     x  u     P  C   y  v  w  x  @   >  t  y  �  8         @   6               7  	      7  
      �     ;  	   <      ;  D   E      ;  
   Y      ;  D   a                 =     =      �     ?   =   >   P     A   @   @   �     B   ?   A   >  <   B              A  L   M   I   K   =  F   N   M   =     O   <   =     P      Q     Q   O       Q     R   O      P     S   Q   R   P   Q     T   S       Q     U   S      Q     V   S      P  C   W   T   U   V   @   �  C   X   N   W   >  E   X              A  
   [   E   Z   =     \   [   �     ]   @   \   >  Y   ]              =     ^   Y   =  C   _   E   �  C   `   _   ^   >  E   `        	      A  L   c   I   b   =  F   d   c   =  C   e   E   �  C   f   d   e   >  a   f              =  C   g   a   O     h   g   g             �  h   8       Z     6               7        �     ;  D   k      ;  D   z      ;  D   �      ;  D   �      ;  �   �           ^      =     r      �     t   r   s   �     v   t   u   A  w   x   p   q   v   =  C   y   x   >  k   y        _      =     {      �     |   {   s   �     ~   |   }   A  w      p   q   ~   =  C   �      >  z   �        `      =     �      �     �   �   s   �     �   �   �   A  w   �   p   q   �   =  C   �   �   >  �   �        a      =     �      �     �   �   s   �     �   �   Z   A  w   �   p   q   �   =  C   �   �   >  �   �        c      A  
   �   k   u   =     �   �   A  
   �   k   }   =     �   �   A  
   �   k   �   =     �   �   P     �   �   �   �   A     �   �   �   >  �   �        d      A  
   �   k   Z   =     �   �   A  
   �   z   u   =     �   �   A  
   �   z   }   =     �   �   P     �   �   �   �   A     �   �   q   >  �   �        e      A  
   �   z   �   =     �   �   A  
   �   z   Z   =     �   �   A  
   �   �   u   =     �   �   P     �   �   �   �   A     �   �   �   >  �   �        f      A  
   �   �   }   =     �   �   |     �   �   A     �   �   b   >  �   �        g      A  
   �   �   �   =     �   �   |     �   �   A     �   �   K   >  �   �        h      A  
   �   �   Z   =     �   �   A  
   �   �   �   >  �   �        i      A  
   �   �   u   =     �   �   A  
   �   �   �   >  �   �        j      A  
   �   �   }   =     �   �   A  
   �   �   �   >  �   �        l      =     �   �   �  �   8       y  =   6               7  
      7  
      �     ;  
   �      ;  
   �      ;  
   �      ;  
   �           {      =     �           �         �   �   >  �   �        |      =     �      =     �      �     �   �   �   >  �   �        ~      =     �   �   >  �   �              =     �   �   =     �   �   �     �   �   @   �     �   �   �   �     �   �   @   >  �   �        �      =     �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �        �      =     �   �   =     �   �   �     �   �   �   �  �   8       �  ^   6     #          7        7         7     !   7  
   "   �  $   ;  
   �      ;  
   �      ;  
   �      ;  
   �      ;  
   �      ;  
   �      ;  
   �      ;  
        ;  
        ;  
        ;  
             �      =     �      =     �       �     �   �   �        �      (   �   �   >  �   �        �      =     �      =     �   !        �   �   �     �   �   �        �      (   �   �   >  �   �        �      =     �   "   �     �   �   @   >  �   �        �      =     �   �   =     �   �   �     �   �   �   �     �   �   �   >  �   �        �      =     �   �   >  �   �        �      =     �   �   =     �   �   �     �   @   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �        �      =     �   �   �  �   �   �   �   �  �       �  �   �     �  �        �      =     �   �   =        �   �       �      >  �     �  �   �    >  �   �   �  �   �  �   =       �        �      >  �          �      =       �   >           �      =       �   =       �   �     	  @     �     
    	  =       �   �       
    >           �      =         �  �       �   �        �        �         �      =         =         �           >      �    �    >    �   �    �    =              �      >           �      =       �   =         �           �    8       �  8   6     (       %   7     &   7  
   '   �  )        �      =       &   =       &   �            =     !  '   �     "  @   !       $        "  #  �     %     $  �     &    %  �  &  8       �  t   6     0       *   7     +   7  
   ,   7  
   -   7  
   .   7  
   /   �  1   ;  
   )     ;     2     ;  
   6     ;  
   ?          �      =     +  ,   �     ,  >   +  =     -  -        .        -       /        .  >   �     0  ,  /  �     1  *  0  >  )  1       �      =     3  +   P     4  �   �   �   �     5  3  4  >  2  5       �      =     7  )  �     8  7  @   =     9  .        :        9  �     ;  @   :       <        ;  #  �     =  8  <  �     >  @   =  >  6  >       �      =     @  )  �     A  @  @   =     B  /        C        B  �     D  @   C       E        D  #  �     F  A  E  �     G  @   F  >  ?  G       �      =     H  2  =     I  6  �     J  H  I  =     K  ?  �     L  J  K  �  L  8       �     6     :       2   7     3   7     4   7     5   7  
   6   7  
   7   7  
   8   7     9   �  ;   ;     O     ;     U     ;  
   \     ;  
   a     ;  
   g     ;  
   l     ;  
   r     ;  
   s     ;  
   u     ;  
   x     ;     y     ;     {     ;     }     ;  
        ;     �     ;     �     ;  
   �     ;     �     ;     �     ;  
   �     ;  
   �     ;  
   �     ;  
   �     ;     �     ;  
   �     ;     �     ;     �     ;  
   �          �      =     P  3   =     Q  9        R  Q  �     S  P  R       T     E   S  >  O  T       �      =     X  4   =     Y  7   P     Z  Y  Y  Y       [     .   W  X  Z  >  U  [       �      =     ]  5   =     ^  3   �     _  ]  ^       `     (   _  �   >  \  `       �      =     b  5   =     c  9        d  c  �     e  b  d       f     (   e  �   >  a  f       �      =     h  5   =     i  O  �     j  h  i       k     (   j  �   >  g  k       �      =     m  9        n  m  =     o  O  �     p  n  o       q     (   p  �   >  l  q              =     t  6   >  s  t  =     v  g  >  u  v  9     w     s  u  >  r  w             =     z  5   >  y  z  =     |  3   >  {  |  =     ~  9   >  }  ~  =     �  6   >    �  9     �  #   y  {  }    >  x  �             =     �  U  >  �  �  =     �  l  >  �  �  9     �  (   �  �  >  �  �             =     �  4   >  �  �  =     �  6   >  �  �  =     �  l  >  �  �  =     �  a  >  �  �  =     �  \  >  �  �  9 	    �  0   �  �  �  �  �  >  �  �             =     �  �  P     �  @   @   @   �     �  �  �  =     �  7   �     �  @   �  �     �  �  �  =     �  �  �     �  �  �  >  �  �             =     �  r  P     �  �  �  �  =     �  x  P     �  �  �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �             =     �  \  �     �  �   �  =     �  a  �     �  �  �  >  �  �             =     �  �  �  �   �  �  �   �  �      �  �  �  �  �  �             =     �  �  =     �  �  P     �  �  �  �  �     �  �  �  >  �  �  �  �  �  �  >  �  �  �  �  �  �  =     �  �             >  �  �             =     �  5   =     �  9        �  �  �     �  �  �  >  �  �             =     �  �  �     �  �  *  �     �  �  *       �     +   �  �   @   =     �  �       �     (   �  �   =     �  8        �     .   �  �  �  >  �  �             =     �  �  =     �  �  �     �  �  �  =     �  �  �     �  �  �  �  �  8                          ��KT��D	�D_w���       ,�'��               ��v)�  lig�%�9�� = li��Y��G�  lig��%��]j	 y; l       N1�8��;                               \�5s����