�nX/J1'��3RayTracedShadows_RenderPipeline                                    ��      #     �        �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main    �   �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedShadows_RayGeneration.glsl  U   �     // OpModuleProcessed entry-point main
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
    vec2 screen_coordinate = (vec2(gl_LaunchIDNV.xy) + vec2(0.5f)) / vec2(gl_LaunchSizeNV.xy);
    vec4 scene_features_2 = imageLoad(SceneFeatures2Half, ivec2(gl_LaunchIDNV.xy));
    vec3 shading_normal = scene_features_2.xyz;
    float depth = scene_features_2.w;
    vec3 world_position = CalculateWorldPosition(screen_coordinate, depth);
    vec4 shadows = vec4(1.0f);
    uint current_shadow_index = 0;
    for (uint light_index = 0; light_index < LIGHTING_HEADER._NumberOfLights; ++light_index)
    {
        Light light = UnpackLight(light_index);
        if (TEST_BIT(light._LightProperties, LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT))
        {
            switch (light._LightType)
            {
                case LIGHT_TYPE_DIRECTIONAL:
                {
                    VISIBILITY = 0.0f;
                    uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                                | gl_RayFlagsSkipClosestHitShaderNV;
                    vec3 direction = -light._TransformData1;
                    /*
                    *   This assumes the directional light is representing a sun.
                    *   The sun is on average 150 million kilometers away from the earth,
                    *   with a radius of 696 340 kilometers.
                    *   If we scale those values down a bit for floating point precision's sake,
                    *   we can imagine a sphere offset in the directional light direction from the origin,
                    *   and grab a random point in that sphere.
                    */
                    vec4 noise_sample = SampleBlueNoiseTexture(uvec2(gl_LaunchIDNV.xy), 0);
                    vec3 sphere_position = world_position + direction;
                    vec2 spherical_coordinates;
                    {
                       spherical_coordinates.x = acos(2 * noise_sample.x - 1.0f) - (PI / 2.0f);
                       spherical_coordinates.y = 2 * PI * noise_sample.y;
                    }
                    vec3 random_point_on_sphere;
                    {
                        float cos_a = cos(spherical_coordinates.x);
                        float cos_b = cos(spherical_coordinates.y);
                        float sin_a = sin(spherical_coordinates.x);
                        float sin_b = sin(spherical_coordinates.y);
                        random_point_on_sphere = vec3(cos_a * cos_b, cos_a * sin_b, sin_a);
                    }
                    random_point_on_sphere = dot(random_point_on_sphere, direction) >= 0.0f ? random_point_on_sphere : -random_point_on_sphere;
                    random_point_on_sphere *= 0.0046422666666667f * 0.0625f;
                    direction = normalize(sphere_position + random_point_on_sphere - world_position);
                    float maximum_distance = FLOAT32_MAXIMUM;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	0, /*missIndex*/
	world_position, /*origin*/
	FLOAT32_EPSILON * 128.0f, /*Tmin*/
	direction, /*direction*/
	maximum_distance, /*Tmax*/
	0 /*payload*/
);
                    shadows[current_shadow_index++] = VISIBILITY;
                    break;
                }
            }
        }
    }
    imageStore(Shadows, ivec2(gl_LaunchIDNV.xy), shadows);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main     
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index         offset_index      !   General  	 !       FULL_MAIN_RESOLUTION      !      INVERSE_FULL_MAIN_RESOLUTION     	 !      HALF_MAIN_RESOLUTION      !      INVERSE_HALF_MAIN_RESOLUTION      !      FRAME     !      VIEW_DISTANCE     #         -   offset_coordinate     F   BLUE_NOISE_TEXTURES   T   near_plane_coordinate     \   view_space_position   ^   Camera   	 ^       WORLD_TO_CLIP_MATRIX     	 ^      WORLD_TO_CAMERA_MATRIX    ^      PREVIOUS_WORLD_TO_CLIP_MATRIX     ^      INVERSE_WORLD_TO_CAMERA_MATRIX    ^      INVERSE_CAMERA_TO_CLIP_MATRIX    	 ^      CAMERA_WORLD_POSITION    	 ^      CAMERA_FORWARD_VECTOR    	 ^      CURRENT_FRAME_JITTER      ^      NEAR_PLANE    ^   	   FAR_PLANE     `         n   inverse_view_space_position_denominator   v   world_space_position      �   light_data_1      �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   light_data_2      �   light_data_3      �   light_data_4      �   light     �   screen_coordinate     �   gl_LaunchIDNV     �   gl_LaunchSizeNV   �   scene_features_2      �   SceneFeatures2Half    �   shading_normal    �   depth     �   world_position    �   param     �   param     �   shadows   �   current_shadow_index      �   light_index     light     	  param       VISIBILITY      ray_tracing_flags       direction       noise_sample      $  param     %  param     '  sphere_position   +  spherical_coordinates     9  cos_a     =  cos_b     A  sin_a     E  sin_b     I  random_point_on_sphere    g  maximum_distance      k  TOP_LEVEL_ACCELERATION_STRUCTURE      {  Shadows   �  TEXTURES     	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  SKY_TEXTURE   �  SAMPLER  
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS  
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �       
 �  MaskedModels_VERTEX_DATA_BUFFER  
 �      MaskedModels_VERTEX_DATA     	 �  MaskedModels_VERTEX_BUFFERS  
 �  MaskedModels_INDEX_DATA_BUFFER   	 �      MaskedModels_INDEX_DATA  	 �  MaskedModels_INDEX_BUFFERS   
 �  MaskedModels_MATERIAL_BUFFER      �      MaskedModels_MATERIAL_INDICES     �      H  !       #       H  !      #      H  !      #      H  !      #      H  !      #       H  !      #   $   G  !      G  #   "      G  #   !      G  F   "       G  F   !      H  ^          H  ^       #       H  ^             H  ^         H  ^      #   @   H  ^            H  ^         H  ^      #   �   H  ^            H  ^         H  ^      #   �   H  ^            H  ^         H  ^      #      H  ^            H  ^      #   @  H  ^      #   P  H  ^      #   `  H  ^      #   h  H  ^   	   #   l  G  ^      G  `   "      G  `   !       H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      G  �      �  G  �      �  G  �   "      G  �   !      G           G  k  "      G  k  !       G  {  "      G  {  !      G  �  "       G  �  !       G  �        H  �      #       G  �     G  �  "       G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	            
                             !        	   
                                                 !               
                            !        
     !                        "      !   ;  "   #        $          +  $   %         &         +     +      +     .       +     3       +     8       	 B                              C   B     D   C   3      E       D   ;  E   F          H       C   +     M      B+     P       +     V      @+     X     �?   [           ]           ^   ]   ]   ]   ]   ]                     _      ^   ;  _   `         a      ]   +     o      +  $   w        �           �        �   �   �      �      �   ;  �   �      +  $   �      +     �         �         +     �         �         +  $   �          �         +  $   �      +  $   �      +  $   �      +  $   �        �            �      �   ;  �   �      +     �      ?,     �   �   �   ;  �   �       	 �                              �       �   ;  �   �         �   $      ,     �   X   X   X   X            �     ;      �  +          +     1  ��?+     4  ��@+     ^  (�9+     h  ��� i     j      i  ;  j  k      +     n  �   +     p    �7;  �   {      +     �       �  B   �     �      �  ;  �  �        �     �    �  �     �     �  ;  �  �      
 �                          +     �       �  �  �    �  �     �     �  ;  �  �      	 �                             �  �     �      �  ;  �  �        �     �      �  ;  �  �        �       �  �    �  �  8      �     �  ;  �  �       �       �  �    �  �  8      �     �  ;  �  �       �          �  �    �  �     �     �  ;  �  �       �       �  �    �  �  8      �     �  ;  �  �       �       �  �    �  �  8      �     �  ;  �  �       �  �    �  �     �     �  ;  �  �          �     6               �     ;     �      ;  [   �      ;  �   �      ;     �      ;  �   �      ;     �      ;     �      ;  [   �      ;  
   �      ;  
   �      ;  �        ;  
   	     ;  
        ;  �        ;  [        ;  	   $     ;  
   %     ;  �   '     ;     +     ;     9     ;     =     ;     A     ;     E     ;  �   I     ;  �   V     ;     g          �      =  �   �   �   O     �   �   �          p     �   �   �     �   �   �   =  �   �   �   O     �   �   �          p     �   �   �     �   �   �   >  �   �        �      =  �   �   �   =  �   �   �   O     �   �   �          |  �   �   �   b     �   �   �   >  �   �        �      =     �   �   O     �   �   �             >  �   �        �      A     �   �   o   =     �   �   >  �   �        �      =     �   �   >  �   �   =     �   �   >  �   �   9     �      �   �   >  �   �        �      >  �   �        �      >  �   .        �      >  �   .   �  �   �  �        �      �           �    �         �      =       �   A  &     �   �   �   =         �          �    �      �  �        �      =     
  �   >  	  
  9          	  >           �      A  
       %   =         �         8   �        8   �        �        �         �      A  
       w   =         �        �            �         �      >    P        �      >           �      A  �       �   =                  >           �      =  �     �   O                   Q     !         Q     "        P     #  !  "  >  $  #  >  %  .   9     &     $  %  >    &       �      =     (  �   =     )    �     *  (  )  >  '  *       �      A     ,    .   =     -  ,  �     .  V   -  �     /  .  X        0        /  �     2  0  1  A     3  +  .   >  3  2       �      A     5    8   =     6  5  �     7  4  6  A     8  +  8   >  8  7             A     :  +  .   =     ;  :       <        ;  >  9  <             A     >  +  8   =     ?  >       @        ?  >  =  @             A     B  +  .   =     C  B       D        C  >  A  D             A     F  +  8   =     G  F       H        G  >  E  H             =     J  9  =     K  =  �     L  J  K  =     M  9  =     N  E  �     O  M  N  =     P  A  P     Q  L  O  P  >  I  Q             =     R  I  =     S    �     T  R  S  �    U  T  P   �  X      �  U  W  Z  �  W             =     Y  I  >  V  Y  �  X  �  Z             =     [  I       \  [  >  V  \  �  X  �  X  =     ]  V             >  I  ]       	      =     _  I  �     `  _  ^  >  I  `       
      =     a  '  =     b  I  �     c  a  b  =     d  �   �     e  c  d       f     E   e  >    f             >  g  h             =  i  l  k  =     m    =     o  �   =     q    =     r  g  � l  m  n  .   .   .   o  p  q  r  �              =     s  �   �     t  s  �   >  �   t  =     u    A     v  �   s  >  v  u             �    �    �    �    �    �         �      =     y  �   �     z  y  �   >  �   z  �  �   �                 =  �   |  {  =  �   }  �   O     ~  }  }         |  �     ~  =     �  �   c  |    �  �  8       B  9   6               7  	      7  
      �     ;  
          ;  	   -           D      A  &   '   #   %   =     (   '   =     )      �     *   (   )   �     ,   *   +   >      ,        H      A  
   /      .   =     0   /   A  &   1   #   %   =     2   1   �     4   2   3   �     5   4   +   �     6   0   5   A  
   7   -   .   >  7   6        I      A  
   9      8   =     :   9   A  &   ;   #   %   =     <   ;   �     =   <   3   �     >   =   3   �     ?   >   +   �     @   :   ?   A  
   A   -   8   >  A   @        K      =     G       A  H   I   F   G   =  C   J   I   =     K   -   p     L   K   P     N   M   M   �     O   L   N   X     Q   J   O      P   �  Q   8       f  @   6               7        7        �     ;     T      ;  [   \      ;     n      ;  [   v           h      =     U      �     W   U   V   P     Y   X   X   �     Z   W   Y   >  T   Z        i      A  a   b   `   %   =  ]   c   b   =     d   T   =     e      Q     f   d       Q     g   d      P     h   f   g   e   Q     i   h       Q     j   h      Q     k   h      P     l   i   j   k   X   �     m   c   l   >  \   m        j      A     p   \   o   =     q   p   �     r   X   q   >  n   r        k      =     s   n   =     t   \   �     u   t   s   >  \   u        l      A  a   x   `   w   =  ]   y   x   =     z   \   �     {   y   z   >  v   {        n      =     |   v   O     }   |   |             �  }   8       �     6               7  
      �     ;  [   �      ;  [   �      ;  [   �      ;  [   �      ;  �   �           �      =     �      �     �   �   �   �     �   �   .   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   8   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   �   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �      �     �   �   �   �     �   �   o   A  �   �   �   �   �   =     �   �   >  �   �        �      A     �   �   .   =     �   �   A     �   �   8   =     �   �   A     �   �   �   =     �   �   P     �   �   �   �   A  �   �   �   �   >  �   �        �      A     �   �   o   =     �   �   A     �   �   .   =     �   �   A     �   �   8   =     �   �   P     �   �   �   �   A  �   �   �   �   >  �   �        �      A     �   �   �   =     �   �   A     �   �   o   =     �   �   A     �   �   .   =     �   �   P     �   �   �   �   A  �   �   �   �   >  �   �        �      A     �   �   8   =     �   �   |     �   �   A  
   �   �   w   >  �   �        �      A     �   �   �   =     �   �   |     �   �   A  
   �   �   %   >  �   �        �      A     �   �   o   =     �   �   A     �   �   �   >  �   �        �      A     �   �   .   =     �   �   A     �   �   �   >  �   �        �      A     �   �   8   =     �   �   A     �   �   �   >  �   �        �      =     �   �   �  �   8         �V      #     [         �  
  SPV_NV_ray_tracing       GLSL.std.450               �     main         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedShadows_RayMiss.glsl       �     // OpModuleProcessed entry-point main
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
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main      	   VISIBILITY       TEXTURES     	    GlobalTextureAverageValues   	        TEXTURE_AVERAGE_VALUES                Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                  BLUE_NOISE_TEXTURES   $   SKY_TEXTURE   (   Camera   	 (       WORLD_TO_CLIP_MATRIX     	 (      WORLD_TO_CAMERA_MATRIX    (      PREVIOUS_WORLD_TO_CLIP_MATRIX     (      INVERSE_WORLD_TO_CAMERA_MATRIX    (      INVERSE_CAMERA_TO_CLIP_MATRIX    	 (      CAMERA_WORLD_POSITION    	 (      CAMERA_FORWARD_VECTOR    	 (      CURRENT_FRAME_JITTER      (      NEAR_PLANE    (   	   FAR_PLANE     *         +   General  	 +       FULL_MAIN_RESOLUTION      +      INVERSE_FULL_MAIN_RESOLUTION     	 +      HALF_MAIN_RESOLUTION      +      INVERSE_HALF_MAIN_RESOLUTION      +      FRAME     +      VIEW_DISTANCE     -         .   LightingHeader    .       _NumberOfLights   .      _MaximumNumberOfShadowCastingLights   0   Lighting      0       LIGHTING_HEADER   0      LIGHT_DATA    2         5   SAMPLER   8   SceneFeatures2Half    9   Shadows   <   TOP_LEVEL_ACCELERATION_STRUCTURE     
 >   OpaqueModels_VERTEX_DATA_BUFFER  
 >       OpaqueModels_VERTEX_DATA     	 B   OpaqueModels_VERTEX_BUFFERS  
 D   OpaqueModels_INDEX_DATA_BUFFER   	 D       OpaqueModels_INDEX_DATA  	 G   OpaqueModels_INDEX_BUFFERS   
 J   OpaqueModels_MATERIAL_BUFFER      J       OpaqueModels_MATERIAL_INDICES     L        
 N   MaskedModels_VERTEX_DATA_BUFFER  
 N       MaskedModels_VERTEX_DATA     	 Q   MaskedModels_VERTEX_BUFFERS  
 S   MaskedModels_INDEX_DATA_BUFFER   	 S       MaskedModels_INDEX_DATA  	 V   MaskedModels_INDEX_BUFFERS   
 X   MaskedModels_MATERIAL_BUFFER      X       MaskedModels_MATERIAL_INDICES     Z       G  	          G     "       G     !       G           H         #       G        G     "       G     !      H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G      "       G      !      G  $   "       G  $   !      H  (          H  (       #       H  (             H  (         H  (      #   @   H  (            H  (         H  (      #   �   H  (            H  (         H  (      #   �   H  (            H  (         H  (      #      H  (            H  (      #   @  H  (      #   P  H  (      #   `  H  (      #   h  H  (   	   #   l  G  (      G  *   "      G  *   !       H  +       #       H  +      #      H  +      #      H  +      #      H  +      #       H  +      #   $   G  +      G  -   "      G  -   !      H  .       #       H  .      #      G  /         H  0       #       H  0      #      G  0      G  2   "      G  2   !      G  5   "      G  5   !      G  8   "      G  8   !      G  9   "      G  9   !      G  <   "      G  <   !       G  =         H  >       #       G  >      G  B   "      G  B   !      G  C         H  D       #       G  D      G  G   "      G  G   !      G  I         H  J       #       G  J      G  L   "      G  L   !      G  M         H  N       #       G  N      G  Q   "      G  Q   !      G  R         H  S       #       G  S      G  V   "      G  V   !      G  W         H  X       #       G  X      G  Z   "      G  Z   !           !                       �     ;     	   �  +     
     �? 	                                          +                                   ;                                                      ;            
                            +                                          ;                   +                                    ;              	 !                              "   !      #       "   ;  #   $         %           &           '           (   %   %   %   %   %   &   &   '            )      (   ;  )   *        +   '   '   '   '            ,      +   ;  ,   -        .           /        0   .   /      1      0   ;  1   2        3      4       3   ;  4   5        	 6                              7       6   ;  7   8       ;  7   9       � :      ;       :   ;  ;   <         =        >   =   +     ?        @   >   ?      A      @   ;  A   B        C        D   C     E   D   ?      F      E   ;  F   G        H           I   H     J   I      K      J   ;  K   L        M        N   M     O   N   ?      P      O   ;  P   Q        R        S   R     T   S   ?      U      T   ;  U   V        W   H     X   W      Y      X   ;  Y   Z           �     6               �          �      >  	   
   �  8         �B�m��Q �      #     �        �    �  
  SPV_EXT_descriptor_indexing 
  SPV_NV_ray_tracing       GLSL.std.450               �     main    F   I   �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedShadows_RayAnyHit.glsl  �   �     // OpModuleProcessed entry-point main
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
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         UnpackColor(u1;      color        HitVertexInformation             _Position           _Normal         _Tangent            _TextureCoordinate   	    GetHitVertexInformation(         GetHitMaterialIndex(         unpacked      ?   vertex_index_0   
 A   MaskedModels_INDEX_DATA_BUFFER   	 A       MaskedModels_INDEX_DATA  	 D   MaskedModels_INDEX_BUFFERS   	 F   gl_InstanceCustomIndexNV      I   gl_PrimitiveID    Q   vertex_data_0_0  
 S   MaskedModels_VERTEX_DATA_BUFFER  
 S       MaskedModels_VERTEX_DATA     	 V   MaskedModels_VERTEX_BUFFERS   ^   vertex_data_0_1   e   vertex_data_0_2   l   vertex_index_1    t   vertex_data_1_0   {   vertex_data_1_1   �   vertex_data_1_2   �   vertex_index_2    �   vertex_data_2_0   �   vertex_data_2_1   �   vertex_data_2_2   �   barycentric_coordinates   �   HIT_ATTRIBUTE     �   hit_vertex_information    �   gl_ObjectToWorldNV   
 ^  MaskedModels_MATERIAL_BUFFER      ^      MaskedModels_MATERIAL_INDICES     `        j  hit_vertex_information    l  hit_material_index    n  Material      n      _Properties   n     _AlbedoThickness     	 n     _NormalMapDisplacement    n     _MaterialProperties   n     _Opacity      n     _EmissiveMultiplier   n     _Padding1     n     _Padding2     q  GlobalMaterials   q      MATERIALS     s        }  opacity   �  TEXTURES      �  SAMPLER   �  param    	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE   �  Camera   	 �      WORLD_TO_CLIP_MATRIX     	 �     WORLD_TO_CAMERA_MATRIX    �     PREVIOUS_WORLD_TO_CLIP_MATRIX     �     INVERSE_WORLD_TO_CAMERA_MATRIX    �     INVERSE_CAMERA_TO_CLIP_MATRIX    	 �     CAMERA_WORLD_POSITION    	 �     CAMERA_FORWARD_VECTOR    	 �     CURRENT_FRAME_JITTER      �     NEAR_PLANE    �  	   FAR_PLANE     �        �  General  	 �      FULL_MAIN_RESOLUTION      �     INVERSE_FULL_MAIN_RESOLUTION     	 �     HALF_MAIN_RESOLUTION      �     INVERSE_HALF_MAIN_RESOLUTION      �     FRAME     �     VIEW_DISTANCE     �        �  LightingHeader    �      _NumberOfLights   �     _MaximumNumberOfShadowCastingLights   �  Lighting      �      LIGHTING_HEADER   �     LIGHT_DATA    �        �  SceneFeatures2Half    �  Shadows   �  TOP_LEVEL_ACCELERATION_STRUCTURE     
 �  OpaqueModels_VERTEX_DATA_BUFFER  
 �      OpaqueModels_VERTEX_DATA     	 �  OpaqueModels_VERTEX_BUFFERS  
 �  OpaqueModels_INDEX_DATA_BUFFER   	 �      OpaqueModels_INDEX_DATA  	 �  OpaqueModels_INDEX_BUFFERS   
 �  OpaqueModels_MATERIAL_BUFFER      �      OpaqueModels_MATERIAL_INDICES     �      G  @         H  A       #       G  A      G  D   "      G  D   !      G  F      �  G  I         G  R         H  S       #       G  S      G  V   "      G  V   !      G  �      �  G  ]        H  ^      #       G  ^     G  `  "      G  `  !      H  n      #       H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      G  p         H  q      #       G  q     G  s  "       G  s  !      G  �  "       G  �  !       G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !      H  �         H  �      #       H  �            H  �        H  �     #   @   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #   �   H  �           H  �        H  �     #      H  �           H  �     #   @  H  �     #   P  H  �     #   `  H  �     #   h  H  �  	   #   l  G  �     G  �  "      G  �  !       H  �      #       H  �     #      H  �     #      H  �     #      H  �     #       H  �     #   $   G  �     G  �  "      G  �  !      H  �      #       H  �     #      G  �        H  �      #       H  �     #      G  �     G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !      G  �  "      G  �  !       G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	         
   	      !     
           	           	                       !        !                 
   +        �   +  	        C+                !      	     $          +  $   %      +     *      +  $   -      +     2      +  $   5      +     :        @        A   @     B   A      C      B   ;  C   D         E      $   ;  E   F      +  $   H       ;  E   I      +  $   K         N           R   
     S   R     T   S      U      T   ;  U   V         [      
   +  $   p      +  $   �         �         +  	   �     �?   �   �     ;  �   �   �     �   �  	      �           �            �      �   ;  �   �      +  	   
         W          \          ]  \    ^  ]     _     ^  ;  _  `     +  $   b      
 n                 	         +     o       p  n  o    q  p     r     q  ;  r  s     +     w  @     y   	 ~  	                         +            �  ~       �      �  ;  �  �         �      ~    �     �      �  ;  �  �        �  ~  +  	   �     ?  �  
       �  �     �     �  ;  �  �     +     �        �  �  �     �      �  ;  �  �       	 �  	                           �  �     �      �  ;  �  �        �  
        �  �  �  �  �  �           	   	      �     �  ;  �  �       �                 	      �     �  ;  �  �       �          �  
     �  �  �     �     �  ;  �  �      	 �  	                           �      �  ;  �  �      ;  �  �      � �     �      �  ;  �  �        �  
     �  �    �  �  *      �     �  ;  �  �       �       �  �    �  �  *      �     �  ;  �  �       �  \    �  �     �     �  ;  �  �               6               �     ;  �   j     ;     l     ;  !   }     ;     �                9     k     >  j  k             9     m     >  l  m             =     t  l  A  N   u  s  H   t  H   =     v  u  �     x  v  w  �  y  z  x  w  �  |      �  z  {  �  �  {             =     �  l  A  N   �  s  H   �  b  =     �  �  A  �  �  �  �  =  ~  �  �  =  �  �  �  V  �  �  �  �  A  W  �  j  K   =     �  �  X  
   �  �  �     
  Q  	   �  �      >  }  �  �  |  �  �             =     �  l  A  N   �  s  H   �  b  =     �  �  >  �  �  9  
   �     �  Q  	   �  �      >  }  �  �  |  �  |             =  	   �  }  �  y  �  �  �  �  �      �  �  �  �  �  �       
      � �  �  �  �  �  8       �      6  
             7        �     ;                      =           �              p  	         �  	            A  !   "          >  "                 =     #      �     &   #   %   �     '   &      p  	   (   '   �  	   )   (      A  !   +      *   >  +   )              =     ,      �     .   ,   -   �     /   .      p  	   0   /   �  	   1   0      A  !   3      2   >  3   1              =     4      �     6   4   5   �     7   6      p  	   8   7   �  	   9   8      A  !   ;      :   >  ;   9              =  
   <      �  <   8       �  .   6               �     ;     ?      ;     Q      ;     ^      ;     e      ;     l      ;     t      ;     {      ;     �      ;     �      ;     �      ;     �      ;     �      ;  �   �      ;  �   �           �      =  $   G   F   =  $   J   I   �  $   L   J   K   �  $   M   L   H   A  N   O   D   G   H   M   =     P   O   >  ?   P        �      =  $   W   F   =     X   ?   �     Y   :   X   �     Z   Y       A  [   \   V   W   H   Z   =  
   ]   \   >  Q   ]        �      =  $   _   F   =     `   ?   �     a   :   `   �     b   a   *   A  [   c   V   _   H   b   =  
   d   c   >  ^   d        �      =  $   f   F   =     g   ?   �     h   :   g   �     i   h   2   A  [   j   V   f   H   i   =  
   k   j   >  e   k        �      =  $   m   F   =  $   n   I   �  $   o   n   K   �  $   q   o   p   A  N   r   D   m   H   q   =     s   r   >  l   s        �      =  $   u   F   =     v   l   �     w   :   v   �     x   w       A  [   y   V   u   H   x   =  
   z   y   >  t   z        �      =  $   |   F   =     }   l   �     ~   :   }   �        ~   *   A  [   �   V   |   H      =  
   �   �   >  {   �        �      =  $   �   F   =     �   l   �     �   :   �   �     �   �   2   A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      =  $   �   F   =  $   �   I   �  $   �   �   K   �  $   �   �   �   A  N   �   D   �   H   �   =     �   �   >  �   �        �      =  $   �   F   =     �   �   �     �   :   �   �     �   �       A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      =  $   �   F   =     �   �   �     �   :   �   �     �   �   *   A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      =  $   �   F   =     �   �   �     �   :   �   �     �   �   2   A  [   �   V   �   H   �   =  
   �   �   >  �   �        �      A  �   �   �       =  	   �   �   �  	   �   �   �   A  �   �   �   *   =  	   �   �   �  	   �   �   �   A  �   �   �       =  	   �   �   A  �   �   �   *   =  	   �   �   P     �   �   �   �   >  �   �        �      A  !   �   Q       =  	   �   �   A  !   �   Q   *   =  	   �   �   A  !   �   Q   2   =  	   �   �   P     �   �   �   �   A  !   �   �       =  	   �   �   �     �   �   �   A  !   �   t       =  	   �   �   A  !   �   t   *   =  	   �   �   A  !   �   t   2   =  	   �   �   P     �   �   �   �   A  !   �   �   *   =  	   �   �   �     �   �   �   �     �   �   �   A  !   �   �       =  	   �   �   A  !   �   �   *   =  	   �   �   A  !   �   �   2   =  	   �   �   P     �   �   �   �   A  !   �   �   2   =  	   �   �   �     �   �   �   �     �   �   �   A  �   �   �   H   >  �   �        �      =  �   �   �   A  �   �   �   p   =     �   �   Q  	   �   �       Q  	   �   �      Q  	   �   �      P  
   �   �   �   �   �   �     �   �   �   A  �   �   �   H   >  �   �        �      A  !   �   Q   :   =  	   �   �   A  !   �   ^       =  	   �   �   A  !   �   ^   *   =  	   �   �   P     �   �   �   �   A  !   �   �       =  	   �   �   �     �   �   �   A  !   �   t   :   =  	   �   �   A  !   �   {       =  	   �   �   A  !   �   {   *   =  	   �   �   P     �   �   �   �   A  !   �   �   *   =  	   �   �   �     �   �   �   �     �   �   �   A  !   �   �   :   =  	   �   �   A  !   �   �       =  	   �   �   A  !   �   �   *   =  	      �   P       �   �      A  !     �   2   =  	       �           �       �     A  �     �   p   >           �      =  �     �   A  �     �   p   =     	    Q  	     	      Q  	     	     Q  	     	     P  
           
  �                     E     A  �     �   p   >           �      A  !     ^   2   =  	       A  !     ^   :   =  	       A  !     e       =  	       P             A  !     �       =  	       �           A  !     {   2   =  	       A  !     {   :   =  	       A  !      �       =  	   !     P     "      !  A  !   #  �   *   =  	   $  #  �     %  "  $  �     &    %  A  !   '  �   2   =  	   (  '  A  !   )  �   :   =  	   *  )  A  !   +  �       =  	   ,  +  P     -  (  *  ,  A  !   .  �   2   =  	   /  .  �     0  -  /  �     1  &  0  A  �   2  �   �   >  2  1       �      =  �   3  �   A  �   4  �   �   =     5  4  Q  	   6  5      Q  	   7  5     Q  	   8  5     P  
   9  6  7  8  
  �     :  3  9       ;     E   :  A  �   <  �   �   >  <  ;       �      A  !   =  e   *   =  	   >  =  A  !   ?  e   2   =  	   @  ?  P     A  >  @  A  !   B  �       =  	   C  B  �     D  A  C  A  !   E  �   *   =  	   F  E  A  !   G  �   2   =  	   H  G  P     I  F  H  A  !   J  �   *   =  	   K  J  �     L  I  K  �     M  D  L  A  !   N  �   *   =  	   O  N  A  !   P  �   2   =  	   Q  P  P     R  O  Q  A  !   S  �   2   =  	   T  S  �     U  R  T  �     V  M  U  A  W  X  �   K   >  X  V       �      =     Y  �   �  Y  8       �     6               �          �      =  $   a  F   �  $   c  a  b  =  $   d  F   �  $   e  d  K   A  N   f  `  H   c  e  =     g  f  �  g  8         ��KT��D	(�D��^��       ,�'��       �]����M��R8�4j                                                    v���L���