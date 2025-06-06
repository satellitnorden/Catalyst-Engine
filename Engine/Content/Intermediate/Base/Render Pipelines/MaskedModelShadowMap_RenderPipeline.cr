ïnX/J1Ü¦Äô&MaskedModelShadowMap_RenderPipeline                              ä8      #     ]                 GLSL.std.450              	        main    %   6   8   >        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\MaskedModelShadowMap_Vertex.glsl     +
   Ì     // OpModuleProcessed entry-point main
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

//Shadow mapping header struct definition.
struct ShadowMappingHeader
{
	uint _NumberOfShadowMapData;
};
//Shadow map data struct definition.
struct ShadowMapData
{
	mat4 _WorldToLightMatrix;
	vec3 _Direction;
	uint _ShadowMapTextureIndex;
	float _MaximumDepthBias;
};
layout (std430, set = 1, binding = 0) buffer ShadowMapping
{
	layout (offset = 0) ShadowMappingHeader SHADOW_MAPPING_HEADER;
	layout (offset = 16) ShadowMapData SHADOW_MAP_DATA[];
};

layout (set = 1, binding = 1) uniform sampler SAMPLER;

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 MODEL_MATRIX;
	layout (offset = 64) uint LIGHT_MATRIX_INDEX;
	layout (offset = 68) uint MATERIAL_INDEX;
};

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec2 InTextureCoordinate;

layout (location = 0) out vec2 OutTextureCoordinate;

void main()
{
    vec4 clip_position = SHADOW_MAP_DATA[LIGHT_MATRIX_INDEX]._WorldToLightMatrix * MODEL_MATRIX * vec4(InPosition, 1.0f);
    clip_position.z = max(clip_position.z, FLOAT32_EPSILON);
    OutTextureCoordinate = InTextureCoordinate;
	gl_Position = clip_position;
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   clip_position        ShadowMappingHeader  	        _NumberOfShadowMapData       ShadowMapData            _WorldToLightMatrix         _Direction   	       _ShadowMapTextureIndex          _MaximumDepthBias        ShadowMapping    	        SHADOW_MAPPING_HEADER           SHADOW_MAP_DATA               PushConstantData             MODEL_MATRIX            LIGHT_MATRIX_INDEX          MATERIAL_INDEX             %   InPosition    6   OutTextureCoordinate      8   InTextureCoordinate   <   gl_PerVertex      <       gl_Position   <      gl_PointSize      <      gl_ClipDistance   <      gl_CullDistance   >         F   TEXTURES     	 H   GlobalTextureAverageValues   	 H       TEXTURE_AVERAGE_VALUES    J         K   Material      K       _Properties   K      _AlbedoThickness     	 K      _NormalMapDisplacement    K      _MaterialProperties   K      _Opacity      K      _EmissiveMultiplier   K      _Padding1     K      _Padding2     N   GlobalMaterials   N       MATERIALS     P         U   BLUE_NOISE_TEXTURES   Y   SKY_TEXTURE   \   SAMPLER H         #       H            H         #       H               H        #   @   H        #   L   H        #   P   G        `   H         #       H        #      G        G     "      G     !       H            H         #       H               H        #   @   H        #   D   G        G  %          G  6          G  8         H  <              H  <            H  <            H  <            G  <      G  F   "       G  F   !       G  G         H  H       #       G  H      G  J   "       G  J   !      H  K       #       H  K      #      H  K      #      H  K      #      H  K      #      H  K      #      H  K      #      H  K      #      G  M          H  N       #       G  N      G  P   "       G  P   !      G  U   "       G  U   !      G  Y   "       G  Y   !      G  \   "      G  \   !           !                               	                                                                                                    ;                       +                               	      ;        	         	      +                               	         $         ;  $   %      +     '     ?+     -         .         +     1      4  4            5      4   ;  5   6         7      4   ;  7   8      +     :        ;      :     <         ;   ;      =      <   ;  =   >         @          	 B                            +     C        D   B   C      E       D   ;  E   F         G      C     H   G      I      H   ;  I   J       
 K                           +     L        M   K   L     N   M      O      N   ;  O   P        Q   B   +     R         S   Q   R      T       S   ;  T   U        	 V                              W   V      X       W   ;  X   Y         Z      [       Z   ;  [   \            6     6               ø     ;  	   
           8      A              =           A                    =           A      !         =     "   !        #      "   =     &   %   Q     (   &       Q     )   &      Q     *   &      P     +   (   )   *   '        ,   #   +   >  
   ,        9      A  .   /   
   -   =     0   /        2      (   0   1   A  .   3   
   -   >  3   2        :      =  4   9   8   >  6   9        ;      =     ?   
   A  @   A   >      >  A   ?   ý  8  x<      #                      GLSL.std.450                     main    d   x   }                ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\MaskedModelShadowMap_Fragment.glsl   
   Ì     // OpModuleProcessed entry-point main
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

//Shadow mapping header struct definition.
struct ShadowMappingHeader
{
	uint _NumberOfShadowMapData;
};
//Shadow map data struct definition.
struct ShadowMapData
{
	mat4 _WorldToLightMatrix;
	vec3 _Direction;
	uint _ShadowMapTextureIndex;
	float _MaximumDepthBias;
};
layout (std430, set = 1, binding = 0) buffer ShadowMapping
{
	layout (offset = 0) ShadowMappingHeader SHADOW_MAPPING_HEADER;
	layout (offset = 16) ShadowMapData SHADOW_MAP_DATA[];
};

layout (set = 1, binding = 1) uniform sampler SAMPLER;

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 MODEL_MATRIX;
	layout (offset = 64) uint LIGHT_MATRIX_INDEX;
	layout (offset = 68) uint MATERIAL_INDEX;
};

layout (location = 0) in vec2 InTextureCoordinate;

layout (location = 0) out vec4 ShadowMap;

void main()
{
    float opacity;
    EVALUATE_OPACITY(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, opacity);
    if (opacity < 0.5f)
    {
        discard;
    }
    float depth = gl_FragCoord.z;
	ShadowMap = vec4(depth,0.0f,0.0f,0.0f);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        unpacked      6   Material      6       _Properties   6      _AlbedoThickness     	 6      _NormalMapDisplacement    6      _MaterialProperties   6      _Opacity      6      _EmissiveMultiplier   6      _Padding1     6      _Padding2     9   GlobalMaterials   9       MATERIALS     ;         >   PushConstantData      >       MODEL_MATRIX      >      LIGHT_MATRIX_INDEX    >      MATERIAL_INDEX    @         N   opacity   S   TEXTURES      ^   SAMPLER   d   InTextureCoordinate   k   param     v   depth     x   gl_FragCoord      }   ShadowMap    	    GlobalTextureAverageValues   	        TEXTURE_AVERAGE_VALUES                BLUE_NOISE_TEXTURES      SKY_TEXTURE      ShadowMappingHeader  	        _NumberOfShadowMapData       ShadowMapData            _WorldToLightMatrix         _Direction   	       _ShadowMapTextureIndex          _MaximumDepthBias        ShadowMapping    	        SHADOW_MAPPING_HEADER           SHADOW_MAP_DATA          H  6       #       H  6      #      H  6      #      H  6      #      H  6      #      H  6      #      H  6      #      H  6      #      G  8          H  9       #       G  9      G  ;   "       G  ;   !      H  >          H  >       #       H  >             H  >      #   @   H  >      #   D   G  >      G  S   "       G  S   !       G  ^   "      G  ^   !      G  d          G  x         G  }          G           H         #       G        G     "       G     !      G     "       G     !      G     "       G     !      H         #       H            H         #       H               H        #   @   H        #   L   H        #   P   G        `   H         #       H        #      G        G     "      G     !            !                                   	         
   	      !     
               
   +        ÿ   +  	        C+                     	               +           +     !      +     $      +     )      +     ,      +     1       
 6                  	         +     7        8   6   7     9   8      :      9   ;  :   ;      +     <         =   
        >   =            ?   	   >   ;  ?   @   	   +     A         B   	         E         +     H   @     J    	 O   	                         +     P        Q   O   P      R       Q   ;  R   S       +     V         Y       O     \      ]       \   ;  ]   ^         `   O     b   	         c      b   ;  c   d      +  	   q      ?   w      
   ;  w   x         y      	      |      
   ;  |   }      +  	               
   P                       ;           +                 `                   ;             	    	                                              ;                         	           =         	                                  ;                5     6               ø     ;     N      ;     k      ;     v           8      A  B   C   @   A   =     D   C   A  E   F   ;   <   D   <   =     G   F   Ç     I   G   H   ª  J   K   I   H   ÷  M       ú  K   L   h   ø  L        8      A  B   T   @   A   =     U   T   A  E   W   ;   <   U   V   =     X   W   A  Y   Z   S   X   =  O   [   Z   =  \   _   ^   V  `   a   [   _   =  b   e   d   W  
   f   a   e   Q  	   g   f       >  N   g   ù  M   ø  h        8      A  B   i   @   A   =     j   i   A  E   l   ;   <   j   V   =     m   l   >  k   m   9  
   n      k   Q  	   o   n       >  N   o   ù  M   ø  M        9      =  	   p   N   ¸  J   r   p   q   ÷  t       ú  r   s   t   ø  s        ;      ü  ø  t        =      A  y   z   x   )   =  	   {   z   >  v   {        >      =  	   ~   v   P  
      ~            >  }      ý  8       þ      6  
             7        ø     ;                      =           Ç              p  	           	            A              >                   =           Â              Ç              p  	           	             A     "      !   >  "                  =     #      Â     %   #   $   Ç     &   %      p  	   '   &     	   (   '      A     *      )   >  *   (              =     +      Â     -   +   ,   Ç     .   -      p  	   /   .     	   0   /      A     2      1   >  2   0              =  
   3      þ  3   8                                  à¥£õð2D                       KÄ©                                  