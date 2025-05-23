ïnX/J1zí­@YAnimatedModel_RenderPipeline                                     Z      #                     GLSL.std.450                      main    /   :   G   P   b   ¥   ·   Ä   Õ           ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\AnimatedModel_Vertex.glsl    -   Ì     // OpModuleProcessed entry-point main
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

layout (std430, set = 1, binding = 1) buffer AnimationBoneTransforms
{
	layout (offset = 0) mat4[] ANIMATION_BONE_TRANSFORMS;
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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 PREVIOUS_TRANSFORMATION;
	layout (offset = 64) mat4 CURRENT_TRANSFORMATION;
	layout (offset = 128) uint START_BONE_TRANSFORM;
};

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec3 InTangent;
layout (location = 3) in uvec4 InBoneIndices;
layout (location = 4) in vec4 InBoneWeights;
layout (location = 5) in vec2 InTextureCoordinate;

layout (location = 0) out mat3 OutTangentSpaceMatrix;
layout (location = 3) out vec3 OutPreviousWorldPosition;
layout (location = 4) out vec3 OutCurrentWorldPosition;

void main()
{
    vec3 world_position = vec3(0.0f, 0.0f, 0.0f);
    vec3 normal = vec3(0.0f, 0.0f, 0.0f);
    vec3 tangent = vec3(0.0f, 0.0f, 0.0f);
    for (uint bone_index = 0; bone_index < 4; ++bone_index)
    {
        mat4 bone_transform = ANIMATION_BONE_TRANSFORMS[START_BONE_TRANSFORM + InBoneIndices[bone_index]];
        world_position += vec3(bone_transform * vec4(InPosition, 1.0f)) * InBoneWeights[bone_index];
        normal += vec3(bone_transform * vec4(InNormal, 0.0f)) * InBoneWeights[bone_index];
        tangent += vec3(bone_transform * vec4(InTangent, 0.0f)) * InBoneWeights[bone_index];
    }
    vec3 world_space_tangent = normalize(vec3(CURRENT_TRANSFORMATION * vec4(tangent, 0.0f)));
    vec3 world_space_bitangent = normalize(vec3(CURRENT_TRANSFORMATION * vec4(cross(normal, tangent), 0.0f)));
    vec3 world_space_normal = normalize(vec3(CURRENT_TRANSFORMATION * vec4(normal, 0.0f)));
    OutTangentSpaceMatrix = mat3(world_space_tangent, world_space_bitangent, world_space_normal);
    OutPreviousWorldPosition = vec3(PREVIOUS_TRANSFORMATION * vec4(world_position, 1.0f));
    OutCurrentWorldPosition = vec3(CURRENT_TRANSFORMATION * vec4(world_position, 1.0f));
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutCurrentWorldPosition,1.0f);
}   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   world_position       normal       tangent      bone_index       bone_transform    !   AnimationBoneTransforms  
 !       ANIMATION_BONE_TRANSFORMS     #         &   PushConstantData     	 &       PREVIOUS_TRANSFORMATION  	 &      CURRENT_TRANSFORMATION   	 &      START_BONE_TRANSFORM      (         /   InBoneIndices     :   InPosition    G   InBoneWeights     P   InNormal      b   InTangent     v   world_space_tangent      world_space_bitangent        world_space_normal    ¥   OutTangentSpaceMatrix    	 ·   OutPreviousWorldPosition      Ä   OutCurrentWorldPosition   Ó   gl_PerVertex      Ó       gl_Position   Ó      gl_PointSize      Ó      gl_ClipDistance   Ó      gl_CullDistance   Õ         ×   Camera   	 ×       WORLD_TO_CLIP_MATRIX     	 ×      WORLD_TO_CAMERA_MATRIX    ×      PREVIOUS_WORLD_TO_CLIP_MATRIX     ×      INVERSE_WORLD_TO_CAMERA_MATRIX    ×      INVERSE_CAMERA_TO_CLIP_MATRIX    	 ×      CAMERA_WORLD_POSITION    	 ×      CAMERA_FORWARD_VECTOR    	 ×      CURRENT_FRAME_JITTER      ×      NEAR_PLANE    ×   	   FAR_PLANE     Ù         è   TEXTURES     	 ê   GlobalTextureAverageValues   	 ê       TEXTURE_AVERAGE_VALUES    ì         í   Material      í       _Properties   í      _AlbedoThickness     	 í      _NormalMapDisplacement    í      _MaterialProperties   í      _Opacity      í      _EmissiveMultiplier   í      _Padding1     í      _Padding2     ð   GlobalMaterials   ð       MATERIALS     ò         ÷   BLUE_NOISE_TEXTURES   û   SKY_TEXTURE   þ   SAMPLER      InTextureCoordinate G         @   H  !          H  !       #       H  !             G  !      G  #   "      G  #   !      H  &          H  &       #       H  &             H  &         H  &      #   @   H  &            H  &      #      G  &      G  /         G  :          G  G         G  P         G  b         G  ¥          G  ·         G  Ä         H  Ó              H  Ó            H  Ó            H  Ó            G  Ó      H  ×          H  ×       #       H  ×             H  ×         H  ×      #   @   H  ×            H  ×         H  ×      #      H  ×            H  ×         H  ×      #   À   H  ×            H  ×         H  ×      #      H  ×            H  ×      #   @  H  ×      #   P  H  ×      #   `  H  ×      #   h  H  ×   	   #   l  G  ×      G  Ù   "      G  Ù   !       G  è   "       G  è   !       G  é         H  ê       #       G  ê      G  ì   "       G  ì   !      H  í       #       H  í      #      H  í      #      H  í      #      H  í      #      H  í      #      H  í      #      H  í      #      G  ï          H  ð       #       G  ð      G  ò   "       G  ò   !      G  ÷   "       G  ÷   !      G  û   "       G  û   !      G  þ   "      G  þ   !      G                !                               	         +            ,                                          +            +                                                             !          "      !   ;  "   #        $          +  $   %         &               '   	   &   ;  '   (   	   +  $   )         *   	        -            .      -   ;  .   /         1            5            9         ;  9   :      +     <     ?   F         ;  F   G         I         ;  9   P      ;  9   b      +  $   t         w   	        £            ¤      £   ;  ¤   ¥         ¶         ;  ¶   ·      ;  ¶   Ä      +     Ñ        Ò      Ñ     Ó         Ò   Ò      Ô      Ó   ;  Ô   Õ        Ö           ×                        Ö            Ø      ×   ;  Ø   Ù         â          	 ä                            +     å        æ   ä   å      ç       æ   ;  ç   è         é      å     ê   é      ë      ê   ;  ë   ì       
 í                           +     î        ï   í   î     ð   ï      ñ      ð   ;  ñ   ò        ó   ä   +     ô         õ   ó   ô      ö       õ   ;  ö   ÷        	 ø                              ù   ø      ú       ù   ;  ú   û         ü      ý       ü   ;  ý   þ          ÿ      Ö   ;  ÿ                   6               ø     ;  	   
      ;  	         ;  	         ;           ;           ;  	   v      ;  	         ;  	                    >  
                 >                   >                   >        ù     ø                ö            ù     ø                =           °              ú           ø                A  *   +   (   )   =     ,   +   =     0      A  1   2   /   0   =     3   2        4   ,   3   A  5   6   #   %   4   =     7   6   >     7              =     8      =     ;   :   Q     =   ;       Q     >   ;      Q     ?   ;      P     @   =   >   ?   <        A   8   @   Q     B   A       Q     C   A      Q     D   A      P     E   B   C   D   =     H      A  I   J   G   H   =     K   J        L   E   K   =     M   
        N   M   L   >  
   N              =     O      =     Q   P   Q     R   Q       Q     S   Q      Q     T   Q      P     U   R   S   T           V   O   U   Q     W   V       Q     X   V      Q     Y   V      P     Z   W   X   Y   =     [      A  I   \   G   [   =     ]   \        ^   Z   ]   =     _           `   _   ^   >     `              =     a      =     c   b   Q     d   c       Q     e   c      Q     f   c      P     g   d   e   f           h   a   g   Q     i   h       Q     j   h      Q     k   h      P     l   i   j   k   =     m      A  I   n   G   m   =     o   n        p   l   o   =     q           r   q   p   >     r   ù     ø                =     s           u   s   t   >     u   ù     ø                A  w   x   (   t   =     y   x   =     z      Q     {   z       Q     |   z      Q     }   z      P     ~   {   |   }              y   ~   Q               Q              Q              P                            E      >  v                 A  w      (   t   =           =           =                      D         Q               Q              Q              P                                  Q               Q              Q              P                            E      >                   A  w      (   t   =           =           Q               Q              Q              P                                  Q               Q              Q               P     ¡                  ¢      E   ¡   >     ¢              =     ¦   v   =     §      =     ¨      Q     ©   ¦       Q     ª   ¦      Q     «   ¦      Q     ¬   §       Q     ­   §      Q     ®   §      Q     ¯   ¨       Q     °   ¨      Q     ±   ¨      P     ²   ©   ª   «   P     ³   ¬   ­   ®   P     ´   ¯   °   ±   P  £   µ   ²   ³   ´   >  ¥   µ              A  w   ¸   (   %   =     ¹   ¸   =     º   
   Q     »   º       Q     ¼   º      Q     ½   º      P     ¾   »   ¼   ½   <        ¿   ¹   ¾   Q     À   ¿       Q     Á   ¿      Q     Â   ¿      P     Ã   À   Á   Â   >  ·   Ã              A  w   Å   (   t   =     Æ   Å   =     Ç   
   Q     È   Ç       Q     É   Ç      Q     Ê   Ç      P     Ë   È   É   Ê   <        Ì   Æ   Ë   Q     Í   Ì       Q     Î   Ì      Q     Ï   Ì      P     Ð   Í   Î   Ï   >  Ä   Ð              A  5   Ú   Ù   %   =     Û   Ú   =     Ü   Ä   Q     Ý   Ü       Q     Þ   Ü      Q     ß   Ü      P     à   Ý   Þ   ß   <        á   Û   à   A  â   ã   Õ   %   >  ã   á   ý  8  ÐO      #     ¬                 GLSL.std.450                     main    ^   b   m   p   s   w                         	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\AnimatedModel_Fragment.glsl  W   Ì     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
layout (early_fragment_tests) in;

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

layout (std430, set = 1, binding = 1) buffer AnimationBoneTransforms
{
	layout (offset = 0) mat4[] ANIMATION_BONE_TRANSFORMS;
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

layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 PREVIOUS_TRANSFORMATION;
	layout (offset = 64) mat4 CURRENT_TRANSFORMATION;
	layout (offset = 128) uint START_BONE_TRANSFORM;
};

layout (location = 0) in mat3 InTangentSpaceMatrix;
layout (location = 3) in vec3 InPreviousWorldPosition;
layout (location = 4) in vec3 InCurrentWorldPosition;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;
layout (location = 4) out vec4 Scene;

void main()
{
    vec2 velocity = CalculateCurrentScreenCoordinate(InCurrentWorldPosition) - CalculatePreviousScreenCoordinate(InPreviousWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(0.25f,0.25f,0.25f,1.0f);
	SceneFeatures2 = vec4(InTangentSpaceMatrix[2],gl_FragCoord.z);
	SceneFeatures3 = vec4(1.0f,0.0f,1.0f,0.0f);
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
	Scene = vec4(0.0f,0.0f,0.0f,1.0f);
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       view_space_position      Camera   	        WORLD_TO_CLIP_MATRIX     	       WORLD_TO_CAMERA_MATRIX          PREVIOUS_WORLD_TO_CLIP_MATRIX           INVERSE_WORLD_TO_CAMERA_MATRIX          INVERSE_CAMERA_TO_CLIP_MATRIX    	       CAMERA_WORLD_POSITION    	       CAMERA_FORWARD_VECTOR    	       CURRENT_FRAME_JITTER            NEAR_PLANE       	   FAR_PLANE              &   denominator   >   view_space_position   H   denominator   \   velocity      ^   InCurrentWorldPosition    _   param     b   InPreviousWorldPosition   c   param     m   SceneFeatures1    p   SceneFeatures2    s   InTangentSpaceMatrix      w   gl_FragCoord         SceneFeatures3       SceneFeatures4       Scene        TEXTURES     	    GlobalTextureAverageValues   	        TEXTURE_AVERAGE_VALUES                Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                 BLUE_NOISE_TEXTURES   ¡   SKY_TEXTURE   £   AnimationBoneTransforms  
 £       ANIMATION_BONE_TRANSFORMS     ¥         ¨   SAMPLER   ©   PushConstantData     	 ©       PREVIOUS_TRANSFORMATION  	 ©      CURRENT_TRANSFORMATION   	 ©      START_BONE_TRANSFORM      «       H            H         #       H               H           H        #   @   H              H           H        #      H              H           H        #   À   H              H           H        #      H              H        #   @  H        #   P  H        #   `  H        #   h  H     	   #   l  G        G     "      G     !       G  ^         G  b         G  m          G  p         G  s          G  w         G           G           G           G     "       G     !       G           H         #       G        G     "       G     !      H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G     "       G     !      G  ¡   "       G  ¡   !      G  ¢      @   H  £          H  £       #       H  £             G  £      G  ¥   "      G  ¥   !      G  ¨   "      G  ¨   !      H  ©          H  ©       #       H  ©             H  ©         H  ©      #   @   H  ©            H  ©      #      G  ©           !                               	           
         !     
   	                                                               
                     ;                       +                        +          ?   %           '           +  '   (      +  '   0       +  '   3      +     8      ?+     ?         [      
      ]         ;  ]   ^      ;  ]   b      +     g         h      
      l         ;  l   m      +     n     >,     o   n   n   n      ;  l   p        q            r      q   ;  r   s         v         ;  v   w      +  '   x         y         ;  l         +            ,                    ;  l         ;  l         ,                     	                             +  '                                 ;                                           ;            
    '   '   '   '   '      '   '   +  '                                        ;                   +  '                                  ;             	                                                   ;      ¡         ¢        £   ¢      ¤      £   ;  ¤   ¥        ¦      §       ¦   ;  §   ¨         ©         '      ª   	   ©   ;  ª   «   	             6               ø     ;  [   \      ;  	   _      ;  	   c                 =     `   ^   >  _   `   9  
   a      _   =     d   b   >  c   d   9  
   e      c     
   f   a   e   A  h   i      g   =  
   j   i     
   k   f   j   >  \   k              >  m   o              A  ]   t   s   ?   =     u   t   A  y   z   w   x   =     {   z   Q     |   u       Q     }   u      Q     ~   u      P        |   }   ~   {   >  p                 >                   =  
      \   Q               Q              P                    >                   >        ý  8       R  :   6  
             7  	      ø     ;           ;  %   &           T      A              =           =           Q                Q     !         Q     "         P     #       !   "           $      #   >     $        U      A  %   )      (   =     *   )        +      *   >  &   +        V      =     ,   &   =     -      O  
   .   -   -            
   /   .   ,   A  %   1      0   Q     2   /       >  1   2   A  %   4      3   Q     5   /      >  4   5        X      =     6      O  
   7   6   6            
   9   7   8   P  
   :   8   8     
   ;   9   :   þ  ;   8       ^  ;   6  
             7  	      ø     ;     >      ;  %   H           `      A     @      ?   =     A   @   =     B      Q     C   B       Q     D   B      Q     E   B      P     F   C   D   E           G   A   F   >  >   G        a      A  %   I   >   (   =     J   I        K      J   >  H   K        b      =     L   H   =     M   >   O  
   N   M   M            
   O   N   L   A  %   P   >   0   Q     Q   O       >  P   Q   A  %   R   >   3   Q     S   O      >  R   S        d      =     T   >   O  
   U   T   T            
   V   U   8   P  
   W   8   8     
   X   V   W   þ  X   8                          KTðÒD	       k$X{(                       ÙÅv)±%9òÁ©×YÝåGì ëz¸¾.v<N1Â8»´;                             	K<Á2¤