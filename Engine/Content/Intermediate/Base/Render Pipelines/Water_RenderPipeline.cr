�nX/J16������Water_RenderPipeline                                             `w      #     _                GLSL.std.450              	        main    }   �     0       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Water_Vertex.glsl    X   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform Water
{
	layout (offset = 0) vec2 WATER_OFFSETS[4];
};

layout (std140, set = 1, binding = 3) uniform Wind
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
layout (std430, set = 1, binding = 4) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 5) uniform sampler SAMPLER;

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
*	Calculates the water height at the given position.
*/
float WaterHeightAtPosition(vec3 world_position, uint texture_index)
{
    #define AMPLITUDE (1.0f)

	float water_texture_sample_1 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 0.125f + WATER_OFFSETS[0]).x;
    float water_texture_sample_2 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 0.25f + WATER_OFFSETS[1]).y;
    float water_texture_sample_3 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 0.5f + WATER_OFFSETS[2]).z;
    float water_texture_sample_4 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 1.0f + WATER_OFFSETS[3]).w;

    return  (water_texture_sample_1 * 1.0f
            + water_texture_sample_2 * 0.5f
            + water_texture_sample_3 * 0.25f
            + water_texture_sample_4 * 0.125f)
            / 1.875f * AMPLITUDE;
}

/*
*	Calculates the water height at the given position.
*/
vec3 WaterNormalAtPosition(vec3 world_position, uint texture_index)
{
	float left = WaterHeightAtPosition(world_position + vec3(-0.25f, 0.0f, 0.0f), texture_index);
	float right = WaterHeightAtPosition(world_position + vec3(0.25f, 0.0f, 0.0f), texture_index);
	float down = WaterHeightAtPosition(world_position + vec3(0.0f, 0.0f, -0.25f), texture_index);
	float up = WaterHeightAtPosition(world_position + vec3(0.0f, 0.0f, 0.25f), texture_index);

	return normalize(vec3(left - right, 0.5f, down - up));
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 WORLD_POSITION;
	layout (offset = 8) uint BORDERS;
	layout (offset = 12) float PATCH_RESOLUTION_RECIPROCAL;
	layout (offset = 16) float PATCH_SIZE;
	layout (offset = 20) uint TEXTURE_INDEX;
};

layout (set = 1, binding = 6) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 7) uniform sampler2D SceneInput;

layout (location = 0) in vec2 InPosition;
layout (location = 1) in uint InBorders;

layout (location = 0) out vec3 OutWorldPosition;

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
    OutWorldPosition.x = WORLD_POSITION.x + mix(-(PATCH_SIZE * 0.5f), (PATCH_SIZE * 0.5f), stitched_position.x);
    OutWorldPosition.y = 0.0f;
    OutWorldPosition.z = WORLD_POSITION.y + mix(-(PATCH_SIZE * 0.5f), (PATCH_SIZE * 0.5f), stitched_position.y);
    OutWorldPosition.y += WaterHeightAtPosition(OutWorldPosition, TEXTURE_INDEX);
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition,1.0f);
}     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     
    WaterHeightAtPosition(vf3;u1;        world_position       texture_index        water_texture_sample_1       TEXTURES         SAMPLER   )   Water     )       WATER_OFFSETS     +         7   water_texture_sample_2    H   water_texture_sample_3    Y   water_texture_sample_4    {   stitched_position     }   InPosition       is_left_multiplier    �   InBorders     �   PushConstantData      �       WORLD_POSITION    �      BORDERS  
 �      PATCH_RESOLUTION_RECIPROCAL   �      PATCH_SIZE    �      TEXTURE_INDEX     �         �   is_right_multiplier   �   vertical_offset   �   is_down_multiplier    �   is_up_multiplier      �   horizontal_offset     �   is_left_multiplier    �   is_right_multiplier   �   vertical_offset   �   is_down_multiplier    �   is_up_multiplier      �   horizontal_offset       OutWorldPosition      #  param     %  param     .  gl_PerVertex      .      gl_Position   .     gl_PointSize      .     gl_ClipDistance   .     gl_CullDistance   0        2  Camera   	 2      WORLD_TO_CLIP_MATRIX     	 2     WORLD_TO_CAMERA_MATRIX    2     PREVIOUS_WORLD_TO_CLIP_MATRIX     2     INVERSE_WORLD_TO_CAMERA_MATRIX    2     INVERSE_CAMERA_TO_CLIP_MATRIX    	 2     CAMERA_WORLD_POSITION    	 2     CAMERA_FORWARD_VECTOR    	 2     CURRENT_FRAME_JITTER      2     NEAR_PLANE    2  	   FAR_PLANE     4       	 A  GlobalTextureAverageValues   	 A      TEXTURE_AVERAGE_VALUES    C        D  Material      D      _Properties   D     _AlbedoThickness     	 D     _NormalMapDisplacement    D     _MaterialProperties   D     _Opacity      D     _EmissiveMultiplier   D     _Padding1     D     _Padding2     G  GlobalMaterials   G      MATERIALS     I        L  BLUE_NOISE_TEXTURES   P  SKY_TEXTURE   Q  General  	 Q      FULL_MAIN_RESOLUTION      Q     INVERSE_FULL_MAIN_RESOLUTION     	 Q     HALF_MAIN_RESOLUTION      Q     INVERSE_HALF_MAIN_RESOLUTION      Q     FRAME     Q     VIEW_DISTANCE     S        T  Wind      T      UPPER_SKY_COLOR   T     LOWER_SKY_COLOR   T     SKY_MODE      T     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     V        W  LightingHeader    W      _NumberOfLights   W     _MaximumNumberOfShadowCastingLights   Y  Lighting      Y      LIGHTING_HEADER   Y     LIGHT_DATA    [        ]  SceneFeatures2    ^  SceneInput  G     "       G     !       G     "      G     !      G  (         H  )       #       G  )      G  +   "      G  +   !      G  }          G  �         H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      G  �      G           H  .             H  .           H  .           H  .           G  .     H  2         H  2      #       H  2            H  2        H  2     #   @   H  2           H  2        H  2     #   �   H  2           H  2        H  2     #   �   H  2           H  2        H  2     #      H  2           H  2     #   @  H  2     #   P  H  2     #   `  H  2     #   h  H  2  	   #   l  G  2     G  4  "      G  4  !       G  @        H  A      #       G  A     G  C  "       G  C  !      H  D      #       H  D     #      H  D     #      H  D     #      H  D     #      H  D     #      H  D     #      H  D     #      G  F         H  G      #       G  G     G  I  "       G  I  !      G  L  "       G  L  !      G  P  "       G  P  !      H  Q      #       H  Q     #      H  Q     #      H  Q     #      H  Q     #       H  Q     #   $   G  Q     G  S  "      G  S  !      H  T      #       H  T     #      H  T     #       H  T     #   $   G  T     G  V  "      G  V  !      H  W      #       H  W     #      G  X        H  Y      #       H  Y     #      G  Y     G  [  "      G  [  !      G  ]  "      G  ]  !      G  ^  "      G  ^  !           !                               	           
                    
   !        	                   	                             +  
                                 ;                                           ;                       "         +     %      >+  
   '        (   "   '     )   (      *      )   ;  *   +        ,          +  ,   -          .      "     2         +     3       +  
   5       +     ?     �>+  ,   A      +  
   F      +     P      ?+  ,   R      +  
   W      +     a     �?+  ,   c      +  
   h      +     u     �?   z      "      |      "   ;  |   }         �      
   ;  �   �        �   "   
         
      �   	   �   ;  �   �   	      �   	   
   +  
   �      +  
   �   @      �   	      +  
   �      +  
   �       +  
   �   �   +     �      @           ;                    +  ,   "       -     F     .  2      -  -     /     .  ;  /  0       1  2        2  1  1  1  1  1        "            3     2  ;  3  4        5     1     >     2     @  2        A  @     B     A  ;  B  C      
 D  
   
   
   
   
      
   
   +  
   E       F  D  E    G  F     H     G  ;  H  I       J      �      K      J  ;  K  L       	 M                             N  M     O      N  ;  O  P        Q  "   "   "   "   
         R     Q  ;  R  S       T        
         U     T  ;  U  V       W  
   
     X  2     Y  W  X     Z     Y  ;  Z  [        \          ;  \  ]      ;  \  ^           �     6               �     ;  z   {      ;           ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;  	   #     ;     %          �      =  "   ~   }   >  {   ~              =  
   �   �   �  
   �   �   F   A  �   �   �   A   =  
   �   �   �  
   �   �   F   �  
   �   �   �   p     �   �   >     �              =  
   �   �   �  
   �   �   '   A  �   �   �   A   =  
   �   �   �  
   �   �   '   �  
   �   �   �   p     �   �   >  �   �              =     �      =     �   �   �     �   �   �        �      %   �   a   >  �   �              =  
   �   �   �  
   �   �   �   A  �   �   �   A   =  
   �   �   �  
   �   �   �   �  
   �   �   �   p     �   �   >  �   �              =  
   �   �   �  
   �   �   �   A  �   �   �   A   =  
   �   �   �  
   �   �   �   �  
   �   �   �   p     �   �   >  �   �              =     �   �   =     �   �   �     �   �   �        �      %   �   a   >  �   �              A  �   �   �   R   =     �   �   =     �   �   �     �   �   �   A     �   {   F   =     �   �   �     �   �   �   A     �   {   F   >  �   �              A  �   �   �   R   =     �   �   =     �   �   �     �   �   �   A     �   {   5   =     �   �   �     �   �   �   A     �   {   5   >  �   �              =  
   �   �   �  
   �   �   W   A  �   �   �   A   =  
   �   �   �  
   �   �   W   �  
   �   �   �   p     �   �   >  �   �              =  
   �   �   �  
   �   �   �   A  �   �   �   A   =  
   �   �   �  
   �   �   �   �  
   �   �   �   p     �   �   >  �   �              =     �   �   =     �   �   �     �   �   �        �      %   �   a   >  �   �              =  
   �   �   �  
   �   �   �   A  �   �   �   A   =  
   �   �   �  
   �   �   �   �  
   �   �   �   p     �   �   >  �   �              =  
   �   �   �  
   �   �   �   A  �   �   �   A   =  
   �   �   �  
   �   �   �   �  
   �   �   �   p     �   �   >  �   �              =     �   �   =     �   �   �     �   �   �        �      %   �   a   >  �   �              A  �   �   �   R   =     �   �   �     �   �   �   =     �   �   �     �   �   �   A     �   {   F   =     �   �   �     �   �   �   A     �   {   F   >  �   �              A  �   �   �   R   =     �   �   �     �   �   �   =     �   �   �     �   �   �   A     �   {   5   =     �   �   �        �   �   A       {   5   >                  A  �     �   -   5   =         A  �     �   c   =         �         P        	    A  �   
  �   c   =       
  �         P   A       {   5   =                   .   	      �           A        5   >                 A        F   >    3              A  �     �   -   F   =         A  �     �   c   =         �         P            A  �     �   c   =         �         P   A       {   F   =                   .         �            A    !    W   >  !                =     $    >  #  $  A  �   &  �   "  =  
   '  &  >  %  '  9     (     #  %  A    )    F   =     *  )  �     +  *  (  A    ,    F   >  ,  +             A  5  6  4  -   =  1  7  6  =     8    Q     9  8      Q     :  8     Q     ;  8     P  2   <  9  :  ;  a   �  2   =  7  <  A  >  ?  0  -   >  ?  =  �  8       �  D   6               7  	      7        �     ;           ;     7      ;     H      ;     Y           �      =  
         A              =           =           V      !         =     #      O  "   $   #   #          �  "   &   $   %   A  .   /   +   -   -   =  "   0   /   �  "   1   &   0   X  2   4   !   1      3   Q     6   4       >     6        �      =  
   8      A     9      8   =     :   9   =     ;      V      <   :   ;   =     =      O  "   >   =   =          �  "   @   >   ?   A  .   B   +   -   A   =  "   C   B   �  "   D   @   C   X  2   E   <   D      3   Q     G   E      >  7   G        �      =  
   I      A     J      I   =     K   J   =     L      V      M   K   L   =     N      O  "   O   N   N          �  "   Q   O   P   A  .   S   +   -   R   =  "   T   S   �  "   U   Q   T   X  2   V   M   U      3   Q     X   V      >  H   X        �      =  
   Z      A     [      Z   =     \   [   =     ]      V      ^   \   ]   =     _      O  "   `   _   _          �  "   b   `   a   A  .   d   +   -   c   =  "   e   d   �  "   f   b   e   X  2   g   ^   f      3   Q     i   g      >  Y   i        �      =     j      �     k   j   a        �      =     l   7   �     m   l   P   �     n   k   m        �      =     o   H   �     p   o   ?   �     q   n   p        �      =     r   Y   �     s   r   %   �     t   q   s   �     v   t   u   �     w   v   a        �      �  w   8  \�      #     �                GLSL.std.450                     main    "  4  �               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Water_Fragment.glsl  f   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform Water
{
	layout (offset = 0) vec2 WATER_OFFSETS[4];
};

layout (std140, set = 1, binding = 3) uniform Wind
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
layout (std430, set = 1, binding = 4) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 5) uniform sampler SAMPLER;

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
*	Calculates the water height at the given position.
*/
float WaterHeightAtPosition(vec3 world_position, uint texture_index)
{
    #define AMPLITUDE (1.0f)

	float water_texture_sample_1 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 0.125f + WATER_OFFSETS[0]).x;
    float water_texture_sample_2 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 0.25f + WATER_OFFSETS[1]).y;
    float water_texture_sample_3 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 0.5f + WATER_OFFSETS[2]).z;
    float water_texture_sample_4 = texture(sampler2D(TEXTURES[texture_index], SAMPLER), world_position.xz * 1.0f + WATER_OFFSETS[3]).w;

    return  (water_texture_sample_1 * 1.0f
            + water_texture_sample_2 * 0.5f
            + water_texture_sample_3 * 0.25f
            + water_texture_sample_4 * 0.125f)
            / 1.875f * AMPLITUDE;
}

/*
*	Calculates the water height at the given position.
*/
vec3 WaterNormalAtPosition(vec3 world_position, uint texture_index)
{
	float left = WaterHeightAtPosition(world_position + vec3(-0.25f, 0.0f, 0.0f), texture_index);
	float right = WaterHeightAtPosition(world_position + vec3(0.25f, 0.0f, 0.0f), texture_index);
	float down = WaterHeightAtPosition(world_position + vec3(0.0f, 0.0f, -0.25f), texture_index);
	float up = WaterHeightAtPosition(world_position + vec3(0.0f, 0.0f, 0.25f), texture_index);

	return normalize(vec3(left - right, 0.5f, down - up));
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 WORLD_POSITION;
	layout (offset = 8) uint BORDERS;
	layout (offset = 12) float PATCH_RESOLUTION_RECIPROCAL;
	layout (offset = 16) float PATCH_SIZE;
	layout (offset = 20) uint TEXTURE_INDEX;
};

layout (set = 1, binding = 6) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 7) uniform sampler2D SceneInput;

layout (location = 0) in vec3 InWorldPosition;

layout (location = 0) out vec4 Scene;

void main()
{
    #define WATER_ALBEDO (vec3(0.0f, 0.25f, 0.25f))
    vec3 view_direction = normalize(CAMERA_WORLD_POSITION - InWorldPosition);
    vec3 water_normal = WaterNormalAtPosition(InWorldPosition, TEXTURE_INDEX);
    vec2 screen_coordinate = vec2(gl_FragCoord.xy) * INVERSE_FULL_MAIN_RESOLUTION;
    vec4 scene_features_2 = texture(SceneFeatures2, screen_coordinate);
    float surface_depth = LinearizeDepth(gl_FragCoord.z);
    float underwater_depth = LinearizeDepth(scene_features_2.w);
    float underwater_weight = exp(-(underwater_depth - surface_depth) * 0.125f * 0.125f);
    screen_coordinate += water_normal.xz * (1.0f - underwater_weight) * 0.125f;
    vec3 underwater_scene = texture(SceneInput, screen_coordinate).rgb;
    underwater_scene *= 0.8f;
    vec3 water_scene = vec3(0.0f);
    {
        float sky_alpha = dot(water_normal, vec3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f;
        vec3 sky_radiance = mix(LOWER_SKY_COLOR, UPPER_SKY_COLOR, sky_alpha);
        water_scene += sky_radiance * WATER_ALBEDO;
    }
    for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
    {
        Light light = UnpackLight(i);
        vec3 light_radiance = light._Color * light._Intensity;
        switch (light._LightType)
        {
            case LIGHT_TYPE_DIRECTIONAL:
            {
                vec3 light_direction = -light._TransformData1;
                vec3 halfway_vector = normalize(view_direction + light_direction);
                water_scene += light_radiance * WATER_ALBEDO * max(dot(water_normal, light_direction), 0.0f) * 0.125f; //Diffuse.
                water_scene += light_radiance * pow(max(dot(water_normal, halfway_vector), 0.0f), 128.0f); //Specular.
                break;
            }
        }
    }
    vec3 final_scene = mix(water_scene, underwater_scene, underwater_weight);
	Scene = vec4(final_scene,1.0f);
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         LinearizeDepth(f1;    
   depth        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index    
    WaterHeightAtPosition(vf3;u1;        world_position       texture_index    
    WaterNormalAtPosition(vf3;u1;        world_position       texture_index     #   Camera   	 #       WORLD_TO_CLIP_MATRIX     	 #      WORLD_TO_CAMERA_MATRIX    #      PREVIOUS_WORLD_TO_CLIP_MATRIX     #      INVERSE_WORLD_TO_CAMERA_MATRIX    #      INVERSE_CAMERA_TO_CLIP_MATRIX    	 #      CAMERA_WORLD_POSITION    	 #      CAMERA_FORWARD_VECTOR    	 #      CURRENT_FRAME_JITTER      #      NEAR_PLANE    #   	   FAR_PLANE     %         =   light_data_1      >   LightingHeader    >       _NumberOfLights   >      _MaximumNumberOfShadowCastingLights   @   Lighting      @       LIGHTING_HEADER   @      LIGHT_DATA    B         L   light_data_2      S   light_data_3      Z   light_data_4      b   light     �   water_texture_sample_1    �   TEXTURES      �   SAMPLER   �   Water     �       WATER_OFFSETS     �         �   water_texture_sample_2    �   water_texture_sample_3    �   water_texture_sample_4    �   left      �   param     �   param     �   right     �   param        param       down        param       param       up      param       param       view_direction    "  InWorldPosition   &  water_normal      '  PushConstantData      '      WORLD_POSITION    '     BORDERS  
 '     PATCH_RESOLUTION_RECIPROCAL   '     PATCH_SIZE    '     TEXTURE_INDEX     )        *  param     ,  param     2  screen_coordinate     4  gl_FragCoord      :  General  	 :      FULL_MAIN_RESOLUTION      :     INVERSE_FULL_MAIN_RESOLUTION     	 :     HALF_MAIN_RESOLUTION      :     INVERSE_HALF_MAIN_RESOLUTION      :     FRAME     :     VIEW_DISTANCE     <        @  scene_features_2      B  SceneFeatures2    F  surface_depth     G  param     L  underwater_depth      M  param     Q  underwater_weight     a  underwater_scene      b  SceneInput    j  water_scene   l  sky_alpha     r  sky_radiance      s  Wind      s      UPPER_SKY_COLOR   s     LOWER_SKY_COLOR   s     SKY_MODE      s     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     u        �  i     �  light     �  param     �  light_radiance    �  light_direction   �  halfway_vector    �  final_scene   �  Scene    	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE H  #          H  #       #       H  #             H  #         H  #      #   @   H  #            H  #         H  #      #   �   H  #            H  #         H  #      #   �   H  #            H  #         H  #      #      H  #            H  #      #   @  H  #      #   P  H  #      #   `  H  #      #   h  H  #   	   #   l  G  #      G  %   "      G  %   !       H  >       #       H  >      #      G  ?         H  @       #       H  @      #      G  @      G  B   "      G  B   !      G  �   "       G  �   !       G  �   "      G  �   !      G  �         H  �       #       G  �      G  �   "      G  �   !      G  "         H  '      #       H  '     #      H  '     #      H  '     #      H  '     #      G  '     G  4        H  :      #       H  :     #      H  :     #      H  :     #      H  :     #       H  :     #   $   G  :     G  <  "      G  <  !      G  B  "      G  B  !      G  b  "      G  b  !      H  s      #       H  s     #      H  s     #       H  s     #   $   G  s     G  u  "      G  u  !      G  �         G  �        H  �      #       G  �     G  �  "       G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !           !                             !  	                                              
                            !                       !              !                            !            "           #   !   !   !   !   !         "            $      #   ;  $   %        &          +  &   '   	      (         +  &   +         <            >           ?         @   >   ?      A      @   ;  A   B      +  &   C      +     E      +     G          I          +     O      +     V      +     ]         a         +  &   c       +  &   t      +  &   }      +  &   �      +  &   �      +  &   �      +  &   �       	 �                            +     �        �   �   �      �       �   ;  �   �          �       �     �      �       �   ;  �   �         �   �   +     �      >  �   "   E     �   �      �      �   ;  �   �         �      "   +     �     �>+     �      ?+     �     �?+     �     �?+     �     ��+     �       ,     �   �   �   �   ,     �   �   �   �   ,       �   �   �   ,       �   �   �                 !        ;  !  "       '  "                  (  	   '  ;  (  )  	      -  	         1     "      3         ;  3  4       :  "   "   "   "            ;     :  ;  ;  <        A      �   ;  A  B         H        ;  A  b      +     g  ��L?,     k  �   �   �   ,     n  �   �   �     s                 t     s  ;  t  u     ,     ~  �   �   �      �          �  +     �     C   �         ;  �  �       �      �     �  �     �     �  ;  �  �      
 �                          +     �       �  �  �    �  �     �     �  ;  �  �     +     �        �  �   �     �      �  ;  �  �       	 �                             �  �     �      �  ;  �  �           �     6               �     ;          ;     &     ;     *     ;     ,     ;  1  2     ;  <   @     ;     F     ;     G     ;     L     ;     M     ;     Q     ;     a     ;     j     ;     l     ;     r     ;     �     ;  a   �     ;     �     ;     �     ;     �     ;     �     ;     �                A      %   �   =          =     #  "  �     $     #       %     E   $  >    %             =     +  "  >  *  +  A  -  .  )  �   =     /  .  >  ,  /  9     0     *  ,  >  &  0             =      5  4  O  "   6  5  5         Q     7  6      Q     8  6     P  "   9  7  8  A  �   =  <  C   =  "   >  =  �  "   ?  9  >  >  2  ?             =  �   C  B  =  "   D  2  W      E  C  D  >  @  E             A  H  I  4  V   =     J  I  >  G  J  9     K     G  >  F  K             A     N  @  ]   =     O  N  >  M  O  9     P     M  >  L  P             =     R  L  =     S  F  �     T  R  S       U  T  �     V  U  �   �     W  V  �        X        W  >  Q  X             =     Y  &  O  "   Z  Y  Y         =     [  Q  �     \  �   [  �  "   ]  Z  \  �  "   ^  ]  �   =  "   _  2  �  "   `  _  ^  >  2  `       	      =  �   c  b  =  "   d  2  W      e  c  d  O     f  e  e            >  a  f       
      =     h  a  �     i  h  g  >  a  i             >  j  k             =     m  &  �     o  m  n  �     p  o  �   �     q  p  �   >  l  q             A    v  u  C   =     w  v  A    x  u  c   =     y  x  =     z  l  P     {  z  z  z       |     .   w  y  {  >  r  |             =     }  r  �       }  ~  =     �  j  �     �  �    >  j  �             >  �  G   �  �  �  �             �  �  �      �  �  �  �             =     �  �  A  �  �  B   c   c   =     �  �  �  �  �  �  �  �  �  �  �  �  �             =     �  �  >  �  �  9     �     �  >  �  �             A     �  �  t   =     �  �  A     �  �  �   =     �  �  �     �  �  �  >  �  �             A     �  �  }   =     �  �  �  �      �  �  �      �  �  �             A     �  �  c   =     �  �       �  �  >  �  �             =     �    =     �  �  �     �  �  �       �     E   �  >  �  �             =     �  �  �     �  �  ~  =     �  &  =     �  �  �     �  �  �       �     (   �  �   �     �  �  �  �     �  �  �   =     �  j  �     �  �  �  >  j  �             =     �  �  =     �  &  =     �  �  �     �  �  �       �     (   �  �        �        �  �  �     �  �  �  =     �  j  �     �  �  �  >  j  �             �  �  �  �  �  �  �  �             =     �  �  �     �  �  C   >  �  �  �  �  �  �       !      =     �  j  =     �  a  =     �  Q  P     �  �  �  �       �     .   �  �  �  >  �  �       "      =     �  �  Q     �  �      Q     �  �     Q     �  �     P      �  �  �  �  �   >  �  �  �  8       M  !   6            	   7     
   �          O      A  (   )   %   '   =     *   )   A  (   ,   %   +   =     -   ,   �     .   *   -   =     /   
   A  (   0   %   '   =     1   0   A  (   2   %   +   =     3   2   �     4   1   3   �     5   /   4   A  (   6   %   +   =     7   6   �     8   5   7   �     9   .   8   �  9   8       �     6               7        �     ;  <   =      ;  <   L      ;  <   S      ;  <   Z      ;  a   b           �      =     D      �     F   D   E   �     H   F   G   A  I   J   B   C   H   =      K   J   >  =   K        �      =     M      �     N   M   E   �     P   N   O   A  I   Q   B   C   P   =      R   Q   >  L   R        �      =     T      �     U   T   E   �     W   U   V   A  I   X   B   C   W   =      Y   X   >  S   Y        �      =     [      �     \   [   E   �     ^   \   ]   A  I   _   B   C   ^   =      `   _   >  Z   `        �      A     d   =   G   =     e   d   A     f   =   O   =     g   f   A     h   =   V   =     i   h   P     j   e   g   i   A     k   b   c   >  k   j        �      A     l   =   ]   =     m   l   A     n   L   G   =     o   n   A     p   L   O   =     q   p   P     r   m   o   q   A     s   b   C   >  s   r        �      A     u   L   V   =     v   u   A     w   L   ]   =     x   w   A     y   S   G   =     z   y   P     {   v   x   z   A     |   b   t   >  |   {        �      A     ~   S   O   =        ~   |     �      A     �   b   }   >  �   �        �      A     �   S   V   =     �   �   |     �   �   A     �   b   �   >  �   �        �      A     �   S   ]   =     �   �   A     �   b   �   >  �   �        �      A     �   Z   G   =     �   �   A     �   b   �   >  �   �        �      A     �   Z   O   =     �   �   A     �   b   �   >  �   �        �      =     �   b   �  �   8       �  D   6               7        7        �     ;     �      ;     �      ;     �      ;     �           �      =     �      A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �      O  "   �   �   �          �  "   �   �   �   A  �   �   �   c   c   =  "   �   �   �  "   �   �   �   W      �   �   �   Q     �   �       >  �   �        �      =     �      A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �      O  "   �   �   �          �  "   �   �   �   A  �   �   �   c   C   =  "   �   �   �  "   �   �   �   W      �   �   �   Q     �   �      >  �   �        �      =     �      A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �      O  "   �   �   �          �  "   �   �   �   A  �   �   �   c   t   =  "   �   �   �  "   �   �   �   W      �   �   �   Q     �   �      >  �   �        �      =     �      A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �      O  "   �   �   �          �  "   �   �   �   A  �   �   �   c   }   =  "   �   �   �  "   �   �   �   W      �   �   �   Q     �   �      >  �   �        �      =     �   �   �     �   �   �        �      =     �   �   �     �   �   �   �     �   �   �        �      =     �   �   �     �   �   �   �     �   �   �        �      =     �   �   �     �   �   �   �     �   �   �   �     �   �   �   �     �   �   �        �      �  �   8       �  C   6               7        7        �     ;     �      ;     �      ;     �      ;     �      ;     �      ;           ;          ;          ;          ;          ;          ;               �      =     �      �     �   �   �   >  �   �   =     �      >  �   �   9     �      �   �   >  �   �        �      =     �      �     �   �   �   >  �   �   =          >       9          �      >  �          �      =          �           >      =     	     >    	  9     
         >    
       �      =          �           >      =          >      9              >           �      =       �   =       �   �           =         =         �           P         �               E     �    8                          ��KT��D	(�D��^����-b��V�D_w���       ,�'��               �%�9�� Y  ��0����   f)        N1�8��;                             ��-b��V