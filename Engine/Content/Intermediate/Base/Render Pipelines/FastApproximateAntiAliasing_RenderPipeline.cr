�nX/J1�~$�[��FastApproximateAntiAliasing_RenderPipeline                       X;      #     U                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\FastApproximateAntiAliasing_Vertex.glsl  G   �     // OpModuleProcessed entry-point main
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

/*
*	Approximates the inverse gamma transformation of a fragment to determine it's perceptual luminance.
*/
float PerceptualLuminance(vec3 fragment)
{
	return sqrt(dot(fragment, vec3(0.299f, 0.587f, 0.114f)));
}

/*
*	Applies a vignette effect on the given fragment with the given parameters.
*/
vec3 ApplyVignette(vec3 fragment, float edge_factor, float vignette_intensity)
{
	return fragment * mix(1.0f - vignette_intensity, 1.0f, edge_factor);
}

/*
*	Applies brightness to the given fragment with the given parameters.
*/
vec3 ApplyBrightness(vec3 fragment, float brightness)
{
	return fragment + vec3(brightness - 1.0f);
}

/*
*	Applies contrast to the given fragment with the given parameters.
*/
vec3 ApplyContrast(vec3 fragment, float contrast)
{
	return ((fragment - vec3(0.5f)) * contrast) + vec3(0.5f);
}

/*
*	Applies saturation to the given fragment with the given parameters.
*/
vec3 ApplySaturation(vec3 fragment, float saturation)
{
	return mix(vec3(PerceptualLuminance(fragment)), fragment, saturation);
}

/*
*	Applies tint to the given fragment with the given parameters.
*/
vec3 ApplyTint(vec3 fragment, vec3 tint_color, float tint_intensity)
{
	return mix(fragment, fragment * tint_color, tint_intensity);
}

/*
*	Applies horizontal border to the given fragment with the given parameters.
*/
vec3 ApplyHorizontalBorder(vec3 fragment, vec2 screen_coordinate, float horizontal_border)
{
	return fragment * float(screen_coordinate.y >= horizontal_border && screen_coordinate.y <= (1.0f - horizontal_border));
}

layout (set = 1, binding = 1) uniform sampler2D InputRenderTarget;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   P   General  	 P       FULL_MAIN_RESOLUTION      P      INVERSE_FULL_MAIN_RESOLUTION     	 P      HALF_MAIN_RESOLUTION      P      INVERSE_HALF_MAIN_RESOLUTION      P      FRAME     P      VIEW_DISTANCE     R         T   InputRenderTarget   G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  P       #       H  P      #      H  P      #      H  P      #      H  P      #       H  P      #   $   G  P      G  R   "      G  R   !       G  T   "      G  T   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P               #         Q      P   ;  Q   R         S       G   ;  S   T            [     6               �     ;     	      ;                ]      =           �              �              o           �        
      >  	           ^      =           �              �              o           �        
      >             _      =        	   �               �     "       !   A  %   &      $   >  &   "        `      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        a      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  (�      #     n                GLSL.std.450                     main       V               ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\FastApproximateAntiAliasing_Fragment.glsl    �   �     // OpModuleProcessed entry-point main
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

/*
*	Approximates the inverse gamma transformation of a fragment to determine it's perceptual luminance.
*/
float PerceptualLuminance(vec3 fragment)
{
	return sqrt(dot(fragment, vec3(0.299f, 0.587f, 0.114f)));
}

/*
*	Applies a vignette effect on the given fragment with the given parameters.
*/
vec3 ApplyVignette(vec3 fragment, float edge_factor, float vignette_intensity)
{
	return fragment * mix(1.0f - vignette_intensity, 1.0f, edge_factor);
}

/*
*	Applies brightness to the given fragment with the given parameters.
*/
vec3 ApplyBrightness(vec3 fragment, float brightness)
{
	return fragment + vec3(brightness - 1.0f);
}

/*
*	Applies contrast to the given fragment with the given parameters.
*/
vec3 ApplyContrast(vec3 fragment, float contrast)
{
	return ((fragment - vec3(0.5f)) * contrast) + vec3(0.5f);
}

/*
*	Applies saturation to the given fragment with the given parameters.
*/
vec3 ApplySaturation(vec3 fragment, float saturation)
{
	return mix(vec3(PerceptualLuminance(fragment)), fragment, saturation);
}

/*
*	Applies tint to the given fragment with the given parameters.
*/
vec3 ApplyTint(vec3 fragment, vec3 tint_color, float tint_intensity)
{
	return mix(fragment, fragment * tint_color, tint_intensity);
}

/*
*	Applies horizontal border to the given fragment with the given parameters.
*/
vec3 ApplyHorizontalBorder(vec3 fragment, vec2 screen_coordinate, float horizontal_border)
{
	return fragment * float(screen_coordinate.y >= horizontal_border && screen_coordinate.y <= (1.0f - horizontal_border));
}

layout (set = 1, binding = 1) uniform sampler2D InputRenderTarget;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 OutputRenderTarget;

void main()
{
    #define EDGE_THRESHOLD_MINIMUM (0.0312f)
    #define EDGE_THRESHOLD_MAXIMUM (0.125f)
    #define ITERATIONS (12)
    #define QUALITY(INDEX) (1.0f * float(INDEX) * 0.25f)
    #define SUBPIXEL_QUALITY (0.75f)
    vec3 center_color = texture(InputRenderTarget, InScreenCoordinate).rgb;
    float center_luminance = PerceptualLuminance(center_color);
    float down_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(0.0f, -1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float up_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(0.0f, 1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float left_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(-1.0f, 0.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float right_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(1.0f, 0.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float minimum_luminance = min(center_luminance, min(min(down_luminance, up_luminance), min(left_luminance, right_luminance)));
    float maximum_luminance = max(center_luminance, max(max(down_luminance, up_luminance), max(left_luminance, right_luminance)));
    float luminance_range = maximum_luminance - minimum_luminance;
    /*
    if (luminance_range < max(EDGE_THRESHOLD_MINIMUM, maximum_luminance * EDGE_THRESHOLD_MAXIMUM))
    {
	OutputRenderTarget = vec4(center_color,1.0f);
        return;
    }
    */
    float down_left_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(-1.0f, -1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float up_right_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(1.0f, 1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float up_left_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(-1.0f, 1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float down_right_luminance = PerceptualLuminance(texture(InputRenderTarget, InScreenCoordinate + vec2(1.0f, -1.0f) * INVERSE_FULL_MAIN_RESOLUTION).rgb);
    float down_up_luminance = down_luminance + up_luminance;
    float left_right_luminance = left_luminance + right_luminance;
    float left_corners_luminance = down_left_luminance + up_left_luminance;
    float down_corners_luminance = down_left_luminance + down_right_luminance;
    float right_corners_luminance = down_right_luminance + up_right_luminance;
    float up_corners_luminance = up_right_luminance + up_left_luminance;
    float horizontal_edge =  abs(-2.0f * left_luminance + left_corners_luminance) + abs(-2.0f * center_luminance + down_up_luminance ) * 2.0f + abs(-2.0f * right_luminance + right_corners_luminance);
    float vertical_edge = abs(-2.0f * up_luminance + up_corners_luminance) + abs(-2.0f * center_luminance + left_right_luminance) * 2.0 + abs(-2.0f * down_luminance + down_corners_luminance);
    bool is_horizontal_edge = horizontal_edge >= vertical_edge;
    float luminance_1 = is_horizontal_edge ? down_luminance : left_luminance;
    float luminance_2 = is_horizontal_edge ? up_luminance : right_luminance;
    float gradient_1 = luminance_1 - center_luminance;
    float gradient_2 = luminance_2 - center_luminance;
    bool is_1_steepest = abs(gradient_1) >= abs(gradient_2);
    float scaled_gradient = 0.25f * max(abs(gradient_1), abs(gradient_2));
    float step_length = is_horizontal_edge ? INVERSE_FULL_MAIN_RESOLUTION.y : INVERSE_FULL_MAIN_RESOLUTION.x;
    float local_average_luminance = 0.0f;
    if (is_1_steepest)
    {
        step_length = -step_length;
        local_average_luminance = 0.5f * (luminance_1 + center_luminance);
    }
    else
    {
        local_average_luminance = 0.5f * (luminance_2 + center_luminance);
    }
    vec2 current_screen_coordinate = InScreenCoordinate;
    if (is_horizontal_edge)
    {
        current_screen_coordinate.y += step_length * 0.5f;
    }
    else
    {
        current_screen_coordinate.x += step_length * 0.5f;
    }
    vec2 offset = is_horizontal_edge ? vec2(INVERSE_FULL_MAIN_RESOLUTION.x, 0.0f) : vec2(0.0f, INVERSE_FULL_MAIN_RESOLUTION.y);
    vec2 screen_coordinate_1 = current_screen_coordinate - offset;
    vec2 screen_coordinate_2 = current_screen_coordinate + offset;
    float end_luminance_1 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_1).rgb);
    float end_luminance_2 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_2).rgb);
    end_luminance_1 -= local_average_luminance;
    end_luminance_2 -= local_average_luminance;
    bool reached_1 = abs(end_luminance_1) >= scaled_gradient;
    bool reached_2 = abs(end_luminance_2) >= scaled_gradient;
    bool reached_both = reached_1 && reached_2;
    if (!reached_1)
    {
        screen_coordinate_1 -= offset;
    }
    if (!reached_2)
    {
        screen_coordinate_2 += offset;
    }
    if (!reached_both)
    {
        for (uint i = 2; i < ITERATIONS; ++i)
        {
            if (!reached_1)
            {
                end_luminance_1 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_1).rgb);
                end_luminance_1 = end_luminance_1 - local_average_luminance;
            }
            if (!reached_2)
            {
                end_luminance_2 = PerceptualLuminance(texture(InputRenderTarget, screen_coordinate_2).rgb);
                end_luminance_2 = end_luminance_2 - local_average_luminance;
            }
            bool reached_1 = abs(end_luminance_1) >= scaled_gradient;
            bool reached_2 = abs(end_luminance_2) >= scaled_gradient;
            bool reached_both = reached_1 && reached_2;
            if (!reached_1)
            {
                screen_coordinate_1 -= offset;
            }
            if (!reached_2)
            {
                screen_coordinate_2 += offset;
            }
            if (reached_both)
            {
                break;
            }
        }
    }
    float distance_1 = is_horizontal_edge ? (InScreenCoordinate.x - screen_coordinate_1.x) : (InScreenCoordinate.y - screen_coordinate_1.y);
    float distance_2 = is_horizontal_edge ? (screen_coordinate_2.x - InScreenCoordinate.x) : (screen_coordinate_2.y - InScreenCoordinate.y);
    bool is_direction_1 = distance_1 < distance_2;
    float final_distance = min(distance_1, distance_2);
    float edge_thickness = (distance_1 + distance_2);
    float texel_offset = -final_distance / edge_thickness + 0.5f;
    bool is_center_luminance_smaller = center_luminance < local_average_luminance;
    bool is_correct_variation = ((is_direction_1 ? end_luminance_1 : end_luminance_2) < 0.0f) != is_center_luminance_smaller;
    float final_offset = is_correct_variation ? texel_offset : 0.0f;
    {
        float average_luminance = (1.0f / 12.0f) * (2.0f * (down_up_luminance + left_right_luminance) + left_corners_luminance + right_corners_luminance);
        float sub_pixel_offset_1 = clamp(abs(average_luminance - center_luminance) / luminance_range, 0.0f, 1.0f);
        float sub_pixel_offset_2 = (-2.0f * sub_pixel_offset_1 + 3.0f) * sub_pixel_offset_1 * sub_pixel_offset_1;
        float sub_pixel_offset_final = sub_pixel_offset_2 * sub_pixel_offset_2 * SUBPIXEL_QUALITY;
        final_offset = max(final_offset, sub_pixel_offset_final);
    }
    vec2 final_screen_coordinate = InScreenCoordinate;
    if (is_horizontal_edge)
    {
        final_screen_coordinate.y += final_offset * step_length;
    }
    else
    {
        final_screen_coordinate.x += final_offset * step_length;
    }
    vec4 final_color = texture(InputRenderTarget, final_screen_coordinate);
	OutputRenderTarget = final_color;
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     	    PerceptualLuminance(vf3;         fragment         center_color         InputRenderTarget        InScreenCoordinate    %   center_luminance      &   param     )   down_luminance    0   General  	 0       FULL_MAIN_RESOLUTION      0      INVERSE_FULL_MAIN_RESOLUTION     	 0      HALF_MAIN_RESOLUTION      0      INVERSE_HALF_MAIN_RESOLUTION      0      FRAME     0      VIEW_DISTANCE     2         ;   param     >   up_luminance      H   param     K   left_luminance    T   param     W   right_luminance   `   param     c   minimum_luminance     m   maximum_luminance     w   luminance_range   {   down_left_luminance   �   param     �   up_right_luminance    �   param     �   up_left_luminance     �   param     �   down_right_luminance      �   param     �   down_up_luminance     �   left_right_luminance      �   left_corners_luminance    �   down_corners_luminance    �   right_corners_luminance   �   up_corners_luminance      �   horizontal_edge   �   vertical_edge     �   is_horizontal_edge    �   luminance_1   �   luminance_2   �   gradient_1    �   gradient_2      is_1_steepest     	  scaled_gradient     step_length     local_average_luminance  	 0  current_screen_coordinate     B  offset    O  screen_coordinate_1   S  screen_coordinate_2   W  end_luminance_1   [  param     ^  end_luminance_2   b  param     k  reached_1     p  reached_2     u  reached_both      �  i     �  param     �  param     �  reached_1     �  reached_2     �  reached_both      �  distance_1    �  distance_2    �  is_direction_1    �  final_distance    �  edge_thickness      texel_offset     	   is_center_luminance_smaller     is_correct_variation        final_offset        average_luminance     "  sub_pixel_offset_1    *  sub_pixel_offset_2    3  sub_pixel_offset_final    <  final_screen_coordinate   Q  final_color   V  OutputRenderTarget    [  TEXTURES     	 ]  GlobalTextureAverageValues   	 ]      TEXTURE_AVERAGE_VALUES    _        `  Material      `      _Properties   `     _AlbedoThickness     	 `     _NormalMapDisplacement    `     _MaterialProperties   `     _Opacity      `     _EmissiveMultiplier   `     _Padding1     `     _Padding2     c  GlobalMaterials   c      MATERIALS     e        i  BLUE_NOISE_TEXTURES   m  SKY_TEXTURE G     "      G     !      G            H  0       #       H  0      #      H  0      #      H  0      #      H  0      #       H  0      #   $   G  0      G  2   "      G  2   !       G  V         G  [  "       G  [  !       G  \        H  ]      #       G  ]     G  _  "       G  _  !      H  `      #       H  `     #      H  `     #      H  `     #      H  `     #      H  `     #      H  `     #      H  `     #      G  b         H  c      #       G  c     G  e  "       G  e  !      G  i  "       G  i  !      G  m  "       G  m  !           !                               	         !  
      	   +        ��>+        �E?+        �x�=,                  	                                                  ;                                   ;             !            $         +     ,       +     -     ��,     .   ,   -     /             0               /         1      0   ;  1   2        3          +  3   4         5         +     A     �?,     B   ,   A   ,     N   -   ,   ,     Z   A   ,   ,     ~   -   -   ,     �   A   A   ,     �   -   A   ,     �   A   -   +     �      �+     �      @  �      �      �   +     
    �>+  /                   +  /         +     %     ?   /           �     /   +  /   �     +  /   �        �        +       ���=+     -    @@+     7    @?   P     !      U     !   ;  U  V     +  /   X       Y     X     Z      Y  ;  Z  [        \  !   X    ]  \     ^     ]  ;  ^  _      
 `  /   /   /   /   /      /   /   +  /   a       b  `  a    c  b     d     c  ;  d  e     +  /   f        g     f     h      g  ;  h  i       	 j                             k  j     l      k  ;  l  m           ]     6               �     ;  	         ;  $   %      ;  	   &      ;  $   )      ;  	   ;      ;  $   >      ;  	   H      ;  $   K      ;  	   T      ;  $   W      ;  	   `      ;  $   c      ;  $   m      ;  $   w      ;  $   {      ;  	   �      ;  $   �      ;  	   �      ;  $   �      ;  	   �      ;  $   �      ;  	   �      ;  $   �      ;  $   �      ;  $   �      ;  $   �      ;  $   �      ;  $   �      ;  $   �      ;  $   �      ;  �   �      ;  $   �      ;  $   �      ;  $   �      ;  $   �      ;  �        ;  $   	     ;  $        ;  $        ;  $        ;  /  0     ;  /  B     ;  /  D     ;  /  O     ;  /  S     ;  $   W     ;  	   [     ;  $   ^     ;  	   b     ;  �   k     ;  �   p     ;  �   u     ;  �  �     ;  	   �     ;  	   �     ;  �   �     ;  �   �     ;  �   �     ;  $   �     ;  $   �     ;  $   �     ;  $   �     ;  �   �     ;  $   �     ;  $   �     ;  $        ;  �        ;  �        ;  $        ;  $        ;  $   "     ;  $   *     ;  $   3     ;  /  <     ;  P  Q          d      =           =            W  !   "          O     #   "   "             >     #        e      =     '      >  &   '   9     (      &   >  %   (        f      =     *      =     +      A  5   6   2   4   =     7   6   �     8   .   7   �     9   +   8   W  !   :   *   9   O     <   :   :             >  ;   <   9     =      ;   >  )   =        g      =     ?      =     @      A  5   C   2   4   =     D   C   �     E   B   D   �     F   @   E   W  !   G   ?   F   O     I   G   G             >  H   I   9     J      H   >  >   J        h      =     L      =     M      A  5   O   2   4   =     P   O   �     Q   N   P   �     R   M   Q   W  !   S   L   R   O     U   S   S             >  T   U   9     V      T   >  K   V        i      =     X      =     Y      A  5   [   2   4   =     \   [   �     ]   Z   \   �     ^   Y   ]   W  !   _   X   ^   O     a   _   _             >  `   a   9     b      `   >  W   b        j      =     d   %   =     e   )   =     f   >        g      %   e   f   =     h   K   =     i   W        j      %   h   i        k      %   g   j        l      %   d   k   >  c   l        k      =     n   %   =     o   )   =     p   >        q      (   o   p   =     r   K   =     s   W        t      (   r   s        u      (   q   t        v      (   n   u   >  m   v        l      =     x   m   =     y   c   �     z   x   y   >  w   z        t      =     |      =     }      A  5      2   4   =     �      �     �   ~   �   �     �   }   �   W  !   �   |   �   O     �   �   �             >  �   �   9     �      �   >  {   �        u      =     �      =     �      A  5   �   2   4   =     �   �   �     �   �   �   �     �   �   �   W  !   �   �   �   O     �   �   �             >  �   �   9     �      �   >  �   �        v      =     �      =     �      A  5   �   2   4   =     �   �   �     �   �   �   �     �   �   �   W  !   �   �   �   O     �   �   �             >  �   �   9     �      �   >  �   �        w      =     �      =     �      A  5   �   2   4   =     �   �   �     �   �   �   �     �   �   �   W  !   �   �   �   O     �   �   �             >  �   �   9     �      �   >  �   �        x      =     �   )   =     �   >   �     �   �   �   >  �   �        y      =     �   K   =     �   W   �     �   �   �   >  �   �        z      =     �   {   =     �   �   �     �   �   �   >  �   �        {      =     �   {   =     �   �   �     �   �   �   >  �   �        |      =     �   �   =     �   �   �     �   �   �   >  �   �        }      =     �   �   =     �   �   �     �   �   �   >  �   �        ~      =     �   K   �     �   �   �   =     �   �   �     �   �   �        �         �   =     �   %   �     �   �   �   =     �   �   �     �   �   �        �         �   �     �   �   �   �     �   �   �   =     �   W   �     �   �   �   =     �   �   �     �   �   �        �         �   �     �   �   �   >  �   �              =     �   >   �     �   �   �   =     �   �   �     �   �   �        �         �   =     �   %   �     �   �   �   =     �   �   �     �   �   �        �         �   �     �   �   �   �     �   �   �   =     �   )   �     �   �   �   =     �   �   �     �   �   �        �         �   �     �   �   �   >  �   �        �      =     �   �   =     �   �   �  �   �   �   �   >  �   �        �      =  �   �   �   =     �   )   =     �   K   �     �   �   �   �   >  �   �        �      =  �   �   �   =     �   >   =     �   W   �     �   �   �   �   >  �   �        �      =     �   �   =     �   %   �     �   �   �   >  �   �        �      =        �   =       %   �            >  �          �      =       �                  =       �                  �  �         >           �      =       �                  =       �                            (       �       
    >  	         �      =  �     �   �        �        �         �      A      2   4     =         >      �    �         �      A      2   4     =         >      �    �    =              �      >           �      >    ,        �      =  �        �  "      �     !  *  �  !       �      =     #         $  #  >    $       �      =     &  �   =     '  %   �     (  &  '  �     )  %  (  >    )  �  "  �  *       �      =     +  �   =     ,  %   �     -  +  ,  �     .  %  -  >    .  �  "  �  "       �      =     1     >  0  1       �      =  �   2  �   �  4      �  2  3  ;  �  3       �      =     5    �     6  5  %  A  $   7  0    =     8  7  �     9  8  6  A  $   :  0    >  :  9  �  4  �  ;       �      =     <    �     =  <  %  A  $   >  0    =     ?  >  �     @  ?  =  A  $   A  0    >  A  @  �  4  �  4       �      =  �   C  �   �  F      �  C  E  J  �  E       �      A    G  2   4     =     H  G  P     I  H  ,   >  D  I  �  F  �  J       �      A    K  2   4     =     L  K  P     M  ,   L  >  D  M  �  F  �  F  =     N  D       �      >  B  N       �      =     P  0  =     Q  B  �     R  P  Q  >  O  R       �      =     T  0  =     U  B  �     V  T  U  >  S  V       �      =     X     =     Y  O  W  !   Z  X  Y  O     \  Z  Z            >  [  \  9     ]     [  >  W  ]       �      =     _     =     `  S  W  !   a  _  `  O     c  a  a            >  b  c  9     d     b  >  ^  d       �      =     e    =     f  W  �     g  f  e  >  W  g       �      =     h    =     i  ^  �     j  i  h  >  ^  j       �      =     l  W       m        l  =     n  	  �  �   o  m  n  >  k  o       �      =     q  ^       r        q  =     s  	  �  �   t  r  s  >  p  t       �      =  �   v  k  =  �   w  p  �  �   x  v  w  >  u  x       �      =  �   y  k  �  �   z  y  �  |      �  z  {  |  �  {       �      =     }  B  =     ~  O  �       ~  }  >  O    �  |  �  |       �      =  �   �  p  �  �   �  �  �  �      �  �  �  �  �  �       �      =     �  B  =     �  S  �     �  �  �  >  S  �  �  �  �  �       �      =  �   �  u  �  �   �  �  �  �      �  �  �  �  �  �       �      >  �  �  �  �  �  �       �      �  �  �      �  �  �  �       �      =  /   �  �  �  �   �  �  �  �  �  �  �  �  �       �      =  �   �  k  �  �   �  �  �  �      �  �  �  �  �  �       �      =     �     =     �  O  W  !   �  �  �  O     �  �  �            >  �  �  9     �     �  >  W  �       �      =     �  W  =     �    �     �  �  �  >  W  �  �  �  �  �       �      =  �   �  p  �  �   �  �  �  �      �  �  �  �  �  �       �      =     �     =     �  S  W  !   �  �  �  O     �  �  �            >  �  �  9     �     �  >  ^  �       �      =     �  ^  =     �    �     �  �  �  >  ^  �  �  �  �  �       �      =     �  W       �        �  =     �  	  �  �   �  �  �  >  �  �       �      =     �  ^       �        �  =     �  	  �  �   �  �  �  >  �  �       �      =  �   �  �  =  �   �  �  �  �   �  �  �  >  �  �       �      =  �   �  �  �  �   �  �  �  �      �  �  �  �  �  �       �      =     �  B  =     �  O  �     �  �  �  >  O  �  �  �  �  �       �      =  �   �  �  �  �   �  �  �  �      �  �  �  �  �  �       �      =     �  B  =     �  S  �     �  �  �  >  S  �  �  �  �  �       �      =  �   �  �  �  �      �  �  �  �  �  �       �      �  �  �  �  �  �  �  �       �      =  /   �  �  �  /   �  �  4   >  �  �  �  �  �  �  �  �  �  �       �      =  �   �  �   �  �      �  �  �  �  �  �       �      A  �  �       =     �  �  A  $   �  O    =     �  �  �     �  �  �  >  �  �  �  �  �  �       �      A  �  �       =     �  �  A  $   �  O    =     �  �  �     �  �  �  >  �  �  �  �  �  �  =     �  �       �      >  �  �       �      =  �   �  �   �  �      �  �  �  �  �  �       �      A  $   �  S    =     �  �  A  �  �       =     �  �  �     �  �  �  >  �  �  �  �  �  �       �      A  $   �  S    =     �  �  A  �  �       =     �  �  �     �  �  �  >  �  �  �  �  �  �  =     �  �       �      >  �  �       �      =     �  �  =     �  �  �  �   �  �  �  >  �  �       �      =     �  �  =     �  �       �     %   �  �  >  �  �       �      =     �  �  =     �  �  �        �  �  >  �          �      =       �           =       �  �           �         %  >           �      =       %   =     	    �  �   
    	  >    
       �      =  �     �  =       W  =       ^  �             �  �       ,   =  �       �  �         >           �      =  �       =         �           ,   >           �      =       �   =       �   �           �       �     =       �   �           =       �   �            �     !       >    !       �      =     #    =     $  %   �     %  #  $       &        %  =     '  w   �     (  &  '       )     +   (  ,   A   >  "  )       �      =     +  "  �     ,  �   +  �     .  ,  -  =     /  "  �     0  .  /  =     1  "  �     2  0  1  >  *  2       �      =     4  *  =     5  *  �     6  4  5  �     8  6  7  >  3  8       �      =     9    =     :  3       ;     (   9  :  >    ;       �      =     =     >  <  =       �      =  �   >  �   �  @      �  >  ?  H  �  ?       �      =     A    =     B    �     C  A  B  A  $   D  <    =     E  D  �     F  E  C  A  $   G  <    >  G  F  �  @  �  H       �      =     I    =     J    �     K  I  J  A  $   L  <    =     M  L  �     N  M  K  A  $   O  <    >  O  N  �  @  �  @       �      =     R     =     S  <  W  !   T  R  S  >  Q  T       �      =  !   W  Q  >  V  W  �  8       "  (   6            
   7  	      �          $      =           �                               �     8                          (�D��^��                       �ĸ�q�Z  Y         ��#�7��                                   \�5s����