ïnX/J1s#§ÊñûØEditorSelectedOutline_RenderPipeline                             ð)      #     Q                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\EditorSelectedOutline_Vertex.glsl    '   Ì     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 0) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

/*
*	Editor metadata struct definition.
*/
struct EditorMetadataStruct
{
	//Denotes whether or not the pixel is selected.
    bool _PixelIsSelected;
};

//Unpacks editor metadata.
EditorMetadataStruct UnpackEditorMetadata(vec4 data)
{
	EditorMetadataStruct editor_metadata;

  	editor_metadata._PixelIsSelected = data.x > 0.0f;

	return editor_metadata;
}

layout (set = 1, binding = 1) uniform sampler2D EditorMetadata;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES      =   Material      =       _Properties   =      _AlbedoThickness     	 =      _NormalMapDisplacement    =      _MaterialProperties   =      _Opacity      =      _EmissiveMultiplier   =      _Padding1     =      _Padding2     @   GlobalMaterials   @       MATERIALS     B         G   BLUE_NOISE_TEXTURES   K   SKY_TEXTURE   L   General  	 L       FULL_MAIN_RESOLUTION      L      INVERSE_FULL_MAIN_RESOLUTION     	 L      HALF_MAIN_RESOLUTION      L      INVERSE_HALF_MAIN_RESOLUTION      L      FRAME     L      VIEW_DISTANCE     N         P   EditorMetadata  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       H  =       #       H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      H  =      #      G  ?          H  @       #       G  @      G  B   "       G  B   !      G  G   "       G  G   !      G  K   "       G  K   !      H  L       #       H  L      #      H  L      #      H  L      #      H  L      #       H  L      #   $   G  L      G  N   "      G  N   !       G  P   "      G  P   !           !                             +     
     ¿                        ;           +           +                                  ;           +          ?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <        
 =   #   #   #   #   #      #   #   +  #   >        ?   =   >     @   ?      A      @   ;  A   B        C   8   +  #   D         E   C   D      F       E   ;  F   G        	 H                              I   H      J       I   ;  J   K         L               #         M      L   ;  M   N         O       C   ;  O   P            è      6               ø     ;     	      ;                ê       =           Ç              Ä              o                   
      >  	           ë       =           Ç              Ä              o                   
      >             ì       =        	                       "       !   A  %   &      $   >  &   "        í       =     '           (   '           )   (   !   A  %   +      *   >  +   )        î       =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   ý  8  6      #     ¤                 GLSL.std.450                     main    (                   ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\EditorSelectedOutline_Fragment.glsl  @   Ì     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 0) uniform General
{
	layout (offset = 0) vec2 FULL_MAIN_RESOLUTION;
	layout (offset = 8) vec2 INVERSE_FULL_MAIN_RESOLUTION;
	layout (offset = 16) vec2 HALF_MAIN_RESOLUTION;
	layout (offset = 24) vec2 INVERSE_HALF_MAIN_RESOLUTION;
	layout (offset = 32) uint FRAME;
	layout (offset = 36) float VIEW_DISTANCE;
};

/*
*	Editor metadata struct definition.
*/
struct EditorMetadataStruct
{
	//Denotes whether or not the pixel is selected.
    bool _PixelIsSelected;
};

//Unpacks editor metadata.
EditorMetadataStruct UnpackEditorMetadata(vec4 data)
{
	EditorMetadataStruct editor_metadata;

  	editor_metadata._PixelIsSelected = data.x > 0.0f;

	return editor_metadata;
}

layout (set = 1, binding = 1) uniform sampler2D EditorMetadata;

layout (location = 0) in vec2 InScreenCoordinate;

layout (location = 0) out vec4 Scene;

void main()
{
    #define OUTLINE_WIDTH (2)
    EditorMetadataStruct editor_metadata = UnpackEditorMetadata(texture(EditorMetadata, InScreenCoordinate));
    bool any_neighboring_pixel_not_selected = false;
    bool any_neighboring_pixel_selected = false;
    for (int Y = -OUTLINE_WIDTH; Y <= OUTLINE_WIDTH; ++Y)
    {
        for (int X = -OUTLINE_WIDTH; X <= OUTLINE_WIDTH; ++X)
        {
            if (X == 0 && Y == 0)
            {
                continue;
            }
            EditorMetadataStruct neighboring_editor_metadata = UnpackEditorMetadata(texture(EditorMetadata, InScreenCoordinate + vec2(float(X), float(Y)) * INVERSE_FULL_MAIN_RESOLUTION));
            any_neighboring_pixel_not_selected = any_neighboring_pixel_not_selected || !neighboring_editor_metadata._PixelIsSelected;
            any_neighboring_pixel_selected = any_neighboring_pixel_selected || neighboring_editor_metadata._PixelIsSelected;
        }
    }
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    float inner_color_weight = float(editor_metadata._PixelIsSelected && any_neighboring_pixel_not_selected);
    float outer_color_weight = float(!editor_metadata._PixelIsSelected && any_neighboring_pixel_selected);
    color += vec3(1.0f, 1.0f, 1.0f) * 4.0f * inner_color_weight;
	Scene = vec4(color,min(inner_color_weight+outer_color_weight,1.0f));
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         EditorMetadataStruct             _PixelIsSelected     	    UnpackEditorMetadata(vf4;        data         editor_metadata       editor_metadata   $   EditorMetadata    (   InScreenCoordinate    +   param     -   any_neighboring_pixel_not_selected   
 /   any_neighboring_pixel_selected    1   Y     ;   X    	 K   neighboring_editor_metadata   S   General  	 S       FULL_MAIN_RESOLUTION      S      INVERSE_FULL_MAIN_RESOLUTION     	 S      HALF_MAIN_RESOLUTION      S      INVERSE_HALF_MAIN_RESOLUTION      S      FRAME     S      VIEW_DISTANCE     U         ]   param     q   color     s   inner_color_weight    z   outer_color_weight       Scene        TEXTURES         Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                 BLUE_NOISE_TEXTURES   £   SKY_TEXTURE G  $   "      G  $   !      G  (          H  S       #       H  S      #      H  S      #      H  S      #      H  S      #       H  S      #   $   G  S      G  U   "      G  U   !       G            G     "       G     !       H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G     "       G     !      G  £   "       G  £   !           !                               	           
        
   !        	                           +                         +                        +                     
    	 !                              "   !      #       "   ;  #   $         &            '      &   ;  '   (      *  
   .      0         +     2   þÿÿÿ+     9        S   &   &   &   &            T      S   ;  T   U      +     V         W      &     o            p      o   ,  o   r            +     x     ?+          @,  o                           ;           +                !                   ;             
                            +                                          ;           +                 "                   ;             	                                ¡          ¢       ¡   ;  ¢   £            ê      6               ø     ;            ;  	   +      ;     -      ;     /      ;  0   1      ;  0   ;      ;     K      ;  	   ]      ;  p   q      ;     s      ;     z           í       =  "   %   $   =  &   )   (   W     *   %   )   >  +   *   9     ,      +   >      ,        î       >  -   .        ï       >  /   .        ð       >  1   2   ù  3   ø  3        ð       ö  5   6       ù  7   ø  7        ð       =     8   1   ³  
   :   8   9   ú  :   4   5   ø  4        ò       >  ;   2   ù  <   ø  <        ò       ö  >   ?       ù  @   ø  @        ò       =     A   ;   ³  
   B   A   9   ú  B   =   >   ø  =        ô       =     C   ;   ª  
   D   C      =     E   1   ª  
   F   E      §  
   G   D   F   ÷  I       ú  G   H   I   ø  H        ö       ù  ?   ø  I        ø       =  "   L   $   =  &   M   (   =     N   ;   o     O   N   =     P   1   o     Q   P   P  &   R   O   Q   A  W   X   U   V   =  &   Y   X     &   Z   R   Y     &   [   M   Z   W     \   L   [   >  ]   \   9     ^      ]   >  K   ^        ù       =  
   _   -   ¨  
   `   _   ÷  b       ú  `   a   b   ø  a        ù       A     c   K      =  
   d   c   ¨  
   e   d   ù  b   ø  b   õ  
   f   _   I   e   a        ù       >  -   f        ú       =  
   g   /   A     h   K      =  
   i   h   ¦  
   j   g   i   >  /   j   ù  ?   ø  ?        ò       =     k   ;        l   k   V   >  ;   l   ù  <   ø  >   ù  6   ø  6        ð       =     m   1        n   m   V   >  1   n   ù  3   ø  5        ý       >  q   r        þ       A     t          =  
   u   t   =  
   v   -   §  
   w   u   v   ©     y   w   x      >  s   y        ÿ       A     {          =  
   |   {   ¨  
   }   |   =  
   ~   /   §  
      }   ~   ©           x      >  z                  =        s     o            =  o      q     o            >  q                 =  o      q   =        s   =        z                            %      x   Q               Q              Q              P                    >        ý  8       Û   4   6               7  	      ø     ;                ß       A              =           º  
            A              >             á       =           þ     8                          (ÈDüÅ^ùú                       ·¥Õàÿíä â         N1Â8»´;                                   \5sú 