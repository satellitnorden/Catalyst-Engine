�nX/J1�F[r�|�Particles_RenderPipeline                                         M      #     �                 GLSL.std.450                      main    C   �   �   �   �   �        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Particles_Vertex.glsl    �   �     // OpModuleProcessed entry-point main
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

layout (std430, set = 1, binding = 1) buffer Particles
{
	layout (offset = 0) vec4[] PARTICLES;
};

layout (set = 1, binding = 2) uniform sampler SAMPLER;

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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint MATERIAL_INDEX;
	layout (offset = 4) uint START_INDEX;
};

layout (location = 0) out vec3 OutWorldPosition;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec2 OutTextureCoordinate;

void main()
{
    vec4 particle_data_1 = PARTICLES[(START_INDEX + gl_InstanceIndex) * 2 + 0];
    vec4 particle_data_2 = PARTICLES[(START_INDEX + gl_InstanceIndex) * 2 + 1];
    vec3 particle_position = particle_data_1.xyz;
    vec2 particle_size = vec2(particle_data_1.w, particle_data_2.x);
    vec2 half_particle_size = particle_size * 0.5f;
    float particle_normalized_age = particle_data_2.y;
    half_particle_size *= smoothstep(0.0f, 0.1f, particle_normalized_age);
    half_particle_size *= 1.0f - smoothstep(0.9f, 1.0f, particle_normalized_age);
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
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition,1.0f);
}     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         CalculateGramSchmidtRotationMatrix(vf3;vf3;      normal       random_tilt      random_tangent       random_bitangent      3   particle_data_1   5   Particles     5       PARTICLES     7         ;   PushConstantData      ;       MATERIAL_INDEX    ;      START_INDEX   =         C   gl_InstanceIndex      N   particle_data_2   Y   particle_position     ^   particle_size     f   half_particle_size    j   particle_normalized_age   x   forward_vector    z   Camera   	 z       WORLD_TO_CLIP_MATRIX     	 z      WORLD_TO_CAMERA_MATRIX    z      PREVIOUS_WORLD_TO_CLIP_MATRIX     z      INVERSE_WORLD_TO_CAMERA_MATRIX    z      INVERSE_CAMERA_TO_CLIP_MATRIX    	 z      CAMERA_WORLD_POSITION    	 z      CAMERA_FORWARD_VECTOR    	 z      CURRENT_FRAME_JITTER      z      NEAR_PLANE    z   	   FAR_PLANE     |         �   tangent_space_matrix      �   param     �   param     �   right_vector      �   up_vector     �   OutWorldPosition      �   gl_VertexIndex    �   OutNormal     �   OutTextureCoordinate      �   gl_PerVertex      �       gl_Position   �      gl_PointSize      �      gl_ClipDistance   �      gl_CullDistance   �         �   TEXTURES      �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �         �   BLUE_NOISE_TEXTURES   �   SKY_TEXTURE   �   SAMPLER G  4         H  5       #       G  5      G  7   "      G  7   !      H  ;       #       H  ;      #      G  ;      G  C      +   H  z          H  z       #       H  z             H  z         H  z      #   @   H  z            H  z         H  z      #   �   H  z            H  z         H  z      #   �   H  z            H  z         H  z      #      H  z            H  z      #   @  H  z      #   P  H  z      #   `  H  z      #   h  H  z   	   #   l  G  z      G  |   "      G  |   !       G  �          G  �      *   G  �         G  �         H  �              H  �            H  �            H  �            G  �      G  �   "       G  �   !       H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      G  �   "       G  �   !      G  �   "       G  �   !      G  �   "      G  �   !           !                               	           
         !     
   	   	   +           �?+     !         1            2      1     4   1     5   4      6      5   ;  6   7        8          +  8   9         :             ;   :   :      <   	   ;   ;  <   =   	   +  8   >         ?   	   :      B      8   ;  B   C      +  :   G      +  :   I          K      1   +  :   U        \            ]      \   +  :   _         `         +     h      ?+     m   ���=+     r   fff?  y   1        z   y   y   y   y   y         \            {      z   ;  {   |      +  8   }         ~            �      
   ,     �   !       !      �         ;  �   �      ;  B   �        �   ;  �   �         �      \   ;  �   �         �           �      U     �   1      �   �      �      �   ;  �   �         �      y      �      1    	 �                            +  :   �        �   �   �      �       �   ;  �   �        
 �   :   :   :   :   :      :   :   +  :   �        �   �   �     �   �      �      �   ;  �   �        �   �   +  :   �         �   �   �      �       �   ;  �   �        	 �                              �   �      �       �   ;  �   �         �      �       �   ;  �   �            R     6               �     ;  2   3      ;  2   N      ;  	   Y      ;  ]   ^      ;  ]   f      ;  `   j      ;  	   x      ;  �   �      ;  	   �      ;  	   �      ;  	   �      ;  	   �           T      A  ?   @   =   >   =  :   A   @   =  8   D   C   |  :   E   D   �  :   F   A   E   �  :   H   F   G   �  :   J   H   I   A  K   L   7   9   J   =  1   M   L   >  3   M        U      A  ?   O   =   >   =  :   P   O   =  8   Q   C   |  :   R   Q   �  :   S   P   R   �  :   T   S   G   �  :   V   T   U   A  K   W   7   9   V   =  1   X   W   >  N   X        V      =  1   Z   3   O     [   Z   Z             >  Y   [        W      A  `   a   3   _   =     b   a   A  `   c   N   I   =     d   c   P  \   e   b   d   >  ^   e        X      =  \   g   ^   �  \   i   g   h   >  f   i        Y      A  `   k   N   U   =     l   k   >  j   l        Z      =     n   j        o      1   !   m   n   =  \   p   f   �  \   q   p   o   >  f   q        [      =     s   j        t      1   r       s   �     u       t   =  \   v   f   �  \   w   v   u   >  f   w        \      A  ~      |   }   =     �      =     �   Y   �     �   �   �        �      E   �   >  x   �        ]      =     �   x   >  �   �   >  �   �   9  
   �      �   �   >  �   �        ^      A  	   �   �   9   =     �   �   >  �   �        _      A  	   �   �   >   =     �   �   >  �   �        `      =     �   Y   >  �   �        a      =     �   �   A  `   �   f   I   =     �   �        �   �   A  `   �   f   I   =     �   �   =  8   �   �   �  8   �   �   >   o     �   �        �      .   �   �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �        b      =     �   �   A  `   �   f   U   =     �   �        �   �   A  `   �   f   U   =     �   �   =  8   �   �   �  �   �   �   >   �     �   �       !        �      .   �   �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �        c      =     �   x   >  �   �        d      =  8   �   �   �  �   �   �   >   �     �   �       !   A  �   �   �   I   >  �   �        e      =  8   �   �   �  8   �   �   >   o     �   �   �     �       �   A  �   �   �   U   >  �   �        f      A  �   �   |   9   =  y   �   �   =     �   �   Q     �   �       Q     �   �      Q     �   �      P  1   �   �   �   �       �  1   �   �   �   A  �   �   �   9   >  �   �   �  8       8  F   6  
             7  	      7  	      �     ;  	         ;  	              :      =           =           =           =           �              �              �                         E      >             ;      =           =                      D         >             =      =           =           =           Q     "          Q     #         Q     $         Q     %          Q     &         Q     '         Q     (          Q     )         Q     *         P     +   "   #   $   P     ,   %   &   '   P     -   (   )   *   P  
   .   +   ,   -   �  .   8  �T      #     J                GLSL.std.450                     main    �        +  ,  /  7  9               ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Particles_Fragment.glsl  w   �     // OpModuleProcessed entry-point main
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

layout (std430, set = 1, binding = 1) buffer Particles
{
	layout (offset = 0) vec4[] PARTICLES;
};

layout (set = 1, binding = 2) uniform sampler SAMPLER;

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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint MATERIAL_INDEX;
	layout (offset = 4) uint START_INDEX;
};

layout (location = 0) in vec3 InWorldPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec2 InTextureCoordinate;

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
    if (opacity < 0.5f)
    {
        discard;
    }
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(albedo_thickness.rgb,albedo_thickness.a*opacity);
	SceneFeatures2 = vec4(InNormal,gl_FragCoord.z);
	SceneFeatures3 = material_properties;
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       unpacked      @   view_space_position   B   Camera   	 B       WORLD_TO_CLIP_MATRIX     	 B      WORLD_TO_CAMERA_MATRIX    B      PREVIOUS_WORLD_TO_CLIP_MATRIX     B      INVERSE_WORLD_TO_CAMERA_MATRIX    B      INVERSE_CAMERA_TO_CLIP_MATRIX    	 B      CAMERA_WORLD_POSITION    	 B      CAMERA_FORWARD_VECTOR    	 B      CURRENT_FRAME_JITTER      B      NEAR_PLANE    B   	   FAR_PLANE     D         P   denominator   d   view_space_position   n   denominator   �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �         �   PushConstantData      �       MATERIAL_INDEX    �      START_INDEX   �         �   albedo_thickness      �   TEXTURES      �   SAMPLER   �   InTextureCoordinate   �   param     �   normal_map_displacement   �   param     �   material_properties   �   param     �   opacity     param       velocity        InWorldPosition     param       param        SceneFeatures1    +  SceneFeatures2    ,  InNormal      /  gl_FragCoord      7  SceneFeatures3    9  SceneFeatures4    A  BLUE_NOISE_TEXTURES   E  SKY_TEXTURE   G  Particles     G      PARTICLES     I      H  B          H  B       #       H  B             H  B         H  B      #   @   H  B            H  B         H  B      #   �   H  B            H  B         H  B      #   �   H  B            H  B         H  B      #      H  B            H  B      #   @  H  B      #   P  H  B      #   `  H  B      #   h  H  B   	   #   l  G  B      G  D   "      G  D   !       H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      H  �       #       H  �      #      G  �      G  �   "       G  �   !       G  �   "      G  �   !      G  �         G           G            G  +        G  ,        G  /        G  7        G  9        G  A  "       G  A  !      G  E  "       G  E  !      G  F        H  G      #       G  G     G  I  "      G  I  !           !                                   	         
   	      !     
           	                       	      !                    
   +        �   +  	        C+     !          "      	     %          +  %   &      +     +      +  %   .      +     3      +  %   6      +     ;        A   
        B   A   A   A   A   A            	   	      C      B   ;  C   D      +  %   E          F      A   +  	   J     �?+  	   ^      ?+  %   e       
 �                  	         +     �        �   �   �     �   �      �      �   ;  �   �        �            �   	   �   ;  �   �   	      �   	         �         +     �        �    	 �   	                         +     �        �   �   �      �       �   ;  �   �       +  %   �         �       �     �      �       �   ;  �   �         �   �      �         ;  �   �      +     �      +     �       +  %   �      +     �   @   +  %   �                            ;         +  %                           
   ;          ;    +     ;    ,        .     
   ;  .  /        0     	   ;    7     ;    9     +  	   ;        ?  �   �      @      ?  ;  @  A       	 B  	                           C  B     D      C  ;  D  E        F  
     G  F     H     G  ;  H  I          W     6               �     ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;  "   �      ;          ;         ;          ;               Z      A  �   �   �   E   =     �   �   A  �   �   �   E   �   E   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        Z      A  �   �   �   E   =     �   �   A  �   �   �   E   �   �   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �   �   W  
   �   �   �   >  �   �   �  �   �  �        Z      A  �   �   �   E   =     �   �   A  �   �   �   E   �   �   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        \      A  �   �   �   E   =     �   �   A  �   �   �   E   �   E   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        \      A  �   �   �   E   =     �   �   A  �   �   �   E   �   e   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �   �   W  
   �   �   �   >  �   �   �  �   �  �        \      A  �   �   �   E   =     �   �   A  �   �   �   E   �   e   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        ^      A  �   �   �   E   =     �   �   A  �   �   �   E   �   E   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        ^      A  �   �   �   E   =     �   �   A  �   �   �   E   �   �   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �   �   W  
   �   �   �   >  �   �   �  �   �  �        ^      A  �   �   �   E   =     �   �   A  �   �   �   E   �   �   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        `      A  �   �   �   E   =     �   �   A  �   �   �   E   �   E   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �     �  �        `      A  �   �   �   E   =     �   �   A  �   �   �   E   �   �   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   =     �   �   W  
      �   �   Q  	            >  �     �  �   �         `      A  �     �   E   =         A  �     �   E     �   =         >      9  
          Q  	   	        >  �   	  �  �   �  �        a      =  	   
  �   �  �     
  ^   �        �        �         c      �  �         e      =         >      9            =         >      9            �           A      D     =         �           >           f      =  
   !  �   O     "  !  !            A  "   #  �   ;   =  	   $  #  =  	   %  �   �  	   &  $  %  Q  	   '  "      Q  	   (  "     Q  	   )  "     P  
   *  '  (  )  &  >     *       g      =     -  ,  A  0  1  /  3   =  	   2  1  Q  	   3  -      Q  	   4  -     Q  	   5  -     P  
   6  3  4  5  2  >  +  6       h      =  
   8  �   >  7  8       i      =     :    Q  	   <  :      Q  	   =  :     P  
   >  <  =  ;  ;  >  9  >  �  8       �      6  
             7        �     ;                �       =           �              p  	         �  	             A  "   #      !   >  #            �       =     $      �     '   $   &   �     (   '      p  	   )   (   �  	   *   )      A  "   ,      +   >  ,   *        �       =     -      �     /   -   .   �     0   /      p  	   1   0   �  	   2   1      A  "   4      3   >  4   2        �       =     5      �     7   5   6   �     8   7      p  	   9   8   �  	   :   9      A  "   <      ;   >  <   :        �       =  
   =      �  =   8         :   6               7        �     ;     @      ;  "   P                 A  F   G   D   E   =  A   H   G   =     I      Q  	   K   I       Q  	   L   I      Q  	   M   I      P  
   N   K   L   M   J   �  
   O   H   N   >  @   O              A  "   Q   @   ;   =  	   R   Q   �  	   S   J   R   >  P   S              =  	   T   P   =  
   U   @   O     V   U   U          �     W   V   T   A  "   X   @   !   Q  	   Y   W       >  X   Y   A  "   Z   @   +   Q  	   [   W      >  Z   [              =  
   \   @   O     ]   \   \          �     _   ]   ^   P     `   ^   ^   �     a   _   `   �  a   8         ;   6               7        �     ;     d      ;  "   n                 A  F   f   D   e   =  A   g   f   =     h      Q  	   i   h       Q  	   j   h      Q  	   k   h      P  
   l   i   j   k   J   �  
   m   g   l   >  d   m              A  "   o   d   ;   =  	   p   o   �  	   q   J   p   >  n   q              =  	   r   n   =  
   s   d   O     t   s   s          �     u   t   r   A  "   v   d   !   Q  	   w   u       >  v   w   A  "   x   d   +   Q  	   y   u      >  x   y              =  
   z   d   O     {   z   z          �     |   {   ^   P     }   ^   ^   �     ~   |   }   �  ~   8                          ��KT��D	       �}�l�!�                       ��v)��%�9����Y��G� �z��.v<             	                �}�l�!�