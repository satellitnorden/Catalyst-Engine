�nX/J19{Bf?�wSceneFeaturesDownsample_RenderPipeline                           �6      #     X                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\SceneFeaturesDownsample_Vertex.glsl  �	   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 0) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

layout (set = 1, binding = 1) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures4;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   P   General  	 P       FULL_MAIN_RESOLUTION      P      INVERSE_FULL_MAIN_RESOLUTION     	 P      HALF_MAIN_RESOLUTION      P      INVERSE_HALF_MAIN_RESOLUTION      P      FRAME     P      VIEW_DISTANCE     R         T   SceneFeatures1    U   SceneFeatures2    V   SceneFeatures3    W   SceneFeatures4  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  P       #       H  P      #      H  P      #      H  P      #      H  P      #       H  P      #   $   G  P      G  R   "      G  R   !       G  T   "      G  T   !      G  U   "      G  U   !      G  V   "      G  V   !      G  W   "      G  W   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P               #         Q      P   ;  Q   R         S       G   ;  S   T       ;  S   U       ;  S   V       ;  S   W            &     6               �     ;     	      ;                (      =           �              �              o           �        
      >  	           )      =           �              �              o           �        
      >             *      =        	   �               �     "       !   A  %   &      $   >  &   "        +      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        ,      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8   Q      #                     GLSL.std.450              
       main    %   k   x   z   �                ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\SceneFeaturesDownsample_Fragment.glsl    \   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 0) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

layout (set = 1, binding = 1) uniform sampler2D SceneFeatures1;
layout (set = 1, binding = 2) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures3;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures4;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 SceneFeatures1Half;
layout (location = 1) out vec4 SceneFeatures2Half;
layout (location = 2) out vec4 SceneFeatures3Half;
layout (location = 3) out vec4 SceneFeatures4Half;

void main()
{
	vec2 texel_offset = vec2(INVERSE_FULL_MAIN_RESOLUTION.x * 0.5f, INVERSE_FULL_MAIN_RESOLUTION.y * 0.5f);
	vec4 scene_features_2_sample_1 = texture(SceneFeatures2, InScreenCoordinate + vec2(-texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_2 = texture(SceneFeatures2, InScreenCoordinate + vec2(-texel_offset.x, texel_offset.y));
	vec4 scene_features_2_sample_3 = texture(SceneFeatures2, InScreenCoordinate + vec2(texel_offset.x, -texel_offset.y));
	vec4 scene_features_2_sample_4 = texture(SceneFeatures2, InScreenCoordinate + vec2(texel_offset.x, texel_offset.y));
    if (scene_features_2_sample_1.w > scene_features_2_sample_2.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_3.w
    	&& scene_features_2_sample_1.w > scene_features_2_sample_4.w)
    {
	SceneFeatures1Half = texture(SceneFeatures1,InScreenCoordinate+vec2(-texel_offset.x,-texel_offset.y));
	SceneFeatures2Half = scene_features_2_sample_1;
	SceneFeatures3Half = texture(SceneFeatures3,InScreenCoordinate+vec2(-texel_offset.x,-texel_offset.y));
	SceneFeatures4Half = texture(SceneFeatures4,InScreenCoordinate+vec2(-texel_offset.x,-texel_offset.y));
    }
    else if (	scene_features_2_sample_2.w > scene_features_2_sample_3.w
    			&& scene_features_2_sample_2.w > scene_features_2_sample_4.w)
    {
	SceneFeatures1Half = texture(SceneFeatures1,InScreenCoordinate+vec2(-texel_offset.x,texel_offset.y));
	SceneFeatures2Half = scene_features_2_sample_2;
	SceneFeatures3Half = texture(SceneFeatures3,InScreenCoordinate+vec2(-texel_offset.x,texel_offset.y));
	SceneFeatures4Half = texture(SceneFeatures4,InScreenCoordinate+vec2(-texel_offset.x,texel_offset.y));
    }
    else if (scene_features_2_sample_3.w > scene_features_2_sample_4.w)
    {
	SceneFeatures1Half = texture(SceneFeatures1,InScreenCoordinate+vec2(texel_offset.x,-texel_offset.y));
	SceneFeatures2Half = scene_features_2_sample_3;
	SceneFeatures3Half = texture(SceneFeatures3,InScreenCoordinate+vec2(texel_offset.x,-texel_offset.y));
	SceneFeatures4Half = texture(SceneFeatures4,InScreenCoordinate+vec2(texel_offset.x,-texel_offset.y));
    }
    else
    {
	SceneFeatures1Half = texture(SceneFeatures1,InScreenCoordinate+vec2(texel_offset.x,texel_offset.y));
	SceneFeatures2Half = scene_features_2_sample_4;
	SceneFeatures3Half = texture(SceneFeatures3,InScreenCoordinate+vec2(texel_offset.x,texel_offset.y));
	SceneFeatures4Half = texture(SceneFeatures4,InScreenCoordinate+vec2(texel_offset.x,texel_offset.y));
    }
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   texel_offset         General  	        FULL_MAIN_RESOLUTION            INVERSE_FULL_MAIN_RESOLUTION     	       HALF_MAIN_RESOLUTION            INVERSE_HALF_MAIN_RESOLUTION            FRAME           VIEW_DISTANCE             	    scene_features_2_sample_1     "   SceneFeatures2    %   InScreenCoordinate   	 1   scene_features_2_sample_2    	 <   scene_features_2_sample_3    	 G   scene_features_2_sample_4     k   SceneFeatures1Half    l   SceneFeatures1    x   SceneFeatures2Half    z   SceneFeatures3Half    {   SceneFeatures3    �   SceneFeatures4Half    �   SceneFeatures4    
  TEXTURES     	   GlobalTextureAverageValues   	       TEXTURE_AVERAGE_VALUES              Material            _Properties        _AlbedoThickness     	      _NormalMapDisplacement         _MaterialProperties        _Opacity           _EmissiveMultiplier        _Padding1          _Padding2       GlobalMaterials         MATERIALS               BLUE_NOISE_TEXTURES     SKY_TEXTURE H         #       H        #      H        #      H        #      H        #       H        #   $   G        G     "      G     !       G  "   "      G  "   !      G  %          G  k          G  l   "      G  l   !      G  x         G  z         G  {   "      G  {   !      G  �         G  �   "      G  �   !      G  
  "       G  
  !       G          H        #       G       G    "       G    !      H        #       H       #      H       #      H       #      H       #      H       #      H       #      H       #      G           H        #       G       G    "       G    !      G    "       G    !      G    "       G    !           !                               	                                                         ;                       +           +                        +           ?+                                   	                                         !           ;  !   "          $         ;  $   %         '           Q   +     R         j         ;  j   k      ;  !   l       ;  j   x      ;  j   z      ;  !   {       ;  j   �      ;  !   �       +                      	        ;  	  
                               ;          
                           +                                  ;         +                                ;           	                                             ;               +     6               �     ;  	   
      ;           ;     1      ;     <      ;     G           -      A                 =           �              A                 =           �              P              >  
           .      =      #   "   =     &   %   A  '   (   
      =     )   (        *   )   A  '   +   
      =     ,   +        -   ,   P     .   *   -   �     /   &   .   W     0   #   /   >     0        /      =      2   "   =     3   %   A  '   4   
      =     5   4        6   5   A  '   7   
      =     8   7   P     9   6   8   �     :   3   9   W     ;   2   :   >  1   ;        0      =      =   "   =     >   %   A  '   ?   
      =     @   ?   A  '   A   
      =     B   A        C   B   P     D   @   C   �     E   >   D   W     F   =   E   >  <   F        1      =      H   "   =     I   %   A  '   J   
      =     K   J   A  '   L   
      =     M   L   P     N   K   M   �     O   I   N   W     P   H   O   >  G   P        2      A  '   S      R   =     T   S   A  '   U   1   R   =     V   U   �  Q   W   T   V   �  Y       �  W   X   Y   �  X        3      A  '   Z      R   =     [   Z   A  '   \   <   R   =     ]   \   �  Q   ^   [   ]   �  Y   �  Y   �  Q   _   W      ^   X   �  a       �  _   `   a   �  `        4      A  '   b      R   =     c   b   A  '   d   G   R   =     e   d   �  Q   f   c   e   �  a   �  a   �  Q   g   _   Y   f   `   �  i       �  g   h   �   �  h        6      =      m   l   =     n   %   A  '   o   
      =     p   o        q   p   A  '   r   
      =     s   r        t   s   P     u   q   t   �     v   n   u   W     w   m   v   >  k   w        7      =     y      >  x   y        8      =      |   {   =     }   %   A  '   ~   
      =        ~        �      A  '   �   
      =     �   �        �   �   P     �   �   �   �     �   }   �   W     �   |   �   >  z   �        9      =      �   �   =     �   %   A  '   �   
      =     �   �        �   �   A  '   �   
      =     �   �        �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  �   �   �  i   �  �        ;      A  '   �   1   R   =     �   �   A  '   �   <   R   =     �   �   �  Q   �   �   �   �  �       �  �   �   �   �  �        <      A  '   �   1   R   =     �   �   A  '   �   G   R   =     �   �   �  Q   �   �   �   �  �   �  �   �  Q   �   �   �   �   �   �  �       �  �   �   �   �  �        >      =      �   l   =     �   %   A  '   �   
      =     �   �        �   �   A  '   �   
      =     �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  k   �        ?      =     �   1   >  x   �        @      =      �   {   =     �   %   A  '   �   
      =     �   �        �   �   A  '   �   
      =     �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  z   �        A      =      �   �   =     �   %   A  '   �   
      =     �   �        �   �   A  '   �   
      =     �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  �   �   �  �   �  �        C      A  '   �   <   R   =     �   �   A  '   �   G   R   =     �   �   �  Q   �   �   �   �  �       �  �   �   �   �  �        E      =      �   l   =     �   %   A  '   �   
      =     �   �   A  '   �   
      =     �   �        �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  k   �        F      =     �   <   >  x   �        G      =      �   {   =     �   %   A  '   �   
      =     �   �   A  '   �   
      =     �   �        �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  z   �        H      =      �   �   =     �   %   A  '   �   
      =     �   �   A  '   �   
      =     �   �        �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  �   �   �  �   �  �        L      =      �   l   =     �   %   A  '   �   
      =     �   �   A  '   �   
      =     �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  k   �        M      =     �   G   >  x   �        N      =      �   {   =     �   %   A  '   �   
      =     �   �   A  '   �   
      =     �   �   P     �   �   �   �     �   �   �   W     �   �   �   >  z   �        O      =      �   �   =     �   %   A  '      
      =          A  '     
      =         P           �       �     W       �     >  �     �  �   �  �   �  �   �  �   �  i   �  i   �  8                          (�D��^��                       ��v)� Y  �%�9�� HT_T��Y��G� �   �z��.v<            ����N⣲�]����M��х��E�s�H�W�                                   \�5s����