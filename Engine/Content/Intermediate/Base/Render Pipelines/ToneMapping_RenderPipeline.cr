�nX/J1��U3�mToneMapping_RenderPipeline                                       �F      #     X                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\ToneMapping_Vertex.glsl  �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 1) uniform PostProcessing
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

/*
*	Applis tone mapping.
*/
vec3 ApplyToneMapping(vec3 fragment)
{
	//Define constants.
	#define GAMMA (1.0f / 2.2f)
	#define A (2.51f)
	#define B (0.03f)
	#define C (2.43f)
	#define D (0.59f)
	#define E (0.14f)

	//Apply color mapping.
	fragment *= 0.6f;

	//Calculate the tone mapped fragment.
	vec3 tone_mapped = (fragment * (A * fragment + B)) / (fragment * (C * fragment + D) + E);

	//Apply gamma correction.
	tone_mapped[0] = pow(tone_mapped[0], GAMMA);
	tone_mapped[1] = pow(tone_mapped[1], GAMMA);
	tone_mapped[2] = pow(tone_mapped[2], GAMMA);

	//Return the tone mapped fragment!
	return tone_mapped;

	//Undefine constants.
	#undef GAMMA
	#undef A
	#undef B
	#undef C
	#undef D
	#undef E
}

#if 0 //If color grading textures should be a thing again. (:
//Constants.
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS (16.0f)
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS - 1.0f)
#define TONE_MAPPING_COLOR_GRADING_WIDTH (256.0f)
#define TONE_MAPPING_COLOR_GRADING_HEIGHT (16.0f)

/*
*   Applies color grading.
*/
vec3 ApplyColorGrading(vec3 fragment)
{
    //Calculate the cell that should be sampled based on the blue channel.
    float cell = fragment.b * TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE;

    //Calculate the current cell and the next cell.
    float current_cell = floor(cell);
    float next_cell = ceil(cell);

    //Calculate the red and green offsets.
    float red_offset = (0.5f / TONE_MAPPING_COLOR_GRADING_WIDTH) + fragment.r / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);
    float green_offset = (0.5f / TONE_MAPPING_COLOR_GRADING_HEIGHT) + fragment.g * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);

    //Sample the current and next color graded values.
    vec3 current_color_graded_value = texture(sampler2D(GLOBAL_TEXTURES[COLOR_GRADING_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(current_cell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + red_offset, green_offset)).rgb;
    vec3 next_colorgraded_value = texture(sampler2D(GLOBAL_TEXTURES[COLOR_GRADING_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(next_cell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + red_offset, green_offset)).rgb;

    //The color graded value is a linearly interpolated value of the current and the next cells!
    return mix(current_color_graded_value, next_colorgraded_value, fract(cell));
}
#endif

layout (set = 1, binding = 2) uniform sampler2D Scene;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   P   General  	 P       FULL_MAIN_RESOLUTION      P      INVERSE_FULL_MAIN_RESOLUTION     	 P      HALF_MAIN_RESOLUTION      P      INVERSE_HALF_MAIN_RESOLUTION      P      FRAME     P      VIEW_DISTANCE     R         S   PostProcessing    S       TINT      S      BLOOM_INTENSITY   S      BRIGHTNESS    S      CONTRAST      S      CHROMATIC_ABERRATION_INTENSITY    S      EXPOSURE     	 S      FILM_GRAIN_INTENSITY      S      HORIZONTAL_BORDER    	 S      MOTION_BLUR_INTENSITY     S   	   SATURATION    U         W   Scene   G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  P       #       H  P      #      H  P      #      H  P      #      H  P      #       H  P      #   $   G  P      G  R   "      G  R   !       H  S       #       H  S      #      H  S      #      H  S      #      H  S      #      H  S      #       H  S      #   $   H  S      #   (   H  S      #   ,   H  S   	   #   0   G  S      G  U   "      G  U   !      G  W   "      G  W   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P               #         Q      P   ;  Q   R        S   ,                                 T      S   ;  T   U         V       G   ;  V   W            �     6               �     ;     	      ;                �      =           �              �              o           �        
      >  	           �      =           �              �              o           �        
      >             �      =        	   �               �     "       !   A  %   &      $   >  &   "        �      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        �      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  U      #     �                 GLSL.std.450                     main    �   �   �                ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\ToneMapping_Fragment.glsl       �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 1) uniform PostProcessing
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

/*
*	Applis tone mapping.
*/
vec3 ApplyToneMapping(vec3 fragment)
{
	//Define constants.
	#define GAMMA (1.0f / 2.2f)
	#define A (2.51f)
	#define B (0.03f)
	#define C (2.43f)
	#define D (0.59f)
	#define E (0.14f)

	//Apply color mapping.
	fragment *= 0.6f;

	//Calculate the tone mapped fragment.
	vec3 tone_mapped = (fragment * (A * fragment + B)) / (fragment * (C * fragment + D) + E);

	//Apply gamma correction.
	tone_mapped[0] = pow(tone_mapped[0], GAMMA);
	tone_mapped[1] = pow(tone_mapped[1], GAMMA);
	tone_mapped[2] = pow(tone_mapped[2], GAMMA);

	//Return the tone mapped fragment!
	return tone_mapped;

	//Undefine constants.
	#undef GAMMA
	#undef A
	#undef B
	#undef C
	#undef D
	#undef E
}

#if 0 //If color grading textures should be a thing again. (:
//Constants.
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS (16.0f)
#define TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS - 1.0f)
#define TONE_MAPPING_COLOR_GRADING_WIDTH (256.0f)
#define TONE_MAPPING_COLOR_GRADING_HEIGHT (16.0f)

/*
*   Applies color grading.
*/
vec3 ApplyColorGrading(vec3 fragment)
{
    //Calculate the cell that should be sampled based on the blue channel.
    float cell = fragment.b * TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE;

    //Calculate the current cell and the next cell.
    float current_cell = floor(cell);
    float next_cell = ceil(cell);

    //Calculate the red and green offsets.
    float red_offset = (0.5f / TONE_MAPPING_COLOR_GRADING_WIDTH) + fragment.r / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);
    float green_offset = (0.5f / TONE_MAPPING_COLOR_GRADING_HEIGHT) + fragment.g * (TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS_MINUS_ONE / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS);

    //Sample the current and next color graded values.
    vec3 current_color_graded_value = texture(sampler2D(GLOBAL_TEXTURES[COLOR_GRADING_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(current_cell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + red_offset, green_offset)).rgb;
    vec3 next_colorgraded_value = texture(sampler2D(GLOBAL_TEXTURES[COLOR_GRADING_TEXTURE_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), vec2(next_cell / TONE_MAPPING_COLOR_GRADING_NUMBER_OF_CELLS + red_offset, green_offset)).rgb;

    //The color graded value is a linearly interpolated value of the current and the next cells!
    return mix(current_color_graded_value, next_colorgraded_value, fract(cell));
}
#endif

layout (set = 1, binding = 2) uniform sampler2D Scene;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 SceneLowDynamicRange1;

void main()
{
    vec3 scene = texture(Scene, InScreenCoordinate).rgb;
    vec3 tone_mapped_scene = ApplyToneMapping(scene);
    {
        vec4 blue_noise_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);
        tone_mapped_scene[0] = clamp(tone_mapped_scene[0] + ((blue_noise_sample[0] - 0.5f) * 0.003921568627451f), 0.0f, 1.0f);
        tone_mapped_scene[1] = clamp(tone_mapped_scene[1] + ((blue_noise_sample[1] - 0.5f) * 0.003921568627451f), 0.0f, 1.0f);
        tone_mapped_scene[2] = clamp(tone_mapped_scene[2] + ((blue_noise_sample[2] - 0.5f) * 0.003921568627451f), 0.0f, 1.0f);
    }
    float luminance = sqrt(Luminance(tone_mapped_scene));
	SceneLowDynamicRange1 = vec4(tone_mapped_scene,luminance);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         Luminance(vf3;       color    
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index        ApplyToneMapping(vf3;        fragment      0   offset_index      2   General  	 2       FULL_MAIN_RESOLUTION      2      INVERSE_FULL_MAIN_RESOLUTION     	 2      HALF_MAIN_RESOLUTION      2      INVERSE_HALF_MAIN_RESOLUTION      2      FRAME     2      VIEW_DISTANCE     4         >   offset_coordinate     U   BLUE_NOISE_TEXTURES   e   tone_mapped   �   scene     �   Scene     �   InScreenCoordinate    �   tone_mapped_scene     �   param     �   blue_noise_sample     �   gl_FragCoord      �   param     �   param     �   luminance     �   param     �   SceneLowDynamicRange1     �   TEXTURES     	 �   GlobalTextureAverageValues   	 �       TEXTURE_AVERAGE_VALUES    �         �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �         �   SKY_TEXTURE   �   PostProcessing    �       TINT      �      BLOOM_INTENSITY   �      BRIGHTNESS    �      CONTRAST      �      CHROMATIC_ABERRATION_INTENSITY    �      EXPOSURE     	 �      FILM_GRAIN_INTENSITY      �      HORIZONTAL_BORDER    	 �      MOTION_BLUR_INTENSITY     �   	   SATURATION    �       H  2       #       H  2      #      H  2      #      H  2      #      H  2      #       H  2      #   $   G  2      G  4   "      G  4   !       G  U   "       G  U   !      G  �   "      G  �   !      G  �          G  �         G  �          G  �   "       G  �   !       G  �         H  �       #       G  �      G  �   "       G  �   !      H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      G  �   "       G  �   !      H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   H  �      #   (   H  �      #   ,   H  �   	   #   0   G  �      G  �   "      G  �   !           !                               	         !  
      	                                                              !              !        	   +                        +         гY>+     "      +     %   Y7?+     (      +     +   �ݓ=  1           2   1   1   1   1            3      2   ;  3   4        5          +  5   6         7         +     <      +     C        	 Q                              R   Q     S   R   C      T       S   ;  T   U          W       R   +     \      B+     b   ��?+     g   ף @+     j   ���<+     o   �@+     r   =
?+     v   )\>+     |   /��>;  W   �          �      1   ;  �   �         �            �         ;  �   �      +     �      ?+     �   ���;+     �       +     �     �?   �         ;  �   �      +     �        �   Q   �      �       �   ;  �   �         �      �     �   �      �      �   ;  �   �       
 �                           +     �        �   �   �     �   �      �      �   ;  �   �       	 �                              �   �      �       �   ;  �   �         �                                    �      �   ;  �   �           �     6               �     ;  	   �      ;  	   �      ;  	   �      ;  �   �      ;     �      ;     �      ;     �      ;  	   �           �      =  R   �   �   =  1   �   �   W     �   �   �   O     �   �   �             >  �   �        �      =     �   �   >  �   �   9     �      �   >  �   �        �      =     �   �   O  1   �   �   �          m     �   �   >  �   �   >  �      9     �      �   �   >  �   �        �      A     �   �      =     �   �   A     �   �      =     �   �   �     �   �   �   �     �   �   �   �     �   �   �        �      +   �   �   �   A     �   �      >  �   �        �      A     �   �   "   =     �   �   A     �   �   "   =     �   �   �     �   �   �   �     �   �   �   �     �   �   �        �      +   �   �   �   A     �   �   "   >  �   �        �      A     �   �   (   =     �   �   A     �   �   (   =     �   �   �     �   �   �   �     �   �   �   �     �   �   �        �      +   �   �   �   A     �   �   (   >  �   �        �      =     �   �   >  �   �   9     �      �        �         �   >  �   �        �      =     �   �   =     �   �   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   �   >  �   �   �  8       �      6            
   7  	      �          �       A              =           �     !          A     #      "   =     $   #   �     &   $   %   �     '   !   &   A     )      (   =     *   )   �     ,   *   +   �     -   '   ,   �  -   8       0  9   6               7        7        �     ;     0      ;     >           2      A  7   8   4   6   =     9   8   =     :      �     ;   9   :   �     =   ;   <   >  0   =        6      A     ?         =     @   ?   A  7   A   4   6   =     B   A   �     D   B   C   �     E   D   <   �     F   @   E   A     G   >      >  G   F        7      A     H      "   =     I   H   A  7   J   4   6   =     K   J   �     L   K   C   �     M   L   C   �     N   M   <   �     O   I   N   A     P   >   "   >  P   O        9      =     V   0   A  W   X   U   V   =  R   Y   X   =     Z   >   p  1   [   Z   P  1   ]   \   \   �  1   ^   [   ]   W     _   Y   ^   �  _   8       ?  $   6               7  	      �     ;  	   e           J      =     c      �     d   c   b   >     d        M      =     f      =     h      �     i   h   g   P     k   j   j   j   �     l   i   k   �     m   f   l   =     n      =     p      �     q   p   o   P     s   r   r   r   �     t   q   s   �     u   n   t   P     w   v   v   v   �     x   u   w   �     y   m   x   >  e   y        P      A     z   e      =     {   z        }         {   |   A     ~   e      >  ~   }        Q      A        e   "   =     �           �         �   |   A     �   e   "   >  �   �        R      A     �   e   (   =     �   �        �         �   |   A     �   e   (   >  �   �        U      =     �   e   �  �   8                          (�D��^�����'p���                       N1�8��; Y         <�A�cz                                   \�5s����