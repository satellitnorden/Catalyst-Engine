�nX/J1���@y���VolumetricLightingApplication_RenderPipeline                     �Q      #     \                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\VolumetricLightingApplication_Vertex.glsl    �   �     // OpModuleProcessed entry-point main
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

/*
*   Combines two hashes.
*/
uint CombineHash(uint hash_1, uint hash_2)
{
    return 3141592653 * hash_1 + hash_2;
}

/*
*   Hash function taking a uint.
*/
uint Hash(uint seed)
{
    seed ^= seed >> 17;
    seed *= 0xed5ad4bbU;
    seed ^= seed >> 11;
    seed *= 0xac4c1b51U;
    seed ^= seed >> 15;
    seed *= 0x31848babU;
    seed ^= seed >> 14;
    return seed;
}

/*
*   Hash function taking a uvec2.
*/
uint Hash2(uvec2 seed)
{
    return Hash(seed.x) ^ Hash(seed.y);
}

/*
*   Hash function taking a uvec3.
*/
uint Hash3(uvec3 seed)
{
    //return Hash( Hash( Hash( Hash(seed.x) ^ Hash(seed.y) ^ Hash(seed.z) ) ) );
    //return Hash( Hash( Hash(seed.x) + Hash(seed.y) ) + Hash(seed.z) );
    return Hash( CombineHash(CombineHash(Hash(seed.x), Hash(seed.y)), Hash(seed.z)) );
}

/*
*   Given a seed, returns a random number.
*/
float RandomFloat(inout uint seed)
{
    return Hash(seed) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate and a seed, returns a random number.
*/
float RandomFloat(uvec2 coordinate, uint seed)
{
    return float(Hash3(uvec3(coordinate.xy, seed))) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate, returns a random number.
*/
float RandomFloat(vec2 coordinate)
{
    return fract(sin(dot(coordinate, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

/*
*	Returns the interleaved gradient noise for the given coordinate at the given frame.
*/
float InterleavedGradientNoise(uvec2 coordinate, uint frame)
{
	frame = frame % 64;

	float x = float(coordinate.x) + 5.588238f * float(frame);
	float y = float(coordinate.y) + 5.588238f * float(frame);

	return mod(52.9829189f * mod(0.06711056f * x + 0.00583715f * y, 1.0f), 1.0f);
}

/*
*	Returns the extinction at the given position.
*/
float GetExtinctionAtPosition(vec3 position)
{
	#define BASE_EXTINCTION (0.00125f)

	return mix(BASE_EXTINCTION, BASE_EXTINCTION * 0.125f, Square(clamp(position.y / 512.0f, 0.0f, 1.0f)));

	#undef BASE_EXTINCTION
}

/*
*	Calculates the attenuation in the given direction.
*/
float CalculateAttenuationInDirection(vec3 position, vec3 direction, float distance)
{
	#define NUMBER_OF_SAMPLES (4)

	float attenuation = 1.0f;
	float step_size = distance / float(NUMBER_OF_SAMPLES);

	for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
	{
		vec3 sample_position = position + direction * float(i) * step_size;
		attenuation *= exp(-GetExtinctionAtPosition(sample_position) * step_size);
	}

	return attenuation;
	
	#undef NUMBER_OF_SAMPLES
}

/*
*	The Henyey-Greenstein phase function.
*/
float HenyeyGreensteinPhaseFunction(vec3 outgoing_direction, vec3 incoming_direction)
{
	float G = 0.25f;
	float dot_product = dot(outgoing_direction, -incoming_direction);

	return (1.0f - G * G) / (4.0f * PI * pow(1.0 + G * G - 2.0f * G * dot_product, 3.0f / 2.0f));
}

/*
*	Calculates the scattering with the given properties.
*/
vec3 CalculateScattering(vec3 ray_origin, vec3 ray_direction)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

layout (set = 1, binding = 2) uniform sampler2D VolumetricLighting;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures2Half;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   R   Camera   	 R       WORLD_TO_CLIP_MATRIX     	 R      WORLD_TO_CAMERA_MATRIX    R      PREVIOUS_WORLD_TO_CLIP_MATRIX     R      INVERSE_WORLD_TO_CAMERA_MATRIX    R      INVERSE_CAMERA_TO_CLIP_MATRIX    	 R      CAMERA_WORLD_POSITION    	 R      CAMERA_FORWARD_VECTOR    	 R      CURRENT_FRAME_JITTER      R      NEAR_PLANE    R   	   FAR_PLANE     T         U   General  	 U       FULL_MAIN_RESOLUTION      U      INVERSE_FULL_MAIN_RESOLUTION     	 U      HALF_MAIN_RESOLUTION      U      INVERSE_HALF_MAIN_RESOLUTION      U      FRAME     U      VIEW_DISTANCE     W         Y   VolumetricLighting    Z   SceneFeatures2    [   SceneFeatures2Half  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  R          H  R       #       H  R             H  R         H  R      #   @   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #      H  R            H  R      #   @  H  R      #   P  H  R      #   `  H  R      #   h  H  R   	   #   l  G  R      G  T   "      G  T   !       H  U       #       H  U      #      H  U      #      H  U      #      H  U      #       H  U      #   $   G  U      G  W   "      G  W   !      G  Y   "      G  Y   !      G  Z   "      G  Z   !      G  [   "      G  [   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P   ,        Q           R   P   P   P   P   P   Q   Q               S      R   ;  S   T        U               #         V      U   ;  V   W         X       G   ;  X   Y       ;  X   Z       ;  X   [            �     6               �     ;     	      ;                      =           �              �              o           �        
      >  	                 =           �              �              o           �        
      >                   =        	   �               �     "       !   A  %   &      $   >  &   "              =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )              =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  ,�      #     �                GLSL.std.450                     main    �   n  �               ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\VolumetricLightingApplication_Fragment.glsl  �   �     // OpModuleProcessed entry-point main
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

/*
*   Combines two hashes.
*/
uint CombineHash(uint hash_1, uint hash_2)
{
    return 3141592653 * hash_1 + hash_2;
}

/*
*   Hash function taking a uint.
*/
uint Hash(uint seed)
{
    seed ^= seed >> 17;
    seed *= 0xed5ad4bbU;
    seed ^= seed >> 11;
    seed *= 0xac4c1b51U;
    seed ^= seed >> 15;
    seed *= 0x31848babU;
    seed ^= seed >> 14;
    return seed;
}

/*
*   Hash function taking a uvec2.
*/
uint Hash2(uvec2 seed)
{
    return Hash(seed.x) ^ Hash(seed.y);
}

/*
*   Hash function taking a uvec3.
*/
uint Hash3(uvec3 seed)
{
    //return Hash( Hash( Hash( Hash(seed.x) ^ Hash(seed.y) ^ Hash(seed.z) ) ) );
    //return Hash( Hash( Hash(seed.x) + Hash(seed.y) ) + Hash(seed.z) );
    return Hash( CombineHash(CombineHash(Hash(seed.x), Hash(seed.y)), Hash(seed.z)) );
}

/*
*   Given a seed, returns a random number.
*/
float RandomFloat(inout uint seed)
{
    return Hash(seed) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate and a seed, returns a random number.
*/
float RandomFloat(uvec2 coordinate, uint seed)
{
    return float(Hash3(uvec3(coordinate.xy, seed))) * UINT32_MAXIMUM_RECIPROCAL;
}

/*
*   Given a coordinate, returns a random number.
*/
float RandomFloat(vec2 coordinate)
{
    return fract(sin(dot(coordinate, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

/*
*	Returns the interleaved gradient noise for the given coordinate at the given frame.
*/
float InterleavedGradientNoise(uvec2 coordinate, uint frame)
{
	frame = frame % 64;

	float x = float(coordinate.x) + 5.588238f * float(frame);
	float y = float(coordinate.y) + 5.588238f * float(frame);

	return mod(52.9829189f * mod(0.06711056f * x + 0.00583715f * y, 1.0f), 1.0f);
}

/*
*	Returns the extinction at the given position.
*/
float GetExtinctionAtPosition(vec3 position)
{
	#define BASE_EXTINCTION (0.00125f)

	return mix(BASE_EXTINCTION, BASE_EXTINCTION * 0.125f, Square(clamp(position.y / 512.0f, 0.0f, 1.0f)));

	#undef BASE_EXTINCTION
}

/*
*	Calculates the attenuation in the given direction.
*/
float CalculateAttenuationInDirection(vec3 position, vec3 direction, float distance)
{
	#define NUMBER_OF_SAMPLES (4)

	float attenuation = 1.0f;
	float step_size = distance / float(NUMBER_OF_SAMPLES);

	for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
	{
		vec3 sample_position = position + direction * float(i) * step_size;
		attenuation *= exp(-GetExtinctionAtPosition(sample_position) * step_size);
	}

	return attenuation;
	
	#undef NUMBER_OF_SAMPLES
}

/*
*	The Henyey-Greenstein phase function.
*/
float HenyeyGreensteinPhaseFunction(vec3 outgoing_direction, vec3 incoming_direction)
{
	float G = 0.25f;
	float dot_product = dot(outgoing_direction, -incoming_direction);

	return (1.0f - G * G) / (4.0f * PI * pow(1.0 + G * G - 2.0f * G * dot_product, 3.0f / 2.0f));
}

/*
*	Calculates the scattering with the given properties.
*/
vec3 CalculateScattering(vec3 ray_origin, vec3 ray_direction)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

layout (set = 1, binding = 2) uniform sampler2D VolumetricLighting;
layout (set = 1, binding = 3) uniform sampler2D SceneFeatures2;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures2Half;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 Scene;

void main()
{
    vec4 scene_features_2 = texture(SceneFeatures2, InScreenCoordinate);
    float depth = scene_features_2.w;
    float linearized_depth = LinearizeDepth(depth);
    vec3 volumetric_lighting;
    {
        vec2 sample_coordinate_1 = InScreenCoordinate;
        vec2 sample_coordinate_2 = InScreenCoordinate + vec2(0.0f, 1.0f) * INVERSE_HALF_MAIN_RESOLUTION;
        vec2 sample_coordinate_3 = InScreenCoordinate + vec2(1.0f, 0.0f) * INVERSE_HALF_MAIN_RESOLUTION;
        vec2 sample_coordinate_4 = InScreenCoordinate + vec2(1.0f, 1.0f) * INVERSE_HALF_MAIN_RESOLUTION;
        vec3 volumetric_lighting_1 = texture(VolumetricLighting, sample_coordinate_1).rgb;
        vec3 volumetric_lighting_2 = texture(VolumetricLighting, sample_coordinate_2).rgb;
        vec3 volumetric_lighting_3 = texture(VolumetricLighting, sample_coordinate_3).rgb;
        vec3 volumetric_lighting_4 = texture(VolumetricLighting, sample_coordinate_4).rgb;
        float linearized_depth_1 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_1).w);
        float linearized_depth_2 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_2).w);
        float linearized_depth_3 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_3).w);
        float linearized_depth_4 = LinearizeDepth(texture(SceneFeatures2Half, sample_coordinate_4).w);
        float horizontal_weight = fract(InScreenCoordinate.x * HALF_MAIN_RESOLUTION.x);
        float vertical_weight = fract(InScreenCoordinate.y * HALF_MAIN_RESOLUTION.y);
        float weight_1 = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
	    float weight_2 = (1.0f - horizontal_weight) * vertical_weight;
	    float weight_3 = horizontal_weight * (1.0f - vertical_weight);
	    float weight_4 = horizontal_weight * vertical_weight;
        weight_1 = max(weight_1 * exp(-abs(linearized_depth - linearized_depth_1)), FLOAT32_EPSILON * 1.0f);
        weight_2 = max(weight_2 * exp(-abs(linearized_depth - linearized_depth_2)), FLOAT32_EPSILON * 1.0f);
        weight_3 = max(weight_3 * exp(-abs(linearized_depth - linearized_depth_3)), FLOAT32_EPSILON * 1.0f);
        weight_4 = max(weight_4 * exp(-abs(linearized_depth - linearized_depth_4)), FLOAT32_EPSILON * 1.0f);
        float total_weight_reciprocal = 1.0f / (weight_1 + weight_2 + weight_3 + weight_4);
	    weight_1 *= total_weight_reciprocal;
	    weight_2 *= total_weight_reciprocal;
	    weight_3 *= total_weight_reciprocal;
        weight_4 *= total_weight_reciprocal;
        volumetric_lighting =   volumetric_lighting_1 * weight_1
                                + volumetric_lighting_2 * weight_2
                                + volumetric_lighting_3 * weight_3
                                + volumetric_lighting_4 * weight_4;
    }
    volumetric_lighting *= mix(0.875f, 1.25f, InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME));
    vec3 world_position = CalculateWorldPosition(InScreenCoordinate, depth);
    float hit_distance = length(world_position - CAMERA_WORLD_POSITION);
    float transmittance = 1.0f;
    for (uint i = 0; i < 4; ++i)
    {
        vec3 sample_position = mix(CAMERA_WORLD_POSITION, world_position, float(i) / 4);
        float extinction = GetExtinctionAtPosition(sample_position);
        float attenuation_factor = exp(-extinction * hit_distance * 0.25f);
        transmittance *= attenuation_factor;
    }
	Scene = vec4(volumetric_lighting,1.0f);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         Square(f1;    
   X        LinearizeDepth(f1;       depth    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        InterleavedGradientNoise(vu2;u1;         coordinate       frame    
 $   GetExtinctionAtPosition(vf3;      #   position      -   Camera   	 -       WORLD_TO_CLIP_MATRIX     	 -      WORLD_TO_CAMERA_MATRIX    -      PREVIOUS_WORLD_TO_CLIP_MATRIX     -      INVERSE_WORLD_TO_CAMERA_MATRIX    -      INVERSE_CAMERA_TO_CLIP_MATRIX    	 -      CAMERA_WORLD_POSITION    	 -      CAMERA_FORWARD_VECTOR    	 -      CURRENT_FRAME_JITTER      -      NEAR_PLANE    -   	   FAR_PLANE     /         F   near_plane_coordinate     N   view_space_position   ]   inverse_view_space_position_denominator   e   world_space_position      r   x     |   y     �   param     �   scene_features_2      �   SceneFeatures2    �   InScreenCoordinate    �   depth     �   linearized_depth      �   param     �   sample_coordinate_1   �   sample_coordinate_2   �   General  	 �       FULL_MAIN_RESOLUTION      �      INVERSE_FULL_MAIN_RESOLUTION     	 �      HALF_MAIN_RESOLUTION      �      INVERSE_HALF_MAIN_RESOLUTION      �      FRAME     �      VIEW_DISTANCE     �         �   sample_coordinate_3   �   sample_coordinate_4   �   volumetric_lighting_1     �   VolumetricLighting    �   volumetric_lighting_2     �   volumetric_lighting_3     �   volumetric_lighting_4     �   linearized_depth_1    �   SceneFeatures2Half    �   param     �   linearized_depth_2    �   param     �   linearized_depth_3    �   param     �   linearized_depth_4    �   param     �   horizontal_weight       vertical_weight     weight_1        weight_2        weight_3        weight_4      F  total_weight_reciprocal   [  volumetric_lighting   n  gl_FragCoord      r  param     s  param     {  world_position    |  param     ~  param     �  hit_distance      �  transmittance     �  i     �  sample_position   �  extinction    �  param     �  attenuation_factor    �  Scene     �  TEXTURES     	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE H  -          H  -       #       H  -             H  -         H  -      #   @   H  -            H  -         H  -      #   �   H  -            H  -         H  -      #   �   H  -            H  -         H  -      #      H  -            H  -      #   @  H  -      #   P  H  -      #   `  H  -      #   h  H  -   	   #   l  G  -      G  /   "      G  /   !       G  �   "      G  �   !      G  �          H  �       #       H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   G  �      G  �   "      G  �   !      G  �   "      G  �   !      G  �   "      G  �   !      G  n        G  �         G  �  "       G  �  !       G  �        H  �      #       G  �     G  �  "       G  �  !      H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !           !                             !  	                                           !                                                              !                 !         !  "      !     +           ,   +        -   ,   ,   ,   ,   ,                     .      -   ;  .   /        0          +  0   1   	      2         +  0   5      +     H      @+     J     �?   M      +   +  0   O         P      ,   +     ^      +  0   f      +     p   @   +     s       +     w   �Ҳ@+     }      +     �   ��SB+     �   Cq�=+     �   �E�;+     �   
ף:+     �   
�#9+     �      D+     �        	 �                              �   �      �       �   ;  �   �          �         ;  �   �      ,     �   �   J     �                        �      �   ;  �   �         �         ,     �   J   �   ,     �   J   J   ;  �   �       ;  �   �          �         +  0        +     )     4+     k    `?+     l    �?   m     +   ;  m  n        t        +  0   �        �        +     �       �  +     �    �@+     �    �>+  0   �        �     +   ;  �  �     +     �       �  �   �     �      �  ;  �  �        �  +   �    �  �     �     �  ;  �  �      
 �                          +     �       �  �  �    �  �     �     �  ;  �  �     +     �        �  �   �     �      �  ;  �  �       	 �                             �  �     �      �  ;  �  �                6               �     ;  M   �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;  !   �      ;  !   �      ;  !   �      ;  !   �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;          ;          ;          ;          ;          ;     F     ;  !   [     ;     r     ;     s     ;  !   {     ;     |     ;     ~     ;     �     ;     �     ;     �     ;  !   �     ;     �     ;  !   �     ;     �                =  �   �   �   =     �   �   W  +   �   �   �   >  �   �              A     �   �   ^   =     �   �   >  �   �              =     �   �   >  �   �   9     �      �   >  �   �              =     �   �   >  �   �        	      =     �   �   A  �   �   �   f   =     �   �   �     �   �   �   �     �   �   �   >  �   �        
      =     �   �   A  �   �   �   f   =     �   �   �     �   �   �   �     �   �   �   >  �   �              =     �   �   A  �   �   �   f   =     �   �   �     �   �   �   �     �   �   �   >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   O     �   �   �             >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   O     �   �   �             >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   O     �   �   �             >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   O     �   �   �             >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   Q     �   �      >  �   �   9     �      �   >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   Q     �   �      >  �   �   9     �      �   >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   Q     �   �      >  �   �   9     �      �   >  �   �              =  �   �   �   =     �   �   W  +   �   �   �   Q     �   �      >  �   �   9     �      �   >  �   �              A  �   �   �   s   =        �   A  2     �     s   =         �                      
     >  �                A  �     �   }   =         A  2   	  �     }   =     
  	  �         
            
     >                 =       �   �       J     =         �       J     �           >                 =       �   �       J     =         �           >                 =       �   =         �       J     �           >                 =       �   =         �            >                  =     !    =     "  �   =     #  �   �     $  "  #       %        $       &  %       '        &  �     (  !  '       *     (   (  )  >    *             =     +    =     ,  �   =     -  �   �     .  ,  -       /        .       0  /       1        0  �     2  +  1       3     (   2  )  >    3             =     4    =     5  �   =     6  �   �     7  5  6       8        7       9  8       :        9  �     ;  4  :       <     (   ;  )  >    <             =     =    =     >  �   =     ?  �   �     @  >  ?       A        @       B  A       C        B  �     D  =  C       E     (   D  )  >    E             =     G    =     H    �     I  G  H  =     J    �     K  I  J  =     L    �     M  K  L  �     N  J   M  >  F  N             =     O  F  =     P    �     Q  P  O  >    Q              =     R  F  =     S    �     T  S  R  >    T       !      =     U  F  =     V    �     W  V  U  >    W       "      =     X  F  =     Y    �     Z  Y  X  >    Z       #      =     \  �   =     ]    �     ^  \  ]       $      =     _  �   =     `    �     a  _  `  �     b  ^  a       %      =     c  �   =     d    �     e  c  d  �     f  b  e       &      =     g  �   =     h    �     i  g  h  �     j  f  i       #      >  [  j       (      =  +   o  n  O     p  o  o         m     q  p  >  r  q  A  t  u  �   O   =     v  u  >  s  v  9     w     r  s       x     .   k  l  w  =     y  [  �     z  y  x  >  [  z       )      =     }  �   >  |  }  =       �   >  ~    9     �     |  ~  >  {  �       *      =     �  {  A  �  �  /   �  =     �  �  �     �  �  �       �     B   �  >  �  �       +      >  �  J        ,      >  �  s   �  �  �  �       ,      �  �  �      �  �  �  �       ,      =     �  �  �  �  �  �  �  �  �  �  �  �  �       .      A  �  �  /   �  =     �  �  =     �  {  =     �  �  p     �  �  �     �  �  �  P     �  �  �  �       �     .   �  �  �  >  �  �       /      =     �  �  >  �  �  9     �  $   �  >  �  �       0      =     �  �       �  �  =     �  �  �     �  �  �  �     �  �  �       �        �  >  �  �       1      =     �  �  =     �  �  �     �  �  �  >  �  �  �  �  �  �       ,      =     �  �  �     �  �  �  >  �  �  �  �  �  �       3      =     �  [  Q     �  �      Q     �  �     Q     �  �     P  +   �  �  �  �  J   >  �  �  �  8       �      6            	   7     
   �          �       =     &   
   =     '   
   �     (   &   '   �  (   8       0  !   6            	   7        �          2      A  2   3   /   1   =     4   3   A  2   6   /   5   =     7   6   �     8   4   7   =     9      A  2   :   /   1   =     ;   :   A  2   <   /   5   =     =   <   �     >   ;   =   �     ?   9   >   A  2   @   /   5   =     A   @   �     B   ?   A   �     C   8   B   �  C   8       E  @   6               7        7        �     ;     F      ;  M   N      ;     ]      ;  M   e           G      =     G      �     I   G   H   P     K   J   J   �     L   I   K   >  F   L        H      A  P   Q   /   O   =  ,   R   Q   =     S   F   =     T      Q     U   S       Q     V   S      P     W   U   V   T   Q     X   W       Q     Y   W      Q     Z   W      P  +   [   X   Y   Z   J   �  +   \   R   [   >  N   \        I      A     _   N   ^   =     `   _   �     a   J   `   >  ]   a        J      =     b   ]   =  +   c   N   �  +   d   c   b   >  N   d        K      A  P   g   /   f   =  ,   h   g   =  +   i   N   �  +   j   h   i   >  e   j        M      =  +   k   e   O     l   k   k             �  l   8       �  <   6               7        7        �      ;     r      ;     |           �      =     o      �     q   o   p   >     q        �      A     t      s   =     u   t   p     v   u   =     x      p     y   x   �     z   w   y   �     {   v   z   >  r   {        �      A     ~      }   =        ~   p     �      =     �      p     �   �   �     �   w   �   �     �   �   �   >  |   �        �      =     �   r   �     �   �   �   =     �   |   �     �   �   �   �     �   �   �   �     �   �   J   �     �   �   �   �     �   �   J   �  �   8       �  ,   6     $       "   7  !   #   �  %   ;     �           �      A     �   #   }   =     �   �   �     �   �   �        �      +   �   �   J   >  �   �   9     �      �        �      .   �   �   �   �  �   8                          ��KT��D	(�D��^��                       �_`Wv� t i �%�9�� ++i)�]����M ing        N1�8��;                                   \�5s����