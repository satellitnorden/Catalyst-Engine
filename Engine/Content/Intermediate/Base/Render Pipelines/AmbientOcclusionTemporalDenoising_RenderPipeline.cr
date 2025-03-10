�nX/J1��ڻ��{AmbientOcclusionTemporalDenoising_RenderPipeline                 �/      #     X                 GLSL.std.450                      main          0         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\AmbientOcclusionTemporalDenoising_Vertex.glsl    �   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 2) uniform sampler2D PreviousTemporalBuffer;
layout (set = 1, binding = 3) uniform sampler2D InputAmbientOcclusion;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures4Half;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES      =   Material      =       _Properties   =      _AlbedoThickness     	 =      _NormalMapDisplacement    =      _MaterialProperties   =      _Opacity      =      _EmissiveMultiplier   =      _Padding1     =      _Padding2     @   GlobalMaterials   @       MATERIALS     B         G   BLUE_NOISE_TEXTURES   K   SKY_TEXTURE   N   Camera   	 N       WORLD_TO_CLIP_MATRIX     	 N      WORLD_TO_CAMERA_MATRIX    N      PREVIOUS_WORLD_TO_CLIP_MATRIX     N      INVERSE_WORLD_TO_CAMERA_MATRIX    N      INVERSE_CAMERA_TO_CLIP_MATRIX    	 N      CAMERA_WORLD_POSITION    	 N      CAMERA_FORWARD_VECTOR    	 N      CURRENT_FRAME_JITTER      N      NEAR_PLANE    N   	   FAR_PLANE     P         Q   General  	 Q       FULL_MAIN_RESOLUTION      Q      INVERSE_FULL_MAIN_RESOLUTION     	 Q      HALF_MAIN_RESOLUTION      Q      INVERSE_HALF_MAIN_RESOLUTION      Q      FRAME     Q      VIEW_DISTANCE     S         U   PreviousTemporalBuffer    V   InputAmbientOcclusion     W   SceneFeatures4Half  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       H  =       #       H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      G  ?          H  @       #       G  @      G  B   "       G  B   !      G  G   "       G  G   !      G  K   "       G  K   !      H  N          H  N       #       H  N             H  N         H  N      #   @   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #      H  N            H  N      #   @  H  N      #   P  H  N      #   `  H  N      #   h  H  N   	   #   l  G  N      G  P   "      G  P   !       H  Q       #       H  Q      #      H  Q      #      H  Q      #      H  Q      #       H  Q      #   $   G  Q      G  S   "      G  S   !      G  U   "      G  U   !      G  V   "      G  V   !      G  W   "      G  W   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <        
 =   #   #   #   #   #      #   #   +  #   >        ?   =   >     @   ?      A      @   ;  A   B        C   8   +  #   D         E   C   D      F       E   ;  F   G        	 H                              I   H      J       I   ;  J   K         L   ,        M           N   L   L   L   L   L   M   M               O      N   ;  O   P        Q               #         R      Q   ;  R   S         T       C   ;  T   U       ;  T   V       ;  T   W            �      6               �     ;     	      ;                �       =           �              �              o           �        
      >  	           �       =           �              �              o           �        
      >             �       =        	   �               �     "       !   A  %   &      $   >  &   "        �       =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        �       =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  �?      #     �                 GLSL.std.450                     main    E   �   �                 ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\AmbientOcclusionTemporalDenoising_Fragment.glsl  	   �     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 2) uniform sampler2D PreviousTemporalBuffer;
layout (set = 1, binding = 3) uniform sampler2D InputAmbientOcclusion;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures4Half;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 CurrentTemporalBuffer;
layout (location = 1) out vec4 OutputAmbientOcclusion;

void main()
{
    /*
    *   Calculate the minimum/maximum ambient occlusion values in the neighborhood of the current frame.
    *   Also retrieve the current ambient occlusion here.
    */
    float current_ambient_occlusion = 0.0f;
	float minimum_ambient_occlusion = 1.0f;
	float maximum_ambient_occlusion = 0.0f;
	for (int Y = -1; Y <= 1; ++Y)
	{
		for (int X = -1; X <= 1; ++X)
		{
			vec2 sample_coordinate = InScreenCoordinate + vec2(float(X), float(Y)) * INVERSE_HALF_MAIN_RESOLUTION;
			float neighborhood_sample = texture(InputAmbientOcclusion, sample_coordinate).x;
            current_ambient_occlusion += neighborhood_sample * float(X == 0 && Y == 0);
			minimum_ambient_occlusion = min(minimum_ambient_occlusion, neighborhood_sample);
			maximum_ambient_occlusion = max(maximum_ambient_occlusion, neighborhood_sample);
		}
	}
    vec2 previous_screen_coordinate = InScreenCoordinate - texture(SceneFeatures4Half, InScreenCoordinate).xy;
    float previous_ambient_occlusion = texture(PreviousTemporalBuffer, previous_screen_coordinate).x;
    previous_ambient_occlusion = clamp(previous_ambient_occlusion, minimum_ambient_occlusion, maximum_ambient_occlusion);
    /*
	*	Calculate the weight between the current frame and the history depending on certain criteria.
	*
	*	1. Is the previous screen coordinate outside the screen? If so, it's not valid.
	*/
	float previous_sample_weight = 1.0f;
	previous_sample_weight *= float(ValidScreenCoordinate(previous_screen_coordinate));
    previous_sample_weight *= 0.9f;
    float blended_ambient_occlusion = mix(current_ambient_occlusion, previous_ambient_occlusion, previous_sample_weight);
	CurrentTemporalBuffer = vec4(blended_ambient_occlusion);
	OutputAmbientOcclusion = vec4(blended_ambient_occlusion);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     	    ValidScreenCoordinate(vf2;       X    	 ,   current_ambient_occlusion    	 -   minimum_ambient_occlusion    	 .   maximum_ambient_occlusion     1   Y     ;   X     C   sample_coordinate     E   InScreenCoordinate    L   General  	 L       FULL_MAIN_RESOLUTION      L      INVERSE_FULL_MAIN_RESOLUTION     	 L      HALF_MAIN_RESOLUTION      L      INVERSE_HALF_MAIN_RESOLUTION      L      FRAME     L      VIEW_DISTANCE     N         U   neighborhood_sample   Y   InputAmbientOcclusion    	 t   previous_screen_coordinate    v   SceneFeatures4Half   	 |   previous_ambient_occlusion    }   PreviousTemporalBuffer    �   previous_sample_weight    �   param    	 �   blended_ambient_occlusion     �   CurrentTemporalBuffer     �   OutputAmbientOcclusion    �   TEXTURES      �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �         �   BLUE_NOISE_TEXTURES   �   SKY_TEXTURE   �   Camera   	 �       WORLD_TO_CLIP_MATRIX     	 �      WORLD_TO_CAMERA_MATRIX    �      PREVIOUS_WORLD_TO_CLIP_MATRIX     �      INVERSE_WORLD_TO_CAMERA_MATRIX    �      INVERSE_CAMERA_TO_CLIP_MATRIX    	 �      CAMERA_WORLD_POSITION    	 �      CAMERA_FORWARD_VECTOR    	 �      CURRENT_FRAME_JITTER      �      NEAR_PLANE    �   	   FAR_PLANE     �       G  E          H  L       #       H  L      #      H  L      #      H  L      #      H  L      #       H  L      #   $   G  L      G  N   "      G  N   !      G  Y   "      G  Y   !      G  v   "      G  v   !      G  }   "      G  }   !      G  �          G  �         G  �   "       G  �   !       H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      G  �   "       G  �   !      G  �   "       G  �   !      H  �          H  �       #       H  �             H  �         H  �      #   @   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #      H  �            H  �      #   @  H  �      #   P  H  �      #   `  H  �      #   h  H  �   	   #   l  G  �      G  �   "      G  �   !            !                               	           
   !     
   	                +                        +            +          �?+             /             0      /   +  /   2   ����+  /   9         D         ;  D   E        L                        M      L   ;  M   N      +  /   O         P          	 V                              W   V      X       W   ;  X   Y         \         +  /   a       ;  X   v       ;  X   }       +     �   fff?   �      \   ;  �   �      ;  �   �      +     �        �   V   �      �       �   ;  �   �        
 �                           +     �        �   �   �     �   �      �      �   ;  �   �      +     �         �   W   �      �       �   ;  �   �        	 �                              �   �      �       �   ;  �   �         �   \        �           �   �   �   �   �   �   �   �               �      �   ;  �   �           �      6               �     ;     ,      ;     -      ;     .      ;  0   1      ;  0   ;      ;  	   C      ;     U      ;  	   t      ;     |      ;     �      ;  	   �      ;     �           �       >  ,           �       >  -           �       >  .           �       >  1   2   �  3   �  3        �       �  5   6       �  7   �  7        �       =  /   8   1   �  
   :   8   9   �  :   4   5   �  4        �       >  ;   2   �  <   �  <        �       �  >   ?       �  @   �  @        �       =  /   A   ;   �  
   B   A   9   �  B   =   >   �  =        �       =     F   E   =  /   G   ;   o     H   G   =  /   I   1   o     J   I   P     K   H   J   A  P   Q   N   O   =     R   Q   �     S   K   R   �     T   F   S   >  C   T        �       =  W   Z   Y   =     [   C   W  \   ]   Z   [   Q     ^   ]       >  U   ^        �       =     _   U   =  /   `   ;   �  
   b   `   a   =  /   c   1   �  
   d   c   a   �  
   e   b   d   �     f   e         �     g   _   f   =     h   ,   �     i   h   g   >  ,   i        �       =     j   -   =     k   U        l      %   j   k   >  -   l        �       =     m   .   =     n   U        o      (   m   n   >  .   o   �  ?   �  ?        �       =  /   p   ;   �  /   q   p   9   >  ;   q   �  <   �  >   �  6   �  6        �       =  /   r   1   �  /   s   r   9   >  1   s   �  3   �  5        �       =     u   E   =  W   w   v   =     x   E   W  \   y   w   x   O     z   y   y          �     {   u   z   >  t   {        �       =  W   ~   }   =        t   W  \   �   ~      Q     �   �       >  |   �        �       =     �   |   =     �   -   =     �   .        �      +   �   �   �   >  |   �              >  �                 =     �   t   >  �   �   9  
   �      �   �     �   �         =     �   �   �     �   �   �   >  �   �              =     �   �   �     �   �   �   >  �   �              =     �   ,   =     �   |   =     �   �        �      .   �   �   �   >  �   �              =     �   �   P  \   �   �   �   �   �   >  �   �        	      =     �   �   P  \   �   �   �   �   �   >  �   �   �  8       �   "   6  
             7  	      �          �       A              =           �  
            �         �           �          �       A              =           �  
            �     �     �  
                  �         �           �          �       A               =     !       �  
   "   !      �     �     �  
   #         "      �  %       �  #   $   %   �  $        �       A     &         =     '   &   �  
   (   '      �  %   �  %   �  
   )   #      (   $        �       �  )   8                          ��KT��D	(�D��^��                       2���ڥ�8  rren���g_3� clus�s�H�W� te         tHkB��ɶ�mi{�y�                                   \�5s����