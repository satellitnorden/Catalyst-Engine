ïnX/J1'Å3RayTracedShadows_RenderPipeline                                    Ðz      #     ¯        Ü  
  SPV_NV_ray_tracing       GLSL.std.450               Á     main    Ø   ß        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedShadows_RayGeneration.glsl  [   Ì     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require
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

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 2) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

layout (set = 1, binding = 4, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 5, rgba32f) uniform image2D Shadows; 

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

//Constants.
#define LIGHT_TYPE_DIRECTIONAL (0)
#define LIGHT_TYPE_POINT (1)
#define LIGHT_TYPE_BOX (2)

#define LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT (BIT(0))
#define LIGHT_PROPERTY_VOLUMETRIC_BIT (BIT(1))
#define LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT (BIT(2))

/*
*	Light struct definition.
*/
struct Light
{
	/*
	*	First transform data.
	*	Direction for directional lights, position for point lights, minimum world position for box lights.
	*/
	vec3 _TransformData1;

	/*
	*	Second transform data.
	*	Maximum world position for box lights.
	*/
	vec3 _TransformData2;
	vec3 _Color;
	uint _LightType;
	uint _LightProperties;
	float _Intensity;
	float _Radius;
	float _Size;
};

/*
*	Unpacks the light at the given index.
*   Requies the Lighting storage buffer to be included.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 light_data_1 = LIGHT_DATA[index * 4 + 0];
  	vec4 light_data_2 = LIGHT_DATA[index * 4 + 1];
  	vec4 light_data_3 = LIGHT_DATA[index * 4 + 2];
  	vec4 light_data_4 = LIGHT_DATA[index * 4 + 3];

  	light._TransformData1 = vec3(light_data_1.x, light_data_1.y, light_data_1.z);
  	light._TransformData2 = vec3(light_data_1.w, light_data_2.x, light_data_2.y);
  	light._Color = vec3(light_data_2.z, light_data_2.w, light_data_3.x);
  	light._LightType = floatBitsToUint(light_data_3.y);
  	light._LightProperties = floatBitsToUint(light_data_3.z);
  	light._Intensity = light_data_3.w;
  	light._Radius = light_data_4.x;
  	light._Size = light_data_4.y;

	return light;
}

layout (set = 2, binding = 0) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 2, binding = 1) buffer OpaqueModels_VERTEX_DATA_BUFFER { vec4 OpaqueModels_VERTEX_DATA[]; } OpaqueModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 2) buffer OpaqueModels_INDEX_DATA_BUFFER { uint OpaqueModels_INDEX_DATA[]; } OpaqueModels_INDEX_BUFFERS[];
layout (set = 2, binding = 3) buffer OpaqueModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] OpaqueModels_MATERIAL_INDICES; };
layout (set = 2, binding = 4) buffer MaskedModels_VERTEX_DATA_BUFFER { vec4 MaskedModels_VERTEX_DATA[]; } MaskedModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 5) buffer MaskedModels_INDEX_DATA_BUFFER { uint MaskedModels_INDEX_DATA[]; } MaskedModels_INDEX_BUFFERS[];
layout (set = 2, binding = 6) buffer MaskedModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] MaskedModels_MATERIAL_INDICES; };
layout (location = 0) rayPayloadNV float VISIBILITY;

void main()
{
    vec2 screen_coordinate = (vec2(gl_LaunchIDNV.xy) + vec2(0.5f)) / vec2(gl_LaunchSizeNV.xy);
    vec4 scene_features_2 = imageLoad(SceneFeatures2Half, ivec2(gl_LaunchIDNV.xy));
    vec3 shading_normal = scene_features_2.xyz;
    float depth = scene_features_2.w;
    vec3 world_position = CalculateWorldPosition(screen_coordinate, depth);
    vec4 shadows = vec4(1.0f);
    uint current_shadow_index = 0;
    for (uint light_index = 0; light_index < LIGHTING_HEADER._NumberOfLights; ++light_index)
    {
        Light light = UnpackLight(light_index);
        if (TEST_BIT(light._LightProperties, LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT))
        {
            switch (light._LightType)
            {
                case LIGHT_TYPE_DIRECTIONAL:
                {
                    VISIBILITY = 0.0f;
                    uint ray_tracing_flags =    gl_RayFlagsTerminateOnFirstHitNV
                                                | gl_RayFlagsSkipClosestHitShaderNV;
                    vec3 direction = -light._TransformData1;
                    /*
                    *   This assumes the directional light is representing a sun.
                    *   The sun is on average 150 million kilometers away from the earth,
                    *   with a radius of 696 340 kilometers.
                    *   If we scale those values down a bit for floating point precision's sake,
                    *   we can imagine a sphere offset in the directional light direction from the origin,
                    *   and grab a random point in that sphere.
                    */
                    vec4 noise_sample = SampleBlueNoiseTexture(uvec2(gl_LaunchIDNV.xy), 0);
                    vec3 sphere_position = world_position + direction;
                    vec2 spherical_coordinates;
                    {
                       spherical_coordinates.x = acos(2 * noise_sample.x - 1.0f) - (PI / 2.0f);
                       spherical_coordinates.y = 2 * PI * noise_sample.y;
                    }
                    vec3 random_point_on_sphere;
                    {
                        float cos_a = cos(spherical_coordinates.x);
                        float cos_b = cos(spherical_coordinates.y);
                        float sin_a = sin(spherical_coordinates.x);
                        float sin_b = sin(spherical_coordinates.y);
                        random_point_on_sphere = vec3(cos_a * cos_b, cos_a * sin_b, sin_a);
                    }
                    random_point_on_sphere = dot(random_point_on_sphere, direction) >= 0.0f ? random_point_on_sphere : -random_point_on_sphere;
                    random_point_on_sphere *= 0.0046422666666667f * 0.125f;
                    direction = normalize(sphere_position + random_point_on_sphere - world_position);
                    float maximum_distance = FLOAT32_MAXIMUM;
traceNV
(
	TOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/
	ray_tracing_flags, /*rayFlags*/
	0xff, /*cullMask*/
	0, /*sbtRecordOffset*/
	0, /*sbtRecordStride*/
	0, /*missIndex*/
	world_position, /*origin*/
	FLOAT32_EPSILON * 16.0f, /*Tmin*/
	direction, /*direction*/
	maximum_distance, /*Tmax*/
	0 /*payload*/
);
                    shadows[current_shadow_index++] = VISIBILITY;
                    break;
                }
            }
        }
    }
    imageStore(Shadows, ivec2(gl_LaunchIDNV.xy), shadows);
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main     
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index    
    CalculateWorldPosition(vf2;f1;       screen_coordinate        depth        Light            _TransformData1         _TransformData2         _Color          _LightType          _LightProperties            _Intensity          _Radius         _Size        UnpackLight(u1;      index         offset_index      !   General  	 !       FULL_MAIN_RESOLUTION      !      INVERSE_FULL_MAIN_RESOLUTION     	 !      HALF_MAIN_RESOLUTION      !      INVERSE_HALF_MAIN_RESOLUTION      !      FRAME     !      VIEW_DISTANCE     #         -   offset_coordinate     F   BLUE_NOISE_TEXTURES   T   near_plane_coordinate     \   view_space_position   ^   Camera   	 ^       WORLD_TO_CLIP_MATRIX     	 ^      WORLD_TO_CAMERA_MATRIX    ^      PREVIOUS_WORLD_TO_CLIP_MATRIX     ^      INVERSE_WORLD_TO_CAMERA_MATRIX    ^      INVERSE_CAMERA_TO_CLIP_MATRIX    	 ^      CAMERA_WORLD_POSITION    	 ^      CAMERA_FORWARD_VECTOR    	 ^      CURRENT_FRAME_JITTER      ^      NEAR_PLANE    ^   	   FAR_PLANE     `         n   inverse_view_space_position_denominator   v   world_space_position         light_data_1         LightingHeader           _NumberOfLights         _MaximumNumberOfShadowCastingLights      Lighting             LIGHTING_HEADER         LIGHT_DATA                light_data_2         light_data_3         light_data_4      ¢   light     Õ   screen_coordinate     Ø   gl_LaunchIDNV     ß   gl_LaunchSizeNV   ä   scene_features_2      ç   SceneFeatures2Half    î   shading_normal    ñ   depth     ô   world_position    õ   param     ÷   param     ú   shadows   ü   current_shadow_index      ý   light_index     light     	  param       VISIBILITY      ray_tracing_flags       direction       noise_sample      $  param     %  param     '  sphere_position   +  spherical_coordinates     9  cos_a     =  cos_b     A  sin_a     E  sin_b     I  random_point_on_sphere    g  maximum_distance      k  TOP_LEVEL_ACCELERATION_STRUCTURE      {  Shadows     TEXTURES        Material            _Properties        _AlbedoThickness     	      _NormalMapDisplacement         _MaterialProperties        _Opacity           _EmissiveMultiplier        _Padding1          _Padding2       GlobalMaterials         MATERIALS               SKY_TEXTURE     SAMPLER  
   OpaqueModels_VERTEX_DATA_BUFFER  
       OpaqueModels_VERTEX_DATA     	   OpaqueModels_VERTEX_BUFFERS  
   OpaqueModels_INDEX_DATA_BUFFER   	       OpaqueModels_INDEX_DATA  	   OpaqueModels_INDEX_BUFFERS   
   OpaqueModels_MATERIAL_BUFFER            OpaqueModels_MATERIAL_INDICES             
 ¢  MaskedModels_VERTEX_DATA_BUFFER  
 ¢      MaskedModels_VERTEX_DATA     	 ¥  MaskedModels_VERTEX_BUFFERS  
 §  MaskedModels_INDEX_DATA_BUFFER   	 §      MaskedModels_INDEX_DATA  	 ª  MaskedModels_INDEX_BUFFERS   
 ¬  MaskedModels_MATERIAL_BUFFER      ¬      MaskedModels_MATERIAL_INDICES     ®      H  !       #       H  !      #      H  !      #      H  !      #      H  !      #       H  !      #   $   G  !      G  #   "      G  #   !      G  F   "       G  F   !      H  ^          H  ^       #       H  ^             H  ^         H  ^      #   @   H  ^            H  ^         H  ^      #      H  ^            H  ^         H  ^      #   À   H  ^            H  ^         H  ^      #      H  ^            H  ^      #   @  H  ^      #   P  H  ^      #   `  H  ^      #   h  H  ^   	   #   l  G  ^      G  `   "      G  `   !       H         #       H        #      G           H         #       H        #      G        G     "      G     !      G  Ø      Ç  G  ß      È  G  ç   "      G  ç   !      G           G  k  "      G  k  !       G  {  "      G  {  !      G    "       G    !       H        #       H       #      H       #      H       #      H       #      H       #      H       #      H       #      G           H        #       G       G    "       G    !      G    "       G    !      G    "      G    !      G          H        #       G       G    "      G    !      G          H        #       G       G    "      G    !      G          H        #       G       G     "      G     !      G  ¡        H  ¢      #       G  ¢     G  ¥  "      G  ¥  !      G  ¦        H  §      #       G  §     G  ª  "      G  ª  !      G  «        H  ¬      #       G  ¬     G  ®  "      G  ®  !           !                                   	            
                             !        	   
                                                 !               
                            !        
     !                        "      !   ;  "   #        $          +  $   %         &         +     +      +     .       +     3       +     8       	 B                              C   B     D   C   3      E       D   ;  E   F          H       C   +     M      B+     P       +     V      @+     X     ?   [           ]           ^   ]   ]   ]   ]   ]                     _      ^   ;  _   `         a      ]   +     o      +  $   w                                                ;           +  $         +                       +              ¡         +  $   £          «         +  $   µ      +  $   Æ      +  $   Ê      +  $   Î        Ö            ×      Ö   ;  ×   Ø      +     Ü      ?,     Ý   Ü   Ü   ;  ×   ß       	 å                              æ       å   ;  æ   ç         ë   $      ,     û   X   X   X   X            Ú     ;      Ú  +          +     1  ØÉ?+     4  ØÉ@+     ^  (:+     h  ÿÿÝ i     j      i  ;  j  k      +     n  ÿ   +     p     6;  æ   {      +              B                ;           
                           +                                  ;          	                                             ;                         ;                             8             ;                            8             ;                                         ;            ¡       ¢  ¡    £  ¢  8      ¤     £  ;  ¤  ¥       ¦       §  ¦    ¨  §  8      ©     ¨  ;  ©  ª       «      ¬  «     ­     ¬  ;  ­  ®               6               ø     ;     Õ      ;  [   ä      ;  «   î      ;     ñ      ;  «   ô      ;     õ      ;     ÷      ;  [   ú      ;  
   ü      ;  
   ý      ;  ¡        ;  
   	     ;  
        ;  «        ;  [        ;  	   $     ;  
   %     ;  «   '     ;     +     ;     9     ;     =     ;     A     ;     E     ;  «   I     ;  «   V     ;     g                =  Ö   Ù   Ø   O     Ú   Ù   Ù          p     Û   Ú        Þ   Û   Ý   =  Ö   à   ß   O     á   à   à          p     â   á        ã   Þ   â   >  Õ   ã              =  å   è   ç   =  Ö   é   Ø   O     ê   é   é          |  ë   ì   ê   b     í   è   ì   >  ä   í              =     ï   ä   O     ð   ï   ï             >  î   ð              A     ò   ä   o   =     ó   ò   >  ñ   ó              =     ö   Õ   >  õ   ö   =     ø   ñ   >  ÷   ø   9     ù      õ   ÷   >  ô   ù              >  ú   û              >  ü   .              >  ý   .   ù  þ   ø  þ              ö           ù    ø               =       ý   A  &        £   £   =         °          ú    ÿ      ø  ÿ              =     
  ý   >  	  
  9          	  >                 A  
       %   =         Ç         8   ª        8   ÷        ú        ø               A  
       w   =         ÷        û            ø               >    P               >           ¢      A  «       £   =                  >           «      =  Ö     Ø   O                   Q     !         Q     "        P     #  !  "  >  $  #  >  %  .   9     &     $  %  >    &       ¬      =     (  ô   =     )         *  (  )  >  '  *       ¯      A     ,    .   =     -  ,       .  V   -       /  .  X        0        /       2  0  1  A     3  +  .   >  3  2       °      A     5    8   =     6  5       7  4  6  A     8  +  8   >  8  7       ´      A     :  +  .   =     ;  :       <        ;  >  9  <       µ      A     >  +  8   =     ?  >       @        ?  >  =  @       ¶      A     B  +  .   =     C  B       D        C  >  A  D       ·      A     F  +  8   =     G  F       H        G  >  E  H       ¸      =     J  9  =     K  =       L  J  K  =     M  9  =     N  E       O  M  N  =     P  A  P     Q  L  O  P  >  I  Q       º      =     R  I  =     S         T  R  S  ¾    U  T  P   ÷  X      ú  U  W  Z  ø  W       º      =     Y  I  >  V  Y  ù  X  ø  Z       º      =     [  I       \  [  >  V  \  ù  X  ø  X  =     ]  V       º      >  I  ]       »      =     _  I       `  _  ^  >  I  `       ¼      =     a  '  =     b  I       c  a  b  =     d  ô        e  c  d       f     E   e  >    f       ½      >  g  h       Ë      =  i  l  k  =     m    =     o  ô   =     q    =     r  g  Ù l  m  n  .   .   .   o  p  q  r  £        Ì      =     s  ü        t  s     >  ü   t  =     u    A     v  ú   s  >  v  u       Í      ù    ø    ù    ø    ù    ø               =     y  ý        z  y     >  ý   z  ù  þ   ø          Ò      =  å   |  {  =  Ö   }  Ø   O     ~  }  }         |  ë     ~  =       ú   c  |      ý  8       ô   9   6               7  	      7  
      ø     ;  
          ;  	   -           ö       A  &   '   #   %   =     (   '   =     )           *   (   )   Ç     ,   *   +   >      ,        ú       A  
   /      .   =     0   /   A  &   1   #   %   =     2   1        4   2   3   Ç     5   4   +        6   0   5   A  
   7   -   .   >  7   6        û       A  
   9      8   =     :   9   A  &   ;   #   %   =     <   ;        =   <   3        >   =   3   Ç     ?   >   +        @   :   ?   A  
   A   -   8   >  A   @        ý       =     G       A  H   I   F   G   =  C   J   I   =     K   -   p     L   K   P     N   M   M        O   L   N   X     Q   J   O      P   þ  Q   8         @   6               7        7        ø     ;     T      ;  [   \      ;     n      ;  [   v                 =     U           W   U   V   P     Y   X   X        Z   W   Y   >  T   Z              A  a   b   `   %   =  ]   c   b   =     d   T   =     e      Q     f   d       Q     g   d      P     h   f   g   e   Q     i   h       Q     j   h      Q     k   h      P     l   i   j   k   X        m   c   l   >  \   m              A     p   \   o   =     q   p        r   X   q   >  n   r              =     s   n   =     t   \        u   t   s   >  \   u              A  a   x   `   w   =  ]   y   x   =     z   \        {   y   z   >  v   {               =     |   v   O     }   |   |             þ  }   8       o     6               7  
      ø     ;  [         ;  [         ;  [         ;  [         ;  ¡   ¢           s      =                                    .   A                 =           >             t      =                                    8   A                 =           >             u      =                                       A                 =           >             v      =                                    o   A                 =            >              x      A     ¤      .   =     ¥   ¤   A     ¦      8   =     §   ¦   A     ¨         =     ©   ¨   P     ª   ¥   §   ©   A  «   ¬   ¢   £   >  ¬   ª        y      A     ­      o   =     ®   ­   A     ¯      .   =     °   ¯   A     ±      8   =     ²   ±   P     ³   ®   °   ²   A  «   ´   ¢      >  ´   ³        z      A     ¶         =     ·   ¶   A     ¸      o   =     ¹   ¸   A     º      .   =     »   º   P     ¼   ·   ¹   »   A  «   ½   ¢   µ   >  ½   ¼        {      A     ¾      8   =     ¿   ¾   |     À   ¿   A  
   Á   ¢   w   >  Á   À        |      A     Â         =     Ã   Â   |     Ä   Ã   A  
   Å   ¢   %   >  Å   Ä        }      A     Ç      o   =     È   Ç   A     É   ¢   Æ   >  É   È        ~      A     Ë      .   =     Ì   Ë   A     Í   ¢   Ê   >  Í   Ì              A     Ï      8   =     Ð   Ï   A     Ñ   ¢   Î   >  Ñ   Ð              =     Ò   ¢   þ  Ò   8         ÄI      #     W         Ü  
  SPV_NV_ray_tracing       GLSL.std.450               Å     main         ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedShadows_RayMiss.glsl       Ì     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require
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

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 2) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

layout (set = 1, binding = 4, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 5, rgba32f) uniform image2D Shadows; 

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

//Constants.
#define LIGHT_TYPE_DIRECTIONAL (0)
#define LIGHT_TYPE_POINT (1)
#define LIGHT_TYPE_BOX (2)

#define LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT (BIT(0))
#define LIGHT_PROPERTY_VOLUMETRIC_BIT (BIT(1))
#define LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT (BIT(2))

/*
*	Light struct definition.
*/
struct Light
{
	/*
	*	First transform data.
	*	Direction for directional lights, position for point lights, minimum world position for box lights.
	*/
	vec3 _TransformData1;

	/*
	*	Second transform data.
	*	Maximum world position for box lights.
	*/
	vec3 _TransformData2;
	vec3 _Color;
	uint _LightType;
	uint _LightProperties;
	float _Intensity;
	float _Radius;
	float _Size;
};

/*
*	Unpacks the light at the given index.
*   Requies the Lighting storage buffer to be included.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 light_data_1 = LIGHT_DATA[index * 4 + 0];
  	vec4 light_data_2 = LIGHT_DATA[index * 4 + 1];
  	vec4 light_data_3 = LIGHT_DATA[index * 4 + 2];
  	vec4 light_data_4 = LIGHT_DATA[index * 4 + 3];

  	light._TransformData1 = vec3(light_data_1.x, light_data_1.y, light_data_1.z);
  	light._TransformData2 = vec3(light_data_1.w, light_data_2.x, light_data_2.y);
  	light._Color = vec3(light_data_2.z, light_data_2.w, light_data_3.x);
  	light._LightType = floatBitsToUint(light_data_3.y);
  	light._LightProperties = floatBitsToUint(light_data_3.z);
  	light._Intensity = light_data_3.w;
  	light._Radius = light_data_4.x;
  	light._Size = light_data_4.y;

	return light;
}

layout (set = 2, binding = 0) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 2, binding = 1) buffer OpaqueModels_VERTEX_DATA_BUFFER { vec4 OpaqueModels_VERTEX_DATA[]; } OpaqueModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 2) buffer OpaqueModels_INDEX_DATA_BUFFER { uint OpaqueModels_INDEX_DATA[]; } OpaqueModels_INDEX_BUFFERS[];
layout (set = 2, binding = 3) buffer OpaqueModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] OpaqueModels_MATERIAL_INDICES; };
layout (set = 2, binding = 4) buffer MaskedModels_VERTEX_DATA_BUFFER { vec4 MaskedModels_VERTEX_DATA[]; } MaskedModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 5) buffer MaskedModels_INDEX_DATA_BUFFER { uint MaskedModels_INDEX_DATA[]; } MaskedModels_INDEX_BUFFERS[];
layout (set = 2, binding = 6) buffer MaskedModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] MaskedModels_MATERIAL_INDICES; };
layout (location = 0) rayPayloadInNV float VISIBILITY;

void main()
{
    VISIBILITY = 1.0f;
}
   	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main      	   VISIBILITY       TEXTURES         Material             _Properties         _AlbedoThickness     	       _NormalMapDisplacement          _MaterialProperties         _Opacity            _EmissiveMultiplier         _Padding1           _Padding2        GlobalMaterials          MATERIALS                 BLUE_NOISE_TEXTURES      SKY_TEXTURE   $   Camera   	 $       WORLD_TO_CLIP_MATRIX     	 $      WORLD_TO_CAMERA_MATRIX    $      PREVIOUS_WORLD_TO_CLIP_MATRIX     $      INVERSE_WORLD_TO_CAMERA_MATRIX    $      INVERSE_CAMERA_TO_CLIP_MATRIX    	 $      CAMERA_WORLD_POSITION    	 $      CAMERA_FORWARD_VECTOR    	 $      CURRENT_FRAME_JITTER      $      NEAR_PLANE    $   	   FAR_PLANE     &         '   General  	 '       FULL_MAIN_RESOLUTION      '      INVERSE_FULL_MAIN_RESOLUTION     	 '      HALF_MAIN_RESOLUTION      '      INVERSE_HALF_MAIN_RESOLUTION      '      FRAME     '      VIEW_DISTANCE     )         *   LightingHeader    *       _NumberOfLights   *      _MaximumNumberOfShadowCastingLights   ,   Lighting      ,       LIGHTING_HEADER   ,      LIGHT_DATA    .         1   SAMPLER   4   SceneFeatures2Half    5   Shadows   8   TOP_LEVEL_ACCELERATION_STRUCTURE     
 :   OpaqueModels_VERTEX_DATA_BUFFER  
 :       OpaqueModels_VERTEX_DATA     	 >   OpaqueModels_VERTEX_BUFFERS  
 @   OpaqueModels_INDEX_DATA_BUFFER   	 @       OpaqueModels_INDEX_DATA  	 C   OpaqueModels_INDEX_BUFFERS   
 F   OpaqueModels_MATERIAL_BUFFER      F       OpaqueModels_MATERIAL_INDICES     H        
 J   MaskedModels_VERTEX_DATA_BUFFER  
 J       MaskedModels_VERTEX_DATA     	 M   MaskedModels_VERTEX_BUFFERS  
 O   MaskedModels_INDEX_DATA_BUFFER   	 O       MaskedModels_INDEX_DATA  	 R   MaskedModels_INDEX_BUFFERS   
 T   MaskedModels_MATERIAL_BUFFER      T       MaskedModels_MATERIAL_INDICES     V       G  	          G     "       G     !       H         #       H        #      H        #      H        #      H        #      H        #      H        #      H        #      G            H         #       G        G     "       G     !      G     "       G     !      G     "       G     !      H  $          H  $       #       H  $             H  $         H  $      #   @   H  $            H  $         H  $      #      H  $            H  $         H  $      #   À   H  $            H  $         H  $      #      H  $            H  $      #   @  H  $      #   P  H  $      #   `  H  $      #   h  H  $   	   #   l  G  $      G  &   "      G  &   !       H  '       #       H  '      #      H  '      #      H  '      #      H  '      #       H  '      #   $   G  '      G  )   "      G  )   !      H  *       #       H  *      #      G  +         H  ,       #       H  ,      #      G  ,      G  .   "      G  .   !      G  1   "      G  1   !      G  4   "      G  4   !      G  5   "      G  5   !      G  8   "      G  8   !       G  9         H  :       #       G  :      G  >   "      G  >   !      G  ?         H  @       #       G  @      G  C   "      G  C   !      G  E         H  F       #       G  F      G  H   "      G  H   !      G  I         H  J       #       G  J      G  M   "      G  M   !      G  N         H  O       #       G  O      G  R   "      G  R   !      G  S         H  T       #       G  T      G  V   "      G  V   !           !                       Þ     ;     	   Þ  +     
     ? 	                                          +                                   ;             
                            +                                          ;                   +                                    ;             	                                                  ;                          !            "           #           $   !   !   !   !   !   "   "   #            %      $   ;  %   &        '   #   #   #   #            (      '   ;  (   )        *           +         ,   *   +      -      ,   ;  -   .        /      0       /   ;  0   1        	 2                              3       2   ;  3   4       ;  3   5       Ý 6      7       6   ;  7   8         9         :   9   +     ;        <   :   ;      =      <   ;  =   >        ?        @   ?     A   @   ;      B      A   ;  B   C        D           E   D     F   E      G      F   ;  G   H        I         J   I     K   J   ;      L      K   ;  L   M        N        O   N     P   O   ;      Q      P   ;  Q   R        S   D     T   S      U      T   ;  U   V                6               ø                >  	   
   ý  8         áBÒmüÊQ 0z      #     È        ¶    Ü  
  SPV_EXT_descriptor_indexing 
  SPV_NV_ray_tracing       GLSL.std.450               Ã     main    F   I   Ü        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\RayTracedShadows_RayAnyHit.glsl  Ö   Ì     // OpModuleProcessed entry-point main
// OpModuleProcessed client vulkan100
// OpModuleProcessed target-env vulkan1.0
// OpModuleProcessed entry-point main
#line 1
#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_NV_ray_tracing : require
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

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 2) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
};

layout (set = 1, binding = 3) uniform sampler SAMPLER;

layout (set = 1, binding = 4, rgba32f) uniform image2D SceneFeatures2Half; 
layout (set = 1, binding = 5, rgba32f) uniform image2D Shadows; 

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

//Constants.
#define LIGHT_TYPE_DIRECTIONAL (0)
#define LIGHT_TYPE_POINT (1)
#define LIGHT_TYPE_BOX (2)

#define LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT (BIT(0))
#define LIGHT_PROPERTY_VOLUMETRIC_BIT (BIT(1))
#define LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT (BIT(2))

/*
*	Light struct definition.
*/
struct Light
{
	/*
	*	First transform data.
	*	Direction for directional lights, position for point lights, minimum world position for box lights.
	*/
	vec3 _TransformData1;

	/*
	*	Second transform data.
	*	Maximum world position for box lights.
	*/
	vec3 _TransformData2;
	vec3 _Color;
	uint _LightType;
	uint _LightProperties;
	float _Intensity;
	float _Radius;
	float _Size;
};

/*
*	Unpacks the light at the given index.
*   Requies the Lighting storage buffer to be included.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 light_data_1 = LIGHT_DATA[index * 4 + 0];
  	vec4 light_data_2 = LIGHT_DATA[index * 4 + 1];
  	vec4 light_data_3 = LIGHT_DATA[index * 4 + 2];
  	vec4 light_data_4 = LIGHT_DATA[index * 4 + 3];

  	light._TransformData1 = vec3(light_data_1.x, light_data_1.y, light_data_1.z);
  	light._TransformData2 = vec3(light_data_1.w, light_data_2.x, light_data_2.y);
  	light._Color = vec3(light_data_2.z, light_data_2.w, light_data_3.x);
  	light._LightType = floatBitsToUint(light_data_3.y);
  	light._LightProperties = floatBitsToUint(light_data_3.z);
  	light._Intensity = light_data_3.w;
  	light._Radius = light_data_4.x;
  	light._Size = light_data_4.y;

	return light;
}

layout (set = 2, binding = 0) uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;
layout (set = 2, binding = 1) buffer OpaqueModels_VERTEX_DATA_BUFFER { vec4 OpaqueModels_VERTEX_DATA[]; } OpaqueModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 2) buffer OpaqueModels_INDEX_DATA_BUFFER { uint OpaqueModels_INDEX_DATA[]; } OpaqueModels_INDEX_BUFFERS[];
layout (set = 2, binding = 3) buffer OpaqueModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] OpaqueModels_MATERIAL_INDICES; };
layout (set = 2, binding = 4) buffer MaskedModels_VERTEX_DATA_BUFFER { vec4 MaskedModels_VERTEX_DATA[]; } MaskedModels_VERTEX_BUFFERS[];
layout (set = 2, binding = 5) buffer MaskedModels_INDEX_DATA_BUFFER { uint MaskedModels_INDEX_DATA[]; } MaskedModels_INDEX_BUFFERS[];
layout (set = 2, binding = 6) buffer MaskedModels_MATERIAL_BUFFER { layout (offset = 0) uvec4[] MaskedModels_MATERIAL_INDICES; };
hitAttributeNV vec3 HIT_ATTRIBUTE;

struct HitVertexInformation
{
	vec3 _Position;
	vec3 _Normal;
	vec3 _Tangent;
	vec2 _TextureCoordinate;
};

HitVertexInformation GetHitVertexInformation()
{
	HitVertexInformation hit_vertex_information;
	uint vertex_index_0 = MaskedModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_INDEX_DATA[gl_PrimitiveID * 3 + 0];
	vec4 vertex_data_0_0 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_0 + 0];
	vec4 vertex_data_0_1 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_0 + 1];
	vec4 vertex_data_0_2 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_0 + 2];
	uint vertex_index_1 = MaskedModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_INDEX_DATA[gl_PrimitiveID * 3 + 1];
	vec4 vertex_data_1_0 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_1 + 0];
	vec4 vertex_data_1_1 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_1 + 1];
	vec4 vertex_data_1_2 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_1 + 2];
	uint vertex_index_2 = MaskedModels_INDEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_INDEX_DATA[gl_PrimitiveID * 3 + 2];
	vec4 vertex_data_2_0 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_2 + 0];
	vec4 vertex_data_2_1 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_2 + 1];
	vec4 vertex_data_2_2 = MaskedModels_VERTEX_BUFFERS[gl_InstanceCustomIndexNV].MaskedModels_VERTEX_DATA[3 * vertex_index_2 + 2];
	vec3 barycentric_coordinates = vec3(1.0f - HIT_ATTRIBUTE.x - HIT_ATTRIBUTE.y, HIT_ATTRIBUTE.x, HIT_ATTRIBUTE.y);
	hit_vertex_information._Position = vec3(vertex_data_0_0.x, vertex_data_0_0.y, vertex_data_0_0.z) * barycentric_coordinates[0] + vec3(vertex_data_1_0.x, vertex_data_1_0.y, vertex_data_1_0.z) * barycentric_coordinates[1] + vec3(vertex_data_2_0.x, vertex_data_2_0.y, vertex_data_2_0.z) * barycentric_coordinates[2];
	hit_vertex_information._Position = gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 1.0f);
	hit_vertex_information._Normal = vec3(vertex_data_0_0.w, vertex_data_0_1.x, vertex_data_0_1.y) * barycentric_coordinates[0] + vec3(vertex_data_1_0.w, vertex_data_1_1.x, vertex_data_1_1.y) * barycentric_coordinates[1] + vec3(vertex_data_2_0.w, vertex_data_2_1.x, vertex_data_2_1.y) * barycentric_coordinates[2];
	hit_vertex_information._Normal = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 0.0f));
	hit_vertex_information._Tangent = vec3(vertex_data_0_1.z, vertex_data_0_1.w, vertex_data_0_2.x) * barycentric_coordinates[0] + vec3(vertex_data_1_1.z, vertex_data_1_1.w, vertex_data_1_2.x) * barycentric_coordinates[1] + vec3(vertex_data_2_1.z, vertex_data_2_1.w, vertex_data_2_2.x) * barycentric_coordinates[2];
	hit_vertex_information._Tangent = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Tangent, 0.0f));
	hit_vertex_information._TextureCoordinate = vec2(vertex_data_0_2.y, vertex_data_0_2.z) * barycentric_coordinates[0] + vec2(vertex_data_1_2.y, vertex_data_1_2.z) * barycentric_coordinates[1] + vec2(vertex_data_2_2.y, vertex_data_2_2.z) * barycentric_coordinates[2];
	return hit_vertex_information;
}

uint GetHitMaterialIndex()
{
	return MaskedModels_MATERIAL_INDICES[gl_InstanceCustomIndexNV / 4][gl_InstanceCustomIndexNV & 3];
}

void main()
{
    HitVertexInformation hit_vertex_information = GetHitVertexInformation();
    uint hit_material_index = GetHitMaterialIndex();
    float opacity;
    EVALUATE_OPACITY(MATERIALS[hit_material_index], hit_vertex_information._TextureCoordinate, SAMPLER, opacity);
    if (opacity < 0.5f)
    {
        ignoreIntersectionNV();
    }
}
     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive   GL_NV_ray_tracing        main         UnpackColor(u1;      color        HitVertexInformation             _Position           _Normal         _Tangent            _TextureCoordinate   	    GetHitVertexInformation(         GetHitMaterialIndex(         unpacked      ?   vertex_index_0   
 A   MaskedModels_INDEX_DATA_BUFFER   	 A       MaskedModels_INDEX_DATA  	 D   MaskedModels_INDEX_BUFFERS   	 F   gl_InstanceCustomIndexNV      I   gl_PrimitiveID    Q   vertex_data_0_0  
 S   MaskedModels_VERTEX_DATA_BUFFER  
 S       MaskedModels_VERTEX_DATA     	 V   MaskedModels_VERTEX_BUFFERS   ^   vertex_data_0_1   e   vertex_data_0_2   l   vertex_index_1    t   vertex_data_1_0   {   vertex_data_1_1      vertex_data_1_2      vertex_index_2       vertex_data_2_0      vertex_data_2_1      vertex_data_2_2   §   barycentric_coordinates   ª   HIT_ATTRIBUTE     ¸   hit_vertex_information    Ü   gl_ObjectToWorldNV   
 ^  MaskedModels_MATERIAL_BUFFER      ^      MaskedModels_MATERIAL_INDICES     `        j  hit_vertex_information    l  hit_material_index    n  Material      n      _Properties   n     _AlbedoThickness     	 n     _NormalMapDisplacement    n     _MaterialProperties   n     _Opacity      n     _EmissiveMultiplier   n     _Padding1     n     _Padding2     q  GlobalMaterials   q      MATERIALS     s        }  opacity     TEXTURES        SAMPLER     param     ¢  BLUE_NOISE_TEXTURES   ¦  SKY_TEXTURE   ¨  Camera   	 ¨      WORLD_TO_CLIP_MATRIX     	 ¨     WORLD_TO_CAMERA_MATRIX    ¨     PREVIOUS_WORLD_TO_CLIP_MATRIX     ¨     INVERSE_WORLD_TO_CAMERA_MATRIX    ¨     INVERSE_CAMERA_TO_CLIP_MATRIX    	 ¨     CAMERA_WORLD_POSITION    	 ¨     CAMERA_FORWARD_VECTOR    	 ¨     CURRENT_FRAME_JITTER      ¨     NEAR_PLANE    ¨  	   FAR_PLANE     ª        «  General  	 «      FULL_MAIN_RESOLUTION      «     INVERSE_FULL_MAIN_RESOLUTION     	 «     HALF_MAIN_RESOLUTION      «     INVERSE_HALF_MAIN_RESOLUTION      «     FRAME     «     VIEW_DISTANCE     ­        ®  LightingHeader    ®      _NumberOfLights   ®     _MaximumNumberOfShadowCastingLights   °  Lighting      °      LIGHTING_HEADER   °     LIGHT_DATA    ²        µ  SceneFeatures2Half    ¶  Shadows   ¹  TOP_LEVEL_ACCELERATION_STRUCTURE     
 »  OpaqueModels_VERTEX_DATA_BUFFER  
 »      OpaqueModels_VERTEX_DATA     	 ¾  OpaqueModels_VERTEX_BUFFERS  
 À  OpaqueModels_INDEX_DATA_BUFFER   	 À      OpaqueModels_INDEX_DATA  	 Ã  OpaqueModels_INDEX_BUFFERS   
 Å  OpaqueModels_MATERIAL_BUFFER      Å      OpaqueModels_MATERIAL_INDICES     Ç      G  @         H  A       #       G  A      G  D   "      G  D   !      G  F      Ï  G  I         G  R         H  S       #       G  S      G  V   "      G  V   !      G  Ü      Ò  G  ]        H  ^      #       G  ^     G  `  "      G  `  !      H  n      #       H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      H  n     #      G  p         H  q      #       G  q     G  s  "       G  s  !      G    "       G    !       G    "      G    !      G  ¢  "       G  ¢  !      G  ¦  "       G  ¦  !      H  ¨         H  ¨      #       H  ¨            H  ¨        H  ¨     #   @   H  ¨           H  ¨        H  ¨     #      H  ¨           H  ¨        H  ¨     #   À   H  ¨           H  ¨        H  ¨     #      H  ¨           H  ¨     #   @  H  ¨     #   P  H  ¨     #   `  H  ¨     #   h  H  ¨  	   #   l  G  ¨     G  ª  "      G  ª  !       H  «      #       H  «     #      H  «     #      H  «     #      H  «     #       H  «     #   $   G  «     G  ­  "      G  ­  !      H  ®      #       H  ®     #      G  ¯        H  °      #       H  °     #      G  °     G  ²  "      G  ²  !      G  µ  "      G  µ  !      G  ¶  "      G  ¶  !      G  ¹  "      G  ¹  !       G  º        H  »      #       G  »     G  ¾  "      G  ¾  !      G  ¿        H  À      #       G  À     G  Ã  "      G  Ã  !      G  Ä        H  Å      #       G  Å     G  Ç  "      G  Ç  !           !                                   	         
   	      !     
           	           	                       !        !                 
   +        ÿ   +  	        C+                !      	     $          +  $   %      +     *      +  $   -      +     2      +  $   5      +     :        @        A   @     B   A      C      B   ;  C   D         E      $   ;  E   F      +  $   H       ;  E   I      +  $   K         N           R   
     S   R     T   S      U      T   ;  U   V         [      
   +  $   p      +  $            ¦         +  	   ¨     ?   ©   Û     ;  ©   ª   Û     «   Û  	      ·           Ú            Û      Ú   ;  Û   Ü      +  	   
         W          \          ]  \    ^  ]     _     ^  ;  _  `     +  $   b      
 n                 	         +     o       p  n  o    q  p     r     q  ;  r  s     +     w  @     y   	 ~  	                         +              ~               ;                   ~                 ;              ~  +  	        ?+                       ¡         ;  ¡  ¢       	 £  	                           ¤  £     ¥      ¤  ;  ¥  ¦        §  
        ¨  §  §  §  §  §           	   	      ©     ¨  ;  ©  ª       «                 	      ¬     «  ;  ¬  ­       ®          ¯  
     °  ®  ¯     ±     °  ;  ±  ²      	 ³  	                           ´      ³  ;  ´  µ      ;  ´  ¶      Ý ·     ¸      ·  ;  ¸  ¹        º  
     »  º    ¼  »  *      ½     ¼  ;  ½  ¾       ¿       À  ¿    Á  À  *      Â     Á  ;  Â  Ã       Ä  \    Å  Ä     Æ     Å  ;  Æ  Ç          ´     6               ø     ;  ·   j     ;     l     ;  !   }     ;               ¶      9     k     >  j  k       ·      9     m     >  l  m       ¹      =     t  l  A  N   u  s  H   t  H   =     v  u  Ç     x  v  w  ª  y  z  x  w  ÷  |      ú  z  {    ø  {       ¹      =       l  A  N     s  H     b  =         A          =  ~      =        V          A  W    j  K   =         X  
            
  Q  	           >  }    ù  |  ø         ¹      =       l  A  N     s  H     b  =         >      9  
          Q  	           >  }    ù  |  ø  |       º      =  	     }  ¸  y        ÷        ú        ø         ¼      × ù    ø    ý  8       ±      6  
             7        ø     ;                µ       =           Ç              p  	           	            A  !   "          >  "           ¶       =     #      Â     &   #   %   Ç     '   &      p  	   (   '     	   )   (      A  !   +      *   >  +   )        ·       =     ,      Â     .   ,   -   Ç     /   .      p  	   0   /     	   1   0      A  !   3      2   >  3   1        ¸       =     4      Â     6   4   5   Ç     7   6      p  	   8   7     	   9   8      A  !   ;      :   >  ;   9        º       =  
   <      þ  <   8         .   6               ø     ;     ?      ;     Q      ;     ^      ;     e      ;     l      ;     t      ;     {      ;           ;           ;           ;           ;           ;  ¦   §      ;  ·   ¸                 =  $   G   F   =  $   J   I     $   L   J   K     $   M   L   H   A  N   O   D   G   H   M   =     P   O   >  ?   P              =  $   W   F   =     X   ?        Y   :   X        Z   Y       A  [   \   V   W   H   Z   =  
   ]   \   >  Q   ]              =  $   _   F   =     `   ?        a   :   `        b   a   *   A  [   c   V   _   H   b   =  
   d   c   >  ^   d              =  $   f   F   =     g   ?        h   :   g        i   h   2   A  [   j   V   f   H   i   =  
   k   j   >  e   k              =  $   m   F   =  $   n   I     $   o   n   K     $   q   o   p   A  N   r   D   m   H   q   =     s   r   >  l   s              =  $   u   F   =     v   l        w   :   v        x   w       A  [   y   V   u   H   x   =  
   z   y   >  t   z              =  $   |   F   =     }   l        ~   :   }           ~   *   A  [      V   |   H      =  
         >  {                 =  $      F   =        l           :                 2   A  [      V      H      =  
         >                    =  $      F   =  $      I     $         K     $            A  N      D      H      =           >             ¡      =  $      F   =                   :                     A  [      V      H      =  
         >             ¢      =  $      F   =                   :                 *   A  [      V      H      =  
         >             £      =  $       F   =     ¡           ¢   :   ¡        £   ¢   2   A  [   ¤   V       H   £   =  
   ¥   ¤   >     ¥        ¤      A  «   ¬   ª       =  	   ­   ¬     	   ®   ¨   ­   A  «   ¯   ª   *   =  	   °   ¯     	   ±   ®   °   A  «   ²   ª       =  	   ³   ²   A  «   ´   ª   *   =  	   µ   ´   P     ¶   ±   ³   µ   >  §   ¶        ¥      A  !   ¹   Q       =  	   º   ¹   A  !   »   Q   *   =  	   ¼   »   A  !   ½   Q   2   =  	   ¾   ½   P     ¿   º   ¼   ¾   A  !   À   §       =  	   Á   À        Â   ¿   Á   A  !   Ã   t       =  	   Ä   Ã   A  !   Å   t   *   =  	   Æ   Å   A  !   Ç   t   2   =  	   È   Ç   P     É   Ä   Æ   È   A  !   Ê   §   *   =  	   Ë   Ê        Ì   É   Ë        Í   Â   Ì   A  !   Î          =  	   Ï   Î   A  !   Ð      *   =  	   Ñ   Ð   A  !   Ò      2   =  	   Ó   Ò   P     Ô   Ï   Ñ   Ó   A  !   Õ   §   2   =  	   Ö   Õ        ×   Ô   Ö        Ø   Í   ×   A  ¦   Ù   ¸   H   >  Ù   Ø        ¦      =  Ú   Ý   Ü   A  ¦   Þ   ¸   p   =     ß   Þ   Q  	   à   ß       Q  	   á   ß      Q  	   â   ß      P  
   ã   à   á   â   ¨        ä   Ý   ã   A  ¦   å   ¸   H   >  å   ä        §      A  !   æ   Q   :   =  	   ç   æ   A  !   è   ^       =  	   é   è   A  !   ê   ^   *   =  	   ë   ê   P     ì   ç   é   ë   A  !   í   §       =  	   î   í        ï   ì   î   A  !   ð   t   :   =  	   ñ   ð   A  !   ò   {       =  	   ó   ò   A  !   ô   {   *   =  	   õ   ô   P     ö   ñ   ó   õ   A  !   ÷   §   *   =  	   ø   ÷        ù   ö   ø        ú   ï   ù   A  !   û      :   =  	   ü   û   A  !   ý          =  	   þ   ý   A  !   ÿ      *   =  	      ÿ   P       ü   þ      A  !     §   2   =  	                         ú     A  ¦     ¸   p   >           ¨      =  Ú     Ü   A  ¦     ¸   p   =     	    Q  	     	      Q  	     	     Q  	     	     P  
           
                       E     A  ¦     ¸   p   >           ©      A  !     ^   2   =  	       A  !     ^   :   =  	       A  !     e       =  	       P             A  !     §       =  	                  A  !     {   2   =  	       A  !     {   :   =  	       A  !             =  	   !     P     "      !  A  !   #  §   *   =  	   $  #       %  "  $       &    %  A  !   '     2   =  	   (  '  A  !   )     :   =  	   *  )  A  !   +         =  	   ,  +  P     -  (  *  ,  A  !   .  §   2   =  	   /  .       0  -  /       1  &  0  A  ¦   2  ¸      >  2  1       ª      =  Ú   3  Ü   A  ¦   4  ¸      =     5  4  Q  	   6  5      Q  	   7  5     Q  	   8  5     P  
   9  6  7  8  
       :  3  9       ;     E   :  A  ¦   <  ¸      >  <  ;       «      A  !   =  e   *   =  	   >  =  A  !   ?  e   2   =  	   @  ?  P     A  >  @  A  !   B  §       =  	   C  B       D  A  C  A  !   E     *   =  	   F  E  A  !   G     2   =  	   H  G  P     I  F  H  A  !   J  §   *   =  	   K  J       L  I  K       M  D  L  A  !   N     *   =  	   O  N  A  !   P     2   =  	   Q  P  P     R  O  Q  A  !   S  §   2   =  	   T  S       U  R  T       V  M  U  A  W  X  ¸   K   >  X  V       ¬      =     Y  ¸   þ  Y  8       ¯     6               ø          ±      =  $   a  F     $   c  a  b  =  $   d  F   Ç  $   e  d  K   A  N   f  `  H   c  e  =     g  f  þ  g  8         KTðÒD	(ÈDüÅ^ùú       ,ì'Þë©       Õ]°ô¹ Më·ñR8Ý4j                                                    v®LÂùø