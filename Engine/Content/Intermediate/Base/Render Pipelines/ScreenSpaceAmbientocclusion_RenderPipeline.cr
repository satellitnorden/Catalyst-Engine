�nX/J1�:	=;8�ScreenSpaceAmbientOcclusion_RenderPipeline                       �G      #     _                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\ScreenSpaceAmbientOcclusion_Vertex.glsl  3   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform HammersleyHemisphereSamples
{
	layout (offset = 0) vec4 HAMMERSLEY_COSINUS_SAMPLES[64];
	layout (offset = 1024) vec4 HAMMERSLEY_UNIFORM_SAMPLES[64];
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
*	Rotates the given vector around the yaw.
*/
vec3 RotateYaw(vec3 X, float angle)
{
	float sine = sin(angle);
    float cosine = cos(angle);

    float temp = X.x * cosine + X.z * sine;
    X.z = -X.x * sine + X.z * cosine;
    X.x = temp;

    return X;
}

/*
*   Calculates a Gram-Schmidt rotation matrix based on a normal and a random tilt.
*/
mat3 CalculateGramSchmidtRotationMatrix(vec3 normal, vec3 random_tilt)
{
    vec3 random_tangent = normalize(random_tilt - normal * dot(random_tilt, normal));
    vec3 random_bitangent = cross(normal, random_tangent);

    return mat3(random_tangent, random_bitangent, normal);
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint NUMBER_OF_SAMPLES;
};

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures2Half;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES      =   Material      =       _Properties   =      _AlbedoThickness     	 =      _NormalMapDisplacement    =      _MaterialProperties   =      _Opacity      =      _EmissiveMultiplier   =      _Padding1     =      _Padding2     @   GlobalMaterials   @       MATERIALS     B         G   BLUE_NOISE_TEXTURES   K   SKY_TEXTURE   N   Camera   	 N       WORLD_TO_CLIP_MATRIX     	 N      WORLD_TO_CAMERA_MATRIX    N      PREVIOUS_WORLD_TO_CLIP_MATRIX     N      INVERSE_WORLD_TO_CAMERA_MATRIX    N      INVERSE_CAMERA_TO_CLIP_MATRIX    	 N      CAMERA_WORLD_POSITION    	 N      CAMERA_FORWARD_VECTOR    	 N      CURRENT_FRAME_JITTER      N      NEAR_PLANE    N   	   FAR_PLANE     P         Q   General  	 Q       FULL_MAIN_RESOLUTION      Q      INVERSE_FULL_MAIN_RESOLUTION     	 Q      HALF_MAIN_RESOLUTION      Q      INVERSE_HALF_MAIN_RESOLUTION      Q      FRAME     Q      VIEW_DISTANCE     S        	 W   HammersleyHemisphereSamples  
 W       HAMMERSLEY_COSINUS_SAMPLES   
 W      HAMMERSLEY_UNIFORM_SAMPLES    Y         Z   PushConstantData      Z       NUMBER_OF_SAMPLES     \         ^   SceneFeatures2Half  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       H  =       #       H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      G  ?          H  @       #       G  @      G  B   "       G  B   !      G  G   "       G  G   !      G  K   "       G  K   !      H  N          H  N       #       H  N             H  N         H  N      #   @   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #   �   H  N            H  N         H  N      #      H  N            H  N      #   @  H  N      #   P  H  N      #   `  H  N      #   h  H  N   	   #   l  G  N      G  P   "      G  P   !       H  Q       #       H  Q      #      H  Q      #      H  Q      #      H  Q      #       H  Q      #   $   G  Q      G  S   "      G  S   !      G  U         G  V         H  W       #       H  W      #      G  W      G  Y   "      G  Y   !      H  Z       #       G  Z      G  ^   "      G  ^   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <        
 =   #   #   #   #   #      #   #   +  #   >        ?   =   >     @   ?      A      @   ;  A   B        C   8   +  #   D         E   C   D      F       E   ;  F   G        	 H                              I   H      J       I   ;  J   K         L   ,        M           N   L   L   L   L   L   M   M               O      N   ;  O   P        Q               #         R      Q   ;  R   S      +  #   T   @     U   ,   T     V   ,   T     W   U   V      X      W   ;  X   Y        Z   #      [   	   Z   ;  [   \   	      ]       C   ;  ]   ^            �     6               �     ;     	      ;                �      =           �              �              o           �        
      >  	           �      =           �              �              o           �        
      >             �      =        	   �               �     "       !   A  %   &      $   >  &   "        �      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        �      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  \v      #     �                GLSL.std.450                     main        �               ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\ScreenSpaceAmbientOcclusion_Fragment.glsl    K   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform HammersleyHemisphereSamples
{
	layout (offset = 0) vec4 HAMMERSLEY_COSINUS_SAMPLES[64];
	layout (offset = 1024) vec4 HAMMERSLEY_UNIFORM_SAMPLES[64];
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
*	Rotates the given vector around the yaw.
*/
vec3 RotateYaw(vec3 X, float angle)
{
	float sine = sin(angle);
    float cosine = cos(angle);

    float temp = X.x * cosine + X.z * sine;
    X.z = -X.x * sine + X.z * cosine;
    X.x = temp;

    return X;
}

/*
*   Calculates a Gram-Schmidt rotation matrix based on a normal and a random tilt.
*/
mat3 CalculateGramSchmidtRotationMatrix(vec3 normal, vec3 random_tilt)
{
    vec3 random_tangent = normalize(random_tilt - normal * dot(random_tilt, normal));
    vec3 random_bitangent = cross(normal, random_tangent);

    return mat3(random_tangent, random_bitangent, normal);
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint NUMBER_OF_SAMPLES;
};

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures2Half;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 AmbientOcclusion;

void main()
{
    #define AMBIENT_OCCLUSION_RADIUS (1.0f)
    vec4 scene_features_2 = texture(SceneFeatures2Half, InScreenCoordinate);
    vec3 normal = scene_features_2.xyz;
    float depth = scene_features_2.w;
    vec3 world_position = CalculateWorldPosition(InScreenCoordinate, depth);
	vec4 noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);
	uint random_hemisphere_sample_start_index = uint(noise_texture_sample.w * 64.0f);
    mat3 random_rotation = CalculateGramSchmidtRotationMatrix(normal, noise_texture_sample.xyz * 2.0f - 1.0f);
	float total_weight = 0.0f;
	float occlusion = 0.0f;
    for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        vec4 random_hemisphere_sample = HAMMERSLEY_COSINUS_SAMPLES[(random_hemisphere_sample_start_index + i) & 63];
        vec3 random_hemisphere_direction = random_hemisphere_sample.xyz;
        float random_hemisphere_length = random_hemisphere_sample.w;
		vec3 random_direction = random_rotation * random_hemisphere_direction;
		random_direction = dot(random_direction, normal) >= 0.0f ? random_direction : -random_direction;
		vec3 sample_position = world_position + random_direction * InterleavedGradientNoise(uvec2(gl_FragCoord.xy), i) * AMBIENT_OCCLUSION_RADIUS;
		vec4 sample_view_space_position = WORLD_TO_CLIP_MATRIX * vec4(sample_position, 1.0f);
		float sample_screen_coordinate_inverse_denominator = 1.0f / sample_view_space_position.w;
		vec2 sample_screen_coordinate = sample_view_space_position.xy * sample_screen_coordinate_inverse_denominator * 0.5f + 0.5f;
		float expected_view_distance = CalculateViewSpacePosition(InScreenCoordinate, sample_view_space_position.z * sample_screen_coordinate_inverse_denominator).z;
		vec4 sample_scene_features = texture(SceneFeatures2Half, sample_screen_coordinate);
		float sample_view_distance = CalculateViewSpacePosition(InScreenCoordinate, sample_scene_features.w).z;
		float distance_falloff = SmoothStep(1.0f - (min(abs(expected_view_distance - sample_view_distance), AMBIENT_OCCLUSION_RADIUS) / AMBIENT_OCCLUSION_RADIUS));
		float sample_weight = 1.0f;
		occlusion += float(expected_view_distance < sample_view_distance) * distance_falloff * sample_weight;
		total_weight += sample_weight;
    }
	occlusion = total_weight != 0.0f ? 1.0f - (occlusion / total_weight) : 1.0f;
	AmbientOcclusion = vec4(occlusion);
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main     
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index        CalculateViewSpacePosition(vf2;f1;       texture_coordinate       depth    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth     $   CalculateGramSchmidtRotationMatrix(vf3;vf3;   "   normal    #   random_tilt   (   SmoothStep(f1;    '   number    -   InterleavedGradientNoise(vu2;u1;      +   coordinate    ,   frame     /   offset_index      0   General  	 0       FULL_MAIN_RESOLUTION      0      INVERSE_FULL_MAIN_RESOLUTION     	 0      HALF_MAIN_RESOLUTION      0      INVERSE_HALF_MAIN_RESOLUTION      0      FRAME     0      VIEW_DISTANCE     2         <   offset_coordinate     U   BLUE_NOISE_TEXTURES   b   near_plane_coordinate     j   view_space_position   l   Camera   	 l       WORLD_TO_CLIP_MATRIX     	 l      WORLD_TO_CAMERA_MATRIX    l      PREVIOUS_WORLD_TO_CLIP_MATRIX     l      INVERSE_WORLD_TO_CAMERA_MATRIX    l      INVERSE_CAMERA_TO_CLIP_MATRIX    	 l      CAMERA_WORLD_POSITION    	 l      CAMERA_FORWARD_VECTOR    	 l      CURRENT_FRAME_JITTER      l      NEAR_PLANE    l   	   FAR_PLANE     n         |   inverse_view_space_position_denominator   �   near_plane_coordinate     �   view_space_position   �   inverse_view_space_position_denominator   �   world_space_position      �   random_tangent    �   random_bitangent      �   x     �   y     �   scene_features_2      �   SceneFeatures2Half      InScreenCoordinate      normal      depth       world_position      param       param       noise_texture_sample        gl_FragCoord        param       param       random_hemisphere_sample_start_index      !  random_rotation   '  param     )  param     +  total_weight      ,  occlusion     -  i     4  PushConstantData      4      NUMBER_OF_SAMPLES     6       	 =  random_hemisphere_sample     	 @  HammersleyHemisphereSamples  
 @      HAMMERSLEY_COSINUS_SAMPLES   
 @     HAMMERSLEY_UNIFORM_SAMPLES    B       	 K  random_hemisphere_direction  	 N  random_hemisphere_length      Q  random_direction      a  sample_position   g  param     h  param    	 n  sample_view_space_position    w  sample_screen_coordinate_inverse_denominator     	 {  sample_screen_coordinate      �  expected_view_distance    �  param     �  param     �  sample_scene_features     �  sample_view_distance      �  param     �  param     �  distance_falloff      �  param     �  sample_weight     �  AmbientOcclusion      �  TEXTURES      �  Material      �      _Properties   �     _AlbedoThickness     	 �     _NormalMapDisplacement    �     _MaterialProperties   �     _Opacity      �     _EmissiveMultiplier   �     _Padding1     �     _Padding2     �  GlobalMaterials   �      MATERIALS     �        �  SKY_TEXTURE H  0       #       H  0      #      H  0      #      H  0      #      H  0      #       H  0      #   $   G  0      G  2   "      G  2   !      G  U   "       G  U   !      H  l          H  l       #       H  l             H  l         H  l      #   @   H  l            H  l         H  l      #   �   H  l            H  l         H  l      #   �   H  l            H  l         H  l      #      H  l            H  l      #   @  H  l      #   P  H  l      #   `  H  l      #   h  H  l   	   #   l  G  l      G  n   "      G  n   !       G  �   "      G  �   !      G           G          H  4      #       G  4     G  >        G  ?        H  @      #       H  @     #      G  @     G  B  "      G  B  !      G  �         G  �  "       G  �  !       H  �      #       H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      H  �     #      G  �         H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !           !                                   	            
                             !        	   
                                                 !                                      !  !             !  &         !  *      	   
     0                        1      0   ;  1   2        3          +  3   4         5         +     :      +     =       +     B       +     G       	 Q                              R   Q     S   R   B      T       S   ;  T   U          W       R   +     \      B+     d      @+     f     �?   i           k           l   k   k   k   k   k                     m      l   ;  m   n         o      k   +     }      +     �      +  3   �      +     �       +     �     @@+     �   @   +     �   �Ҳ@+     �   ��SB+     �   Cq�=+     �   �E�;;  W   �                  ;                    ;         +         �B               4        5  	   4  ;  5  6  	   +  3   7         8  	        ;    >     �     ?     �     @  >  ?     A     @  ;  A  B     +     F  ?      H        +     �     ?+  3   �        �        ;  �  �     +     �       �  Q   �     �      �  ;  �  �       
 �                          +     �       �  �  �    �  �     �     �  ;  �  �      	 �                             �  �     �      �  ;  �  �           �     6               �     ;  i   �      ;          ;          ;          ;          ;          ;  i        ;  	        ;  
        ;  
        ;     !     ;     '     ;     )     ;     +     ;     ,     ;  
   -     ;  i   =     ;     K     ;     N     ;     Q     ;     Y     ;     a     ;  	   g     ;  
   h     ;  i   n     ;     w     ;     {     ;     �     ;     �     ;     �     ;  i   �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �          �      =  R      �   =         W            >  �          �      =       �   O                     >           �      A     	  �   }   =     
  	  >    
       �      =         >      =         >      9              >           �      =         O                  m         >      >    =   9              >           �      A         }   =         �           m         >           �      =     "    O     #  "  "            �     $  #  d   P     %  f   f   f   �     &  $  %  =     (    >  '  (  >  )  &  9      *  $   '  )  >  !  *       �      >  +  �        �      >  ,  �        �      >  -  =   �  .  �  .       �      �  0  1      �  2  �  2       �      =     3  -  A  8  9  6  7  =     :  9  �  ;  <  3  :  �  <  /  0  �  /       �      =     C    =     D  -  �     E  C  D  �     G  E  F  A  H  I  B  7  G  =     J  I  >  =  J       �      =     L  =  O     M  L  L            >  K  M       �      A     O  =  }   =     P  O  >  N  P       �      =      R  !  =     S  K  �     T  R  S  >  Q  T       �      =     U  Q  =     V    �     W  U  V  �  ;  X  W  �   �  [      �  X  Z  ]  �  Z       �      =     \  Q  >  Y  \  �  [  �  ]       �      =     ^  Q       _  ^  >  Y  _  �  [  �  [  =     `  Y       �      >  Q  `       �      =     b    =     c  Q  =     d    O     e  d  d         m     f  e  >  g  f  =     i  -  >  h  i  9     j  -   g  h  �     k  c  j  �     l  k  f   �     m  b  l  >  a  m       �      A  o   o  n   7  =  k   p  o  =     q  a  Q     r  q      Q     s  q     Q     t  q     P     u  r  s  t  f   �     v  p  u  >  n  v       �      A     x  n  }   =     y  x  �     z  f   y  >  w  z       �      =     |  n  O     }  |  |         =     ~  w  �       }  ~  �     �    �  P     �  �  �  �     �  �  �  >  {  �       �      A     �  n  �   =     �  �  =     �  w  �     �  �  �  =     �    >  �  �  >  �  �  9     �     �  �  Q     �  �     >  �  �       �      =  R   �  �   =     �  {  W     �  �  �  >  �  �       �      =     �    >  �  �  A     �  �  }   =     �  �  >  �  �  9     �     �  �  Q     �  �     >  �  �       �      =     �  �  =     �  �  �     �  �  �       �        �       �     %   �  f   �     �  �  f   �     �  f   �  >  �  �  9     �  (   �  >  �  �       �      >  �  f        �      =     �  �  =     �  �  �  ;  �  �  �  �     �  �  f   �   =     �  �  �     �  �  �  =     �  �  �     �  �  �  =     �  ,  �     �  �  �  >  ,  �       �      =     �  �  =     �  +  �     �  �  �  >  +  �  �  1  �  1       �      =     �  -  �     �  �  �  >  -  �  �  .  �  0       �      =     �  +  �  ;  �  �  �   �  �      �  �  �  �  �  �       �      =     �  ,  =     �  +  �     �  �  �  �     �  f   �  >  �  �  �  �  �  �  >  �  f   �  �  �  �  =     �  �       �      >  ,  �       �      =     �  ,  P     �  �  �  �  �  >  �  �  �  8       �   9   6               7  	      7  
      �     ;  
   /      ;  	   <           �       A  5   6   2   4   =     7   6   =     8      �     9   7   8   �     ;   9   :   >  /   ;        �       A  
   >      =   =     ?   >   A  5   @   2   4   =     A   @   �     C   A   B   �     D   C   :   �     E   ?   D   A  
   F   <   =   >  F   E        �       A  
   H      G   =     I   H   A  5   J   2   4   =     K   J   �     L   K   B   �     M   L   B   �     N   M   :   �     O   I   N   A  
   P   <   G   >  P   O        �       =     V   /   A  W   X   U   V   =  R   Y   X   =     Z   <   p     [   Z   P     ]   \   \   �     ^   [   ]   W     _   Y   ^   �  _   8       �   E   6               7        7        �     ;     b      ;  i   j      ;     |                 =     c      �     e   c   d   P     g   f   f   �     h   e   g   >  b   h              A  o   p   n   4   =  k   q   p   =     r   b   =     s      Q     t   r       Q     u   r      P     v   t   u   s   Q     w   v       Q     x   v      Q     y   v      P     z   w   x   y   f   �     {   q   z   >  j   {              A     ~   j   }   =        ~   �     �   f      >  |   �              =     �   |   =     �   j   O     �   �   �             �     �   �   �   A     �   j   =   Q     �   �       >  �   �   A     �   j   G   Q     �   �      >  �   �   A     �   j   �   Q     �   �      >  �   �              =     �   j   O     �   �   �             �  �   8         @   6               7        7        �     ;     �      ;  i   �      ;     �      ;  i   �                 =     �      �     �   �   d   P     �   f   f   �     �   �   �   >  �   �              A  o   �   n   4   =  k   �   �   =     �   �   =     �      Q     �   �       Q     �   �      P     �   �   �   �   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   f   �     �   �   �   >  �   �              A     �   �   }   =     �   �   �     �   f   �   >  �   �              =     �   �   =     �   �   �     �   �   �   >  �   �              A  o   �   n   �   =  k   �   �   =     �   �   �     �   �   �   >  �   �              =     �   �   O     �   �   �             �  �   8       P  F   6      $       !   7     "   7     #   �  %   ;     �      ;     �           R      =     �   #   =     �   "   =     �   #   =     �   "   �     �   �   �   �     �   �   �   �     �   �   �        �      E   �   >  �   �        S      =     �   "   =     �   �        �      D   �   �   >  �   �        U      =     �   �   =     �   �   =     �   "   Q     �   �       Q     �   �      Q     �   �      Q     �   �       Q     �   �      Q     �   �      Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   P      �   �   �   �   �  �   8       [     6     (       &   7     '   �  )        ]      =     �   '   =     �   '   �     �   �   �   =     �   '   �     �   d   �   �     �   �   �   �     �   �   �   �  �   8       �  <   6     -       *   7  	   +   7  
   ,   �  .   ;     �      ;     �           �      =     �   ,   �     �   �   �   >  ,   �        �      A  
   �   +   =   =     �   �   p     �   �   =     �   ,   p     �   �   �     �   �   �   �     �   �   �   >  �   �        �      A  
   �   +   G   =     �   �   p     �   �   =     �   ,   p     �   �   �     �   �   �   �     �   �   �   >  �   �        �      =     �   �   �     �   �   �   =     �   �   �     �   �   �   �     �   �   �   �     �   �   f   �     �   �   �   �     �   �   f   �  �   8                          ��KT��D	(�D��^���l�a�                       �]����M �         �^�5�|V                                   <!�#INU�