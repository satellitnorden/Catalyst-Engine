ïnX/J1O¤Ü6)o§OpaqueModelDoubleSided_RenderPipeline                            N      #     ¨                 GLSL.std.450                      main       '   E   X   \   g   v   x   ~        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\OpaqueModelDoubleSided_Vertex.glsl   b   Ì     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 1) uniform sampler SAMPLER;

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
	layout (offset = 0) mat4 PREVIOUS_MODEL_MATRIX;
	layout (offset = 64) mat4 CURRENT_MODEL_MATRIX;
	layout (offset = 128) uint MATERIAL_INDEX;
};

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec3 InTangent;
layout (location = 3) in vec2 InTextureCoordinate;

layout (location = 0) out mat3 OutTangentSpaceMatrix;
layout (location = 3) out vec3 OutPreviousWorldPosition;
layout (location = 4) out vec3 OutCurrentWorldPosition;
layout (location = 5) out vec2 OutTextureCoordinate;

void main()
{
    vec3 tangent = normalize(vec3(CURRENT_MODEL_MATRIX * vec4(InTangent, 0.0f)));
    vec3 bitangent = normalize(vec3(CURRENT_MODEL_MATRIX * vec4(cross(InNormal, InTangent), 0.0f)));
    vec3 normal = normalize(vec3(CURRENT_MODEL_MATRIX * vec4(InNormal, 0.0f)));
    OutTangentSpaceMatrix = mat3(tangent, bitangent, normal);
    OutPreviousWorldPosition = vec3(PREVIOUS_MODEL_MATRIX * vec4(InPosition, 1.0f));
    OutCurrentWorldPosition = vec3(CURRENT_MODEL_MATRIX * vec4(InPosition, 1.0f));
    OutTextureCoordinate = InTextureCoordinate;
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutCurrentWorldPosition,1.0f);
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   tangent      PushConstantData     	        PREVIOUS_MODEL_MATRIX    	       CURRENT_MODEL_MATRIX            MATERIAL_INDEX                InTangent     $   bitangent     '   InNormal      5   normal    E   OutTangentSpaceMatrix    	 X   OutPreviousWorldPosition      \   InPosition    g   OutCurrentWorldPosition   v   OutTextureCoordinate      x   InTextureCoordinate   |   gl_PerVertex      |       gl_Position   |      gl_PointSize      |      gl_ClipDistance   |      gl_CullDistance   ~            Camera   	        WORLD_TO_CLIP_MATRIX     	       WORLD_TO_CAMERA_MATRIX          PREVIOUS_WORLD_TO_CLIP_MATRIX           INVERSE_WORLD_TO_CAMERA_MATRIX          INVERSE_CAMERA_TO_CLIP_MATRIX    	       CAMERA_WORLD_POSITION    	       CAMERA_FORWARD_VECTOR    	       CURRENT_FRAME_JITTER            NEAR_PLANE       	   FAR_PLANE                 TEXTURES     	    GlobalTextureAverageValues   	        TEXTURE_AVERAGE_VALUES                Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                  BLUE_NOISE_TEXTURES   ¤   SKY_TEXTURE   §   SAMPLER H            H         #       H               H           H        #   @   H              H        #      G        G           G  '         G  E          G  X         G  \          G  g         G  v         G  x         H  |              H  |            H  |            H  |            G  |      H            H         #       H               H           H        #   @   H              H           H        #      H              H           H        #   À   H              H           H        #      H              H        #   @  H        #   P  H        #   `  H        #   h  H     	   #   l  G        G     "      G     !       G     "       G     !       G           H         #       G        G     "       G     !      H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G      "       G      !      G  ¤   "       G  ¤   !      G  §   "      G  §   !           !                               	                                                                	      ;        	               +                 	                  ;           +            ;     '        C            D      C   ;  D   E      +     I     ?   W         ;  W   X      +     Y       ;     \      ;  W   g        t            u      t   ;  u   v         w      t   ;  w   x      +     z        {      z     |         {   {      }      |   ;  }   ~                                t                     ;                                    	                             +                                   ;                                           ;            
                            +                                          ;                   +                                    ;              	 ¡                              ¢   ¡      £       ¢   ;  £   ¤         ¥      ¦       ¥   ;  ¦   §                 6               ø     ;  	   
      ;  	   $      ;  	   5                 A              =           =           Q               Q              Q              P                                  Q               Q               Q     !         P     "          !        #      E   "   >  
   #              A     %         =     &   %   =     (   '   =     )           *      D   (   )   Q     +   *       Q     ,   *      Q     -   *      P     .   +   ,   -           /   &   .   Q     0   /       Q     1   /      Q     2   /      P     3   0   1   2        4      E   3   >  $   4              A     6         =     7   6   =     8   '   Q     9   8       Q     :   8      Q     ;   8      P     <   9   :   ;           =   7   <   Q     >   =       Q     ?   =      Q     @   =      P     A   >   ?   @        B      E   A   >  5   B              =     F   
   =     G   $   =     H   5   Q     J   F       Q     K   F      Q     L   F      Q     M   G       Q     N   G      Q     O   G      Q     P   H       Q     Q   H      Q     R   H      P     S   J   K   L   P     T   M   N   O   P     U   P   Q   R   P  C   V   S   T   U   >  E   V              A     Z      Y   =     [   Z   =     ]   \   Q     ^   ]       Q     _   ]      Q     `   ]      P     a   ^   _   `   I        b   [   a   Q     c   b       Q     d   b      Q     e   b      P     f   c   d   e   >  X   f              A     h         =     i   h   =     j   \   Q     k   j       Q     l   j      Q     m   j      P     n   k   l   m   I        o   i   n   Q     p   o       Q     q   o      Q     r   o      P     s   p   q   r   >  g   s              =  t   y   x   >  v   y              A           Y   =           =        g   Q               Q              Q              P                 I                 A        ~   Y   >        ý  8  d`      #     I                GLSL.std.450                     main    «   õ   ü   	          %  '  -               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\OpaqueModelDoubleSided_Fragment.glsl     ö   Ì     // OpModuleProcessed entry-point main
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

layout (set = 1, binding = 1) uniform sampler SAMPLER;

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
	layout (offset = 0) mat4 PREVIOUS_MODEL_MATRIX;
	layout (offset = 64) mat4 CURRENT_MODEL_MATRIX;
	layout (offset = 128) uint MATERIAL_INDEX;
};

layout (location = 0) in mat3 InTangentSpaceMatrix;
layout (location = 3) in vec3 InPreviousWorldPosition;
layout (location = 4) in vec3 InCurrentWorldPosition;
layout (location = 5) in vec2 InTextureCoordinate;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;
layout (location = 4) out vec4 Scene;

void main()
{
    vec4 albedo_thickness;
    EVALUATE_ALBEDO_THICKNESS(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, albedo_thickness);
    vec4 normal_map_displacement;
    EVALUATE_NORMAL_MAP_DISPLACEMENT(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, normal_map_displacement);
    vec4 material_properties;
    EVALUATE_MATERIAL_PROPERTIES(MATERIALS[MATERIAL_INDEX], InTextureCoordinate, SAMPLER, material_properties);
    vec3 shading_normal = normal_map_displacement.xyz * 2.0f - 1.0f;
    shading_normal = InTangentSpaceMatrix * shading_normal;
    shading_normal = normalize(shading_normal);
    shading_normal = gl_FrontFacing ? shading_normal : -shading_normal;
    vec2 velocity = CalculateCurrentScreenCoordinate(InCurrentWorldPosition) - CalculatePreviousScreenCoordinate(InPreviousWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = albedo_thickness;
	SceneFeatures2 = vec4(shading_normal,gl_FragCoord.z);
	SceneFeatures3 = material_properties;
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
	Scene = vec4(albedo_thickness.rgb*material_properties[3]*MATERIALS[MATERIAL_INDEX]._EmissiveMultiplier,1.0f);
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       unpacked      @   view_space_position   B   Camera   	 B       WORLD_TO_CLIP_MATRIX     	 B      WORLD_TO_CAMERA_MATRIX    B      PREVIOUS_WORLD_TO_CLIP_MATRIX     B      INVERSE_WORLD_TO_CAMERA_MATRIX    B      INVERSE_CAMERA_TO_CLIP_MATRIX    	 B      CAMERA_WORLD_POSITION    	 B      CAMERA_FORWARD_VECTOR    	 B      CURRENT_FRAME_JITTER      B      NEAR_PLANE    B   	   FAR_PLANE     D         P   denominator   d   view_space_position   n   denominator      Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                 PushConstantData     	        PREVIOUS_MODEL_MATRIX    	       CURRENT_MODEL_MATRIX            MATERIAL_INDEX                albedo_thickness         TEXTURES      ¦   SAMPLER   «   InTextureCoordinate   ±   param     ¾   normal_map_displacement   Ì   param     Ù   material_properties   è   param     ì   shading_normal    õ   InTangentSpaceMatrix      ü   gl_FrontFacing      velocity      	  InCurrentWorldPosition    
  param       InPreviousWorldPosition     param       SceneFeatures1      SceneFeatures2      gl_FragCoord      %  SceneFeatures3    '  SceneFeatures4    -  Scene    	 ?  GlobalTextureAverageValues   	 ?      TEXTURE_AVERAGE_VALUES    A        D  BLUE_NOISE_TEXTURES   H  SKY_TEXTURE H  B          H  B       #       H  B             H  B         H  B      #   @   H  B            H  B         H  B      #      H  B            H  B         H  B      #   À   H  B            H  B         H  B      #      H  B            H  B      #   @  H  B      #   P  H  B      #   `  H  B      #   h  H  B   	   #   l  G  B      G  D   "      G  D   !       H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      H            H         #       H               H           H        #   @   H              H        #      G        G     "       G     !       G  ¦   "      G  ¦   !      G  «         G  õ          G  ü         G  	        G          G           G          G          G  %        G  '        G  -        G  >        H  ?      #       G  ?     G  A  "       G  A  !      G  D  "       G  D  !      G  H  "       G  H  !           !                                   	         
   	      !     
           	                       	      !                    
   +        ÿ   +  	        C+     !          "      	     %          +  %   &      +     +      +  %   .      +     3      +  %   6      +     ;        A   
        B   A   A   A   A   A            	   	      C      B   ;  C   D      +  %   E          F      A   +  	   J     ?+  	   ^      ?+  %   e       
                   	         +                                          ;                A   A            	      ;        	         	                  +                 	    	                         +                                   ;            +  %            ¡            ¤      ¥       ¤   ;  ¥   ¦         ¨         ª         ;  ª   «      +     ¹      +     Ô       +  %   Ü      +  	   ï      @  ó            ô      ó   ;  ô   õ         û         ;  û   ü                            ;    	     ;         +  %                           
   ;         ;                 
   ;                 	   ;    %     ;    '     +  	   )      ;    -     +  %   5        6     	     >  
        ?  >     @     ?  ;  @  A       B  ¨   Ô      C      B  ;  C  D       	 E  	                           F  E     G      F  ;  G  H                6               ø     ;           ;     ±      ;     ¾      ;     Ì      ;     Ù      ;     è      ;     ì      ;     þ      ;         ;     
     ;                     A           e   =           A           E      E   =           Ç              ª              ÷         ú        ®   ø                A           e   =           A           E         =            A  ¡   ¢          =     £   ¢   =  ¤   §   ¦   V  ¨   ©   £   §   =     ¬   «   W  
   ­   ©   ¬   >     ­   ù     ø  ®              A     ¯      e   =     °   ¯   A     ²      E   °      =     ³   ²   >  ±   ³   9  
   ´      ±   >     ´   ù     ø                A     µ      e   =     ¶   µ   A     ·      E   ¶   E   =     ¸   ·   Ç     º   ¸   ¹   ª     »   º   ¹   ÷  ½       ú  »   ¼   É   ø  ¼              A     ¿      e   =     À   ¿   A     Á      E   À   e   =     Â   Á   A  ¡   Ã      Â   =     Ä   Ã   =  ¤   Å   ¦   V  ¨   Æ   Ä   Å   =     Ç   «   W  
   È   Æ   Ç   >  ¾   È   ù  ½   ø  É              A     Ê      e   =     Ë   Ê   A     Í      E   Ë   e   =     Î   Í   >  Ì   Î   9  
   Ï      Ì   >  ¾   Ï   ù  ½   ø  ½              A     Ð      e   =     Ñ   Ð   A     Ò      E   Ñ   E   =     Ó   Ò   Ç     Õ   Ó   Ô   ª     Ö   Õ   Ô   ÷  Ø       ú  Ö   ×   å   ø  ×              A     Ú      e   =     Û   Ú   A     Ý      E   Û   Ü   =     Þ   Ý   A  ¡   ß      Þ   =     à   ß   =  ¤   á   ¦   V  ¨   â   à   á   =     ã   «   W  
   ä   â   ã   >  Ù   ä   ù  Ø   ø  å              A     æ      e   =     ç   æ   A     é      E   ç   Ü   =     ê   é   >  è   ê   9  
   ë      è   >  Ù   ë   ù  Ø   ø  Ø              =  
   í   ¾   O     î   í   í                  ð   î   ï   P     ñ   J   J   J        ò   ð   ñ   >  ì   ò              =  ó   ö   õ   =     ÷   ì        ø   ö   ÷   >  ì   ø              =     ù   ì        ú      E   ù   >  ì   ú              =     ý   ü   ÷         ú  ý   ÿ     ø  ÿ              =       ì   >  þ     ù     ø               =       ì            >  þ     ù     ø     =       þ              >  ì                =       	  >  
    9          
  =         >      9                       A      D     =                    >                 =  
        >                 =       ì   A        3   =  	        Q  	   !        Q  	   "       Q  	   #       P  
   $  !  "  #     >    $             =  
   &  Ù   >  %  &             =     (    Q  	   *  (      Q  	   +  (     P  
   ,  *  +  )  )  >  '  ,             =  
   .     O     /  .  .            A  "   0  Ù   ;   =  	   1  0       2  /  1  A     3     e   =     4  3  A  6  7     E   4  5  =  	   8  7       9  2  8  Q  	   :  9      Q  	   ;  9     Q  	   <  9     P  
   =  :  ;  <  J   >  -  =  ý  8             6  
             7        ø     ;                      =           Ç              p  	           	             A  "   #      !   >  #                  =     $      Â     '   $   &   Ç     (   '      p  	   )   (     	   *   )      A  "   ,      +   >  ,   *              =     -      Â     /   -   .   Ç     0   /      p  	   1   0     	   2   1      A  "   4      3   >  4   2              =     5      Â     7   5   6   Ç     8   7      p  	   9   8     	   :   9      A  "   <      ;   >  <   :        	      =  
   =      þ  =   8       M  :   6               7        ø     ;     @      ;  "   P           O      A  F   G   D   E   =  A   H   G   =     I      Q  	   K   I       Q  	   L   I      Q  	   M   I      P  
   N   K   L   M   J     
   O   H   N   >  @   O        P      A  "   Q   @   ;   =  	   R   Q     	   S   J   R   >  P   S        Q      =  	   T   P   =  
   U   @   O     V   U   U               W   V   T   A  "   X   @   !   Q  	   Y   W       >  X   Y   A  "   Z   @   +   Q  	   [   W      >  Z   [        S      =  
   \   @   O     ]   \   \               _   ]   ^   P     `   ^   ^        a   _   `   þ  a   8       Y  ;   6               7        ø     ;     d      ;  "   n           [      A  F   f   D   e   =  A   g   f   =     h      Q  	   i   h       Q  	   j   h      Q  	   k   h      P  
   l   i   j   k   J     
   m   g   l   >  d   m        \      A  "   o   d   ;   =  	   p   o     	   q   J   p   >  n   q        ]      =  	   r   n   =  
   s   d   O     t   s   s               u   t   r   A  "   v   d   !   Q  	   w   u       >  v   w   A  "   x   d   +   Q  	   y   u      >  x   y        _      =  
   z   d   O     {   z   z               |   {   ^   P     }   ^   ^        ~   |   }   þ  ~   8                          KTðÒD	                               ÙÅv)±%9òÁ©×YÝåGì ëz¸¾.v<N1Â8»´;                             -\8Ýî¡