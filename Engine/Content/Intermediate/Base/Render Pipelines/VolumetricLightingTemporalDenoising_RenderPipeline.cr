�nX/J1��Q��H�VolumetricLightingTemporalDenoising_RenderPipeline               `<      #     Y                 GLSL.std.450                      main          0         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\VolumetricLightingTemporalDenoising_Vertex.glsl  �
   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
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
*	Constrains the the given sample, by clipping it against the minimum/maximum bounding volume.
*/
vec3 Constrain(vec3 _sample, vec3 minimum, vec3 maximum)
{
#if 0
	return clamp(_sample, minimum, maximum);
#else
	vec3 p_clip = 0.5f * (maximum + minimum);
	vec3 e_clip = 0.5f * (maximum - minimum);

	vec3 v_clip = _sample - p_clip;
	vec3 v_unit = v_clip / e_clip;
	vec3 a_unit = abs(v_unit);

	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0f)
	{
		return p_clip + v_clip / ma_unit;
	}

	else
	
	{
		return _sample;
	}
#endif
}

layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures4Half;
layout (set = 1, binding = 4) uniform sampler2D VolumetricLighting;
layout (set = 1, binding = 5) uniform sampler2D PreviousTemporalBuffer;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES      =   Material      =       _Properties   =      _AlbedoThickness     	 =      _NormalMapDisplacement    =      _MaterialProperties   =      _Opacity      =      _EmissiveMultiplier   =      _Padding1     =      _Padding2     @   GlobalMaterials   @       MATERIALS     B         G   BLUE_NOISE_TEXTURES   K   SKY_TEXTURE   N   Camera   	 N       WORLD_TO_CLIP_MATRIX     	 N      WORLD_TO_CAMERA_MATRIX    N      PREVIOUS_WORLD_TO_CLIP_MATRIX     N      INVERSE_WORLD_TO_CAMERA_MATRIX    N      INVERSE_CAMERA_TO_CLIP_MATRIX    	 N      CAMERA_WORLD_POSITION    	 N      CAMERA_FORWARD_VECTOR    	 N      CURRENT_FRAME_JITTER      N      NEAR_PLANE    N   	   FAR_PLANE     P         Q   General  	 Q       FULL_MAIN_RESOLUTION      Q      INVERSE_FULL_MAIN_RESOLUTION     	 Q      HALF_MAIN_RESOLUTION      Q      INVERSE_HALF_MAIN_RESOLUTION      Q      FRAME     Q      VIEW_DISTANCE     S         U   SceneFeatures2Half    V   SceneFeatures4Half    W   VolumetricLighting    X   PreviousTemporalBuffer  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       H  =       #       H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      G  ?          H  @       #       G  @      G  B   "       G  B   !      G  G   "       G  G   !      G  K   "       G  K   !      H  N          H  N       #       H  N             H  N         H  N      #   @   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #      H  N            H  N      #   @  H  N      #   P  H  N      #   `  H  N      #   h  H  N   	   #   l  G  N      G  P   "      G  P   !       H  Q       #       H  Q      #      H  Q      #      H  Q      #      H  Q      #       H  Q      #   $   G  Q      G  S   "      G  S   !      G  U   "      G  U   !      G  V   "      G  V   !      G  W   "      G  W   !      G  X   "      G  X   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <        
 =   #   #   #   #   #      #   #   +  #   >        ?   =   >     @   ?      A      @   ;  A   B        C   8   +  #   D         E   C   D      F       E   ;  F   G        	 H                              I   H      J       I   ;  J   K         L   ,        M           N   L   L   L   L   L   M   M               O      N   ;  O   P        Q               #         R      Q   ;  R   S         T       C   ;  T   U       ;  T   V       ;  T   W       ;  T   X            N     6               �     ;     	      ;                P      =           �              �              o           �        
      >  	           Q      =           �              �              o           �        
      >             R      =        	   �               �     "       !   A  %   &      $   >  &   "        S      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        T      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  pT      #     	                GLSL.std.450                     main    �   �   �            !    ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\VolumetricLightingTemporalDenoising_Fragment.glsl       �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 1) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
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
*	Constrains the the given sample, by clipping it against the minimum/maximum bounding volume.
*/
vec3 Constrain(vec3 _sample, vec3 minimum, vec3 maximum)
{
#if 0
	return clamp(_sample, minimum, maximum);
#else
	vec3 p_clip = 0.5f * (maximum + minimum);
	vec3 e_clip = 0.5f * (maximum - minimum);

	vec3 v_clip = _sample - p_clip;
	vec3 v_unit = v_clip / e_clip;
	vec3 a_unit = abs(v_unit);

	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0f)
	{
		return p_clip + v_clip / ma_unit;
	}

	else
	
	{
		return _sample;
	}
#endif
}

layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures4Half;
layout (set = 1, binding = 4) uniform sampler2D VolumetricLighting;
layout (set = 1, binding = 5) uniform sampler2D PreviousTemporalBuffer;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 CurrentTemporalBuffer;
layout (location = 1) out vec4 CurrentVolumetricLighting;

void main()
{
	#define FEEDBACK_FACTOR (0.9f)
	float closest_depth = 0.0f;
	vec2 closest_velocity = vec2(0.0f);
	vec3 minimum = vec3(FLOAT32_MAXIMUM);
	vec3 center = vec3(0.0f);
	vec3 maximum = vec3(0.0f);
	for (int Y = -3; Y <= 3; ++Y)
	{
		for (int X = -3; X <= 3; ++X)
		{
			vec2 sample_coordinate = InScreenCoordinate + vec2(float(X), float(Y)) * INVERSE_FULL_MAIN_RESOLUTION;
			vec4 neighborhood_sample = texture(VolumetricLighting, sample_coordinate);
			minimum = min(minimum, neighborhood_sample.rgb);
			center += neighborhood_sample.rgb * float(X == 0 && Y == 0);
			maximum = max(maximum, neighborhood_sample.rgb);
			float neighborhood_depth = texture(SceneFeatures2Half, sample_coordinate).w;
			if (closest_depth < neighborhood_depth)
			{
				closest_depth = neighborhood_depth;
				closest_velocity = texture(SceneFeatures4Half, sample_coordinate).xy;
			}
		}
	}
	vec2 previous_screen_coordinate = InScreenCoordinate - closest_velocity;
	vec3 previous_frame = texture(PreviousTemporalBuffer, previous_screen_coordinate).rgb;
	previous_frame.rgb = Constrain(previous_frame.rgb, minimum, maximum);
	/*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the sample coordinate valid?
	*/
	float previous_frame_weight = 1.0f;
	previous_frame_weight *= float(ValidScreenCoordinate(previous_screen_coordinate));
	vec3 blended_frame = mix(center, previous_frame, previous_frame_weight * FEEDBACK_FACTOR);
	CurrentTemporalBuffer = vec4(blended_frame,1.0f);
	CurrentVolumetricLighting = vec4(blended_frame,1.0f);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     	    ValidScreenCoordinate(vf2;       X        Constrain(vf3;vf3;vf3;       _sample      minimum      maximum   4   p_clip    :   e_clip    ?   v_clip    C   v_unit    G   a_unit    J   ma_unit   c   closest_depth     d   closest_velocity      f   minimum   i   center    k   maximum   n   Y     x   X     �   sample_coordinate     �   InScreenCoordinate    �   General  	 �       FULL_MAIN_RESOLUTION      �      INVERSE_FULL_MAIN_RESOLUTION     	 �      HALF_MAIN_RESOLUTION      �      INVERSE_HALF_MAIN_RESOLUTION      �      FRAME     �      VIEW_DISTANCE     �         �   neighborhood_sample   �   VolumetricLighting    �   neighborhood_depth    �   SceneFeatures2Half    �   SceneFeatures4Half   	 �   previous_screen_coordinate    �   previous_frame    �   PreviousTemporalBuffer    �   param     �   param     �   param     �   previous_frame_weight     �   param     �   blended_frame     �   CurrentTemporalBuffer    	 �   CurrentVolumetricLighting     �   TEXTURES      �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �            BLUE_NOISE_TEXTURES     SKY_TEXTURE     Camera   	       WORLD_TO_CLIP_MATRIX     	      WORLD_TO_CAMERA_MATRIX         PREVIOUS_WORLD_TO_CLIP_MATRIX          INVERSE_WORLD_TO_CAMERA_MATRIX         INVERSE_CAMERA_TO_CLIP_MATRIX    	      CAMERA_WORLD_POSITION    	      CAMERA_FORWARD_VECTOR    	      CURRENT_FRAME_JITTER           NEAR_PLANE      	   FAR_PLANE           G  �          H  �       #       H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   G  �      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !      G  �          G  �         G  �   "       G  �   !       H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      G     "       G     !      G    "       G    !      H           H        #       H              H          H       #   @   H             H          H       #   �   H             H          H       #   �   H             H          H       #      H             H       #   @  H       #   P  H       #   `  H       #   h  H    	   #   l  G       G    "      G    !            !                               	           
   !     
   	                          !                              +                        +            +     "     �?+     '      +     5      ?+     O      ,     e         +     g   ��,     h   g   g   g   ,     j              l             m      l   +  l   o   ����+  l   v         �         ;  �   �        �                        �      �   ;  �   �      +  l   �         �           �            �      �    	 �                              �   �      �       �   ;  �   �       +  l   �       ;  �   �       +     �      ;  �   �       ;  �   �       +     �   fff?   �      �   ;  �   �      ;  �   �      +     �        �   �   �      �       �   ;  �   �        
 �                           +     �        �   �   �     �   �      �      �   ;  �   �      +     �         �   �   �      �       �   ;  �           	                                             ;              �                                             ;              Q     6               �     ;     c      ;  	   d      ;     f      ;     i      ;     k      ;  m   n      ;  m   x      ;  	   �      ;  �   �      ;     �      ;  	   �      ;     �      ;     �      ;     �      ;     �      ;     �      ;  	   �      ;     �           T      >  c           U      >  d   e        V      >  f   h        W      >  i   j        X      >  k   j        Y      >  n   o   �  p   �  p        Y      �  r   s       �  t   �  t        Y      =  l   u   n   �  
   w   u   v   �  w   q   r   �  q        [      >  x   o   �  y   �  y        [      �  {   |       �  }   �  }        [      =  l   ~   x   �  
      ~   v   �     z   {   �  z        ]      =     �   �   =  l   �   x   o     �   �   =  l   �   n   o     �   �   P     �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   �     �   �   �   >  �   �        ^      =  �   �   �   =     �   �   W  �   �   �   �   >  �   �        _      =     �   f   =  �   �   �   O     �   �   �                  �      %   �   �   >  f   �        `      =  �   �   �   O     �   �   �             =  l   �   x   �  
   �   �   �   =  l   �   n   �  
   �   �   �   �  
   �   �   �   �     �   �   "      �     �   �   �   =     �   i   �     �   �   �   >  i   �        a      =     �   k   =  �   �   �   O     �   �   �                  �      (   �   �   >  k   �        b      =  �   �   �   =     �   �   W  �   �   �   �   Q     �   �      >  �   �        c      =     �   c   =     �   �   �  
   �   �   �   �  �       �  �   �   �   �  �        e      =     �   �   >  c   �        f      =  �   �   �   =     �   �   W  �   �   �   �   O     �   �   �          >  d   �   �  �   �  �   �  |   �  |        [      =  l   �   x   �  l   �   �   �   >  x   �   �  y   �  {   �  s   �  s        Y      =  l   �   n   �  l   �   �   �   >  n   �   �  p   �  r        j      =     �   �   =     �   d   �     �   �   �   >  �   �        k      =  �   �   �   =     �   �   W  �   �   �   �   O     �   �   �             >  �   �        l      =     �   �   >  �   �   =     �   f   >  �   �   =     �   k   >  �   �   9     �      �   �   �   >  �   �        r      >  �   "        s      =     �   �   >  �   �   9  
   �      �   �     �   �   "      =     �   �   �     �   �   �   >  �   �        t      =     �   i   =     �   �   =     �   �   �     �   �   �   P     �   �   �   �        �      .   �   �   �   >  �   �        u      =     �   �   Q     �   �       Q     �   �      Q     �   �      P  �   �   �   �   �   "   >  �   �        v      =     �   �   Q     �   �       Q     �   �      Q     �   �      P  �   �   �   �   �   "   >  �   �   �  8       �   "   6  
             7  	      �          �       A              =           �  
            �         �           �          �       A               =     !       �  
   #   !   "   �     �     �  
   $         #      �  &       �  $   %   &   �  %        �       A     (      '   =     )   (   �  
   *   )      �  &   �  &   �  
   +   $      *   %   �  -       �  +   ,   -   �  ,        �       A     .      '   =     /   .   �  
   0   /   "   �  -   �  -   �  
   1   +   &   0   ,        �       �  1   8       ,  8   6               7        7        7        �     ;     4      ;     :      ;     ?      ;     C      ;     G      ;     J           1      =     6      =     7      �     8   6   7   �     9   8   5   >  4   9        2      =     ;      =     <      �     =   ;   <   �     >   =   5   >  :   >        4      =     @      =     A   4   �     B   @   A   >  ?   B        5      =     D   ?   =     E   :   �     F   D   E   >  C   F        6      =     H   C        I         H   >  G   I        8      A     K   G      =     L   K   A     M   G   '   =     N   M   A     P   G   O   =     Q   P        R      (   N   Q        S      (   L   R   >  J   S        :      =     T   J   �  
   U   T   "   �  W       �  U   V   _   �  V        <      =     X   4   =     Y   ?   =     Z   J   P     [   Z   Z   Z   �     \   Y   [   �     ]   X   \   �  ]   �  _        B      =     `      �  `   �  W   �  8                          ��KT��D	(�D��^��                       �]����M  max�s�H�W� ighb�_`Wv� e.rg2���ڥ�8   / w       tHkB��ɶ��jjib�                                   \�5s����