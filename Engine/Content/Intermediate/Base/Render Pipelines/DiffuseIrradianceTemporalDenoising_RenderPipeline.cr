�nX/J1,J��iDiffuseIrradianceTemporalDenoising_RenderPipeline                 G      #     ]                 GLSL.std.450                      main          0         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\DiffuseIrradianceTemporalDenoising_Vertex.glsl      �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures4Half;
layout (set = 1, binding = 4) uniform sampler2D InputDiffuseIrradiance;
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
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   R   Camera   	 R       WORLD_TO_CLIP_MATRIX     	 R      WORLD_TO_CAMERA_MATRIX    R      PREVIOUS_WORLD_TO_CLIP_MATRIX     R      INVERSE_WORLD_TO_CAMERA_MATRIX    R      INVERSE_CAMERA_TO_CLIP_MATRIX    	 R      CAMERA_WORLD_POSITION    	 R      CAMERA_FORWARD_VECTOR    	 R      CURRENT_FRAME_JITTER      R      NEAR_PLANE    R   	   FAR_PLANE     T         U   General  	 U       FULL_MAIN_RESOLUTION      U      INVERSE_FULL_MAIN_RESOLUTION     	 U      HALF_MAIN_RESOLUTION      U      INVERSE_HALF_MAIN_RESOLUTION      U      FRAME     U      VIEW_DISTANCE     W         Y   SceneFeatures2Half    Z   SceneFeatures4Half    [   InputDiffuseIrradiance    \   PreviousTemporalBuffer  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  R          H  R       #       H  R             H  R         H  R      #   @   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #      H  R            H  R      #   @  H  R      #   P  H  R      #   `  H  R      #   h  H  R   	   #   l  G  R      G  T   "      G  T   !       H  U       #       H  U      #      H  U      #      H  U      #      H  U      #       H  U      #   $   G  U      G  W   "      G  W   !      G  Y   "      G  Y   !      G  Z   "      G  Z   !      G  [   "      G  [   !      G  \   "      G  \   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P   ,        Q           R   P   P   P   P   P   Q   Q               S      R   ;  S   T        U               #         V      U   ;  V   W         X       G   ;  X   Y       ;  X   Z       ;  X   [       ;  X   \            ~     6               �     ;     	      ;                �      =           �              �              o           �        
      >  	           �      =           �              �              o           �        
      >             �      =        	   �               �     "       !   A  %   &      $   >  &   "        �      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        �      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  df      #     A                GLSL.std.450                     main    T     %           !    ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\DiffuseIrradianceTemporalDenoising_Fragment.glsl     \   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures4Half;
layout (set = 1, binding = 4) uniform sampler2D InputDiffuseIrradiance;
layout (set = 1, binding = 5) uniform sampler2D PreviousTemporalBuffer;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 CurrentTemporalBuffer;
layout (location = 1) out vec4 OutputDiffuseIrradiance;

void main()
{
    #define FEEDBACK_FACTOR (0.99f)
    vec3 current_diffuse_irradiance = texture(InputDiffuseIrradiance, InScreenCoordinate).rgb;
    float current_depth = LinearizeDepth(texture(SceneFeatures2Half, InScreenCoordinate).w);
    vec2 velocity = texture(SceneFeatures4Half, InScreenCoordinate).xy;
    vec2 previous_screen_coordinate = InScreenCoordinate - velocity - INVERSE_HALF_MAIN_RESOLUTION * 0.5f;
    vec3 previous_diffuse_irradiance = vec3(0.0f);
    float previous_sample_weight = 0.0f;
    {
        vec2 sample_coordinates[4];
        sample_coordinates[0] = previous_screen_coordinate;
        sample_coordinates[1] = previous_screen_coordinate + vec2(INVERSE_HALF_MAIN_RESOLUTION.x, 0.0f);
        sample_coordinates[2] = previous_screen_coordinate + vec2(0.0f, INVERSE_HALF_MAIN_RESOLUTION.y);
        sample_coordinates[3] = previous_screen_coordinate + vec2(INVERSE_HALF_MAIN_RESOLUTION.x, INVERSE_HALF_MAIN_RESOLUTION.y);
        vec4 previous_irradiance_samples[4];
        for (uint i = 0; i < 4; ++i)
        {
           previous_irradiance_samples[i] = texture(PreviousTemporalBuffer, sample_coordinates[i]);
        }
        vec2 fractions = fract(previous_screen_coordinate * HALF_MAIN_RESOLUTION);
        float weights[4];
        for (uint i = 0; i < 4; ++i)
        {
            weights[i] = exp(-abs(current_depth - previous_irradiance_samples[i].w));
        }
        previous_sample_weight = max(max(weights[0], weights[1]), max(weights[2], weights[3]));
        float weight_sum = weights[0] + weights[1] + weights[2] + weights[3];
        float inverse_weight_sum = weight_sum > 0.0f ? 1.0f / weight_sum : 1.0f;
        for (uint i = 0; i < 4; ++i)
        {
            weights[i] *= inverse_weight_sum;
        }
        for (uint i = 0; i < 4; ++i)
        {
            previous_diffuse_irradiance += previous_irradiance_samples[i].rgb * weights[i];
        }
    }
    /*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*/
	previous_sample_weight *= float(ValidScreenCoordinate(previous_screen_coordinate));
    previous_sample_weight *= FEEDBACK_FACTOR;
    vec3 blended_diffuse_irradiance = mix(current_diffuse_irradiance, previous_diffuse_irradiance, previous_sample_weight);
	CurrentTemporalBuffer = vec4(blended_diffuse_irradiance,current_depth);
	OutputDiffuseIrradiance = vec4(blended_diffuse_irradiance,1.0f);
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     	    ValidScreenCoordinate(vf2;       X        LinearizeDepth(f1;       depth     3   Camera   	 3       WORLD_TO_CLIP_MATRIX     	 3      WORLD_TO_CAMERA_MATRIX    3      PREVIOUS_WORLD_TO_CLIP_MATRIX     3      INVERSE_WORLD_TO_CAMERA_MATRIX    3      INVERSE_CAMERA_TO_CLIP_MATRIX    	 3      CAMERA_WORLD_POSITION    	 3      CAMERA_FORWARD_VECTOR    	 3      CURRENT_FRAME_JITTER      3      NEAR_PLANE    3   	   FAR_PLANE     5        	 M   current_diffuse_irradiance    Q   InputDiffuseIrradiance    T   InScreenCoordinate    X   current_depth     Y   SceneFeatures2Half    ]   param     a   velocity      b   SceneFeatures4Half   	 g   previous_screen_coordinate    k   General  	 k       FULL_MAIN_RESOLUTION      k      INVERSE_FULL_MAIN_RESOLUTION     	 k      HALF_MAIN_RESOLUTION      k      INVERSE_HALF_MAIN_RESOLUTION      k      FRAME     k      VIEW_DISTANCE     m        	 u   previous_diffuse_irradiance   w   previous_sample_weight    {   sample_coordinates    �   i    	 �   previous_irradiance_samples   �   PreviousTemporalBuffer    �   fractions     �   i     �   weights   �   weight_sum    �   inverse_weight_sum    �   i     �   i       param    	   blended_diffuse_irradiance      CurrentTemporalBuffer     %  OutputDiffuseIrradiance   .  TEXTURES     	 0  GlobalTextureAverageValues   	 0      TEXTURE_AVERAGE_VALUES    2        3  Material      3      _Properties   3     _AlbedoThickness     	 3     _NormalMapDisplacement    3     _MaterialProperties   3     _Opacity      3     _EmissiveMultiplier   3     _Padding1     3     _Padding2     6  GlobalMaterials   6      MATERIALS     8        <  BLUE_NOISE_TEXTURES   @  SKY_TEXTURE H  3          H  3       #       H  3             H  3         H  3      #   @   H  3            H  3         H  3      #   �   H  3            H  3         H  3      #   �   H  3            H  3         H  3      #      H  3            H  3      #   @  H  3      #   P  H  3      #   `  H  3      #   h  H  3   	   #   l  G  3      G  5   "      G  5   !       G  Q   "      G  Q   !      G  T          G  Y   "      G  Y   !      G  b   "      G  b   !      H  k       #       H  k      #      H  k      #      H  k      #      H  k      #       H  k      #   $   G  k      G  m   "      G  m   !      G  �   "      G  �   !      G           G  %        G  .  "       G  .  !       G  /        H  0      #       G  0     G  2  "       G  2  !      H  3      #       H  3     #      H  3     #      H  3     #      H  3     #      H  3     #      H  3     #      H  3     #      G  5         H  6      #       G  6     G  8  "       G  8  !      G  <  "       G  <  !      G  @  "       G  @  !           !                               	           
   !     
   	               !                        +            +            +          �?+     #        0           1   0        2           3   1   1   1   1   1   2   2               4      3   ;  4   5        6          +  6   7   	      8         +  6   ;         L      2    	 N                              O   N      P       O   ;  P   Q          S         ;  S   T      ;  P   Y       +     ^      ;  P   b         k                        l      k   ;  l   m      +  6   n         o         +     r      ?,  2   v            +     x        y      x      z      y   +  6   |       +  6         +  6   �         �           �   0   x      �      �   ;  P   �          �      0     �      x      �      �   +       �p}?        0   ;         ;    %     +     +       ,  N   +     -      ,  ;  -  .        /  0   +    0  /     1     0  ;  1  2      
 3                          +     4       5  3  4    6  5     7     6  ;  7  8     +     9        :  O   9     ;      :  ;  ;  <       	 =                             >  =     ?      >  ;  ?  @           �     6               �     ;  L   M      ;     X      ;     ]      ;  	   a      ;  	   g      ;  L   u      ;     w      ;  z   {      ;  �   �      ;  �   �      ;  	   �      ;  �   �      ;  �   �      ;     �      ;     �      ;     �      ;  �   �      ;  �   �      ;  	        ;  L             �      =  O   R   Q   =     U   T   W  0   V   R   U   O  2   W   V   V             >  M   W        �      =  O   Z   Y   =     [   T   W  0   \   Z   [   Q     _   \      >  ]   _   9     `      ]   >  X   `        �      =  O   c   b   =     d   T   W  0   e   c   d   O     f   e   e          >  a   f        �      =     h   T   =     i   a   �     j   h   i   A  o   p   m   n   =     q   p   �     s   q   r   �     t   j   s   >  g   t        �      >  u   v        �      >  w           �      =     }   g   A  	   ~   {   |   >  ~   }        �      =     �   g   A  8   �   m   n      =     �   �   P     �   �      �     �   �   �   A  	   �   {      >  �   �        �      =     �   g   A  8   �   m   n   #   =     �   �   P     �      �   �     �   �   �   A  	   �   {   �   >  �   �        �      =     �   g   A  8   �   m   n      =     �   �   A  8   �   m   n   #   =     �   �   P     �   �   �   �     �   �   �   A  	   �   {   n   >  �   �        �      >  �      �  �   �  �        �      �  �   �       �  �   �  �        �      =     �   �   �  
   �   �   x   �  �   �   �   �  �        �      =     �   �   =  O   �   �   =     �   �   A  	   �   {   �   =     �   �   W  0   �   �   �   A  �   �   �   �   >  �   �   �  �   �  �        �      =     �   �   �     �   �      >  �   �   �  �   �  �        �      =     �   g   A  o   �   m   �   =     �   �   �     �   �   �        �      
   �   >  �   �        �      >  �      �  �   �  �        �      �  �   �       �  �   �  �        �      =     �   �   �  
   �   �   x   �  �   �   �   �  �        �      =     �   �   =     �   X   =     �   �   A     �   �   �   ^   =     �   �   �     �   �   �        �         �        �   �        �         �   A     �   �   �   >  �   �   �  �   �  �        �      =     �   �   �     �   �      >  �   �   �  �   �  �        �      A     �   �   |   =     �   �   A     �   �      =     �   �        �      (   �   �   A     �   �   �   =     �   �   A     �   �   n   =     �   �        �      (   �   �        �      (   �   �   >  w   �        �      A     �   �   |   =     �   �   A     �   �      =     �   �   �     �   �   �   A     �   �   �   =     �   �   �     �   �   �   A     �   �   n   =     �   �   �     �   �   �   >  �   �        �      =     �   �   �  
   �   �      �  �       �  �   �   �   �  �        �      =     �   �   �     �      �   >  �   �   �  �   �  �   >  �      �  �   �  �   =     �   �        �      >  �   �        �      >  �      �  �   �  �        �      �  �   �       �  �   �  �        �      =     �   �   �  
   �   �   x   �  �   �   �   �  �        �      =     �   �   =     �   �   A     �   �   �   =     �   �   �     �   �   �   A     �   �   �   >  �   �   �  �   �  �        �      =     �   �   �     �   �      >  �   �   �  �   �  �        �      >  �      �  �   �  �        �      �  �   �       �  �   �  �        �      =        �   �  
        x   �    �   �   �  �        �      =       �   A  �     �     =  0       O  2                   =       �   A       �     =         �  2   	      =  2   
  u   �  2     
  	  >  u     �  �   �  �        �      =       �   �            >  �     �  �   �  �        �      =       g   >      9  
          �               =       w   �           >  w          �      =       w   �           >  w          �      =  2     M   =  2     u   =       w   P  2             2        .         >           �      =  2       =        X   Q     !        Q     "       Q     #       P  0   $  !  "  #     >    $       �      =  2   &    Q     '  &      Q     (  &     Q     )  &     P  0   *  '  (  )     >  %  *  �  8         "   6  
             7  	      �                A              =           �  
            �         �           �                A              =           �  
            �     �     �  
                   �  "       �      !   "   �  !              A     $      #   =     %   $   �  
   &   %      �  "   �  "   �  
   '          &   !   �  )       �  '   (   )   �  (              A     *      #   =     +   *   �  
   ,   +      �  )   �  )   �  
   -   '   "   ,   (              �  -   8       0  !   6               7        �          2      A  8   9   5   7   =     :   9   A  8   <   5   ;   =     =   <   �     >   :   =   =     ?      A  8   @   5   7   =     A   @   A  8   B   5   ;   =     C   B   �     D   A   C   �     E   ?   D   A  8   F   5   ;   =     G   F   �     H   E   G   �     I   >   H   �  I   8                          ��KT��D	(�D��^��                       �]����M Y  �s�H�W� Y  ��!D�� Y  2���ڥ�8             tHkB��ɶΓ>�ab(%                                   \�5s����