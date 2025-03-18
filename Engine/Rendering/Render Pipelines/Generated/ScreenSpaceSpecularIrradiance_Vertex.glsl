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
	layout (offset = 0) vec3 UPPER_SKY_COLOR;
	layout (offset = 16) vec3 LOWER_SKY_COLOR;
	layout (offset = 32) uint SKY_MODE;
	layout (offset = 36) float MAXIMUM_SKY_TEXTURE_MIP_LEVEL;
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

/////////////
// GENERAL //
/////////////

/*
*   Ray struct definition.
*/
struct Ray
{

    //The origin.
    vec3 _Origin;

    //The direction.
    vec3 _Direction;

};

/*
*   Axis aligned bounding box struct definition.
*/
struct AxisAlignedBoundingBox
{

    //The minimum.
    vec3 _Minimum;

    //The maximum.
    vec3 _Maximum;

};

/*
*   Returns the axis aligned bounding box component at the given index.
*/
vec3 AxisAlignedBoundingBoxComponent(AxisAlignedBoundingBox axis_aligned_bounding_box, uint index)
{
    return index == 0 ? axis_aligned_bounding_box._Minimum : axis_aligned_bounding_box._Maximum;
}

/*
*   Performs a ray/axis aligned bounding box intersection.
*/
vec3 RayAxisAlignedBoundingBoxIntersection(Ray ray, AxisAlignedBoundingBox axis_aligned_bounding_box)
{
    //Calculate the ray reciprocals.
    vec3 ray_reciprocals = vec3(1.0f) / ray._Direction;

    //Calculate the ray signs.
    uvec3 ray_signs = uvec3(ray._Direction.x >= 0.0f ? 1 : 0, ray._Direction.y >= 0.0f ? 1 : 0, ray._Direction.z >= 0.0f ? 1 : 0);

    //Find the minimum/maximum.
    float minimum = -FLOAT32_MAXIMUM;
    float maximum = FLOAT32_MAXIMUM;

    //Test the X-axis slab.
    minimum = max(minimum, (AxisAlignedBoundingBoxComponent(axis_aligned_bounding_box, 1 - ray_signs[0]).x - ray._Origin.x) * ray_reciprocals.x);
    maximum = min(maximum, (AxisAlignedBoundingBoxComponent(axis_aligned_bounding_box, ray_signs[0]).x - ray._Origin.x) * ray_reciprocals.x);

    //Test the Y-axis slab.
    minimum = max(minimum, (AxisAlignedBoundingBoxComponent(axis_aligned_bounding_box, 1 - ray_signs[1]).y - ray._Origin.y) * ray_reciprocals.y);
    maximum = min(maximum, (AxisAlignedBoundingBoxComponent(axis_aligned_bounding_box, ray_signs[1]).y - ray._Origin.y) * ray_reciprocals.y);

    //Test the Z-axis slab.
    minimum = max(minimum, (AxisAlignedBoundingBoxComponent(axis_aligned_bounding_box, 1 - ray_signs[2]).z - ray._Origin.z) * ray_reciprocals.z);
    maximum = min(maximum, (AxisAlignedBoundingBoxComponent(axis_aligned_bounding_box, ray_signs[2]).z - ray._Origin.z) * ray_reciprocals.z);

    return ray._Origin + ray._Direction * minimum;
}

/*
*   Performs a ray/viewport intersection.
*/
vec3 RayViewportIntersection(Ray ray)
{
    //Calculate the dots.
    float horizontal_dot = dot(ray._Direction, vec3(1.0f, 0.0f, 0.0f));
    float vertical_dot = dot(ray._Direction, vec3(0.0f, 1.0f, 0.0f));
    float depth_dot = dot(ray._Direction, vec3(0.0f, 0.0f, 1.0f));

    //Perform the horizontal intersection.
    float horizontal_intersection_distance = FLOAT32_MAXIMUM;

    if (horizontal_dot > 0.0f)
    {   
        vec3 plane_position = vec3(1.0f, 0.0f, 0.0f);
        vec3 plane_normal = vec3(-1.0f, 0.0f, 0.0f);

        horizontal_intersection_distance = dot(plane_position - ray._Origin, plane_normal) / dot(ray._Direction, plane_normal);
    }

    else if (horizontal_dot < 0.0f)
    {
        vec3 plane_position = vec3(0.0f, 0.0f, 0.0f);
        vec3 plane_normal = vec3(1.0f, 0.0f, 0.0f);

        horizontal_intersection_distance = dot(plane_position - ray._Origin, plane_normal) / dot(ray._Direction, plane_normal);
    }

    //Perform the vertical intersection.
    float vertical_intersection_distance = FLOAT32_MAXIMUM;

    if (vertical_dot > 0.0f)
    {   
        vec3 plane_position = vec3(0.0f, 1.0f, 0.0f);
        vec3 plane_normal = vec3(0.0f, -1.0f, 0.0f);

        vertical_intersection_distance = dot(plane_position - ray._Origin, plane_normal) / dot(ray._Direction, plane_normal);
    }

    else if (vertical_dot < 0.0f)
    {
        vec3 plane_position = vec3(0.0f, 0.0f, 0.0f);
        vec3 plane_normal = vec3(0.0f, 1.0f, 0.0f);

        vertical_intersection_distance = dot(plane_position - ray._Origin, plane_normal) / dot(ray._Direction, plane_normal);
    }

    //Perform the depth intersection.
    float depth_intersection_distance = FLOAT32_MAXIMUM;

    if (depth_dot > 0.0f)
    {   
        vec3 plane_position = vec3(0.0f, 0.0f, FLOAT32_MAXIMUM);
        vec3 plane_normal = vec3(0.0f, 0.0f, -1.0f);

        depth_intersection_distance = dot(plane_position - ray._Origin, plane_normal) / dot(ray._Direction, plane_normal);
    }

    else if (depth_dot < 0.0f)
    {
        vec3 plane_position = vec3(0.0f, 0.0f, 0.0f);
        vec3 plane_normal = vec3(0.0f, 0.0f, 1.0f);

        depth_intersection_distance = dot(plane_position - ray._Origin, plane_normal) / dot(ray._Direction, plane_normal);
    }

    //Return the intersection point.
    return ray._Origin + ray._Direction * min(horizontal_intersection_distance, min(vertical_intersection_distance, depth_intersection_distance));
}

//The seed.
int SEED;

/*
*	Seeds the random number generator.
*/
void Seed(ivec2 coordinate, int frame)
{
	int n = frame;
    n = (n<<13)^n; n=n*(n*n*15731+789221) + 1376312589;
    n += coordinate.y;
    n = (n<<13)^n; n=n*(n*n*15731+789221) + 1376312589;
    n += coordinate.x;
    n = (n<<13)^n; n=n*(n*n*15731+789221) + 1376312589;
    SEED = n;
}

/*
*	Generates a random float with the given seed (and modifies it, so subsequent calls result in different values).
*/
float Random()
{
	SEED = SEED * 0x343fd + 0x269ec3;

	return float((SEED >> 16 & 32767)) / 32767.0f;
}

/*
*	The diffuse probability density function.
*/
float DiffuseProbabilityDensityFunction(vec3 direction, vec3 normal)
{
	return max(dot(normal, direction), 0.0f);
}

/*
*   Generates a diffuse direction.
*/
vec3 GenerateDiffuseDirection(vec3 normal)
{
    #define NUMBER_OF_CANDIDATES (64)

    vec3 candidate_directions[NUMBER_OF_CANDIDATES];
    float candidate_weights[NUMBER_OF_CANDIDATES];

    for (uint i = 0; i < NUMBER_OF_CANDIDATES; ++i)
    {
        float random_x = Random();
		float random_y = Random();

		float spherical_coordinate_x = acos(2.0f * random_x - 1.0f) - (PI / 2.0f);
		float spherical_coordinate_y = 2.0f * PI  * random_y;

		float cosine_a = cos(spherical_coordinate_x);
		float cosine_b = cos(spherical_coordinate_y);
		float sine_a = sin(spherical_coordinate_x);
		float sine_b = sin(spherical_coordinate_y);

		vec3 random_direction = vec3(cosine_a * cosine_b, cosine_a * sine_b, sine_a);
    
        random_direction = dot(normal, random_direction) >= 0.0f ? random_direction : -random_direction;

        candidate_directions[i] = random_direction;
        candidate_weights[i] = DiffuseProbabilityDensityFunction(random_direction, normal);
    }

    //Calculate the sum of the weights.
    float weights_sum = 0.0f;

    for (uint i = 0; i < NUMBER_OF_CANDIDATES; ++i)
    {
        weights_sum += candidate_weights[i];
    }

    //Randomize the picked weight.
    float picked_weight = Random() * weights_sum;

    //Pick the candidate!
    for (uint i = 0; i < NUMBER_OF_CANDIDATES; ++i)
    {
        if (picked_weight < candidate_weights[i])
        {
        	return candidate_directions[i];
        }

        else
        {
            picked_weight -= candidate_weights[i];
        }
    }

    return candidate_directions[NUMBER_OF_CANDIDATES - 1];

    #undef NUMBER_OF_CANDIDATES
}

/*
*   Generates a specular lobe direction.
*/
vec3 GenerateSpecularLobeDirection(vec2 jitter, vec3 normal, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0f * PI * jitter.x;
	float cosTheta = sqrt((1.0f - jitter.y) / (1.0f + (a*a - 1.0f) * jitter.y));
	float sinTheta = sqrt(1.0f - cosTheta*cosTheta);

	// from spherical coordinates to cartesian coordinates
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space vector to world-space sample vector
	vec3 up        = abs(normal.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tangent   = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;

	return normalize(sampleVec);
}

//Constants.
#define SKY_MODE_ATMOSPHERIC_SCATTERING (0)
#define SKY_MODE_GRADIENT (1)
#define SKY_MODE_TEXTURE (2)

/*
*	Samples the sky.
*	Requires the "World" uniform buffer to be bound.
*/
vec3 SampleSky(vec3 direction, float mip_level)
{
	//Here because ray tracing sometines passes in invalid stuff...
	if (isnan(direction.x) || isnan(direction.y) || isnan(direction.z))
	{
		return vec3(100.0f, 0.0f, 0.0f);
	}

	switch (SKY_MODE)
	{
		case SKY_MODE_ATMOSPHERIC_SCATTERING:
		{
			//Oh no!
			return vec3(0.0f);
		}

		case SKY_MODE_GRADIENT:
		{
			return mix(LOWER_SKY_COLOR, UPPER_SKY_COLOR, dot(direction, vec3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f);
		}

		case SKY_MODE_TEXTURE:
		{
			return textureLod(SKY_TEXTURE, direction, mip_level).rgb;
		}
	
		default:
		{
			//Oh no!
			return vec3(0.0f);
		}
	}
}

layout (set = 1, binding = 3) uniform sampler2D SceneFeatures2Half;
layout (set = 1, binding = 4) uniform sampler2D SceneFeatures3Half;
layout (set = 1, binding = 5) uniform sampler2D SceneFeatures4Half;
layout (set = 1, binding = 6) uniform sampler2D PreviousScene;

layout (location = 0) out vec2 OutScreenCoordinate;

void main()
{
	float x = -1.0f + float((gl_VertexIndex & 2) << 1);
    float y = -1.0f + float((gl_VertexIndex & 1) << 2);
    OutScreenCoordinate.x = (x + 1.0f) * 0.5f;
    OutScreenCoordinate.y = (y + 1.0f) * 0.5f;
	gl_Position = vec4(x,y,0.0f,1.0f);
}