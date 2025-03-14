οnX/J1«UB;πLuminanceSharpen_RenderPipeline                                  ΰ/      #     T                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\LuminanceSharpen_Vertex.glsl     q   Μ     // OpModuleProcessed entry-point main
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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float SHARPEN_STRENGTH;
	layout (offset = 4) float SHARPEN_CLAMP;
};

layout (set = 1, binding = 1) uniform sampler2D InputRenderTarget;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES      =   Material      =       _Properties   =      _AlbedoThickness     	 =      _NormalMapDisplacement    =      _MaterialProperties   =      _Opacity      =      _EmissiveMultiplier   =      _Padding1     =      _Padding2     @   GlobalMaterials   @       MATERIALS     B         G   BLUE_NOISE_TEXTURES   K   SKY_TEXTURE   L   General  	 L       FULL_MAIN_RESOLUTION      L      INVERSE_FULL_MAIN_RESOLUTION     	 L      HALF_MAIN_RESOLUTION      L      INVERSE_HALF_MAIN_RESOLUTION      L      FRAME     L      VIEW_DISTANCE     N         O   PushConstantData      O       SHARPEN_STRENGTH      O      SHARPEN_CLAMP     Q         S   InputRenderTarget   G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       H  =       #       H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      G  ?          H  @       #       G  @      G  B   "       G  B   !      G  G   "       G  G   !      G  K   "       G  K   !      H  L       #       H  L      #      H  L      #      H  L      #      H  L      #       H  L      #   $   G  L      G  N   "      G  N   !       H  O       #       H  O      #      G  O      G  S   "      G  S   !           !                             +     
     Ώ                        ;           +           +                                  ;           +          ?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <        
 =   #   #   #   #   #      #   #   +  #   >        ?   =   >     @   ?      A      @   ;  A   B        C   8   +  #   D         E   C   D      F       E   ;  F   G        	 H                              I   H      J       I   ;  J   K         L               #         M      L   ;  M   N        O            P   	   O   ;  P   Q   	      R       C   ;  R   S                 6               ψ     ;     	      ;                      =           Η              Δ              o                   
      >  	                 =           Η              Δ              o                   
      >                   =        	                       "       !   A  %   &      $   >  &   "              =     '           (   '           )   (   !   A  %   +      *   >  +   )              =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   ύ  8  88      #                      GLSL.std.450                     main       m                ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\LuminanceSharpen_Fragment.glsl   t	   Μ     // OpModuleProcessed entry-point main
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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float SHARPEN_STRENGTH;
	layout (offset = 4) float SHARPEN_CLAMP;
};

layout (set = 1, binding = 1) uniform sampler2D InputRenderTarget;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 OutputRenderTarget;

void main()
{
    #define LUMINANCE_COEFFICIENTS (vec3(0.2126f, 0.7152f, 0.0722f))
    vec3 original = texture(InputRenderTarget, InScreenCoordinate).rgb;
    vec3 sharpen_strength_luminance = LUMINANCE_COEFFICIENTS * SHARPEN_STRENGTH;
    vec3 blur = texture(InputRenderTarget, InScreenCoordinate + (INVERSE_FULL_MAIN_RESOLUTION / 3.0f)).rgb;
    blur += texture(InputRenderTarget, InScreenCoordinate + (-INVERSE_FULL_MAIN_RESOLUTION / 3.0f)).rgb;
    blur *= 0.5f;
    sharpen_strength_luminance *= 1.5f;
    vec3 sharpen = original - blur;
    vec4 sharpen_strength_luminance_clamp = vec4(sharpen_strength_luminance * (0.5f / SHARPEN_CLAMP), 0.5f);
	float sharpen_luminance = clamp(dot(vec4(sharpen, 1.0f), sharpen_strength_luminance_clamp), 0.0f, 1.0f);
	sharpen_luminance = (SHARPEN_CLAMP * 2.0f) * sharpen_luminance - SHARPEN_CLAMP;
#if 0
    vec3 output_fragment = vec3(0.0f);
    if (gl_FragCoord.x > 960.0f)
    {
        output_fragment = original + sharpen_luminance;
    }
    else
    {
        output_fragment = original;
    }
    output_fragment *= min(abs(gl_FragCoord.x - 960.0f) * 0.25f, 1.0f);
	OutputRenderTarget = vec4(output_fragment,1.0f);
#else
	OutputRenderTarget = vec4(original+sharpen_luminance,1.0f);
#endif
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   original         InputRenderTarget        InScreenCoordinate   	    sharpen_strength_luminance       PushConstantData             SHARPEN_STRENGTH            SHARPEN_CLAMP              %   blur      )   General  	 )       FULL_MAIN_RESOLUTION      )      INVERSE_FULL_MAIN_RESOLUTION     	 )      HALF_MAIN_RESOLUTION      )      INVERSE_HALF_MAIN_RESOLUTION      )      FRAME     )      VIEW_DISTANCE     +         H   sharpen   M   sharpen_strength_luminance_clamp      X   sharpen_luminance     m   OutputRenderTarget    y   TEXTURES      z   Material      z       _Properties   z      _AlbedoThickness     	 z      _NormalMapDisplacement    z      _MaterialProperties   z      _Opacity      z      _EmissiveMultiplier   z      _Padding1     z      _Padding2     }   GlobalMaterials   }       MATERIALS                 BLUE_NOISE_TEXTURES      SKY_TEXTURE G     "      G     !      G            H         #       H        #      G        H  )       #       H  )      #      H  )      #      H  )      #      H  )      #       H  )      #   $   G  )      G  +   "      G  +   !       G  m          G  y   "       G  y   !       H  z       #       H  z      #      H  z      #      H  z      #      H  z      #      H  z      #      H  z      #      H  z      #      G  |          H  }       #       G  }      G     "       G     !      G     "       G     !      G     "       G     !           !                               	          	                                                  ;                                   ;                      +        Π³Y>+        Y7?+        έ=,                                  	      ;        	               +                !   	        (             )               (         *      )   ;  *   +      +     ,         -         +     0     @@+     B      ?+     E     ΐ?   L            W         +     Z     ?+     a       +     e      @   l         ;  l   m      +  (   v        w      v      x       w   ;  x   y        
 z   (   (   (   (   (      (   (   +  (   {        |   z   {     }   |      ~      }   ;  ~         +  (                                  ;             	                                                  ;                      6               ψ     ;  	   
      ;  	         ;  	   %      ;  	   H      ;  L   M      ;  W   X                 =           =           W              O                        >  
                 A  !   "          =     #   "        $      #   >     $              =     &      =     '      A  -   .   +   ,   =     /   .   P     1   0   0        2   /   1        3   '   2   W     4   &   3   O     5   4   4             >  %   5              =     6      =     7      A  -   8   +   ,   =     9   8        :   9   P     ;   0   0        <   :   ;        =   7   <   W     >   6   =   O     ?   >   >             =     @   %        A   @   ?   >  %   A              =     C   %        D   C   B   >  %   D              =     F           G   F   E   >     G              =     I   
   =     J   %        K   I   J   >  H   K              =     N      A  !   O      ,   =     P   O        Q   B   P        R   N   Q   Q     S   R       Q     T   R      Q     U   R      P     V   S   T   U   B   >  M   V               =     Y   H   Q     [   Y       Q     \   Y      Q     ]   Y      P     ^   [   \   ]   Z   =     _   M        `   ^   _        b      +   `   a   Z   >  X   b        !      A  !   c      ,   =     d   c        f   d   e   =     g   X        h   f   g   A  !   i      ,   =     j   i        k   h   j   >  X   k        /      =     n   
   =     o   X   P     p   o   o   o        q   n   p   Q     r   q       Q     s   q      Q     t   q      P     u   r   s   t   Z   >  m   u   ύ  8                          (ΘDόΕ^ωϊ                       αΔΈ±qZ  β         Υέ#δ7ϊ¨                                    