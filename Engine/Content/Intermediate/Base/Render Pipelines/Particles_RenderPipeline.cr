�nX/J1�F[r�|�Particles_RenderPipeline                                         �k      #                     GLSL.std.450                      main    C   �   �   �   �   �   �   �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Particles_Vertex.glsl    E   �     // OpModuleProcessed entry-point main
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

layout (std430, set = 1, binding = 2) buffer Particles
{
	layout (offset = 0) vec4[] PARTICLES;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

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
	layout (offset = 0) uint MATERIAL_INDEX;
	layout (offset = 4) uint START_INDEX;
};

layout (location = 0) out vec3 OutWorldPosition;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec2 OutTextureCoordinate;
layout (location = 3) out uint OutInstanceIndex;
layout (location = 4) out float OutOpacity;

void main()
{
    vec4 particle_data_1 = PARTICLES[(START_INDEX + gl_InstanceIndex) * 2 + 0];
    vec4 particle_data_2 = PARTICLES[(START_INDEX + gl_InstanceIndex) * 2 + 1];
    vec3 particle_position = particle_data_1.xyz;
    vec2 particle_size = vec2(particle_data_1.w, particle_data_2.x);
    vec2 half_particle_size = particle_size * 0.5f;
    float particle_normalized_age = particle_data_2.y;
    float particle_fade_in_time = particle_data_2.z;
    float particle_fade_out_time = particle_data_2.w;
    if (TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_MASKED))
    {
        half_particle_size *= smoothstep(0.0f, particle_fade_in_time, particle_normalized_age);
        half_particle_size *= 1.0f - smoothstep(particle_fade_out_time, 1.0f, particle_normalized_age);
    }
    vec3 forward_vector = normalize(CAMERA_WORLD_POSITION - particle_position);
    mat3 tangent_space_matrix = CalculateGramSchmidtRotationMatrix(forward_vector, vec3(0.0f, 1.0f, 0.0f));
    vec3 right_vector = tangent_space_matrix[0];
    vec3 up_vector = tangent_space_matrix[1];
    OutWorldPosition = particle_position;
    OutWorldPosition += right_vector * mix(-half_particle_size.x, half_particle_size.x, float(gl_VertexIndex & 1));
    OutWorldPosition += up_vector * mix(-half_particle_size.y, half_particle_size.y, float(gl_VertexIndex > 1));
    OutNormal = forward_vector;
    OutTextureCoordinate.x = float(gl_VertexIndex > 1);
    OutTextureCoordinate.y = 1.0f - float(gl_VertexIndex & 1);
    OutInstanceIndex = gl_InstanceIndex;
    if (TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_MASKED))
    {
        OutOpacity = 1.0f;
    }
    else if (TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_TRANSLUCENT))
    {
        OutOpacity = 1.0f;
        OutOpacity *= smoothstep(0.0f, particle_fade_in_time, particle_normalized_age);
        OutOpacity *= 1.0f - smoothstep(particle_fade_out_time, 1.0f, particle_normalized_age);
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         CalculateGramSchmidtRotationMatrix(vf3;vf3;      normal       random_tilt      random_tangent       random_bitangent      3   particle_data_1   5   Particles     5       PARTICLES     7         ;   PushConstantData      ;       MATERIAL_INDEX    ;      START_INDEX   =         C   gl_InstanceIndex      N   particle_data_2   Y   particle_position     ^   particle_size     f   half_particle_size    j   particle_normalized_age   m   particle_fade_in_time     p   particle_fade_out_time    s   Material      s       _Properties   s      _AlbedoThickness     	 s      _NormalMapDisplacement    s      _MaterialProperties   s      _Opacity      s      _EmissiveMultiplier   s      _Padding1     s      _Padding2     v   GlobalMaterials   v       MATERIALS     x         �   forward_vector    �   Camera   	 �       WORLD_TO_CLIP_MATRIX     	 �      WORLD_TO_CAMERA_MATRIX    �      PREVIOUS_WORLD_TO_CLIP_MATRIX     �      INVERSE_WORLD_TO_CAMERA_MATRIX    �      INVERSE_CAMERA_TO_CLIP_MATRIX    	 �      CAMERA_WORLD_POSITION    	 �      CAMERA_FORWARD_VECTOR    	 �      CURRENT_FRAME_JITTER      �      NEAR_PLANE    �   	   FAR_PLANE     �         �   tangent_space_matrix      �   param     �   param     �   right_vector      �   up_vector     �   OutWorldPosition      �   gl_VertexIndex    �   OutNormal     �   OutTextureCoordinate      �   OutInstanceIndex      �   OutOpacity    �   gl_PerVertex      �       gl_Position   �      gl_PointSize      �      gl_ClipDistance   �      gl_CullDistance   �           TEXTURES     	 
  GlobalTextureAverageValues   	 
      TEXTURE_AVERAGE_VALUES              BLUE_NOISE_TEXTURES     SKY_TEXTURE     General  	       FULL_MAIN_RESOLUTION           INVERSE_FULL_MAIN_RESOLUTION     	      HALF_MAIN_RESOLUTION           INVERSE_HALF_MAIN_RESOLUTION           FRAME          VIEW_DISTANCE               SAMPLER G  4         H  5       #       G  5      G  7   "      G  7   !      H  ;       #       H  ;      #      G  ;      G  C      +   H  s       #       H  s      #      H  s      #      H  s      #      H  s      #      H  s      #      H  s      #      H  s      #      G  u          H  v       #       G  v      G  x   "       G  x   !      H  �          H  �       #       H  �             H  �         H  �      #   @   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #      H  �            H  �      #   @  H  �      #   P  H  �      #   `  H  �      #   h  H  �   	   #   l  G  �      G  �   "      G  �   !       G  �          G  �      *   G  �         G  �         G  �         G  �         H  �              H  �            H  �            H  �            G  �      G    "       G    !       G  	        H  
      #       G  
     G    "       G    !      G    "       G    !      G    "       G    !      H        #       H       #      H       #      H       #      H       #       H       #   $   G       G    "      G    !      G    "      G    !           !                               	           
         !     
   	   	   +           �?+     !         1            2      1     4   1     5   4      6      5   ;  6   7        8          +  8   9         :             ;   :   :      <   	   ;   ;  <   =   	   +  8   >         ?   	   :      B      8   ;  B   C      +  :   G      +  :   I          K      1   +  :   U        \            ]      \   +  :   _         `         +     h      ? 
 s   :   :   :   :   :      :   :   +  :   t        u   s   t     v   u      w      v   ;  w   x         {      :          �   1        �   �   �   �   �   �         \            �      �   ;  �   �      +  8   �         �            �      
   ,     �   !       !      �         ;  �   �      ;  B   �      ;  �   �         �      \   ;  �   �         �            �      :   ;  �   �      ;  �   �      +  :   �        �      U     �   1      �   �      �      �   ;  �   �         �      �           1    	                            +  :                           ;            	  1       
  	          
  ;               +  :                            ;           	                                             ;              \   \   \   \   :                ;                        ;               �     6               �     ;  2   3      ;  2   N      ;  	   Y      ;  ]   ^      ;  ]   f      ;  `   j      ;  `   m      ;  `   p      ;  	   �      ;  �   �      ;  	   �      ;  	   �      ;  	   �      ;  	   �           �      A  ?   @   =   >   =  :   A   @   =  8   D   C   |  :   E   D   �  :   F   A   E   �  :   H   F   G   �  :   J   H   I   A  K   L   7   9   J   =  1   M   L   >  3   M        �      A  ?   O   =   >   =  :   P   O   =  8   Q   C   |  :   R   Q   �  :   S   P   R   �  :   T   S   G   �  :   V   T   U   A  K   W   7   9   V   =  1   X   W   >  N   X        �      =  1   Z   3   O     [   Z   Z             >  Y   [        �      A  `   a   3   _   =     b   a   A  `   c   N   I   =     d   c   P  \   e   b   d   >  ^   e        �      =  \   g   ^   �  \   i   g   h   >  f   i        �      A  `   k   N   U   =     l   k   >  j   l        �      A  `   n   N   G   =     o   n   >  m   o        �      A  `   q   N   _   =     r   q   >  p   r        �      A  ?   y   =   9   =  :   z   y   A  {   |   x   9   z   9   =  :   }   |   �  :   ~   }   U   �     �   ~   U   �  �       �  �   �   �   �  �        �      =     �   m   =     �   j        �      1   !   �   �   =  \   �   f   �  \   �   �   �   >  f   �        �      =     �   p   =     �   j        �      1   �       �   �     �       �   =  \   �   f   �  \   �   �   �   >  f   �   �  �   �  �              A  �   �   �   �   =     �   �   =     �   Y   �     �   �   �        �      E   �   >  �   �              =     �   �   >  �   �   >  �   �   9  
   �      �   �   >  �   �              A  	   �   �   9   =     �   �   >  �   �              A  	   �   �   >   =     �   �   >  �   �              =     �   Y   >  �   �              =     �   �   A  `   �   f   I   =     �   �        �   �   A  `   �   f   I   =     �   �   =  8   �   �   �  8   �   �   >   o     �   �        �      .   �   �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �              =     �   �   A  `   �   f   U   =     �   �        �   �   A  `   �   f   U   =     �   �   =  8   �   �   �     �   �   >   �     �   �       !        �      .   �   �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �              =     �   �   >  �   �        	      =  8   �   �   �     �   �   >   �     �   �       !   A  �   �   �   I   >  �   �        
      =  8   �   �   �  8   �   �   >   o     �   �   �     �       �   A  �   �   �   U   >  �   �              =  8   �   C   |  :   �   �   >  �   �              A  ?   �   =   9   =  :   �   �   A  {   �   x   9   �   9   =  :   �   �   �  :   �   �   U   �     �   �   U   �  �       �  �   �   �   �  �              >  �       �  �   �  �              A  ?   �   =   9   =  :   �   �   A  {   �   x   9   �   9   =  :   �   �   �  :   �   �   �   �     �   �   �   �  �       �  �   �   �   �  �              >  �                  =     �   m   =     �   j        �      1   !   �   �   =     �   �   �     �   �   �   >  �   �              =     �   p   =     �   j        �      1   �       �   �     �       �   =     �   �   �     �   �   �   >  �   �   �  �   �  �   �  �   �  �              A  �   �   �   9   =  �   �   �   =     �   �   Q     �   �       Q     �   �      Q     �   �      P  1      �   �   �       �  1     �      A      �   9   >      �  8       �  F   6  
             7  	      7  	      �     ;  	         ;  	              �      =           =           =           =           �              �              �                         E      >             �      =           =                      D         >             �      =           =           =           Q     "          Q     #         Q     $         Q     %          Q     &         Q     '         Q     (          Q     )         Q     *         P     +   "   #   $   P     ,   %   &   '   P     -   (   )   *   P  
   .   +   ,   -   �  .   8  �s      #     �                GLSL.std.450                     main    �   2  M  W  f  t    �  �  �               ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Particles_Fragment.glsl  �   �     // OpModuleProcessed entry-point main
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

layout (std430, set = 1, binding = 2) buffer Particles
{
	layout (offset = 0) vec4[] PARTICLES;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

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
	layout (offset = 0) uint MATERIAL_INDEX;
	layout (offset = 4) uint START_INDEX;
};

layout (location = 0) in vec3 InWorldPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec2 InTextureCoordinate;
layout (location = 3) flat in uint InInstanceIndex;
layout (location = 4) in float InOpacity;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;

void main()
{
    vec4 albedo_thickness;
    EVALUATE_ALBEDO_THICKNESS(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, albedo_thickness);
    vec4 normal_map_displacement;
    EVALUATE_NORMAL_MAP_DISPLACEMENT(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, normal_map_displacement);
    vec4 material_properties;
    EVALUATE_MATERIAL_PROPERTIES(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, material_properties);
    float opacity;
    EVALUATE_OPACITY(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, opacity);
    opacity *= InOpacity;
    if ((TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_MASKED) && opacity < 0.5f)
        ||
        (TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_TRANSLUCENT) && opacity < InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME + InInstanceIndex)))
    {
        discard;
    }
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(albedo_thickness.rgb,albedo_thickness.a*opacity);
	SceneFeatures2 = vec4(InNormal,gl_FragCoord.z);
	SceneFeatures3 = material_properties;
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       InterleavedGradientNoise(vu2;u1;         coordinate       frame     !   unpacked      G   view_space_position   I   Camera   	 I       WORLD_TO_CLIP_MATRIX     	 I      WORLD_TO_CAMERA_MATRIX    I      PREVIOUS_WORLD_TO_CLIP_MATRIX     I      INVERSE_WORLD_TO_CAMERA_MATRIX    I      INVERSE_CAMERA_TO_CLIP_MATRIX    	 I      CAMERA_WORLD_POSITION    	 I      CAMERA_FORWARD_VECTOR    	 I      CURRENT_FRAME_JITTER      I      NEAR_PLANE    I   	   FAR_PLANE     K         W   denominator   k   view_space_position   u   denominator   �   x     �   y     �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �         �   PushConstantData      �       MATERIAL_INDEX    �      START_INDEX   �         �   albedo_thickness      �   TEXTURES      �   SAMPLER   �   InTextureCoordinate   �   param     �   normal_map_displacement   �   param       material_properties     param       opacity   ,  param     2  InOpacity     M  gl_FragCoord      Q  General  	 Q      FULL_MAIN_RESOLUTION      Q     INVERSE_FULL_MAIN_RESOLUTION     	 Q     HALF_MAIN_RESOLUTION      Q     INVERSE_HALF_MAIN_RESOLUTION      Q     FRAME     Q     VIEW_DISTANCE     S        W  InInstanceIndex   Z  param     [  param     d  velocity      f  InWorldPosition   g  param     j  param     t  SceneFeatures1      SceneFeatures2    �  InNormal      �  SceneFeatures3    �  SceneFeatures4   	 �  GlobalTextureAverageValues   	 �      TEXTURE_AVERAGE_VALUES    �        �  BLUE_NOISE_TEXTURES   �  SKY_TEXTURE   �  Particles     �      PARTICLES     �      H  I          H  I       #       H  I             H  I         H  I      #   @   H  I            H  I         H  I      #   �   H  I            H  I         H  I      #   �   H  I            H  I         H  I      #      H  I            H  I      #   @  H  I      #   P  H  I      #   `  H  I      #   h  H  I   	   #   l  G  I      G  K   "      G  K   !       H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      H  �       #       H  �      #      G  �      G  �   "       G  �   !       G  �   "      G  �   !      G  �         G  2        G  M        H  Q      #       H  Q     #      H  Q     #      H  Q     #      H  Q     #       H  Q     #   $   G  Q     G  S  "      G  S  !      G  W     G  W        G  f         G  t         G          G  �        G  �        G  �        G  �        H  �      #       G  �     G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !      G  �        H  �      #       G  �     G  �  "      G  �  !           !                                   	         
   	      !     
           	                       	      !                                  !     	                   
   +     #   �   +  	   &     C+     (          )      	     ,          +  ,   -      +     2      +  ,   5      +     :      +  ,   =      +     B        H   
        I   H   H   H   H   H            	   	      J      I   ;  J   K      +  ,   L          M      H   +  	   Q     �?+  	   e      ?+  ,   l      +     �   @   +  	   �   �Ҳ@+  	   �   ��SB+  	   �   Cq�=+  	   �   �E�; 
 �                  	         +     �        �   �   �     �   �      �      �   ;  �   �        �            �   	   �   ;  �   �   	      �   	         �         +     �        �    	 �   	                         +     �        �   �   �      �       �   ;  �   �       +  ,   �         �       �     �      �       �   ;  �   �         �   �      �         ;  �   �      +     �      +     �       +  ,        +  ,           1     	   ;  1  2     +     F        L     
   ;  L  M       Q                 	      R     Q  ;  R  S        V        ;  V  W        c           e        ;  e  f     +  ,   n        o           s     
   ;  s  t     ;  s       ;  e  �     ;  s  �     ;  s  �     +  	   �        �  
   �     �  �     �     �  ;  �  �       �  �   �      �      �  ;  �  �       	 �  	                           �  �     �      �  ;  �  �        �  
     �  �     �     �  ;  �  �          �     6               �     ;      �      ;     �      ;      �      ;     �      ;           ;          ;  )        ;     ,     ;     Z     ;     [     ;  c  d     ;     g     ;     j          �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   L   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   �   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �   �   W  
   �   �   �   >  �   �   �  �   �  �        �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   �   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   L   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   l   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �   �   W  
   �   �   �   >  �   �   �  �   �  �        �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   l   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        �      A  �   �   �   L   =     �   �   A  �   �   �   L   �   L   =     �   �   �     �   �   �   �  �   �   �   �   �         �  �   �     �  �        �      A  �     �   L   =         A  �     �   L       =         A  �     �     =  �       =  �   	  �   V  �   
    	  =       �   W  
     
    >      �     �         �      A  �     �   L   =         A  �     �   L       =         >      9  
          >      �     �                 A  �     �   L   =         A  �     �   L     L   =         �         �   �  �       �   �        �      )  �                A  �     �   L   =         A  �      �   L       =     !     A  �   "  �   !  =  �   #  "  =  �   $  �   V  �   %  #  $  =     &  �   W  
   '  %  &  Q  	   (  '      >    (  �    �  )              A  �   *  �   L   =     +  *  A  �   -  �   L   +    =     .  -  >  ,  .  9  
   /     ,  Q  	   0  /      >    0  �    �               =  	   3  2  =  	   4    �  	   5  4  3  >    5             A  �   6  �   L   =     7  6  A  �   8  �   L   7  L   =     9  8  �     :  9  2   �  �   ;  :  2   =  	   <    �  �   =  <  e   �  �   >  ;  =  �  �   ?  >  �  A      �  ?  @  A  �  @             A  �   B  �   L   =     C  B  A  �   D  �   L   C  L   =     E  D  �     G  E  F  �  �   H  G  F  �  J      �  H  I  J  �  I             =  	   K    =  
   N  M  O     O  N  N         m     P  O  A  �   T  S    =     U  T  =     X  W  �     Y  U  X  >  Z  P  >  [  Y  9  	   \     Z  [  �  �   ]  K  \  �  J  �  J  �  �   ^  H  @  ]  I  �  A  �  A  �  �   _  >    ^  J  �  a      �  _  `  a  �  `             �  �  a             =     h  f  >  g  h  9     i     g  =     k  f  >  j  k  9     l     j  �     m  i  l  A  o  p  K   n  =     q  p  �     r  m  q  >  d  r       	      =  
   u  �   O     v  u  u            A  )   w  �   B   =  	   x  w  =  	   y    �  	   z  x  y  Q  	   {  v      Q  	   |  v     Q  	   }  v     P  
   ~  {  |  }  z  >  t  ~       
      =     �  �  A  1  �  M  :   =  	   �  �  Q  	   �  �      Q  	   �  �     Q  	   �  �     P  
   �  �  �  �  �  >    �             =  
   �    >  �  �             =     �  d  Q  	   �  �      Q  	   �  �     P  
   �  �  �  �  �  >  �  �  �  8       �      6  
             7        �     ;      !                 =     "      �     $   "   #   p  	   %   $   �  	   '   %   &   A  )   *   !   (   >  *   '              =     +      �     .   +   -   �     /   .   #   p  	   0   /   �  	   1   0   &   A  )   3   !   2   >  3   1              =     4      �     6   4   5   �     7   6   #   p  	   8   7   �  	   9   8   &   A  )   ;   !   :   >  ;   9              =     <      �     >   <   =   �     ?   >   #   p  	   @   ?   �  	   A   @   &   A  )   C   !   B   >  C   A              =  
   D   !   �  D   8       Z  :   6               7        �     ;      G      ;  )   W           \      A  M   N   K   L   =  H   O   N   =     P      Q  	   R   P       Q  	   S   P      Q  	   T   P      P  
   U   R   S   T   Q   �  
   V   O   U   >  G   V        ]      A  )   X   G   B   =  	   Y   X   �  	   Z   Q   Y   >  W   Z        ^      =  	   [   W   =  
   \   G   O     ]   \   \          �     ^   ]   [   A  )   _   G   (   Q  	   `   ^       >  _   `   A  )   a   G   2   Q  	   b   ^      >  a   b        `      =  
   c   G   O     d   c   c          �     f   d   e   P     g   e   e   �     h   f   g   �  h   8       f  ;   6               7        �     ;      k      ;  )   u           h      A  M   m   K   l   =  H   n   m   =     o      Q  	   p   o       Q  	   q   o      Q  	   r   o      P  
   s   p   q   r   Q   �  
   t   n   s   >  k   t        i      A  )   v   k   B   =  	   w   v   �  	   x   Q   w   >  u   x        j      =  	   y   u   =  
   z   k   O     {   z   z          �     |   {   y   A  )   }   k   (   Q  	   ~   |       >  }   ~   A  )      k   2   Q  	   �   |      >     �        l      =  
   �   k   O     �   �   �          �     �   �   e   P     �   e   e   �     �   �   �   �  �   8       �  <   6  	             7        7        �     ;  )   �      ;  )   �           �      =     �      �     �   �   �   >     �        �      A     �      (   =     �   �   p  	   �   �   =     �      p  	   �   �   �  	   �   �   �   �  	   �   �   �   >  �   �        �      A     �      2   =     �   �   p  	   �   �   =     �      p  	   �   �   �  	   �   �   �   �  	   �   �   �   >  �   �        �      =  	   �   �   �  	   �   �   �   =  	   �   �   �  	   �   �   �   �  	   �   �   �   �  	   �   �   Q   �  	   �   �   �   �  	   �   �   Q   �  �   8                          ��KT��D	(�D��^��       �}�l�!�                       ��v)��%�9����Y��G� �z��.v<             	                �}�l�!�