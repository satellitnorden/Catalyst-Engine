ïnX/J1Â!ÍÔ¶¢=InstancedImpostorDepth_RenderPipeline                            XT      #     Ä                 GLSL.std.450              
        main       *   i              ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\InstancedImpostorDepth_Vertex.glsl   u   Ì     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 2) uniform sampler SAMPLER;

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
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
	layout (offset = 16) float HALF_WIDTH;
	layout (offset = 20) float WHOLE_WIDTH;
	layout (offset = 24) float HEIGHT;
	layout (offset = 28) uint MATERIAL_INDEX;
	layout (offset = 32) float START_FADE_IN_DISTANCE;
	layout (offset = 36) float END_FADE_IN_DISTANCE;
	layout (offset = 40) float START_FADE_OUT_DISTANCE;
	layout (offset = 44) float END_FADE_OUT_DISTANCE;
};

layout (location = 0) in vec3 InPosition;

layout (location = 0) out vec2 OutTextureCoordinate;
layout (location = 1) out float OutFadeOpacity;

void main()
{
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);
    vec3 world_position = InPosition + WORLD_GRID_DELTA;
    vec3 forward_vector = CAMERA_WORLD_POSITION - world_position;
    forward_vector.y = 0.0f;
    float distance_to_camera = length(forward_vector);
    float distance_to_camera_reciprocal = 1.0f / distance_to_camera;
    forward_vector *= distance_to_camera_reciprocal;
    vec3 right_vector = cross(forward_vector, vec3(0.0f, 1.0f, 0.0f));
    world_position += (right_vector * HALF_WIDTH + -right_vector * WHOLE_WIDTH * x);
    world_position.y += HEIGHT * y;
    OutTextureCoordinate.x = x;
    OutTextureCoordinate.y = 1.0f - y;
    float fade_in_range = END_FADE_IN_DISTANCE - START_FADE_IN_DISTANCE;
    float fade_in = clamp(distance_to_camera - START_FADE_IN_DISTANCE, 0.0f, fade_in_range) / fade_in_range;
    float fade_out_range = END_FADE_OUT_DISTANCE - START_FADE_OUT_DISTANCE;
    float fade_out = 1.0f - (clamp(distance_to_camera - START_FADE_OUT_DISTANCE, 0.0f, fade_out_range) / fade_out_range);
    OutFadeOpacity = min(fade_in, fade_out);
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(world_position,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y     (   world_position    *   InPosition    -   PushConstantData      -       WORLD_GRID_DELTA      -      HALF_WIDTH    -      WHOLE_WIDTH   -      HEIGHT    -      MATERIAL_INDEX   	 -      START_FADE_IN_DISTANCE   	 -      END_FADE_IN_DISTANCE     	 -      START_FADE_OUT_DISTANCE  	 -      END_FADE_OUT_DISTANCE     /         5   forward_vector    9   Camera   	 9       WORLD_TO_CLIP_MATRIX     	 9      WORLD_TO_CAMERA_MATRIX    9      PREVIOUS_WORLD_TO_CLIP_MATRIX     9      INVERSE_WORLD_TO_CAMERA_MATRIX    9      INVERSE_CAMERA_TO_CLIP_MATRIX    	 9      CAMERA_WORLD_POSITION    	 9      CAMERA_FORWARD_VECTOR    	 9      CURRENT_FRAME_JITTER      9      NEAR_PLANE    9   	   FAR_PLANE     ;         D   distance_to_camera   
 G   distance_to_camera_reciprocal     M   right_vector      i   OutTextureCoordinate      q   fade_in_range     x   fade_in      fade_out_range       fade_out         OutFadeOpacity       gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance            ª   TEXTURES     	 ¬   GlobalTextureAverageValues   	 ¬       TEXTURE_AVERAGE_VALUES    ®         ¯   Material      ¯       _Properties   ¯      _AlbedoThickness     	 ¯      _NormalMapDisplacement    ¯      _MaterialProperties   ¯      _Opacity      ¯      _EmissiveMultiplier   ¯      _Padding1     ¯      _Padding2     ²   GlobalMaterials   ²       MATERIALS     ´         ¹   BLUE_NOISE_TEXTURES   ½   SKY_TEXTURE   ¾   General  	 ¾       FULL_MAIN_RESOLUTION      ¾      INVERSE_FULL_MAIN_RESOLUTION     	 ¾      HALF_MAIN_RESOLUTION      ¾      INVERSE_HALF_MAIN_RESOLUTION      ¾      FRAME     ¾      VIEW_DISTANCE     À         Ã   SAMPLER G        *   G  *          H  -       #       H  -      #      H  -      #      H  -      #      H  -      #      H  -      #       H  -      #   $   H  -      #   (   H  -      #   ,   G  -      H  9          H  9       #       H  9             H  9         H  9      #   @   H  9            H  9         H  9      #      H  9            H  9         H  9      #   À   H  9            H  9         H  9      #      H  9            H  9      #   @  H  9      #   P  H  9      #   `  H  9      #   h  H  9   	   #   l  G  9      G  ;   "      G  ;   !       G  i          G           H                H              H              H              G        G  ª   "       G  ª   !       G  «         H  ¬       #       G  ¬      G  ®   "       G  ®   !      H  ¯       #       H  ¯      #      H  ¯      #      H  ¯      #      H  ¯      #      H  ¯      #      H  ¯      #      H  ¯      #      G  ±          H  ²       #       G  ²      G  ´   "       G  ´   !      G  ¹   "       G  ¹   !      G  ½   "       G  ½   !      H  ¾       #       H  ¾      #      H  ¾      #      H  ¾      #      H  ¾      #       H  ¾      #   $   G  ¾      G  À   "      G  À   !      G  Ã   "      G  Ã   !           !                               
                           ;           +           +           +            +          ?+     "        &            '      &      )      &   ;  )   *        ,             -   &            ,                  .   	   -   ;  .   /   	   +     0          1   	   &     6           7   6        8           9   7   7   7   7   7   &   &   8            :      9   ;  :   ;      +     <         =      &   +  ,   B      ,  &   O               R   	         h      8   ;  h   i      +  ,   k          l         +     r      +           +           ;  l                 B        6                        ;                    7      ¤      6    	 ¦                            +  ,   §        ¨   ¦   §      ©       ¨   ;  ©   ª         «   6   §     ¬   «      ­      ¬   ;  ­   ®       
 ¯   ,   ,   ,   ,   ,      ,   ,   +  ,   °        ±   ¯   °     ²   ±      ³      ²   ;  ³   ´        µ   ¦   +  ,   ¶         ·   µ   ¶      ¸       ·   ;  ¸   ¹        	 º                              »   º      ¼       »   ;  ¼   ½         ¾   8   8   8   8   ,         ¿      ¾   ;  ¿   À        Á      Â       Á   ;  Â   Ã                 6               ø     ;     	      ;           ;  '   (      ;  '   5      ;     D      ;     G      ;  '   M      ;     q      ;     x      ;           ;                      =           ª  
            ¨  
         ÷         ú           ø                =           ª  
            ù     ø     õ  
                  ©                            >  	                 =           ª  
            ¨  
         ÷          ú            ø                =     !      ª  
   #   !   "   ù      ø      õ  
   $         #      ©     %   $                    >     %              =  &   +   *   A  1   2   /   0   =  &   3   2     &   4   +   3   >  (   4              A  =   >   ;   <   =  &   ?   >   =  &   @   (     &   A   ?   @   >  5   A              A     C   5   B   >  C                 =  &   E   5        F      B   E   >  D   F              =     H   D        I      H   >  G   I              =     J   G   =  &   K   5     &   L   K   J   >  5   L              =  &   N   5     &   P      D   N   O   >  M   P              =  &   Q   M   A  R   S   /      =     T   S     &   U   Q   T   =  &   V   M     &   W   V   A  R   X   /      =     Y   X     &   Z   W   Y   =     [   	     &   \   Z   [     &   ]   U   \   =  &   ^   (     &   _   ^   ]   >  (   _              A  R   `   /   "   =     a   `   =     b           c   a   b   A     d   (   B   =     e   d        f   e   c   A     g   (   B   >  g   f              =     j   	   A  l   m   i   k   >  m   j              =     n           o      n   A  l   p   i   B   >  p   o              A  R   s   /   r   =     t   s   A  R   u   /   <   =     v   u        w   t   v   >  q   w              =     y   D   A  R   z   /   <   =     {   z        |   y   {   =     }   q        ~      +   |      }   =        q           ~      >  x                  A  R      /      =           A  R      /      =                         >             ¡      =        D   A  R      /      =                         =                      +            =                                       >             ¢      =        x   =                      %         >             £      A        ;   0   =  7         =  &      (   Q               Q               Q     ¡         P  6   ¢          ¡        6   £      ¢   A  ¤   ¥      0   >  ¥   £   ý  8  Q      #     Ò                 GLSL.std.450                     main       ¡   µ                ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\InstancedImpostorDepth_Fragment.glsl     Ï   Ì     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 2) uniform sampler SAMPLER;

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
	layout (offset = 0) vec3 WORLD_GRID_DELTA;
	layout (offset = 16) float HALF_WIDTH;
	layout (offset = 20) float WHOLE_WIDTH;
	layout (offset = 24) float HEIGHT;
	layout (offset = 28) uint MATERIAL_INDEX;
	layout (offset = 32) float START_FADE_IN_DISTANCE;
	layout (offset = 36) float END_FADE_IN_DISTANCE;
	layout (offset = 40) float START_FADE_OUT_DISTANCE;
	layout (offset = 44) float END_FADE_OUT_DISTANCE;
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
    /*
    *   As impostors might be used to fade between impostors and real models,
    *   use the inverse of the noise value for a smoother fade between them.
    */
    float noise_value = 1.0f - InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME);
    if (opacity < 0.5f
    || (InFadeOpacity < 1.0f && InFadeOpacity < noise_value))
    {
        discard;
    }
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        InterleavedGradientNoise(vu2;u1;         coordinate       frame        unpacked      @   x     I   y     _   opacity   `   Material      `       _Properties   `      _AlbedoThickness     	 `      _NormalMapDisplacement    `      _MaterialProperties   `      _Opacity      `      _EmissiveMultiplier   `      _Padding1     `      _Padding2     c   GlobalMaterials   c       MATERIALS     e         h   PushConstantData      h       WORLD_GRID_DELTA      h      HALF_WIDTH    h      WHOLE_WIDTH   h      HEIGHT    h      MATERIAL_INDEX   	 h      START_FADE_IN_DISTANCE   	 h      END_FADE_IN_DISTANCE     	 h      START_FADE_OUT_DISTANCE  	 h      END_FADE_OUT_DISTANCE     j            TEXTURES         SAMPLER      InTextureCoordinate      param        noise_value   ¡   gl_FragCoord      ¥   General  	 ¥       FULL_MAIN_RESOLUTION      ¥      INVERSE_FULL_MAIN_RESOLUTION     	 ¥      HALF_MAIN_RESOLUTION      ¥      INVERSE_HALF_MAIN_RESOLUTION      ¥      FRAME     ¥      VIEW_DISTANCE     §         ¨   param     ©   param     µ   InFadeOpacity    	 Ã   GlobalTextureAverageValues   	 Ã       TEXTURE_AVERAGE_VALUES    Å         É   BLUE_NOISE_TEXTURES   Í   SKY_TEXTURE   Ï   Camera   	 Ï       WORLD_TO_CLIP_MATRIX     	 Ï      WORLD_TO_CAMERA_MATRIX    Ï      PREVIOUS_WORLD_TO_CLIP_MATRIX     Ï      INVERSE_WORLD_TO_CAMERA_MATRIX    Ï      INVERSE_CAMERA_TO_CLIP_MATRIX    	 Ï      CAMERA_WORLD_POSITION    	 Ï      CAMERA_FORWARD_VECTOR    	 Ï      CURRENT_FRAME_JITTER      Ï      NEAR_PLANE    Ï   	   FAR_PLANE     Ñ       H  `       #       H  `      #      H  `      #      H  `      #      H  `      #      H  `      #      H  `      #      H  `      #      G  b          H  c       #       G  c      G  e   "       G  e   !      H  h       #       H  h      #      H  h      #      H  h      #      H  h      #      H  h      #       H  h      #   $   H  h      #   (   H  h      #   ,   G  h      G     "       G     !       G     "      G     !      G            G  ¡         H  ¥       #       H  ¥      #      H  ¥      #      H  ¥      #      H  ¥      #       H  ¥      #   $   G  ¥      G  §   "      G  §   !      G  µ         G  Â         H  Ã       #       G  Ã      G  Å   "       G  Å   !      G  É   "       G  É   !      G  Í   "       G  Í   !      H  Ï          H  Ï       #       H  Ï             H  Ï         H  Ï      #   @   H  Ï            H  Ï         H  Ï      #      H  Ï            H  Ï         H  Ï      #   À   H  Ï            H  Ï         H  Ï      #      H  Ï            H  Ï      #   @  H  Ï      #   P  H  Ï      #   `  H  Ï      #   h  H  Ï   	   #   l  G  Ï      G  Ñ   "      G  Ñ   !            !                                   	         
   	      !     
                             !     	                  
   +        ÿ   +  	        C+                     	     "          +  "   #      +     (      +  "   +      +     0      +  "   3      +     8      +     >   @   +  	   D   ØÒ²@+  	   Q   îSB+  	   R   Cq=+  	   U   E¿;+  	   Y     ? 
 `                  	         +     a        b   `   a     c   b      d      c   ;  d   e      +  "   f         g   	        h   g   	   	   	      	   	   	   	      i   	   h   ;  i   j   	   +  "   k         l   	         o           s    	    	                         +                                   ;                                           ;                         	                  ;                     
   ;      ¡        ¥                  	      ¦      ¥   ;  ¦   §      +  	   ¯      ?   ´      	   ;  ´   µ        Â   
        Ã   Â      Ä      Ã   ;  Ä   Å      +     Æ         Ç      Æ      È       Ç   ;  È   É        	 Ê   	                           Ë   Ê      Ì       Ë   ;  Ì   Í         Î   
        Ï   Î   Î   Î   Î   Î   g   g      	   	      Ð      Ï   ;  Ð   Ñ                6               ø     ;     _      ;           ;           ;     ¨      ;     ©                 >  _   Y              A  l   m   j   k   =     n   m   A  o   p   e   f   n   f   =     q   p   Ç     r   q   (   ª  s   t   r   (   ÷  v       ú  t   u   v   ø  u              A  l   w   j   k   =     x   w   A  o   y   e   f   x   f   =     z   y   Ç     {   z   >   ª  s   |   {   >   ÷  ~       ú  |   }      ø  }              A  l      j   k   =           A  o      e   f      k   =           A              =           =           V              =           W  
            Q  	             >  _      ù  ~   ø                A  l      j   k   =           A  o      e   f      k   =           >        9  
            Q  	             >  _      ù  ~   ø  ~   ù  v   ø  v              =  
   ¢   ¡   O     £   ¢   ¢          m     ¤   £   >  ¨   ¤   A  o   ª   §   k   =     «   ª   >  ©   «   9  	   ¬      ¨   ©     	   ­   Y   ¬   >     ­              =  	   ®   _   ¸  s   °   ®   ¯   ¨  s   ±   °   ÷  ³       ú  ±   ²   ³   ø  ²              =  	   ¶   µ   ¸  s   ·   ¶   Y   ÷  ¹       ú  ·   ¸   ¹   ø  ¸              =  	   º   µ   =  	   »      ¸  s   ¼   º   »   ù  ¹   ø  ¹   õ  s   ½   ·   ²   ¼   ¸   ù  ³   ø  ³   õ  s   ¾   °   v   ½   ¹   ÷  À       ú  ¾   ¿   À   ø  ¿              ü  ø  À   ý  8       þ      6  
             7        ø     ;                      =           Ç              p  	           	            A               >                    =     !      Â     $   !   #   Ç     %   $      p  	   &   %     	   '   &      A     )      (   >  )   '              =     *      Â     ,   *   +   Ç     -   ,      p  	   .   -     	   /   .      A     1      0   >  1   /              =     2      Â     4   2   3   Ç     5   4      p  	   6   5     	   7   6      A     9      8   >  9   7              =  
   :      þ  :   8       s  <   6  	             7        7        ø     ;     @      ;     I           u      =     =           ?   =   >   >     ?        w      A     A         =     B   A   p  	   C   B   =     E      p  	   F   E     	   G   D   F     	   H   C   G   >  @   H        x      A     J      (   =     K   J   p  	   L   K   =     M      p  	   N   M     	   O   D   N     	   P   L   O   >  I   P        z      =  	   S   @     	   T   R   S   =  	   V   I     	   W   U   V     	   X   T   W     	   Z   X   Y     	   [   Q   Z     	   \   [   Y   þ  \   8                          KTðÒD	(ÈDüÅ^ùú                                                             fÈê8ÓOJ