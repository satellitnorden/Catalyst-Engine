�nX/J1fp�����iVolumetricLighting_RenderPipeline                                �a      #     e                 GLSL.std.450                      main          0        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\VolumetricLighting_Vertex.glsl   �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform RenderingConfiguration
{
	layout (offset = 0) uint DIFFUSE_IRRADIANCE_MODE;
	layout (offset = 4) uint SPECULAR_IRRADIANCE_MODE;
	layout (offset = 8) uint VOLUMETRIC_SHADOWS_MODE;
};

layout (std140, set = 1, binding = 3) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
};

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 4) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
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

//Constants.
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

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

layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2Half;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   x        gl_VertexIndex       y        OutScreenCoordinate   .   gl_PerVertex      .       gl_Position   .      gl_PointSize      .      gl_ClipDistance   .      gl_CullDistance   0         <   TEXTURES     	 >   GlobalTextureAverageValues   	 >       TEXTURE_AVERAGE_VALUES    @         A   Material      A       _Properties   A      _AlbedoThickness     	 A      _NormalMapDisplacement    A      _MaterialProperties   A      _Opacity      A      _EmissiveMultiplier   A      _Padding1     A      _Padding2     D   GlobalMaterials   D       MATERIALS     F         K   BLUE_NOISE_TEXTURES   O   SKY_TEXTURE   R   Camera   	 R       WORLD_TO_CLIP_MATRIX     	 R      WORLD_TO_CAMERA_MATRIX    R      PREVIOUS_WORLD_TO_CLIP_MATRIX     R      INVERSE_WORLD_TO_CAMERA_MATRIX    R      INVERSE_CAMERA_TO_CLIP_MATRIX    	 R      CAMERA_WORLD_POSITION    	 R      CAMERA_FORWARD_VECTOR    	 R      CURRENT_FRAME_JITTER      R      NEAR_PLANE    R   	   FAR_PLANE     T         U   General  	 U       FULL_MAIN_RESOLUTION      U      INVERSE_FULL_MAIN_RESOLUTION     	 U      HALF_MAIN_RESOLUTION      U      INVERSE_HALF_MAIN_RESOLUTION      U      FRAME     U      VIEW_DISTANCE     W         X   RenderingConfiguration   	 X       DIFFUSE_IRRADIANCE_MODE  
 X      SPECULAR_IRRADIANCE_MODE     	 X      VOLUMETRIC_SHADOWS_MODE   Z         [   Wind      [       UPPER_SKY_COLOR   [      LOWER_SKY_COLOR   [      SKY_MODE      [      MAXIMUM_SKY_TEXTURE_MIP_LEVEL     ]         ^   LightingHeader    ^       _NumberOfLights   ^      _MaximumNumberOfShadowCastingLights   `   Lighting      `       LIGHTING_HEADER   `      LIGHT_DATA    b         d   SceneFeatures2Half  G        *   G            H  .              H  .            H  .            H  .            G  .      G  <   "       G  <   !       G  =         H  >       #       G  >      G  @   "       G  @   !      H  A       #       H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      H  A      #      G  C          H  D       #       G  D      G  F   "       G  F   !      G  K   "       G  K   !      G  O   "       G  O   !      H  R          H  R       #       H  R             H  R         H  R      #   @   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #   �   H  R            H  R         H  R      #      H  R            H  R      #   @  H  R      #   P  H  R      #   `  H  R      #   h  H  R   	   #   l  G  R      G  T   "      G  T   !       H  U       #       H  U      #      H  U      #      H  U      #      H  U      #       H  U      #   $   G  U      G  W   "      G  W   !      H  X       #       H  X      #      H  X      #      G  X      G  Z   "      G  Z   !      H  [       #       H  [      #      H  [      #       H  [      #   $   G  [      G  ]   "      G  ]   !      H  ^       #       H  ^      #      G  _         H  `       #       H  `      #      G  `      G  b   "      G  b   !      G  d   "      G  d   !           !                             +     
     ��                        ;           +           +                                  ;           +          �?+     !      ?  #           +  #   $          %         +  #   *        ,           -      *     .   ,      -   -      /      .   ;  /   0      +     1       +     4          6      ,    	 8                            +  #   9        :   8   9      ;       :   ;  ;   <         =   ,   9     >   =      ?      >   ;  ?   @       
 A   #   #   #   #   #      #   #   +  #   B        C   A   B     D   C      E      D   ;  E   F        G   8   +  #   H         I   G   H      J       I   ;  J   K        	 L                              M   L      N       M   ;  N   O         P   ,        Q           R   P   P   P   P   P   Q   Q               S      R   ;  S   T        U               #         V      U   ;  V   W        X   #   #   #      Y      X   ;  Y   Z        [   Q   Q   #         \      [   ;  \   ]        ^   #   #     _   ,     `   ^   _      a      `   ;  a   b         c       G   ;  c   d            m     6               �     ;     	      ;                o      =           �              �              o           �        
      >  	           p      =           �              �              o           �        
      >             q      =        	   �               �     "       !   A  %   &      $   >  &   "        r      =     '      �     (   '      �     )   (   !   A  %   +      *   >  +   )        s      =     2   	   =     3      P  ,   5   2   3   4      A  6   7   0   1   >  7   5   �  8  \�      #     �                GLSL.std.450                     main    �  �  k               ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\VolumetricLighting_Fragment.glsl     �   �     // OpModuleProcessed entry-point main
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

layout (std140, set = 1, binding = 2) uniform RenderingConfiguration
{
	layout (offset = 0) uint DIFFUSE_IRRADIANCE_MODE;
	layout (offset = 4) uint SPECULAR_IRRADIANCE_MODE;
	layout (offset = 8) uint VOLUMETRIC_SHADOWS_MODE;
};

layout (std140, set = 1, binding = 3) uniform Wind
{
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
};

//Lighting header struct definition.
struct LightingHeader
{
	uint _NumberOfLights;
	uint _MaximumNumberOfShadowCastingLights;	
};
layout (std430, set = 1, binding = 4) buffer Lighting
{
	layout (offset = 0) LightingHeader LIGHTING_HEADER;
	layout (offset = 16) vec4[] LIGHT_DATA;
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

//Constants.
#define DIFFUSE_IRRADIANCE_MODE_NONE (0)
#define DIFFUSE_IRRADIANCE_MODE_RAY_TRACED (1)

#define SPECULAR_IRRADIANCE_MODE_NONE (0)
#define SPECULAR_IRRADIANCE_MODE_SCREEN_SPACE (1)
#define SPECULAR_IRRADIANCE_MODE_RAY_TRACED (2)

#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)
#define VOLUMETRIC_SHADOWS_MODE_RAY_TRACED (2)

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

layout (set = 1, binding = 5) uniform sampler2D SceneFeatures2Half;

layout (location = 0) in vec2 InTextureCoordinate;

layout (location = 0) out vec4 VolumetricLighting;

void main()
{
    #define SCATTERING (vec3(0.8f, 0.9f, 1.0f) * 0.125f * 0.125f * 0.125f)
    #define NUMBER_OF_SAMPLES (8)
    #define SAMPLE_RECIPROCAL (1.0f / NUMBER_OF_SAMPLES)
    #define HALF_SAMPLE_RECIPROCAL (SAMPLE_RECIPROCAL / 2)
	vec4 scene_features_2 = texture(SceneFeatures2Half, InTextureCoordinate);
    vec3 start_position = CAMERA_WORLD_POSITION;
	vec3 world_position = CalculateWorldPosition(InTextureCoordinate, scene_features_2.w);
	float hit_distance = length(world_position - start_position);
	float hit_distance_reciprocal = 1.0f / hit_distance;
	vec3 ray_direction = (world_position - start_position) * hit_distance_reciprocal;
    float offsets[NUMBER_OF_SAMPLES];
    for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        offsets[i] = HALF_SAMPLE_RECIPROCAL + SAMPLE_RECIPROCAL * float(i);
    }
    for (uint i = 0; i < NUMBER_OF_SAMPLES; i += 4)
    {
        vec4 blue_noise_texture_sample = (SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), i / 4) - 0.5f) * (SAMPLE_RECIPROCAL - FLOAT32_EPSILON);
        offsets[i * 4 + 0] += blue_noise_texture_sample.x;
        offsets[i * 4 + 1] += blue_noise_texture_sample.y;
        offsets[i * 4 + 2] += blue_noise_texture_sample.z;
        offsets[i * 4 + 3] += blue_noise_texture_sample.w;
    }
    for (uint i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        offsets[i] *= offsets[i];
    }
	vec3 volumetric_lighting = vec3(0.0f);
    float transmittance = 1.0f;
    for (uint sample_index = 0; sample_index < NUMBER_OF_SAMPLES; ++sample_index)
    {
        float previous_offset = sample_index > 0 ? offsets[sample_index - 1] : 0.0f;
        float current_offset = offsets[sample_index];
        vec3 sample_position = mix(start_position, world_position, current_offset);
        float sample_hit_distance = (hit_distance * current_offset) - (hit_distance * previous_offset);
        float extinction = GetExtinctionAtPosition(sample_position);
        float attenuation_factor = exp(-extinction * sample_hit_distance);
        {
            float ambient_attenuation = mix(CalculateAttenuationInDirection(sample_position, vec3(0.0f, 1.0f, 0.0f), FAR_PLANE), CalculateAttenuationInDirection(sample_position, vec3(0.0f, -1.0f, 0.0f), FAR_PLANE), 0.5f);
            vec3 scattering = mix(UPPER_SKY_COLOR, LOWER_SKY_COLOR, 0.5f) * SCATTERING * (1.0f / (4.0f * 3.14f)) * ambient_attenuation;
            vec3 scattering_integral = (scattering - scattering * attenuation_factor) / max(extinction, FLOAT32_EPSILON);
            volumetric_lighting += transmittance * scattering_integral;
        }
        for (uint i = 0; i < LIGHTING_HEADER._NumberOfLights; ++i)
        {
		    Light light = UnpackLight(i);
            if (TEST_BIT(light._LightProperties, LIGHT_PROPERTY_VOLUMETRIC_BIT))
            {
                vec3 light_radiance = light._Color * light._Intensity;
                switch (light._LightType)
                {
                    case LIGHT_TYPE_DIRECTIONAL:
                    {
                        float light_attenuation = CalculateAttenuationInDirection(sample_position, -light._TransformData1, hit_distance);
                        vec3 scattering = light_radiance * SCATTERING * HenyeyGreensteinPhaseFunction(ray_direction, light._TransformData1) * light_attenuation;
                        float screen_space_occlusion = 1.0f;
                        if (VOLUMETRIC_SHADOWS_MODE == VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE)
                        {
                            vec3 screen_space_position = CalculateScreenPosition(sample_position);
                            vec3 screen_space_light_position;
                            {
                                vec3 light_world_position = (CAMERA_WORLD_POSITION + -light._TransformData1 * FAR_PLANE);
                                vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(light_world_position, 1.0f);
                                float view_space_position_coefficient_reciprocal = 1.0f / view_space_position.w;
                                view_space_position.xyz *= view_space_position_coefficient_reciprocal;
                                view_space_position.xy = clamp(view_space_position.xy, vec2(-1.0f), vec2(1.0f));
                                view_space_position.xy = view_space_position.xy * 0.5f + 0.5f;
                                view_space_position.z = LinearizeDepth(view_space_position.z);
                                screen_space_light_position = view_space_position.xyz;
                            }
                            float screen_factor = max(dot(ray_direction, -light._TransformData1), 0.0f);
                            float occlusion = 0.0f;
                            for (uint sub_sample_index = 0; sub_sample_index < 4; ++sub_sample_index)
                            {
                                vec3 expected_screen_space_position = mix(screen_space_position, screen_space_light_position, InterleavedGradientNoise(uvec2(gl_FragCoord.xy), FRAME + 1 + sample_index + sub_sample_index));
                                float sample_depth = LinearizeDepth(texture(SceneFeatures2Half, expected_screen_space_position.xy).w);
                                occlusion += float(sample_depth > expected_screen_space_position.z) * 0.25f;
                            }
                            occlusion *= occlusion * occlusion * occlusion * occlusion;
                            screen_space_occlusion = mix(1.0f, occlusion, screen_factor);
                        }
                        scattering *= screen_space_occlusion;
                        vec3 scattering_integral = (scattering - scattering * attenuation_factor) / max(extinction, FLOAT32_EPSILON);
                        volumetric_lighting += transmittance * scattering_integral;
                        break;
                    }
                }
            }
        }
        transmittance *= attenuation_factor;
    }
	VolumetricLighting = vec4(volumetric_lighting,1.0f);
}
  	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         Square(f1;    
   X    
    SampleBlueNoiseTexture(vu2;u1;       coordinate       index        LinearizeDepth(f1;       depth    
     CalculateWorldPosition(vf2;f1;       screen_coordinate        depth    
 %   CalculateScreenPosition(vf3;      $   world_position    '   Light     '       _TransformData1   '      _TransformData2   '      _Color    '      _LightType    '      _LightProperties      '      _Intensity    '      _Radius   '      _Size     *   UnpackLight(u1;   )   index     /   InterleavedGradientNoise(vu2;u1;      -   coordinate    .   frame    
 3   GetExtinctionAtPosition(vf3;      2   position      9   CalculateAttenuationInDirection(vf3;vf3;f1;   6   position      7   direction     8   distance      >   HenyeyGreensteinPhaseFunction(vf3;vf3;    <   outgoing_direction    =   incoming_direction    E   offset_index      F   General  	 F       FULL_MAIN_RESOLUTION      F      INVERSE_FULL_MAIN_RESOLUTION     	 F      HALF_MAIN_RESOLUTION      F      INVERSE_HALF_MAIN_RESOLUTION      F      FRAME     F      VIEW_DISTANCE     H         R   offset_coordinate     k   BLUE_NOISE_TEXTURES   y   Camera   	 y       WORLD_TO_CLIP_MATRIX     	 y      WORLD_TO_CAMERA_MATRIX    y      PREVIOUS_WORLD_TO_CLIP_MATRIX     y      INVERSE_WORLD_TO_CAMERA_MATRIX    y      INVERSE_CAMERA_TO_CLIP_MATRIX    	 y      CAMERA_WORLD_POSITION    	 y      CAMERA_FORWARD_VECTOR    	 y      CURRENT_FRAME_JITTER      y      NEAR_PLANE    y   	   FAR_PLANE     {         �   near_plane_coordinate     �   view_space_position   �   inverse_view_space_position_denominator   �   world_space_position      �   view_space_position   �   view_space_position_coefficient_reciprocal    �   param     �   light_data_1      �   LightingHeader    �       _NumberOfLights   �      _MaximumNumberOfShadowCastingLights   �   Lighting      �       LIGHTING_HEADER   �      LIGHT_DATA    �         �   light_data_2      �   light_data_3      �   light_data_4        light     :  x     C  y     `  param     e  attenuation   f  step_size     j  i     s  sample_position   |  param     �  G     �  dot_product   �  scene_features_2      �  SceneFeatures2Half    �  InTextureCoordinate   �  start_position    �  world_position    �  param     �  param     �  hit_distance      �  hit_distance_reciprocal   �  ray_direction     �  i     �  offsets   �  i    	 �  blue_noise_texture_sample     �  gl_FragCoord      �  param     �  param       i     *  volumetric_lighting   ,  transmittance     -  sample_index      5  previous_offset   A  current_offset    E  sample_position   K  sample_hit_distance   S  extinction    T  param     W  attenuation_factor    ]  ambient_attenuation   _  param     a  param     b  param     h  param     j  param     k  param     p  scattering    q  Wind      q      UPPER_SKY_COLOR   q     LOWER_SKY_COLOR   q     SKY_MODE      q     MAXIMUM_SKY_TEXTURE_MIP_LEVEL     s        �  scattering_integral   �  i     �  light     �  param     �  light_radiance    �  light_attenuation     �  param     �  param     �  param     �  scattering    �  param     �  param     �  screen_space_occlusion    �  RenderingConfiguration   	 �      DIFFUSE_IRRADIANCE_MODE  
 �     SPECULAR_IRRADIANCE_MODE     	 �     VOLUMETRIC_SHADOWS_MODE   �        �  screen_space_position     �  param     �  light_world_position      �  view_space_position   �  view_space_position_coefficient_reciprocal      param    	   screen_space_light_position     screen_factor       occlusion       sub_sample_index     
   expected_screen_space_position    ,  param     -  param     1  sample_depth      6  param     R  scattering_integral   k  VolumetricLighting    t  TEXTURES     	 v  GlobalTextureAverageValues   	 v      TEXTURE_AVERAGE_VALUES    x        y  Material      y      _Properties   y     _AlbedoThickness     	 y     _NormalMapDisplacement    y     _MaterialProperties   y     _Opacity      y     _EmissiveMultiplier   y     _Padding1     y     _Padding2     |  GlobalMaterials   |      MATERIALS     ~        �  SKY_TEXTURE H  F       #       H  F      #      H  F      #      H  F      #      H  F      #       H  F      #   $   G  F      G  H   "      G  H   !      G  k   "       G  k   !      H  y          H  y       #       H  y             H  y         H  y      #   @   H  y            H  y         H  y      #   �   H  y            H  y         H  y      #   �   H  y            H  y         H  y      #      H  y            H  y      #   @  H  y      #   P  H  y      #   `  H  y      #   h  H  y   	   #   l  G  y      G  {   "      G  {   !       H  �       #       H  �      #      G  �         H  �       #       H  �      #      G  �      G  �   "      G  �   !      G  �  "      G  �  !      G  �         G  �        H  q      #       H  q     #      H  q     #       H  q     #   $   G  q     G  s  "      G  s  !      H  �      #       H  �     #      H  �     #      G  �     G  �  "      G  �  !      G  k         G  t  "       G  t  !       G  u        H  v      #       G  v     G  x  "       G  x  !      H  y      #       H  y     #      H  y     #      H  y     #      H  y     #      H  y     #      H  y     #      H  y     #      G  {         H  |      #       G  |     G  ~  "       G  ~  !      G  �  "       G  �  !           !                             !  	                                                                    !                                                !                 "         !  #      "    
 '                           !  (   '      !  ,            !  1      "   !  5      "   "      !  ;      "   "     F                        G      F   ;  G   H        I          +  I   J         K         +     P      +     S       +     X       +     ]       	 g                              h   g     i   h   X      j       i   ;  j   k          m       h   +     r      B  x           y   x   x   x   x   x                     z      y   ;  z   {      +  I   |   	      }         +  I   �      +     �      @+     �     �?   �            �      x   +     �      +  I   �      +  I   �       +     �      +     �      ?  �           �        �   �   �      �      �   ;  �   �      +  I   �      +     �         �                 '   +  I        +  I   (     +  I   ,     +  I   0     +     8  @   +     >  �Ҳ@+     K  ��SB+     L  Cq�=+     O  �E�;+     X  
ף:+     Y  
�#9+     \     D+     ^      +     h    �@  q  +     �    �>+     �  �IA+     �    �?;  m   �         �        ;  �  �        �        +     �       �     �     �     �  +     �    �=+     �     >   �        ;  �  �     +     �  ���=,     +  ^  ^  ^  ,     ^  ^  �   ^  +     f    ��,     g  ^  f  ^    q                 r     q  ;  r  s     +     z  ���:+     {  ff�:+     |     ;,     }  z  {  |  +       ��=+     �     4  �              �     �  ;  �  �     ,     �  f  f  ,     �  �   �      j        ;  j  k     +     q       r  g   q     s      r  ;  s  t        u     q    v  u     w     v  ;  w  x      
 y                          +     z       {  y  z    |  {     }     |  ;  }  ~      	                              �       �      �  ;  �  �           o     6               �     ;  �   �     ;  "   �     ;  "   �     ;     �     ;     �     ;     �     ;     �     ;  "   �     ;     �     ;  �  �     ;     �     ;  �   �     ;     �     ;     �     ;          ;  "   *     ;     ,     ;     -     ;     5     ;     8     ;     A     ;  "   E     ;     K     ;     S     ;  "   T     ;     W     ;     ]     ;  "   _     ;  "   a     ;     b     ;  "   h     ;  "   j     ;     k     ;  "   p     ;  "   �     ;     �     ;    �     ;     �     ;  "   �     ;     �     ;  "   �     ;  "   �     ;     �     ;  "   �     ;  "   �     ;  "   �     ;     �     ;  "   �     ;  "   �     ;  "   �     ;  �   �     ;     �     ;          ;  "        ;          ;          ;          ;  "        ;     ,     ;     -     ;     1     ;     6     ;  "   R          u      =  h   �  �  =     �  �  W     �  �  �  >  �  �       v      A  �  �  {   (  =     �  �  >  �  �       w      =     �  �  >  �  �  A     �  �  �   =     �  �  >  �  �  9     �      �  �  >  �  �       x      =     �  �  =     �  �  �     �  �  �       �     B   �  >  �  �       y      =     �  �  �     �  �   �  >  �  �       z      =     �  �  =     �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       |      >  �  S   �  �  �  �       |      �  �  �      �  �  �  �       |      =     �  �  �  q  �  �  �  �  �  �  �  �  �       ~      =     �  �  =     �  �  p     �  �  �     �  �  �  �     �  �  �  A     �  �  �  >  �  �  �  �  �  �       |      =     �  �  �     �  �  �   >  �  �  �  �  �  �       �      >  �  S   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  �  q  �  �  �  �  �  �  �  �  �       �      =     �  �  O     �  �  �         m     �  �  =     �  �  �     �  �  �   >  �  �  >  �  �  9     �     �  �  P     �  �   �   �   �   �     �  �  �  �     �  �  �  >  �  �       �      =     �  �  �     �  �  �   �     �  �  S   A     �  �  S   =     �  �  A     �  �  �  =     �  �  �     �  �  �  A     �  �  �  >  �  �       �      =     �  �  �     �  �  �   �     �  �  ]   A     �  �  ]   =     �  �  A        �  �  =          �         �  A       �  �  >           �      =       �  �         �   �         �   A       �  �   =         A     	  �    =     
  	  �       
    A       �    >           �      =       �  �         �   �         �   A       �  �   =         A       �    =         �           A       �    >      �  �  �  �       �      =       �  �         �   >  �    �  �  �  �       �      >    S   �    �         �      �          �    �         �      =         �  q      �  �        �         �      =          =     !    A     "  �  !  =     #  "  A     $  �     =     %  $  �     &  %  #  A     '  �     >  '  &  �    �         �      =     (    �     )  (  �   >    )  �    �         �      >  *  +       �      >  ,  �        �      >  -  S   �  .  �  .       �      �  0  1      �  2  �  2       �      =     3  -  �  q  4  3  �  �  4  /  0  �  /       �      =     6  -  �  q  7  6  S   �  :      �  7  9  ?  �  9       �      =     ;  -  �     <  ;  ]   A     =  �  <  =     >  =  >  8  >  �  :  �  ?  >  8  ^  �  :  �  :  =     @  8       �      >  5  @       �      =     B  -  A     C  �  B  =     D  C  >  A  D       �      =     F  �  =     G  �  =     H  A  P     I  H  H  H       J     .   F  G  I  >  E  J       �      =     L  �  =     M  A  �     N  L  M  =     O  �  =     P  5  �     Q  O  P  �     R  N  Q  >  K  R       �      =     U  E  >  T  U  9     V  3   T  >  S  V       �      =     X  S       Y  X  =     Z  K  �     [  Y  Z       \        [  >  W  \       �      =     `  E  >  _  `  >  a  ^  A  }   c  {   |   =     d  c  >  b  d  9     e  9   _  a  b  =     i  E  >  h  i  >  j  g  A  }   l  {   |   =     m  l  >  k  m  9     n  9   h  j  k       o     .   e  n  �   >  ]  o       �      A  �  t  s  �   =     u  t  A  �  v  s  �   =     w  v  P     x  �   �   �        y     .   u  w  x  �     ~  y  }  �     �  ~    =     �  ]  �     �  �  �  >  p  �       �      =     �  p  =     �  p  =     �  W  �     �  �  �  �     �  �  �  =     �  S       �     (   �  �  P     �  �  �  �  �     �  �  �  >  �  �       �      =     �  ,  =     �  �  �     �  �  �  =     �  *  �     �  �  �  >  *  �       �      >  �  S   �  �  �  �       �      �  �  �      �  �  �  �       �      =     �  �  A  K   �  �   �   �   =     �  �  �  q  �  �  �  �  �  �  �  �  �       �      =     �  �  >  �  �  9  '   �  *   �  >  �  �       �      A     �  �  J   =     �  �  �     �  �  �   �  q  �  �  �   �  �      �  �  �  �  �  �       �      A  "   �  �    =     �  �  A     �  �  (  =     �  �  �     �  �  �  >  �  �       �      A     �  �  �   =     �  �  �  �      �  �  �      �  �  �       �      A  "   �  �  �   =     �  �       �  �  =     �  E  >  �  �  >  �  �  =     �  �  >  �  �  9     �  9   �  �  �  >  �  �       �      =     �  �  �     �  �  }  =     �  �  >  �  �  A  "   �  �  �   =     �  �  >  �  �  9     �  >   �  �  �     �  �  �  =     �  �  �     �  �  �  >  �  �       �      >  �  �        �      A  K   �  �    =     �  �  �  q  �  �  ]   �  �      �  �  �  �  �  �       �      =     �  E  >  �  �  9     �  %   �  >  �  �       �      A  �  �  {   (  =     �  �  A  "   �  �  �   =     �  �       �  �  A  }   �  {   |   =     �  �  �     �  �  �  �     �  �  �  >  �  �       �      A  �   �  {   �   =  x   �  �  =     �  �  Q     �  �      Q     �  �     Q     �  �     P     �  �  �  �  �   �     �  �  �  >  �  �       �      A     �  �  �   =     �  �  �     �  �   �  >  �  �       �      =     �  �  =     �  �  O     �  �  �            �     �  �  �  A     �  �  S   Q     �  �      >  �  �  A     �  �  ]   Q     �  �     >  �  �  A     �  �  �   Q     �  �     >  �  �       �      =     �  �  O     �  �  �              �     +   �  �  �  A     �  �  S   Q     �  �      >  �  �  A     �  �  ]   Q     �  �     >  �  �       �      =     �  �  O     �  �  �         �        �  �   P       �   �   �            A       �  S   Q             >      A       �  ]   Q            >           �      A       �  �   =     	    >    	  9     
       A       �  �   >    
       �      =       �  O                     >           �      =       �  A  "     �  �   =                  �                     (     ^  >           �      >    ^       �      >    S   �    �         �      �          �    �         �      =         �  q      �   �        �         �      =        �  =     !    =     "  �  O     #  "  "         m     $  #  A  K   %  H   J   =     &  %  �     '  &  ]   =     (  -  �     )  '  (  =     *    �     +  )  *  >  ,  $  >  -  +  9     .  /   ,  -  P     /  .  .  .       0     .      !  /  >    0       �      =  h   2  �  =     3    O     4  3  3         W     5  2  4  Q     7  5     >  6  7  9     8     6  >  1  8       �      =     9  1  A     :    �   =     ;  :  �  q  <  9  ;  �     =  <  �   ^  �     >  =  �  =     ?    �     @  ?  >  >    @  �    �         �      =     A    �     B  A  �   >    B  �    �         �      =     C    =     D    �     E  C  D  =     F    �     G  E  F  =     H    �     I  G  H  =     J    �     K  J  I  >    K       �      =     L    =     M         N     .   �   L  M  >  �  N  �  �  �  �       �      =     O  �  =     P  �  �     Q  P  O  >  �  Q       �      =     S  �  =     T  �  =     U  W  �     V  T  U  �     W  S  V  =     X  S       Y     (   X  �  P     Z  Y  Y  Y  �     [  W  Z  >  R  [       �      =     \  ,  =     ]  R  �     ^  ]  \  =     _  *  �     `  _  ^  >  *  `       �      �  �  �  �  �  �  �  �  �  �  �  �       �      =     c  �  �     d  c  �   >  �  d  �  �  �  �       �      =     e  W  =     f  ,  �     g  f  e  >  ,  g  �  1  �  1       �      =     h  -  �     i  h  �   >  -  i  �  .  �  0       �      =     l  *  Q     m  l      Q     n  l     Q     o  l     P     p  m  n  o  �   >  k  p  �  8       �      6            	   7     
   �          �       =     @   
   =     A   
   �     B   @   A   �  B   8       K  9   6               7        7        �     ;     E      ;     R           M      A  K   L   H   J   =     M   L   =     N      �     O   M   N   �     Q   O   P   >  E   Q        Q      A     T      S   =     U   T   A  K   V   H   J   =     W   V   �     Y   W   X   �     Z   Y   P   �     [   U   Z   A     \   R   S   >  \   [        R      A     ^      ]   =     _   ^   A  K   `   H   J   =     a   `   �     b   a   X   �     c   b   X   �     d   c   P   �     e   _   d   A     f   R   ]   >  f   e        T      =     l   E   A  m   n   k   l   =  h   o   n   =     p   R   p     q   p   P     s   r   r   �     t   q   s   W     u   o   t   �  u   8       Z  !   6            	   7        �          \      A  }   ~   {   |   =        ~   A  }   �   {   �   =     �   �   �     �      �   =     �      A  }   �   {   |   =     �   �   A  }   �   {   �   =     �   �   �     �   �   �   �     �   �   �   A  }   �   {   �   =     �   �   �     �   �   �   �     �   �   �   �  �   8       o  @   6                7        7        �  !   ;     �      ;  �   �      ;     �      ;  �   �           q      =     �      �     �   �   �   P     �   �   �   �     �   �   �   >  �   �        r      A  �   �   {   J   =  x   �   �   =     �   �   =     �      Q     �   �       Q     �   �      P     �   �   �   �   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   �   �     �   �   �   >  �   �        s      A     �   �   �   =     �   �   �     �   �   �   >  �   �        t      =     �   �   =     �   �   �     �   �   �   >  �   �        u      A  �   �   {   �   =  x   �   �   =     �   �   �     �   �   �   >  �   �        w      =     �   �   O     �   �   �             �  �   8       �  1   6     %       #   7  "   $   �  &   ;  �   �      ;     �      ;     �           �      A  �   �   {   �   =  x   �   �   =     �   $   Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   �   �     �   �   �   >  �   �        �      A     �   �   �   =     �   �   �     �   �   �   >  �   �        �      =     �   �   =     �   �   O     �   �   �             �     �   �   �   A     �   �   S   Q     �   �       >  �   �   A     �   �   ]   Q     �   �      >  �   �   A     �   �   �   Q     �   �      >  �   �        �      =     �   �   O     �   �   �          �     �   �   �   P     �   �   �   �     �   �   �   A     �   �   S   Q     �   �       >  �   �   A     �   �   ]   Q     �   �      >  �   �        �      A     �   �   �   =     �   �   >  �   �   9     �      �   A     �   �   �   >  �   �        �      =     �   �   O     �   �   �             �  �   8       �     6  '   *       (   7     )   �  +   ;  �   �      ;  �   �      ;  �   �      ;  �   �      ;              �      =     �   )   �     �   �   �   �     �   �   S   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �   )   �     �   �   �   �     �   �   ]   A  �   �   �   �   �   =     �   �   >  �   �        �      =     �   )   �     �   �   �   �     �   �   �   A  �   �   �   �   �   =     �   �   >  �   �        �      =        )   �          �   �         �   A  �     �   �     =         >  �          �      A       �   S   =         A     	  �   ]   =     
  	  A       �   �   =         P         
    A  "       �   >           �      A       �   �   =         A       �   S   =         A       �   ]   =         P             A  "       �   >           �      A       �   �   =         A       �   �   =         A       �   S   =         P             A  "         >           �      A        �   ]   =     !     |     "  !  A     #    �   >  #  "       �      A     $  �   �   =     %  $  |     &  %  A     '    J   >  '  &       �      A     )  �   �   =     *  )  A     +    (  >  +  *       �      A     -  �   S   =     .  -  A     /    ,  >  /  .       �      A     1  �   ]   =     2  1  A     3    0  >  3  2       �      =  '   4    �  4  8         <   6     /       ,   7     -   7     .   �  0   ;     :     ;     C          !      =     7  .   �     9  7  8  >  .   9       #      A     ;  -   S   =     <  ;  p     =  <  =     ?  .   p     @  ?  �     A  >  @  �     B  =  A  >  :  B       $      A     D  -   ]   =     E  D  p     F  E  =     G  .   p     H  G  �     I  >  H  �     J  F  I  >  C  J       &      =     M  :  �     N  L  M  =     P  C  �     Q  O  P  �     R  N  Q  �     S  R  �   �     T  K  S  �     U  T  �   �  U  8       8  ,   6     3       1   7  "   2   �  4   ;     `          <      A     Z  2   ]   =     [  Z  �     ]  [  \       _     +   ]  ^  �   >  `  _  9     a     `       b     .   X  Y  a  �  b  8       D  T   6     9       5   7  "   6   7  "   7   7     8   �  :   ;     e     ;     f     ;     j     ;  "   s     ;  "   |          H      >  e  �        I      =     g  8   �     i  g  h  >  f  i       K      >  j  S   �  k  �  k       K      �  m  n      �  o  �  o       K      =     p  j  �  q  r  p  �   �  r  l  m  �  l       M      =     t  6   =     u  7   =     v  j  p     w  v  �     x  u  w  =     y  f  �     z  x  y  �     {  t  z  >  s  {       N      =     }  s  >  |  }  9     ~  3   |         ~  =     �  f  �     �    �       �        �  =     �  e  �     �  �  �  >  e  �  �  n  �  n       K      =     �  j  �     �  �  �   >  j  �  �  k  �  m       Q      =     �  e  �  �  8       Y  U   6     >       ;   7  "   <   7  "   =   �  ?   ;     �     ;     �          [      >  �  �       \      =     �  <   =     �  =        �  �  �     �  �  �  >  �  �       ^      =     �  �  =     �  �  �     �  �  �  �     �  �   �  =     �  �  =     �  �  �     �  �  �  �     �  �   �  =     �  �  �     �  �   �  =     �  �  �     �  �  �  �     �  �  �       �        �  �  �     �  �  �  �     �  �  �  �  �  8                          ��KT��D	(�D��^������Vb�>�D_w���       ,�'��               �]����M rdin       �_`Wv�                                   \�5s����