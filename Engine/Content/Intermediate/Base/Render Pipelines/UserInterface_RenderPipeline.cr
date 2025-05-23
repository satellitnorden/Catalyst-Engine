ïnX/J1Èäï6·"UserInterface_RenderPipeline                                     9      #     `                 GLSL.std.450              
        main       '   4   7   =        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\UserInterface_Vertex.glsl    D
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

//Constants.
#define USER_INTERFACE_INSTANCE_FLAG_IMAGE (BIT(0))
#define USER_INTERFACE_INSTANCE_FLAG_TEXT (BIT(1))
#define USER_INTERFACE_INSTANCE_FLAG_TEXTURE (BIT(2))
//User interface instance struct definition.
struct UserInterfaceInstance
{
	vec4 _Positions[4];
	vec2 _TextureCoordinates[4];
	uint _Flags;
	uint _ColorOrTexture;
	uint _ColorOpacity;
	float _SmoothingFactor;
};
layout (std430, set = 1, binding = 0) buffer UserInterface
{
	layout (offset = 0) UserInterfaceInstance[] USER_INTERFACE_INSTANCES;
};

layout (set = 1, binding = 1) uniform sampler SAMPLER;

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 TRANSFORMATION;
	layout (offset = 64) uint START_INSTANCE;
};

layout (location = 0) out vec2 OutTextureCoordinate;
layout (location = 1) out uint OutInstanceIndex;

void main()
{
    uint instance_index = START_INSTANCE + gl_InstanceIndex;
    vec4 position = USER_INTERFACE_INSTANCES[instance_index]._Positions[gl_VertexIndex];
    vec2 texture_coordinate = USER_INTERFACE_INSTANCES[instance_index]._TextureCoordinates[gl_VertexIndex];
    OutTextureCoordinate = texture_coordinate;
    OutInstanceIndex = instance_index;
	gl_Position = TRANSFORMATION*position;
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   instance_index       PushConstantData             TRANSFORMATION          START_INSTANCE                gl_InstanceIndex         position          UserInterfaceInstance             _Positions           _TextureCoordinates          _Flags           _ColorOrTexture          _ColorOpacity            _SmoothingFactor      "   UserInterface    
 "       USER_INTERFACE_INSTANCES      $         '   gl_VertexIndex    -   texture_coordinate    4   OutTextureCoordinate      7   OutInstanceIndex      ;   gl_PerVertex      ;       gl_Position   ;      gl_PointSize      ;      gl_ClipDistance   ;      gl_CullDistance   =         I   TEXTURES     	 K   GlobalTextureAverageValues   	 K       TEXTURE_AVERAGE_VALUES    M         N   Material      N       _Properties   N      _AlbedoThickness     	 N      _NormalMapDisplacement    N      _MaterialProperties   N      _Opacity      N      _EmissiveMultiplier   N      _Padding1     N      _Padding2     Q   GlobalMaterials   Q       MATERIALS     S         X   BLUE_NOISE_TEXTURES   \   SKY_TEXTURE   _   SAMPLER H            H         #       H               H        #   @   G        G        +   G           G           H          #       H         #   @   H         #   `   H         #   d   H         #   h   H         #   l   G  !      p   H  "       #       G  "      G  $   "      G  $   !       G  '      *   G  4          G  7         H  ;              H  ;            H  ;            H  ;            G  ;      G  I   "       G  I   !       G  J         H  K       #       G  K      G  M   "       G  M   !      H  N       #       H  N      #      H  N      #      H  N      #      H  N      #      H  N      #      H  N      #      H  N      #      G  P          H  Q       #       G  Q      G  S   "       G  S   !      G  X   "       G  X   !      G  \   "       G  \   !      G  _   "      G  _   !           !                                   
            
                                  	      ;        	               +                 	                  ;                       +                           
                                      
     !         "   !      #      "   ;  #   $      +     %       ;     '         )            ,            0            3         ;  3   4         6         ;  6   7      +     9        :   
   9     ;      
   :   :      <      ;   ;  <   =         >   	         C          	 E   
                         +     F        G   E   F      H       G   ;  H   I         J      F     K   J      L      K   ;  L   M       
 N                  
         +     O        P   N   O     Q   P      R      Q   ;  R   S        T   E   +     U         V   T   U      W       V   ;  W   X        	 Y   
                           Z   Y      [       Z   ;  [   \         ]      ^       ]   ;  ^   _            3     6               ø     ;     	      ;           ;  ,   -           5      A              =           =           |                         >  	           6      =     &   	   =     (   '   A  )   *   $   %   &   %   (   =     +   *   >     +        7      =     .   	   =     /   '   A  0   1   $   %   .      /   =     2   1   >  -   2        8      =     5   -   >  4   5        9      =     8   	   >  7   8        :      A  >   ?      %   =     @   ?   =     A           B   @   A   A  C   D   =   %   >  D   B   ý  8  ¤D      #     À                 GLSL.std.450                     main    :   F   ¨                ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\UserInterface_Fragment.glsl  	   Ì     // OpModuleProcessed entry-point main
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

//Constants.
#define USER_INTERFACE_INSTANCE_FLAG_IMAGE (BIT(0))
#define USER_INTERFACE_INSTANCE_FLAG_TEXT (BIT(1))
#define USER_INTERFACE_INSTANCE_FLAG_TEXTURE (BIT(2))
//User interface instance struct definition.
struct UserInterfaceInstance
{
	vec4 _Positions[4];
	vec2 _TextureCoordinates[4];
	uint _Flags;
	uint _ColorOrTexture;
	uint _ColorOpacity;
	float _SmoothingFactor;
};
layout (std430, set = 1, binding = 0) buffer UserInterface
{
	layout (offset = 0) UserInterfaceInstance[] USER_INTERFACE_INSTANCES;
};

layout (set = 1, binding = 1) uniform sampler SAMPLER;

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 TRANSFORMATION;
	layout (offset = 64) uint START_INSTANCE;
};

layout (location = 0) in vec2 InTextureCoordinate;
layout (location = 1) flat in uint InInstanceIndex;

layout (location = 0) out vec4 SceneLowDynamicRange1;

void main()
{
    vec2 texture_coordinate = InTextureCoordinate;
    vec4 color;
    if (TEST_BIT(USER_INTERFACE_INSTANCES[InInstanceIndex]._Flags, USER_INTERFACE_INSTANCE_FLAG_TEXTURE))
    {
        color = texture(sampler2D(TEXTURES[USER_INTERFACE_INSTANCES[InInstanceIndex]._ColorOrTexture], SAMPLER), texture_coordinate);
    }
    else
    {
        color = UnpackColor(USER_INTERFACE_INSTANCES[InInstanceIndex]._ColorOrTexture);
    }
    if (TEST_BIT(USER_INTERFACE_INSTANCES[InInstanceIndex]._Flags, USER_INTERFACE_INSTANCE_FLAG_TEXT))
    {
        float opacity = smoothstep(0.5f - USER_INTERFACE_INSTANCES[InInstanceIndex]._SmoothingFactor, 0.5f, color.x);
        color = vec4(1.0f, 1.0f, 1.0f, opacity);
        if (false)
        {
            float drop_shadow_opacity = max(1.0f - smoothstep(0.0f, 0.5f, color.x) - opacity, 0.0f);
            color.rgb *= 1.0f - drop_shadow_opacity;
            color.a = max(drop_shadow_opacity, opacity);
        }
    }
    vec4 color_opacity = UnpackColor(USER_INTERFACE_INSTANCES[InInstanceIndex]._ColorOpacity);
    color *= color_opacity;
	SceneLowDynamicRange1 = color;
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        unpacked      8   texture_coordinate    :   InTextureCoordinate   ?   UserInterfaceInstance     ?       _Positions    ?      _TextureCoordinates   ?      _Flags    ?      _ColorOrTexture   ?      _ColorOpacity     ?      _SmoothingFactor      A   UserInterface    
 A       USER_INTERFACE_INSTANCES      C         F   InInstanceIndex   Q   color     V   TEXTURES      `   SAMPLER   h   param     s   opacity      drop_shadow_opacity      color_opacity         param     ¨   SceneLowDynamicRange1    	 «   GlobalTextureAverageValues   	 «       TEXTURE_AVERAGE_VALUES    ­         ®   Material      ®       _Properties   ®      _AlbedoThickness     	 ®      _NormalMapDisplacement    ®      _MaterialProperties   ®      _Opacity      ®      _EmissiveMultiplier   ®      _Padding1     ®      _Padding2     ±   GlobalMaterials   ±       MATERIALS     ³         ·   BLUE_NOISE_TEXTURES   »   SKY_TEXTURE   ½   PushConstantData      ½       TRANSFORMATION    ½      START_INSTANCE    ¿       G  :          G  =         G  >         H  ?       #       H  ?      #   @   H  ?      #   `   H  ?      #   d   H  ?      #   h   H  ?      #   l   G  @      p   H  A       #       G  A      G  C   "      G  C   !       G  F      G  F         G  V   "       G  V   !       G  `   "      G  `   !      G  ¨          G  ª         H  «       #       G  «      G  ­   "       G  ­   !      H  ®       #       H  ®      #      H  ®      #      H  ®      #      H  ®      #      H  ®      #      H  ®      #      H  ®      #      G  °          H  ±       #       G  ±      G  ³   "       G  ³   !      G  ·   "       G  ·   !      G  »   "       G  »   !      H  ½          H  ½       #       H  ½             H  ½      #   @   G  ½           !                                   	         
   	      !     
               
   +        ÿ   +  	        C+                     	               +           +     !      +     $      +     )      +     ,      +     1        6   	         7      6      9      6   ;  9   :      +     <        =   
   <     >   6   <     ?   =   >            	     @   ?     A   @      B      A   ;  B   C      +     D          E         ;  E   F      +     H         I           M    	 R   	                         +     S        T   R   S      U       T   ;  U   V       +     X         [       R     ^      _       ^   ;  _   `         b   R   +  	   t      ?+     v         w      	   +  	   ~     ?*  M      +  	               	      +              §      
   ;  §   ¨        ª   
   S     «   ª      ¬      «   ;  ¬   ­       
 ®                  	         +     ¯        °   ®   ¯     ±   °      ²      ±   ;  ²   ³      +     ´         µ   b   ´      ¶       µ   ;  ¶   ·        	 ¸   	                           ¹   ¸      º       ¹   ;  º   »         ¼   
        ½   ¼         ¾   	   ½   ;  ¾   ¿   	        5     6               ø     ;  7   8      ;     Q      ;     h      ;     s      ;           ;           ;                 7      =  6   ;   :   >  8   ;        9      =     G   F   A  I   J   C   D   G   H   =     K   J   Ç     L   K   <   ª  M   N   L   <   ÷  P       ú  N   O   f   ø  O        ;      =     W   F   A  I   Y   C   D   W   X   =     Z   Y   A  [   \   V   Z   =  R   ]   \   =  ^   a   `   V  b   c   ]   a   =  6   d   8   W  
   e   c   d   >  Q   e   ù  P   ø  f        ?      =     g   F   A  I   i   C   D   g   X   =     j   i   >  h   j   9  
   k      h   >  Q   k   ù  P   ø  P        A      =     l   F   A  I   m   C   D   l   H   =     n   m   Ç     o   n   )   ª  M   p   o   )   ÷  r       ú  p   q   r   ø  q        C      =     u   F   A  w   x   C   D   u   v   =  	   y   x     	   z   t   y   A     {   Q      =  	   |   {     	   }      1   z   t   |   >  s   }        D      =  	      s   P  
      ~   ~   ~      >  Q      ÷         ú           ø          G      A        Q      =  	           	         1      t        	      ~      =  	      s     	              	         (         >             H      =  	           	      ~      =  
      Q   O                                      A        Q      Q  	             >        A        Q   !   Q  	            >        A        Q   )   Q  	            >             I      =  	         =  	      s     	         (         A        Q   1   >        ù     ø     ù  r   ø  r        L      =        F   A  I   ¡   C   D         =     ¢   ¡   >      ¢   9  
   £          >     £        M      =  
   ¤      =  
   ¥   Q     
   ¦   ¥   ¤   >  Q   ¦        N      =  
   ©   Q   >  ¨   ©   ý  8       þ      6  
             7        ø     ;                      =           Ç              p  	           	            A              >                   =           Â              Ç              p  	           	             A     "      !   >  "                  =     #      Â     %   #   $   Ç     &   %      p  	   '   &     	   (   '      A     *      )   >  *   (              =     +      Â     -   +   ,   Ç     .   -      p  	   /   .     	   0   /      A     2      1   >  2   0              =  
   3      þ  3   8                                  <Ì&ÉWMü                       <ÊAcz                                     