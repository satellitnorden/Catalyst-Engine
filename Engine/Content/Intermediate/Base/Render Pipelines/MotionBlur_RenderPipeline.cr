�nX/J1��H�I��MotionBlur_RenderPipeline                                        �B      #     ^                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\MotionBlur_Vertex.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform PostProcessing
{
	layout (offset = 0) vec4 TINT;
	layout (offset = 16) float BLOOM_INTENSITY;
	layout (offset = 20) float BRIGHTNESS;
	layout (offset = 24) float CONTRAST;
	layout (offset = 28) float CHROMATIC_ABERRATION_INTENSITY;
	layout (offset = 32) float EXPOSURE;
	layout (offset = 36) float FILM_GRAIN_INTENSITY;
	layout (offset = 40) float HORIZONTAL_BORDER;
	layout (offset = 44) float MOTION_BLUR_INTENSITY;
	layout (offset = 48) float SATURATION;
};

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

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures4;
layout (set = 1, binding = 4) uniform sampler2D InputRenderTarget;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   R   Camera   	 R       WORLD_TO_CLIP_MATRIX     	 R      WORLD_TO_CAMERA_MATRIX    R      PREVIOUS_WORLD_TO_CLIP_MATRIX     R      INVERSE_WORLD_TO_CAMERA_MATRIX    R      INVERSE_CAMERA_TO_CLIP_MATRIX    	 R      CAMERA_WORLD_POSITION    	 R      CAMERA_FORWARD_VECTOR    	 R      CURRENT_FRAME_JITTER      R      NEAR_PLANE    R   	   FAR_PLANE     T         U   General  	 U       FULL_MAIN_RESOLUTION      U      INVERSE_FULL_MAIN_RESOLUTION     	 U      HALF_MAIN_RESOLUTION      U      INVERSE_HALF_MAIN_RESOLUTION      U      FRAME     U      VIEW_DISTANCE     W         X   PostProcessing    X       TINT      X      BLOOM_INTENSITY   X      BRIGHTNESS    X      CONTRAST      X      CHROMATIC_ABERRATION_INTENSITY    X      EXPOSURE     	 X      FILM_GRAIN_INTENSITY      X      HORIZONTAL_BORDER    	 X      MOTION_BLUR_INTENSITY     X   	   SATURATION    Z         \   SceneFeatures4    ]   InputRenderTarget   G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  R          H  R       #       H  R             H  R         H  R      #   @   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #      H  R            H  R      #   @  H  R      #   P  H  R      #   `  H  R      #   h  H  R   	   #   l  G  R      G  T   "      G  T   !       H  U       #       H  U      #      H  U      #      H  U      #      H  U      #       H  U      #   $   G  U      G  W   "      G  W   !      H  X       #       H  X      #      H  X      #      H  X      #      H  X      #      H  X      #       H  X      #   $   H  X      #   (   H  X      #   ,   H  X   	   #   0   G  X      G  Z   "      G  Z   !      G  \   "      G  \   !      G  ]   "      G  ]   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P   ,        Q           R   P   P   P   P   P   Q   Q               S      R   ;  S   T        U               #         V      U   ;  V   W        X   ,                                 Y      X   ;  Y   Z         [       G   ;  [   \       ;  [   ]            O     6               �     ;     	      ;                Q      =           �              �              o           �        
      >  	           R      =           �              �              o           �        
      >             S      =        	   �               �     "       !   A  %   &      $   >  &   "        T      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        U      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  8_      #     7                GLSL.std.450                     main    �   �                  ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\MotionBlur_Fragment.glsl     9   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform PostProcessing
{
	layout (offset = 0) vec4 TINT;
	layout (offset = 16) float BLOOM_INTENSITY;
	layout (offset = 20) float BRIGHTNESS;
	layout (offset = 24) float CONTRAST;
	layout (offset = 28) float CHROMATIC_ABERRATION_INTENSITY;
	layout (offset = 32) float EXPOSURE;
	layout (offset = 36) float FILM_GRAIN_INTENSITY;
	layout (offset = 40) float HORIZONTAL_BORDER;
	layout (offset = 44) float MOTION_BLUR_INTENSITY;
	layout (offset = 48) float SATURATION;
};

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

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures4;
layout (set = 1, binding = 4) uniform sampler2D InputRenderTarget;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 OutputRenderTarget;

void main()
{
    #define NUMBER_OF_SAMPLES (8)
    vec2 velocity = vec2(0.0f, 0.0f);
    {
        float highest_velocity_magnitude = 0.0f;
        for (int Y = -1; Y <= 1; ++Y)
        {
            for (int X = -1; X <= 1; ++X)
            {
                vec2 sample_velocity = texture(SceneFeatures4, InScreenCoordinate + vec2(float(X), float(Y)) * INVERSE_FULL_MAIN_RESOLUTION).xy;
                float velocity_magnitude = dot(sample_velocity, sample_velocity);
                if (highest_velocity_magnitude < velocity_magnitude)
                {
                    velocity = sample_velocity;
                    highest_velocity_magnitude = velocity_magnitude;
                }
            }
        }
    }
    vec2 blur_direction = velocity * -1.0f * MOTION_BLUR_INTENSITY;
    float offsets[NUMBER_OF_SAMPLES];
    for (uint i = 0; i < NUMBER_OF_SAMPLES; i += 4)
    {
        vec4 blue_noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), i / 4);
        offsets[i + 0] = blue_noise_texture_sample.x;
        offsets[i + 1] = blue_noise_texture_sample.y;
        offsets[i + 2] = blue_noise_texture_sample.z;
        offsets[i + 3] = blue_noise_texture_sample.w;
    }
    vec3 center_scene = texture(InputRenderTarget, InScreenCoordinate).rgb;
    vec3 blurred_scene = vec3(0.0f);
    float weight = 0.0f;
    if (MOTION_BLUR_INTENSITY > 0.0f)
    {
        for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
        {
            vec2 sample_coordinate = InScreenCoordinate + blur_direction * offsets[i];
            vec3 _sample = texture(InputRenderTarget, sample_coordinate).rgb;
            float sample_weight = float(ValidScreenCoordinate(sample_coordinate));
            blurred_scene += _sample * sample_weight;
            weight += sample_weight;
        }
    }
    blurred_scene = weight > 0.0f ? blurred_scene / weight : center_scene;
	OutputRenderTarget = vec4(blurred_scene,1.0f);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     	    ValidScreenCoordinate(vf2;       X    
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index     5   offset_index      6   General  	 6       FULL_MAIN_RESOLUTION      6      INVERSE_FULL_MAIN_RESOLUTION     	 6      HALF_MAIN_RESOLUTION      6      INVERSE_HALF_MAIN_RESOLUTION      6      FRAME     6      VIEW_DISTANCE     8         B   offset_coordinate     Y   BLUE_NOISE_TEXTURES   f   velocity     	 h   highest_velocity_magnitude    j   Y     t   X     |   sample_velocity   }   SceneFeatures4    �   InScreenCoordinate    �   velocity_magnitude    �   blur_direction    �   PostProcessing    �       TINT      �      BLOOM_INTENSITY   �      BRIGHTNESS    �      CONTRAST      �      CHROMATIC_ABERRATION_INTENSITY    �      EXPOSURE     	 �      FILM_GRAIN_INTENSITY      �      HORIZONTAL_BORDER    	 �      MOTION_BLUR_INTENSITY     �   	   SATURATION    �         �   i    	 �   blue_noise_texture_sample     �   gl_FragCoord      �   param     �   param     �   offsets   �   center_scene      �   InputRenderTarget     �   blurred_scene     �   weight    �   i     �   sample_coordinate     �   _sample   �   sample_weight        param       OutputRenderTarget    $  TEXTURES     	 &  GlobalTextureAverageValues   	 &      TEXTURE_AVERAGE_VALUES    (        )  Material      )      _Properties   )     _AlbedoThickness     	 )     _NormalMapDisplacement    )     _MaterialProperties   )     _Opacity      )     _EmissiveMultiplier   )     _Padding1     )     _Padding2     ,  GlobalMaterials   ,      MATERIALS     .        2  SKY_TEXTURE   4  Camera   	 4      WORLD_TO_CLIP_MATRIX     	 4     WORLD_TO_CAMERA_MATRIX    4     PREVIOUS_WORLD_TO_CLIP_MATRIX     4     INVERSE_WORLD_TO_CAMERA_MATRIX    4     INVERSE_CAMERA_TO_CLIP_MATRIX    	 4     CAMERA_WORLD_POSITION    	 4     CAMERA_FORWARD_VECTOR    	 4     CURRENT_FRAME_JITTER      4     NEAR_PLANE    4  	   FAR_PLANE     6      H  6       #       H  6      #      H  6      #      H  6      #      H  6      #       H  6      #   $   G  6      G  8   "      G  8   !      G  Y   "       G  Y   !      G  }   "      G  }   !      G  �          H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   H  �      #   (   H  �      #   ,   H  �   	   #   0   G  �      G  �   "      G  �   !      G  �         G  �   "      G  �   !      G           G  $  "       G  $  !       G  %        H  &      #       G  &     G  (  "       G  (  !      H  )      #       H  )     #      H  )     #      H  )     #      H  )     #      H  )     #      H  )     #      H  )     #      G  +         H  ,      #       G  ,     G  .  "       G  .  !      G  2  "       G  2  !      H  4         H  4      #       H  4            H  4        H  4     #   @   H  4           H  4        H  4     #   �   H  4           H  4        H  4     #   �   H  4           H  4        H  4     #      H  4           H  4     #   @  H  4     #   P  H  4     #   `  H  4     #   h  H  4  	   #   l  G  4     G  6  "      G  6  !            !                               	           
   !     
   	                                                              !              +                        +            +     #     �?+     (        6                        7      6   ;  7   8        9          +  9   :         ;         +     @      +     G        	 U                              V   U     W   V   G      X       W   ;  X   Y          [       V   +     `      B,     g            i      9   +  9   k   ����+  9   r      ;  [   }                   ;     �         �         +     �     ��  �                                    �      �   ;  �   �      +  9   �         �         +     �         �            �         ;  �   �      +     �        �      �      �      �   +     �      +     �        �            �      �   ;  [   �       ,  �   �                       ;         +     !       "  U   !     #      "  ;  #  $        %     !    &  %     '     &  ;  '  (      
 )                          +     *       +  )  *    ,  +     -     ,  ;  -  .      	 /                             0  /     1      0  ;  1  2        3          4  3  3  3  3  3  �   �               5     4  ;  5  6          Q     6               �     ;  	   f      ;     h      ;  i   j      ;  i   t      ;  	   |      ;     �      ;  	   �      ;     �      ;  �   �      ;     �      ;     �      ;  �   �      ;  �   �      ;  �   �      ;     �      ;     �      ;  	   �      ;  �   �      ;     �      ;  	         ;  �             T      >  f   g        V      >  h           W      >  j   k   �  l   �  l        W      �  n   o       �  p   �  p        W      =  9   q   j   �  
   s   q   r   �  s   m   n   �  m        Y      >  t   k   �  u   �  u        Y      �  w   x       �  y   �  y        Y      =  9   z   t   �  
   {   z   r   �  {   v   w   �  v        [      =  V   ~   }   =     �   �   =  9   �   t   o     �   �   =  9   �   j   o     �   �   P     �   �   �   A  �   �   8   r   =     �   �   �     �   �   �   �     �   �   �   W     �   ~   �   O     �   �   �          >  |   �        \      =     �   |   =     �   |   �     �   �   �   >  �   �        ]      =     �   h   =     �   �   �  
   �   �   �   �  �       �  �   �   �   �  �        _      =     �   |   >  f   �        `      =     �   �   >  h   �   �  �   �  �   �  x   �  x        Y      =  9   �   t   �  9   �   �   r   >  t   �   �  u   �  w   �  o   �  o        W      =  9   �   j   �  9   �   �   r   >  j   �   �  l   �  n        e      =     �   f   �     �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   >  �   �        g      >  �      �  �   �  �        g      �  �   �       �  �   �  �        g      =     �   �   �  
   �   �   �   �  �   �   �   �  �        i      =     �   �   O     �   �   �          m     �   �   =     �   �   �     �   �   �   >  �   �   >  �   �   9     �      �   �   >  �   �        j      =     �   �   �     �   �      A     �   �      =     �   �   A     �   �   �   >  �   �        k      =     �   �   �     �   �   (   A     �   �   (   =     �   �   A     �   �   �   >  �   �        l      =     �   �   �     �   �   �   A     �   �   �   =     �   �   A     �   �   �   >  �   �        m      =     �   �   �     �   �   �   A     �   �   �   =     �   �   A     �   �   �   >  �   �   �  �   �  �        g      =     �   �   �     �   �   �   >  �   �   �  �   �  �        o      =  V   �   �   =     �   �   W     �   �   �   O  �   �   �   �             >  �   �        p      >  �   �        q      >  �           r      A  �   �   �   �   =     �   �   �  
   �   �      �  �       �  �   �   �   �  �        t      >  �      �  �   �  �        t      �  �   �       �  �   �  �        t      =     �   �   �  
   �   �   �   �  �   �   �   �  �        v      =     �   �   =     �   �   =     �   �   A     �   �   �   =     �   �   �     �   �   �   �     �   �   �   >  �   �        w      =  V   �   �   =     �   �   W     �   �   �   O  �   �   �   �             >  �   �        x      =       �   >       9  
           �         #      >  �          y      =  �     �   =       �   �  �         =  �     �   �  �         >  �          z      =     	  �   =     
  �   �       
  	  >  �     �  �   �  �        t      =       �   �         r   >  �     �  �   �  �   �  �   �  �        }      =       �   �  
          �        �        �         }      =  �     �   =       �   P  �           �  �         >      �    �         }      =  �     �   >      �    �    =  �            }      >  �          ~      =  �     �   Q             Q            Q            P              #   >       �  8         "   6  
             7  	      �                A              =           �  
            �          �            �                A     !         =     "   !   �  
   $   "   #   �      �      �  
   %         $      �  '       �  %   &   '   �  &              A     )      (   =     *   )   �  
   +   *      �  '   �  '   �  
   ,   %       +   &   �  .       �  ,   -   .   �  -              A     /      (   =     0   /   �  
   1   0   #   �  .   �  .   �  
   2   ,   '   1   -              �  2   8       >  9   6               7        7        �     ;     5      ;     B           @      A  ;   <   8   :   =     =   <   =     >      �     ?   =   >   �     A   ?   @   >  5   A        D      A     C         =     D   C   A  ;   E   8   :   =     F   E   �     H   F   G   �     I   H   @   �     J   D   I   A     K   B      >  K   J        E      A     L      (   =     M   L   A  ;   N   8   :   =     O   N   �     P   O   G   �     Q   P   G   �     R   Q   @   �     S   M   R   A     T   B   (   >  T   S        G      =     Z   5   A  [   \   Y   Z   =  V   ]   \   =     ^   B   p     _   ^   P     a   `   `   �     b   _   a   W     c   ]   b   �  c   8                          ��KT��D	(�D��^�����'p���                        �z��.v<     �ĸ�q�Z  vel        ��#�7��                                   \�5s����