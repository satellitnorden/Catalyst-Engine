�nX/J1�dsTwCInstancedModelDepthSingleSided_RenderPipeline                    �e      #     +                GLSL.std.450                      main    �   �   �   �   �      *       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\InstancedModelDepthSingleSided_Vertex.glsl   <   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec4 PREVIOUS_WIND_DIRECTION_SPEED;
	layout (offset = 16) vec4 CURRENT_WIND_DIRECTION_SPEED;
	layout (offset = 32) float PREVIOUS_WIND_TIME;
	layout (offset = 36) float CURRENT_WIND_TIME;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

//Constants.
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_1 	(1 << 0)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_2 	(1 << 1)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_3 	(1 << 2)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_4 	(1 << 3)
#define MODEL_FLAG_IS_VEGETATION 				(1 << 4)

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
*	Calculates wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculateWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal, vec4 wind_direction_speed, float wind_time)
{
	//Calculate the wind influence at this point.
	float wind_influence = 0.0f;

	{
		float amplitude = 1.0f;
		float frequency = 1.0f;
		float vertex_influence = 0.0f;

		for (uint i = 0; i < 4; ++i)
		{
			wind_influence += (sin((world_position.x + world_position.y + world_position.z + ((vertex_position.x + vertex_position.y + vertex_position.z) * vertex_influence) + wind_time) * wind_direction_speed.w * frequency) * 0.5f + 0.5f) * amplitude;

			amplitude *= 0.5f;
			frequency *= 2.0f;
			vertex_influence += 0.25f;
		}
	}

	//Calculate the displacement.
	vec3 displacement;
	displacement.xz = wind_direction_speed.xz * wind_direction_speed.w * wind_influence;
	displacement.y = -wind_influence * 0.5f;

	//Modify the displacement so it doesn't affect the bottom of the mesh.
	displacement *= max(vertex_position.y * 0.25f, 0.0f);

	//Return the displacement.
	return displacement;
}

/*
*	Calculates previous wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculatePreviousWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	return CalculateWindDisplacement(world_position, vertex_position, normal, PREVIOUS_WIND_DIRECTION_SPEED, PREVIOUS_WIND_TIME);
}

/*
*	Calculates current wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculateCurrentWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	return CalculateWindDisplacement(world_position, vertex_position, normal, CURRENT_WIND_DIRECTION_SPEED, CURRENT_WIND_TIME);
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
	layout (offset = 16) uint MODEL_FLAGS;
	layout (offset = 20) float START_FADE_OUT_DISTANCE_SQUARED;
	layout (offset = 24) float END_FADE_OUT_DISTANCE_SQUARED;
	layout (offset = 28) uint MATERIAL_INDEX;
};

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec2 InTextureCoordinate;
layout (location = 3) in mat4 InTransformation;

layout (location = 0) out vec2 OutTextureCoordinate;
layout (location = 1) out float OutFadeOpacity;

void main()
{
    OutTextureCoordinate = InTextureCoordinate;
    vec3 world_position = vec3(InTransformation * vec4(InPosition, 1.0f)) + WORLD_GRID_DELTA;
    if (START_FADE_OUT_DISTANCE_SQUARED < FLOAT32_MAXIMUM)
	{
		OutFadeOpacity = 1.0f - (clamp(LengthSquared3(world_position - CAMERA_WORLD_POSITION) - START_FADE_OUT_DISTANCE_SQUARED, 0.0f, END_FADE_OUT_DISTANCE_SQUARED - START_FADE_OUT_DISTANCE_SQUARED) / (END_FADE_OUT_DISTANCE_SQUARED - START_FADE_OUT_DISTANCE_SQUARED));
	}
	else
	{
		OutFadeOpacity = 1.0f;
	}
    if (TEST_BIT(MODEL_FLAGS, MODEL_FLAG_IS_VEGETATION))
    {
        world_position += CalculateCurrentWindDisplacement(InTransformation[3].xyz, InPosition, vec3(0.0f));
    }
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(world_position,1.0f);
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         LengthSquared3(vf3;      vector       CalculateWindDisplacement(vf3;vf3;vf3;vf4;f1;        world_position       vertex_position      normal       wind_direction_speed         wind_time        CalculateCurrentWindDisplacement(vf3;vf3;vf3;        world_position       vertex_position      normal    6   wind_influence    8   amplitude     :   frequency     ;   vertex_influence      =   i     v   displacement      �   Wind      �       PREVIOUS_WIND_DIRECTION_SPEED     �      CURRENT_WIND_DIRECTION_SPEED      �      PREVIOUS_WIND_TIME    �      CURRENT_WIND_TIME     �         �   param     �   param     �   param     �   param     �   param     �   OutTextureCoordinate      �   InTextureCoordinate   �   world_position    �   InTransformation      �   InPosition    �   PushConstantData      �       WORLD_GRID_DELTA      �      MODEL_FLAGS   �      START_FADE_OUT_DISTANCE_SQUARED   �      END_FADE_OUT_DISTANCE_SQUARED     �      MATERIAL_INDEX    �         �   OutFadeOpacity    �   Camera   	 �       WORLD_TO_CLIP_MATRIX     	 �      WORLD_TO_CAMERA_MATRIX    �      PREVIOUS_WORLD_TO_CLIP_MATRIX     �      INVERSE_WORLD_TO_CAMERA_MATRIX    �      INVERSE_CAMERA_TO_CLIP_MATRIX    	 �      CAMERA_WORLD_POSITION    	 �      CAMERA_FORWARD_VECTOR    	 �      CURRENT_FRAME_JITTER      �      NEAR_PLANE    �   	   FAR_PLANE     �         �   param     �   param     �   param     �   param     �   gl_PerVertex      �       gl_Position   �      gl_PointSize      �      gl_ClipDistance   �      gl_CullDistance              TEXTURES     	   GlobalTextureAverageValues   	       TEXTURE_AVERAGE_VALUES              Material            _Properties        _AlbedoThickness     	      _NormalMapDisplacement         _MaterialProperties        _Opacity           _EmissiveMultiplier        _Padding1          _Padding2       GlobalMaterials         MATERIALS               BLUE_NOISE_TEXTURES   #  SKY_TEXTURE   $  General  	 $      FULL_MAIN_RESOLUTION      $     INVERSE_FULL_MAIN_RESOLUTION     	 $     HALF_MAIN_RESOLUTION      $     INVERSE_HALF_MAIN_RESOLUTION      $     FRAME     $     VIEW_DISTANCE     &        )  SAMPLER   *  InNormal    H  �       #       H  �      #      H  �      #       H  �      #   $   G  �      G  �   "      G  �   !      G  �          G  �         G  �         G  �          H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      G  �      G  �         H  �          H  �       #       H  �             H  �         H  �      #   @   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #      H  �            H  �      #   @  H  �      #   P  H  �      #   `  H  �      #   h  H  �   	   #   l  G  �      G  �   "      G  �   !       H  �              H  �            H  �            H  �            G  �      G    "       G    !       G          H        #       G       G    "       G    !      H        #       H       #      H       #      H       #      H       #      H       #      H       #      H       #      G           H        #       G       G    "       G    !      G    "       G    !      G  #  "       G  #  !      H  $      #       H  $     #      H  $     #      H  $     #      H  $     #       H  $     #   $   G  $     G  &  "      G  &  !      G  )  "      G  )  !      G  *             !                               	         !  
      	                                      !        	   	   	         !        	   	   	                +             +     &      +     -      +     7       +     9     �?   <         +     D        E   +     \      +     c      ?+     l      @+     o     �>  s          +  s   t        w           �                  �      �   ;  �   �      +  s   �         �            �            �      w   ;  �   �         �      w   ;  �   �        �            �      �   ;  �   �         �         ;  �   �        �                     �   	   �   ;  �   �   	   +  s   �          �   	      +  s   �         �   	      +     �   ��   �         ;  �   �        �   �   �   �   �   �         w            �      �   ;  �   �      +  s   �         �            �   	      +     �      ,     �   7   7   7      �           �      &     �         �   �      �      �   ;  �                 �      
         	                            +                             ;                                   ;          
                           +                                  ;               +                              ;           	                               !        "      !  ;  "  #        $  w   w   w   w            %     $  ;  %  &       '     (      '  ;  (  )      ;  �   *          �     6               �     ;  	   �      ;  	   �      ;  	   �      ;  	   �      ;  	   �           �      =  w   �   �   >  �   �        �      =  �   �   �   =     �   �   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   9   �     �   �   �   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   A  �   �   �   �   =     �   �   �     �   �   �   >  �   �        �      A  �   �   �   �   =     �   �   �  E   �   �   �   �  �       �  �   �   �   �  �        �      =     �   �   A  �   �   �   �   =     �   �   �     �   �   �   >  �   �   9     �      �   A  �   �   �   �   =     �   �   �     �   �   �   A  �   �   �   �   =     �   �   A  �   �   �   �   =     �   �   �     �   �   �        �      +   �   7   �   A  �   �   �   �   =     �   �   A  �   �   �   �   =     �   �   �     �   �   �   �     �   �   �   �     �   9   �   >  �   �   �  �   �  �        �      >  �   9   �  �   �  �        �      A  �   �   �   t   =     �   �   �     �   �   �   �  E   �   �   �   �  �       �  �   �   �   �  �        �      A  �   �   �   �   =     �   �   O     �   �   �             >  �   �   =     �   �   >  �   �   >  �   �   9     �      �   �   �   =     �   �   �     �   �   �   >  �   �   �  �   �  �        �      A      �   �   =  �       =       �   Q             Q            Q            P             9   �     	      A  
       �   >    	  �  8       �   !   6            
   7  	      �          �       A     !          =     "   !   A     #          =     $   #   �     %   "   $   A     '      &   =     (   '   A     )      &   =     *   )   �     +   (   *   �     ,   %   +   A     .      -   =     /   .   A     0      -   =     1   0   �     2   /   1   �     3   ,   2   �  3   8       �  �   6               7  	      7  	      7  	      7        7        �     ;     6      ;     8      ;     :      ;     ;      ;  <   =      ;  	   v           �      >  6   7        �      >  8   9        �      >  :   9        �      >  ;   7        �      >  =       �  >   �  >        �      �  @   A       �  B   �  B        �      =     C   =   �  E   F   C   D   �  F   ?   @   �  ?        �      A     G          =     H   G   A     I      &   =     J   I   �     K   H   J   A     L      -   =     M   L   �     N   K   M   A     O          =     P   O   A     Q      &   =     R   Q   �     S   P   R   A     T      -   =     U   T   �     V   S   U   =     W   ;   �     X   V   W   �     Y   N   X   =     Z      �     [   Y   Z   A     ]      \   =     ^   ]   �     _   [   ^   =     `   :   �     a   _   `        b         a   �     d   b   c   �     e   d   c   =     f   8   �     g   e   f   =     h   6   �     i   h   g   >  6   i        �      =     j   8   �     k   j   c   >  8   k        �      =     m   :   �     n   m   l   >  :   n        �      =     p   ;   �     q   p   o   >  ;   q   �  A   �  A        �      =     r   =   �     u   r   t   >  =   u   �  >   �  @        �      =     x      O  w   y   x   x          A     z      \   =     {   z   �  w   |   y   {   =     }   6   �  w   ~   |   }   A        v       Q     �   ~       >     �   A     �   v   -   Q     �   ~      >  �   �        �      =     �   6        �   �   �     �   �   c   A     �   v   &   >  �   �        �      A     �      &   =     �   �   �     �   �   o        �      (   �   7   =     �   v   �     �   �   �   >  v   �        �      =     �   v   �  �   8       �  ]   6               7  	      7  	      7  	      �     ;  	   �      ;  	   �      ;  	   �      ;     �      ;     �           �      =     �      >  �   �   =     �      >  �   �   =     �      >  �   �   A  �   �   �   t   =     �   �   >  �   �   A  �   �   �   �   =     �   �   >  �   �   9 	    �      �   �   �   �   �   �  �   8  LY      #     �                 GLSL.std.450                     main    �   �   �                 ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\InstancedModelDepthSingleSided_Fragment.glsl     �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform Wind
{
	layout (offset = 0) vec4 PREVIOUS_WIND_DIRECTION_SPEED;
	layout (offset = 16) vec4 CURRENT_WIND_DIRECTION_SPEED;
	layout (offset = 32) float PREVIOUS_WIND_TIME;
	layout (offset = 36) float CURRENT_WIND_TIME;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

//Constants.
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_1 	(1 << 0)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_2 	(1 << 1)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_3 	(1 << 2)
#define MODEL_FLAG_INCLUDE_IN_SHADOW_CASCADE_4 	(1 << 3)
#define MODEL_FLAG_IS_VEGETATION 				(1 << 4)

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
*	Calculates wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculateWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal, vec4 wind_direction_speed, float wind_time)
{
	//Calculate the wind influence at this point.
	float wind_influence = 0.0f;

	{
		float amplitude = 1.0f;
		float frequency = 1.0f;
		float vertex_influence = 0.0f;

		for (uint i = 0; i < 4; ++i)
		{
			wind_influence += (sin((world_position.x + world_position.y + world_position.z + ((vertex_position.x + vertex_position.y + vertex_position.z) * vertex_influence) + wind_time) * wind_direction_speed.w * frequency) * 0.5f + 0.5f) * amplitude;

			amplitude *= 0.5f;
			frequency *= 2.0f;
			vertex_influence += 0.25f;
		}
	}

	//Calculate the displacement.
	vec3 displacement;
	displacement.xz = wind_direction_speed.xz * wind_direction_speed.w * wind_influence;
	displacement.y = -wind_influence * 0.5f;

	//Modify the displacement so it doesn't affect the bottom of the mesh.
	displacement *= max(vertex_position.y * 0.25f, 0.0f);

	//Return the displacement.
	return displacement;
}

/*
*	Calculates previous wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculatePreviousWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	return CalculateWindDisplacement(world_position, vertex_position, normal, PREVIOUS_WIND_DIRECTION_SPEED, PREVIOUS_WIND_TIME);
}

/*
*	Calculates current wind displacement.
*	Requires the Wind uniform buffer to be bound.
*/
vec3 CalculateCurrentWindDisplacement(vec3 world_position, vec3 vertex_position, vec3 normal)
{
	return CalculateWindDisplacement(world_position, vertex_position, normal, CURRENT_WIND_DIRECTION_SPEED, CURRENT_WIND_TIME);
}

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
	layout (offset = 16) uint MODEL_FLAGS;
	layout (offset = 20) float START_FADE_OUT_DISTANCE_SQUARED;
	layout (offset = 24) float END_FADE_OUT_DISTANCE_SQUARED;
	layout (offset = 28) uint MATERIAL_INDEX;
};

layout (location = 0) in vec2 InTextureCoordinate;
layout (location = 1) in float InFadeOpacity;

void main()
{
    float opacity = 1.0f;
    if (TEST_BIT(MATERIALS[MATERIAL_INDEX]._Properties, MATERIAL_PROPERTY_TYPE_MASKED))
    {
        EVALUATE_OPACITY(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, opacity);
    }
    float noise_value = InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME);
    if (opacity < 0.5f
    || (InFadeOpacity < 1.0f && InFadeOpacity < noise_value))
    {
        discard;
    }
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        InterleavedGradientNoise(vu2;u1;         coordinate       frame        unpacked      @   x     I   y     _   opacity   `   Material      `       _Properties   `      _AlbedoThickness     	 `      _NormalMapDisplacement    `      _MaterialProperties   `      _Opacity      `      _EmissiveMultiplier   `      _Padding1     `      _Padding2     c   GlobalMaterials   c       MATERIALS     e         h   PushConstantData      h       WORLD_GRID_DELTA      h      MODEL_FLAGS   h      START_FADE_OUT_DISTANCE_SQUARED   h      END_FADE_OUT_DISTANCE_SQUARED     h      MATERIAL_INDEX    j         �   TEXTURES      �   SAMPLER   �   InTextureCoordinate   �   param     �   noise_value   �   gl_FragCoord      �   General  	 �       FULL_MAIN_RESOLUTION      �      INVERSE_FULL_MAIN_RESOLUTION     	 �      HALF_MAIN_RESOLUTION      �      INVERSE_HALF_MAIN_RESOLUTION      �      FRAME     �      VIEW_DISTANCE     �         �   param     �   param     �   InFadeOpacity    	 �   GlobalTextureAverageValues   	 �       TEXTURE_AVERAGE_VALUES    �         �   BLUE_NOISE_TEXTURES   �   SKY_TEXTURE   �   Camera   	 �       WORLD_TO_CLIP_MATRIX     	 �      WORLD_TO_CAMERA_MATRIX    �      PREVIOUS_WORLD_TO_CLIP_MATRIX     �      INVERSE_WORLD_TO_CAMERA_MATRIX    �      INVERSE_CAMERA_TO_CLIP_MATRIX    	 �      CAMERA_WORLD_POSITION    	 �      CAMERA_FORWARD_VECTOR    	 �      CURRENT_FRAME_JITTER      �      NEAR_PLANE    �   	   FAR_PLANE     �         �   Wind      �       PREVIOUS_WIND_DIRECTION_SPEED     �      CURRENT_WIND_DIRECTION_SPEED      �      PREVIOUS_WIND_TIME    �      CURRENT_WIND_TIME     �       H  `       #       H  `      #      H  `      #      H  `      #      H  `      #      H  `      #      H  `      #      H  `      #      G  b          H  c       #       G  c      G  e   "       G  e   !      H  h       #       H  h      #      H  h      #      H  h      #      H  h      #      G  h      G  �   "       G  �   !       G  �   "      G  �   !      G  �          G  �         H  �       #       H  �      #      H  �      #      H  �      #      H  �      #       H  �      #   $   G  �      G  �   "      G  �   !      G  �         G  �         H  �       #       G  �      G  �   "       G  �   !      G  �   "       G  �   !      G  �   "       G  �   !      H  �          H  �       #       H  �             H  �         H  �      #   @   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #   �   H  �            H  �         H  �      #      H  �            H  �      #   @  H  �      #   P  H  �      #   `  H  �      #   h  H  �   	   #   l  G  �      G  �   "      G  �   !       H  �       #       H  �      #      H  �      #       H  �      #   $   G  �      G  �   "      G  �   !           !                                   	         
   	      !     
                             !     	                  
   +        �   +  	        C+                     	     "          +  "   #      +     (      +  "   +      +     0      +  "   3      +     8      +     >   @   +  	   D   �Ҳ@+  	   Q   ��SB+  	   R   Cq�=+  	   U   �E�;+  	   Y     �? 
 `                  	         +     a        b   `   a     c   b      d      c   ;  d   e      +  "   f         g   	        h   g      	   	         i   	   h   ;  i   j   	   +  "   k         l   	         o           s    	    	                         +     �        �      �      �       �   ;  �   �          �            �      �       �   ;  �   �         �        �   	         �      �   ;  �   �         �      
   ;  �   �        �   �   �   �   �      	      �      �   ;  �   �      +  	   �      ?   �      	   ;  �   �        �   
   �     �   �      �      �   ;  �   �      +     �         �   �   �      �       �   ;  �   �        	 �   	                           �   �      �       �   ;  �   �         �   
        �   �   �   �   �   �   g   g   �   	   	      �      �   ;  �   �        �   
   
   	   	      �      �   ;  �   �           �     6               �     ;     _      ;     �      ;     �      ;     �      ;     �           �      >  _   Y        �      A  l   m   j   k   =     n   m   A  o   p   e   f   n   f   =     q   p   �     r   q   (   �  s   t   r   (   �  v       �  t   u   v   �  u        �      A  l   w   j   k   =     x   w   A  o   y   e   f   x   f   =     z   y   �     {   z   >   �  s   |   {   >   �  ~       �  |   }   �   �  }        �      A  l   �   j   k   =     �   �   A  o   �   e   f   �   k   =     �   �   A  �   �   �   �   =     �   �   =  �   �   �   V  �   �   �   �   =  �   �   �   W  
   �   �   �   Q  	   �   �       >  _   �   �  ~   �  �        �      A  l   �   j   k   =     �   �   A  o   �   e   f   �   k   =     �   �   >  �   �   9  
   �      �   Q  	   �   �       >  _   �   �  ~   �  ~   �  v   �  v        �      =  
   �   �   O  �   �   �   �          m     �   �   >  �   �   A  o   �   �   k   =     �   �   >  �   �   9  	   �      �   �   >  �   �        �      =  	   �   _   �  s   �   �   �   �  s   �   �   �  �       �  �   �   �   �  �        �      =  	   �   �   �  s   �   �   Y   �  �       �  �   �   �   �  �        �      =  	   �   �   =  	   �   �   �  s   �   �   �   �  �   �  �   �  s   �   �   �   �   �   �  �   �  �   �  s   �   �   v   �   �   �  �       �  �   �   �   �  �        �      �  �  �   �  8       �      6  
             7        �     ;                      =           �              p  	         �  	            A               >                    =     !      �     $   !   #   �     %   $      p  	   &   %   �  	   '   &      A     )      (   >  )   '              =     *      �     ,   *   +   �     -   ,      p  	   .   -   �  	   /   .      A     1      0   >  1   /              =     2      �     4   2   3   �     5   4      p  	   6   5   �  	   7   6      A     9      8   >  9   7              =  
   :      �  :   8       �  <   6  	             7        7        �     ;     @      ;     I           �      =     =      �     ?   =   >   >     ?        �      A     A         =     B   A   p  	   C   B   =     E      p  	   F   E   �  	   G   D   F   �  	   H   C   G   >  @   H        �      A     J      (   =     K   J   p  	   L   K   =     M      p  	   N   M   �  	   O   D   N   �  	   P   L   O   >  I   P        �      =  	   S   @   �  	   T   R   S   =  	   V   I   �  	   W   U   V   �  	   X   T   W   �  	   Z   X   Y   �  	   [   Q   Z   �  	   \   [   Y   �  \   8                          ��KT��D	(�D��^��:"@Gs                                                             �=�H=c�