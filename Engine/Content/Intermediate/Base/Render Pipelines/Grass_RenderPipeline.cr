�nX/J1��0!���Grass_RenderPipeline                                             �      #     )                GLSL.std.450                      main       8  H  g  �  �  �  �  �  �       ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Grass_Vertex.glsl        �     // OpModuleProcessed entry-point main
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
*	Evaluates tilt/bend. Returns the offset on the Z axis.
*/
float EvaluateTiltBend(float tilt, float bend, float normalized_height)
{
	float bend_alpha = bend >= 0.5f ? ((bend - 0.5f) * 2.0f) : (bend * 2.0f);
	return mix(0.0f, tilt, mix(InverseSquare(normalized_height), Square(normalized_height), bend_alpha));
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
	layout (offset = 16) vec3 MINIMUM;
	layout (offset = 32) vec3 MAXIMUM;
	layout (offset = 48) uint MATERIAL_INDEX;
	layout (offset = 52) float VERTEX_FACTOR;
	layout (offset = 56) float THICKNESS;
	layout (offset = 60) float HEIGHT;
	layout (offset = 64) float TILT;
	layout (offset = 68) float BEND;
	layout (offset = 72) float FADE_OUT_DISTANCE;
};

layout (location = 0) in uint InInstanceData1;
layout (location = 1) in uint InInstanceData2;

layout (location = 0) out vec3 OutWorldPosition;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out float OutX;
layout (location = 3) out float OutThickness;
layout (location = 4) out float OutAmbientOcclusion;

void main()
{
    vec3 instance_position;
    instance_position.x = mix(MINIMUM.x, MAXIMUM.x, float(InInstanceData1 & 0xffff) * UINT16_RECIPROCAL);
    instance_position.y = mix(MINIMUM.y, MAXIMUM.y, float(InInstanceData1 >> 16) * UINT16_RECIPROCAL);
    instance_position.z = mix(MINIMUM.z, MAXIMUM.z, float(InInstanceData2 & 0xffff) * UINT16_RECIPROCAL);
    float instance_rotation = mix(-PI, PI, float(InInstanceData2 >> 16) * UINT16_RECIPROCAL);
    OutWorldPosition = instance_position + WORLD_GRID_DELTA;
    float distance_from_camera = length(OutWorldPosition - CAMERA_WORLD_POSITION);
    float thickness = THICKNESS + (THICKNESS * 0.01f * distance_from_camera);
    vec3 raw_vertex_position;
    {
        float odd_multiplier = float(gl_VertexIndex & 1) * 2.0f - 1.0f;
        float height = float(gl_VertexIndex >> 1) * VERTEX_FACTOR;
        raw_vertex_position = vec3(0.5f * odd_multiplier * (1.0f - Square(height)), height, 0.0f);
    }
    vec3 vertex_position = raw_vertex_position * vec3(thickness, HEIGHT, 1.0f);
    vertex_position.z = EvaluateTiltBend(TILT, BEND, raw_vertex_position.y);
    vec3 bitangent = vec3(-1.0f, 0.0f, 0.0f);
    vec3 next_position = vec3(vertex_position.x, vertex_position.y + 0.1f, EvaluateTiltBend(TILT, BEND, raw_vertex_position.y + 0.1f));
    vec3 tangent = normalize(next_position - vertex_position);
    vec3 normal = cross(bitangent, tangent);
    normal.x += raw_vertex_position.x * 2.0f;
    vertex_position = RotateYaw(vertex_position, instance_rotation);
    OutWorldPosition += vertex_position;
    OutNormal = RotateYaw(normal, instance_rotation);
    OutWorldPosition += CalculateCurrentWindDisplacement(instance_position, vertex_position, OutNormal);
    OutX = raw_vertex_position.x + 0.5f;
    OutThickness = mix(0.75f, 0.0f, raw_vertex_position.y);
    OutAmbientOcclusion = mix(0.5f, 1.0f, raw_vertex_position.y);
    uint seed = gl_InstanceIndex;
    float cull_value = RandomFloat(seed);
    float fade_opacity = clamp((distance_from_camera - (FADE_OUT_DISTANCE * 0.75f)) / (FADE_OUT_DISTANCE * 0.25f), 0.0f, 1.0f);
    bool should_be_culled = cull_value > fade_opacity;
	gl_Position = WORLD_TO_CLIP_MATRIX*vec4(OutWorldPosition*float(should_be_culled),1.0f);
}     	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         Square(f1;    
   X        InverseSquare(f1;        X    	    EvaluateTiltBend(f1;f1;f1;       tilt         bend         normalized_height        RotateYaw(vf3;f1;        X        angle     !   Hash(u1;          seed      %   RandomFloat(u1;   $   seed      /   CalculateWindDisplacement(vf3;vf3;vf3;vf4;f1;     *   world_position    +   vertex_position   ,   normal    -   wind_direction_speed      .   wind_time     5   CalculateCurrentWindDisplacement(vf3;vf3;vf3;     2   world_position    3   vertex_position   4   normal    ?   param     D   bend_alpha    V   param     Y   param     a   sine      d   cosine    g   temp      �   param     �   wind_influence    �   amplitude     �   frequency     �   vertex_influence      �   i     �   displacement         Wind             PREVIOUS_WIND_DIRECTION_SPEED           CURRENT_WIND_DIRECTION_SPEED            PREVIOUS_WIND_TIME          CURRENT_WIND_TIME               param       param       param     
  param       param       instance_position       PushConstantData            WORLD_GRID_DELTA           MINIMUM        MAXIMUM        MATERIAL_INDEX         VERTEX_FACTOR          THICKNESS          HEIGHT         TILT           BEND        	   FADE_OUT_DISTANCE                InInstanceData1   8  InInstanceData2   ?  instance_rotation     H  OutWorldPosition      O  distance_from_camera      R  Camera   	 R      WORLD_TO_CLIP_MATRIX     	 R     WORLD_TO_CAMERA_MATRIX    R     PREVIOUS_WORLD_TO_CLIP_MATRIX     R     INVERSE_WORLD_TO_CAMERA_MATRIX    R     INVERSE_CAMERA_TO_CLIP_MATRIX    	 R     CAMERA_WORLD_POSITION    	 R     CAMERA_FORWARD_VECTOR    	 R     CURRENT_FRAME_JITTER      R     NEAR_PLANE    R  	   FAR_PLANE     T        [  thickness     e  odd_multiplier    g  gl_VertexIndex    m  height    u  raw_vertex_position   x  param       vertex_position   �  param     �  param     �  param     �  bitangent     �  next_position     �  param     �  param     �  param     �  tangent   �  normal    �  param     �  param     �  OutNormal     �  param     �  param     �  param     �  param     �  param     �  OutX      �  OutThickness      �  OutAmbientOcclusion   �  seed      �  gl_InstanceIndex      �  cull_value    �  param     �  fade_opacity      �  should_be_culled      �  gl_PerVertex      �      gl_Position   �     gl_PointSize      �     gl_ClipDistance   �     gl_CullDistance   �          TEXTURES     	   GlobalTextureAverageValues   	       TEXTURE_AVERAGE_VALUES              Material            _Properties        _AlbedoThickness     	      _NormalMapDisplacement         _MaterialProperties        _Opacity           _EmissiveMultiplier        _Padding1          _Padding2       GlobalMaterials         MATERIALS               BLUE_NOISE_TEXTURES   "  SKY_TEXTURE   #  General  	 #      FULL_MAIN_RESOLUTION      #     INVERSE_FULL_MAIN_RESOLUTION     	 #     HALF_MAIN_RESOLUTION      #     INVERSE_HALF_MAIN_RESOLUTION      #     FRAME     #     VIEW_DISTANCE     %        (  SAMPLER H         #       H        #      H        #       H        #   $   G        G    "      G    !      H        #       H       #      H       #       H       #   0   H       #   4   H       #   8   H       #   <   H       #   @   H       #   D   H    	   #   H   G       G            G  8        G  H         H  R         H  R      #       H  R            H  R        H  R     #   @   H  R           H  R        H  R     #   �   H  R           H  R        H  R     #   �   H  R           H  R        H  R     #      H  R           H  R     #   @  H  R     #   P  H  R     #   `  H  R     #   h  H  R  	   #   l  G  R     G  T  "      G  T  !       G  g     *   G  �        G  �        G  �        G  �        G  �     +   H  �             H  �           H  �           H  �           G  �     G    "       G    !       G          H        #       G       G    "       G    !      H        #       H       #      H       #      H       #      H       #      H       #      H       #      H       #      G           H        #       G       G    "       G    !      G    "       G    !      G  "  "       G  "  !      H  #      #       H  #     #      H  #     #      H  #     #      H  #     #       H  #     #   $   G  #     G  %  "      G  %  !      G  (  "      G  (  !           !                             !  	         !                                        !                                       !           !  #           '            (      '   !  )               (      !  1               +     <     �?+     F      ?  G   +     N      @+     T       +     h       +     m        �          +  �   �      +     �   ��Z�+  �   �      +     �   QL�+  �   �      +     �   ���1+  �   �      +     �     �/+     �      +     �      +     �      +     �     �>+  �   �        �              '   '                    ;         +  �                '                                                     	     ;      	        	      +  �                   ;          +     "  ��  +     %  � �7+  �   .     ;    8     +     @  �I�+     A  �I@   G        ;  G  H     +  �   J         K  	        Q  '        R  Q  Q  Q  Q  Q        �            S     R  ;  S  T     +  �   U        V        +     `  
�#<   f     �   ;  f  g     +  �   q     +  �   �     +  �   �     +  �   �     +     �    ��,     �  �  T   T   +     �  ���=;  G  �        �        ;  �  �     ;  �  �     +     �    @?;  �  �     ;  f  �     +  �   �  	      �     G     �     �     �  '      �  �     �     �  ;  �  �        �     Q     	     '    	                            +                             ;              '                     ;          
                           +                                  ;               +                              ;           	                                      !         ;  !  "        #  �   �   �   �            $     #  ;  $  %       &     '      &  ;  '  (           ?     6               �     ;          ;     ?     ;     O     ;     [     ;     e     ;     m     ;     u     ;     x     ;          ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;  �  �          B      A        �   h   =         A          h   =         =     !     �     #  !  "  p     $  #  �     &  $  %       '     .       &  A     (    h   >  (  '       C      A    )    �   �   =     *  )  A    +      �   =     ,  +  =     -     �     /  -  .  p     0  /  �     1  0  %       2     .   *  ,  1  A     3    �   >  3  2       D      A    4    �   m   =     5  4  A    6      m   =     7  6  =     9  8  �     :  9  "  p     ;  :  �     <  ;  %       =     .   5  7  <  A     >    m   >  >  =       E      =     B  8  �     C  B  .  p     D  C  �     E  D  %       F     .   @  A  E  >  ?  F       F      =     I    A  K  L    J  =     M  L  �     N  I  M  >  H  N       G      =     P  H  A  V  W  T  U  =     X  W  �     Y  P  X       Z     B   Y  >  O  Z       H      A    \    U  =     ]  \  A    ^    U  =     _  ^  �     a  _  `  =     b  O  �     c  a  b  �     d  ]  c  >  [  d       K      =  �   h  g  �  �   i  h  �   o     j  i  �     k  j  N   �     l  k  <   >  e  l       L      =  �   n  g  �  �   o  n  �   o     p  o  A    r    q  =     s  r  �     t  p  s  >  m  t       M      =     v  e  �     w  F   v  =     y  m  >  x  y  9     z     x  �     {  <   z  �     |  w  {  =     }  m  P     ~  |  }  T   >  u  ~       O      =     �  u  =     �  [  A    �    �  =     �  �  P     �  �  �  <   �     �  �  �  >    �       P      A    �    �  =     �  �  >  �  �  A    �    �  =     �  �  >  �  �  A     �  u  �   =     �  �  >  �  �  9     �     �  �  �  A     �    m   >  �  �       Q      >  �  �       R      A     �    h   =     �  �  A     �    �   =     �  �  �     �  �  �  A     �  u  �   =     �  �  �     �  �  �  A    �    �  =     �  �  >  �  �  A    �    �  =     �  �  >  �  �  >  �  �  9     �     �  �  �  P     �  �  �  �  >  �  �       S      =     �  �  =     �    �     �  �  �       �     E   �  >  �  �       T      =     �  �  =     �  �       �     D   �  �  >  �  �       U      A     �  u  h   =     �  �  �     �  �  N   A     �  �  h   =     �  �  �     �  �  �  A     �  �  h   >  �  �       V      =     �    >  �  �  =     �  ?  >  �  �  9     �     �  �  >    �       W      =     �    =     �  H  �     �  �  �  >  H  �       X      =     �  �  >  �  �  =     �  ?  >  �  �  9     �     �  �  >  �  �       Y      =     �    >  �  �  =     �    >  �  �  =     �  �  >  �  �  9     �  5   �  �  �  =     �  H  �     �  �  �  >  H  �       Z      A     �  u  h   =     �  �  �     �  �  F   >  �  �       [      A     �  u  �   =     �  �       �     .   �  T   �  >  �  �       \      A     �  u  �   =     �  �       �     .   F   <   �  >  �  �       ]      =  �   �  �  |     �  �  >  �  �       ^      =     �  �  >  �  �  9     �  %   �  =     �  �  >  �  �  >  �  �       _      =     �  O  A    �    �  =     �  �  �     �  �  �  �     �  �  �  A    �    �  =     �  �  �     �  �  �   �     �  �  �       �     +   �  T   <   >  �  �       `      =     �  �  =     �  �  �  G   �  �  �  >  �  �       a      A  �  �  T  J  =  Q  �  �  =        H  =  G     �  �         <   T   �            Q             Q            Q            P  '           <   �  '     �    A  	  
  �  J  >  
    �  8       �      6            	   7     
   �          �       =     7   
   =     8   
   �     9   7   8   �  9   8       �      6            	   7        �     ;     ?           �       =     =      �     >   <   =   >  ?   >   9     @      ?   �     A   <   @   �  A   8       �  G   6               7        7        7        �     ;     D      ;     I      ;     V      ;     Y           �      =     E      �  G   H   E   F   �  K       �  H   J   P   �  J        �      =     L      �     M   L   F   �     O   M   N   >  I   O   �  K   �  P        �      =     Q      �     R   Q   N   >  I   R   �  K   �  K   =     S   I        �      >  D   S        �      =     U      =     W      >  V   W   9     X      V   =     Z      >  Y   Z   9     [      Y   =     \   D        ]      .   X   [   \        ^      .   T   U   ]   �  ^   8       �  #   6               7        7        �     ;     a      ;     d      ;     g           �      =     b           c         b   >  a   c        �      =     e           f         e   >  d   f        �      A     i      h   =     j   i   =     k   d   �     l   j   k   A     n      m   =     o   n   =     p   a   �     q   o   p   �     r   l   q   >  g   r        �      A     s      h   =     t   s        u   t   =     v   a   �     w   u   v   A     x      m   =     y   x   =     z   d   �     {   y   z   �     |   w   {   A     }      m   >  }   |        �      =     ~   g   A           h   >     ~        �      =     �      �  �   8       �     6     !          7         �  "        �      =     �       �     �   �   �   =     �       �     �   �   �   >      �        �      =     �       �     �   �   �   >      �        �      =     �       �     �   �   �   =     �       �     �   �   �   >      �        �      =     �       �     �   �   �   >      �        �      =     �       �     �   �   �   =     �       �     �   �   �   >      �        �      =     �       �     �   �   �   >      �        �      =     �       �     �   �   �   =     �       �     �   �   �   >      �        �      =     �       �  �   8       �  "   6     %       #   7     $   �  &   ;     �           �      =     �   $   >  �   �   9     �   !   �   p     �   �   �     �   �   �   �  �   8       �  �   6     /       )   7     *   7     +   7     ,   7  (   -   7     .   �  0   ;     �      ;     �      ;     �      ;     �      ;     �      ;     �           �      >  �   T        �      >  �   <        �      >  �   <        �      >  �   T               >  �   h   �  �   �  �               �  �   �       �  �   �  �               =     �   �   �  G   �   �   �   �  �   �   �   �  �              A     �   *   h   =     �   �   A     �   *   �   =     �   �   �     �   �   �   A     �   *   m   =     �   �   �     �   �   �   A     �   +   h   =     �   �   A     �   +   �   =     �   �   �     �   �   �   A     �   +   m   =     �   �   �     �   �   �   =     �   �   �     �   �   �   �     �   �   �   =     �   .   �     �   �   �   A     �   -   �   =     �   �   �     �   �   �   =     �   �   �     �   �   �        �         �   �     �   �   F   �     �   �   F   =     �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �              =     �   �   �     �   �   F   >  �   �              =     �   �   �     �   �   N   >  �   �              =     �   �   �     �   �   �   >  �   �   �  �   �  �               =     �   �   �     �   �   �   >  �   �   �  �   �  �              =  '   �   -   O  �   �   �   �          A     �   -   �   =     �   �   �  �   �   �   �   =     �   �   �  �   �   �   �   A     �   �   h   Q     �   �       >  �   �   A     �   �   m   Q     �   �      >  �   �              =     �   �        �   �   �     �   �   F   A     �   �   �   >  �   �              A     �   +   �   =     �   �   �     �   �   �        �      (   �   T   =     �   �   �     �   �   �   >  �   �              =     �   �   �  �   8       #  ]   6     5       1   7     2   7     3   7     4   �  6   ;          ;          ;          ;  (   
     ;               %      =       2   >      =       3   >      =     	  4   >    	  A        �   =  '       >  
    A          =         >      9 	      /         
    �    8  �z      #     O                GLSL.std.450                     main    �   �         "  (  +  2  5  9               	        ..\..\..\..\Catalyst-Engine\Engine\Rendering\Render Pipelines\Generated\Grass_Fragment.glsl  \   �     // OpModuleProcessed entry-point main
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
*	Evaluates tilt/bend. Returns the offset on the Z axis.
*/
float EvaluateTiltBend(float tilt, float bend, float normalized_height)
{
	float bend_alpha = bend >= 0.5f ? ((bend - 0.5f) * 2.0f) : (bend * 2.0f);
	return mix(0.0f, tilt, mix(InverseSquare(normalized_height), Square(normalized_height), bend_alpha));
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
	layout (offset = 16) vec3 MINIMUM;
	layout (offset = 32) vec3 MAXIMUM;
	layout (offset = 48) uint MATERIAL_INDEX;
	layout (offset = 52) float VERTEX_FACTOR;
	layout (offset = 56) float THICKNESS;
	layout (offset = 60) float HEIGHT;
	layout (offset = 64) float TILT;
	layout (offset = 68) float BEND;
	layout (offset = 72) float FADE_OUT_DISTANCE;
};

layout (location = 0) in vec3 InWorldPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in float InX;
layout (location = 3) in float InThickness;
layout (location = 4) in float InAmbientOcclusion;

layout (location = 0) out vec4 SceneFeatures1;
layout (location = 1) out vec4 SceneFeatures2;
layout (location = 2) out vec4 SceneFeatures3;
layout (location = 3) out vec4 SceneFeatures4;

void main()
{
    float X_factor = InX * 5.0f;
    uint X_index_1 = uint(X_factor);
    uint X_index_2 = min(X_index_1 + 1, 4);
    float X_fractional = fract(X_factor);
    vec4 albedo_thickness;
    EVALUATE_ALBEDO_THICKNESS(MATERIALS[MATERIAL_INDEX], vec2(0.0f, 0.0f), SAMPLER, albedo_thickness);
    vec4 material_properties;
    EVALUATE_MATERIAL_PROPERTIES(MATERIALS[MATERIAL_INDEX], vec2(0.0f, 0.0f), SAMPLER, material_properties);
    float ALBEDO_MULTIPLIERS[5] = float[5]
    (
        0.825f,
        1.125f,
        0.825f,
        1.125f,
        0.825f
    );
    float albedo_multiplier = mix(ALBEDO_MULTIPLIERS[X_index_1], ALBEDO_MULTIPLIERS[X_index_2], X_fractional);
    float ROUGHNESS_MULTIPLIERS[5] = float[5]
    (
        1.0f,
        0.75f,
        1.0f,
        0.75f,
        1.0f
    );
    float roughness_multiplier = mix(ROUGHNESS_MULTIPLIERS[X_index_1], ROUGHNESS_MULTIPLIERS[X_index_2], X_fractional);
    vec3 shading_normal = normalize(InNormal);
    shading_normal.xz *= gl_FrontFacing ? -1.0f : 1.0f;
    vec2 velocity = CalculateCurrentScreenCoordinate(InWorldPosition) - CalculatePreviousScreenCoordinate(InWorldPosition) - CURRENT_FRAME_JITTER;
	SceneFeatures1 = vec4(albedo_thickness.rgb*albedo_multiplier,InThickness);
	SceneFeatures2 = vec4(shading_normal,gl_FragCoord.z);
	SceneFeatures3 = material_properties*vec4(roughness_multiplier,1.0f,InAmbientOcclusion,1.0f);
	SceneFeatures4 = vec4(velocity,0.0f,0.0f);
}
    	 GL_ARB_separate_shader_objects    GL_EXT_nonuniform_qualifier  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         UnpackColor(u1;      color        CalculateCurrentScreenCoordinate(vf3;        world_position       CalculatePreviousScreenCoordinate(vf3;       world_position       unpacked      @   view_space_position   B   Camera   	 B       WORLD_TO_CLIP_MATRIX     	 B      WORLD_TO_CAMERA_MATRIX    B      PREVIOUS_WORLD_TO_CLIP_MATRIX     B      INVERSE_WORLD_TO_CAMERA_MATRIX    B      INVERSE_CAMERA_TO_CLIP_MATRIX    	 B      CAMERA_WORLD_POSITION    	 B      CAMERA_FORWARD_VECTOR    	 B      CURRENT_FRAME_JITTER      B      NEAR_PLANE    B   	   FAR_PLANE     D         P   denominator   d   view_space_position   n   denominator   �   X_factor      �   InX   �   X_index_1     �   X_index_2     �   X_fractional      �   Material      �       _Properties   �      _AlbedoThickness     	 �      _NormalMapDisplacement    �      _MaterialProperties   �      _Opacity      �      _EmissiveMultiplier   �      _Padding1     �      _Padding2     �   GlobalMaterials   �       MATERIALS     �         �   PushConstantData      �       WORLD_GRID_DELTA      �      MINIMUM   �      MAXIMUM   �      MATERIAL_INDEX    �      VERTEX_FACTOR     �      THICKNESS     �      HEIGHT    �      TILT      �      BEND      �   	   FADE_OUT_DISTANCE     �         �   albedo_thickness      �   TEXTURES      �   SAMPLER   �   param     �   material_properties   �   param     �   ALBEDO_MULTIPLIERS    �   albedo_multiplier     �   ROUGHNESS_MULTIPLIERS     �   roughness_multiplier      �   shading_normal    �   InNormal        gl_FrontFacing      velocity        InWorldPosition     param       param       SceneFeatures1    "  InThickness   (  SceneFeatures2    +  gl_FragCoord      2  SceneFeatures3    5  InAmbientOcclusion    9  SceneFeatures4   	 ?  GlobalTextureAverageValues   	 ?      TEXTURE_AVERAGE_VALUES    A        D  BLUE_NOISE_TEXTURES   H  SKY_TEXTURE   I  General  	 I      FULL_MAIN_RESOLUTION      I     INVERSE_FULL_MAIN_RESOLUTION     	 I     HALF_MAIN_RESOLUTION      I     INVERSE_HALF_MAIN_RESOLUTION      I     FRAME     I     VIEW_DISTANCE     K        L  Wind      L      PREVIOUS_WIND_DIRECTION_SPEED     L     CURRENT_WIND_DIRECTION_SPEED      L     PREVIOUS_WIND_TIME    L     CURRENT_WIND_TIME     N      H  B          H  B       #       H  B             H  B         H  B      #   @   H  B            H  B         H  B      #   �   H  B            H  B         H  B      #   �   H  B            H  B         H  B      #      H  B            H  B      #   @  H  B      #   P  H  B      #   `  H  B      #   h  H  B   	   #   l  G  B      G  D   "      G  D   !       G  �         H  �       #       H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      H  �      #      G  �          H  �       #       G  �      G  �   "       G  �   !      H  �       #       H  �      #      H  �      #       H  �      #   0   H  �      #   4   H  �      #   8   H  �      #   <   H  �      #   @   H  �      #   D   H  �   	   #   H   G  �      G  �   "       G  �   !       G  �   "      G  �   !      G  �         G          G           G           G  "        G  (        G  +        G  2        G  5        G  9        G  >        H  ?      #       G  ?     G  A  "       G  A  !      G  D  "       G  D  !      G  H  "       G  H  !      H  I      #       H  I     #      H  I     #      H  I     #      H  I     #       H  I     #   $   G  I     G  K  "      G  K  !      H  L      #       H  L     #      H  L     #       H  L     #   $   G  L     G  N  "      G  N  !           !                                   	         
   	      !     
           	                       	      !                    
   +        �   +  	        C+     !          "      	     %          +  %   &      +     +      +  %   .      +     3      +  %   6      +     ;        A   
        B   A   A   A   A   A            	   	      C      B   ;  C   D      +  %   E          F      A   +  	   J     �?+  	   ^      ?+  %   e         �      	   ;  �   �      +  	   �     �@+     �       
 �                  	         +     �        �   �   �     �   �      �      �   ;  �   �        �               	   	   	   	   	   	      �   	   �   ;  �   �   	   +  %   �         �   	         �         +     �        �    	 �   	                         +     �        �   �   �      �       �   ;  �   �       +  %   �         �       �     �      �       �   ;  �   �         �   �   +  	   �       ,     �   �   �   +     �       +     �        �   	   �      �      �   +  	   �   33S?+  	   �     �?,  �   �   �   �   �   �   �   +  	   �     @?,  �   �   J   �   J   �   J      �         ;  �   �              �   ;         +  	       ��           ;  �        +  %                           
   ;         ;  �   "     ;    (        *     
   ;  *  +     ;    2     ;  �   5     ;    9       >  
   �     ?  >     @     ?  ;  @  A       B  �   �      C      B  ;  C  D       	 E  	                           F  E     G      F  ;  G  H        I                 	      J     I  ;  J  K       L  
   
   	   	      M     L  ;  M  N          C     6               �     ;  "   �      ;     �      ;     �      ;  "   �      ;     �      ;     �      ;     �      ;     �      ;  �   �      ;  "   �      ;  �   �      ;  "   �      ;     �      ;         ;          ;               E      =  	   �   �   �  	   �   �   �   >  �   �        F      =  	   �   �   m     �   �   >  �   �        G      =     �   �   �     �   �   +        �      &   �   �   >  �   �        H      =  	   �   �     	   �      
   �   >  �   �        J      A  �   �   �   �   =     �   �   A  �   �   �   E   �   E   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        J      A  �   �   �   �   =     �   �   A  �   �   �   E   �   �   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   W  
   �   �   �   >  �   �   �  �   �  �        J      A  �   �   �   �   =     �   �   A  �   �   �   E   �   �   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        L      A  �   �   �   �   =     �   �   A  �   �   �   E   �   E   =     �   �   �     �   �   �   �  �   �   �   �   �  �       �  �   �   �   �  �        L      A  �   �   �   �   =     �   �   A  �   �   �   E   �   �   =     �   �   A  �   �   �   �   =  �   �   �   =  �   �   �   V  �   �   �   �   W  
   �   �   �   >  �   �   �  �   �  �        L      A  �   �   �   �   =     �   �   A  �   �   �   E   �   �   =     �   �   >  �   �   9  
   �      �   >  �   �   �  �   �  �        M      >  �   �        U      =     �   �   A  "   �   �   �   =  	   �   �   =     �   �   A  "   �   �   �   =  	   �   �   =  	   �   �     	   �      .   �   �   �   >  �   �        V      >  �   �        ^      =     �   �   A  "   �   �   �   =  	   �   �   =     �   �   A  "   �   �   �   =  	   �   �   =  	   �   �     	   �      .   �   �   �   >  �   �        _      =     �   �              E   �   >  �           `      =  �       �  	         J   =       �   O                  �           A  "   	  �   !   Q  	   
        >  	  
  A  "     �   3   Q  	          >           a      =         >      9            =         >      9            �           A      D     =         �           >           b      =  
     �   O                     =  	      �   �     !       =  	   #  "  Q  	   $  !      Q  	   %  !     Q  	   &  !     P  
   '  $  %  &  #  >    '       c      =     )  �   A  �   ,  +  3   =  	   -  ,  Q  	   .  )      Q  	   /  )     Q  	   0  )     P  
   1  .  /  0  -  >  (  1       d      =  
   3  �   =  	   4  �   =  	   6  5  P  
   7  4  J   6  J   �  
   8  3  7  >  2  8       e      =     :    Q  	   ;  :      Q  	   <  :     P  
   =  ;  <  �   �   >  9  =  �  8             6  
             7        �     ;                      =           �              p  	         �  	             A  "   #      !   >  #                  =     $      �     '   $   &   �     (   '      p  	   )   (   �  	   *   )      A  "   ,      +   >  ,   *              =     -      �     /   -   .   �     0   /      p  	   1   0   �  	   2   1      A  "   4      3   >  4   2              =     5      �     7   5   6   �     8   7      p  	   9   8   �  	   :   9      A  "   <      ;   >  <   :        	      =  
   =      �  =   8       _  :   6               7        �     ;     @      ;  "   P           a      A  F   G   D   E   =  A   H   G   =     I      Q  	   K   I       Q  	   L   I      Q  	   M   I      P  
   N   K   L   M   J   �  
   O   H   N   >  @   O        b      A  "   Q   @   ;   =  	   R   Q   �  	   S   J   R   >  P   S        c      =  	   T   P   =  
   U   @   O     V   U   U          �     W   V   T   A  "   X   @   !   Q  	   Y   W       >  X   Y   A  "   Z   @   +   Q  	   [   W      >  Z   [        e      =  
   \   @   O     ]   \   \          �     _   ]   ^   P     `   ^   ^   �     a   _   `   �  a   8       k  ;   6               7        �     ;     d      ;  "   n           m      A  F   f   D   e   =  A   g   f   =     h      Q  	   i   h       Q  	   j   h      Q  	   k   h      P  
   l   i   j   k   J   �  
   m   g   l   >  d   m        n      A  "   o   d   ;   =  	   p   o   �  	   q   J   p   >  n   q        o      =  	   r   n   =  
   s   d   O     t   s   s          �     u   t   r   A  "   v   d   !   Q  	   w   u       >  v   w   A  "   x   d   +   Q  	   y   u      >  x   y        q      =  
   z   d   O     {   z   z          �     |   {   ^   P     }   ^   ^   �     ~   |   }   �  ~   8                          ��KT��D	(�D��^��:"@Gs                               ��v)��%�9����Y��G� �z��.v<             	                -]��|K7X