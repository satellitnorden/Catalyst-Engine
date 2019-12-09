//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRandomUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (12)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_ORIGIN_BIAS (2.0f) //0.25f step.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_MINIMUM_BIAS (0.01f)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_MAXIMUM_BIAS (0.1f)

//The random samples.
vec4 RANDOM_SAMPLES[64] = vec4[]
(
	vec4(-0.904534f, -0.301511f, 0.301511f, 0.111111f),
	vec4(-0.140028f, 0.70014f, -0.70014f, 0.555556f),
	vec4(0.6289f, -0.748691f, -0.209633f, 0.703704f),
	vec4(-0.744669f, -0.0391931f, 0.666283f, 0.259259f),
	vec4(0.151934f, 0.698898f, -0.698898f, 0.407407f),
	vec4(0.606788f, -0.793492f, 0.046676f, 0.851852f),
	vec4(-0.39013f, 0.248264f, 0.886659f, 0.0123457f),
	vec4(-0.347122f, 0.402661f, -0.846977f, 0.45679f),
	vec4(0.727046f, -0.665169f, 0.17016f, 0.901235f),
	vec4(-0.877863f, -0.228485f, 0.420893f, 0.160494f),
	vec4(-0.015469f, 0.81986f, -0.572355f, 0.604938f),
	vec4(0.720933f, -0.680317f, -0.132002f, 0.753086f),
	vec4(-0.637548f, 0.0650558f, 0.767659f, 0.308642f),
	vec4(0.206655f, 0.691843f, -0.691843f, 0.358025f),
	vec4(0.463825f, -0.882763f, -0.0748104f, 0.802469f),
	vec4(-0.51496f, 0.16328f, 0.84152f, 0.0617284f),
	vec4(-0.254355f, 0.553597f, -0.79299f, 0.506173f),
	vec4(0.809963f, -0.515431f, 0.279805f, 0.950617f),
	vec4(-0.825834f, -0.139757f, 0.546321f, 0.209877f),
	vec4(0.103086f, 0.898322f, -0.427071f, 0.654321f),
	vec4(0.682765f, -0.694288f, -0.227588f, 0.670782f),
	vec4(-0.802029f, -0.107223f, 0.587582f, 0.226337f),
	vec4(0.0982905f, 0.647361f, -0.755819f, 0.374486f),
	vec4(0.512677f, -0.857846f, -0.0355321f, 0.81893f),
	vec4(-0.472987f, 0.193308f, 0.859602f, 0.0781893f),
	vec4(-0.218268f, 0.604045f, -0.766478f, 0.522634f),
	vec4(0.828872f, -0.464743f, 0.311426f, 0.967078f),
	vec4(-0.898053f, -0.27897f, 0.340114f, 0.127572f),
	vec4(-0.0988578f, 0.744035f, -0.660786f, 0.572016f),
	vec4(0.660631f, -0.727701f, -0.18444f, 0.720165f),
	vec4(-0.711451f, -0.00436473f, 0.702722f, 0.27572f),
	vec4(0.178022f, 0.722164f, -0.668422f, 0.423868f),
	vec4(0.65038f, -0.754441f, 0.0884517f, 0.868313f),
	vec4(-0.349972f, 0.273055f, 0.896081f, 0.0288066f),
	vec4(-0.318907f, 0.452181f, -0.832965f, 0.473251f),
	vec4(0.759148f, -0.616489f, 0.208893f, 0.917695f),
	vec4(-0.863631f, -0.200559f, 0.462513f, 0.176955f),
	vec4(0.0254747f, 0.850857f, -0.52478f, 0.621399f),
	vec4(0.749041f, -0.65414f, -0.105069f, 0.769547f),
	vec4(-0.597746f, 0.0989075f, 0.795561f, 0.325103f),
	vec4(0.222122f, 0.689442f, -0.689442f, 0.341564f),
	vec4(0.414861f, -0.902933f, -0.112257f, 0.786008f),
	vec4(-0.556711f, 0.131741f, 0.820193f, 0.0452675f),
	vec4(-0.287962f, 0.502714f, -0.81508f, 0.489712f),
	vec4(0.786782f, -0.566283f, 0.245556f, 0.934156f),
	vec4(-0.84634f, -0.170936f, 0.504469f, 0.193416f),
	vec4(0.0651475f, 0.876987f, -0.476078f, 0.63786f),
	vec4(0.684969f, -0.696934f, -0.21237f, 0.687243f),
	vec4(-0.774935f, -0.0735972f, 0.627741f, 0.242798f),
	vec4(0.125301f, 0.673918f, -0.728103f, 0.390947f),
	vec4(0.560613f, -0.828062f, 0.00514324f, 0.835391f),
	vec4(-0.431245f, 0.221668f, 0.874581f, 0.0946502f),
	vec4(-0.180013f, 0.653192f, -0.735483f, 0.539095f),
	vec4(0.843816f, -0.414915f, 0.340324f, 0.983539f),
	vec4(-0.889253f, -0.254632f, 0.379989f, 0.144033f),
	vec4(-0.0571222f, 0.784133f, -0.617959f, 0.588477f),
	vec4(0.691389f, -0.704879f, -0.158514f, 0.736625f),
	vec4(-0.675607f, 0.0305113f, 0.73663f, 0.292181f),
	vec4(0.203411f, 0.743618f, -0.636911f, 0.440329f),
	vec4(0.69066f, -0.711432f, 0.129823f, 0.884774f),
	vec4(-0.311032f, 0.296042f, 0.903116f, 0.00137174f),
	vec4(-0.364428f, 0.370477f, -0.854365f, 0.445816f),
	vec4(0.703245f, -0.696333f, 0.143414f, 0.890261f),
	vec4(-0.885757f, -0.246117f, 0.393524f, 0.14952f)
);

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the random direction and length at the given index.
*/
void SampleRandom(int index, out vec3 direction, out float length)
{
#if 1
	vec4 random_sample = RANDOM_SAMPLES[(index + int(gl_FragCoord.x) + int(gl_FragCoord.y)) & 63];

	direction = random_sample.xyz;
	length = random_sample.w;
#else
	vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);
	vec4 random_sample = texture(sampler2D(GLOBAL_TEXTURES[(activeNoiseTextureIndex + index) & 63], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate);

	direction = normalize(random_sample.xyz * 2.0f - 1.0f);
	length = random_sample.w;
#endif
}

void main()
{
	//Sample the scene features.
	vec4 scene_features = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec3 normal = UnpackNormal(scene_features.x);
	float depth = scene_features.w;

	//Calculate the world position at this fragment.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, depth);

	//Calculate the bias.
	float bias = mix(SCREEN_SPACE_AMBIENT_OCCLUSION_MAXIMUM_BIAS, SCREEN_SPACE_AMBIENT_OCCLUSION_MINIMUM_BIAS, depth);

	//Calculate the noise texture coordinate.
	vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);

	//Calculate the random rotation matrix.
	vec4 random_sample = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate);

	vec3 random_normal = random_sample.xyz * 2.0f - 1.0f;
	vec3 random_tangent = normalize(random_normal - normal * dot(random_normal, normal));
	vec3 random_bitangent = cross(random_normal, random_tangent);

	mat3 random_rotation = mat3(random_tangent, random_bitangent, random_normal);

	//Calculate the random start index.
	int random_start_index = int(random_sample.w * 64.0f);

	//Calculate the occlusion.
	float occlusion = 0.0f;

	for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
	{
		//Sample the random direction and length.
		vec3 random_direction;
		float random_length;

		SampleRandom(random_start_index + i, random_direction, random_length);

		//Rotate the random direction.
		random_direction = random_rotation * random_direction;

		//Flip the random direction if needed.
		random_direction = dot(random_direction, normal) >= 0.0f ? random_direction : random_direction * -1.0f;

		//Calculate the sample position.
		vec3 sample_position = world_position + random_direction * random_length;

		//Calculate the sample screen coordinate.
		vec4 sample_view_space_position = viewMatrix * vec4(sample_position, 1.0f);
		float sample_screen_coordinate_inverse_denominator = 1.0f / sample_view_space_position.w;
		vec2 sample_screen_coordinate = sample_view_space_position.xy * sample_screen_coordinate_inverse_denominator * 0.5f + 0.5f;
		float expected_view_distance = CalculateViewSpacePosition(fragment_texture_coordinate, sample_view_space_position.z * sample_screen_coordinate_inverse_denominator).z;

		//Sample the samplescene features.
		vec4 sample_scene_features = texture(scene_features_2_texture, sample_screen_coordinate);
		float sample_view_distance = CalculateViewSpacePosition(fragment_texture_coordinate, sample_scene_features.w).z - bias;

		//Calculate the distance falloff.
		float distance_falloff = SmoothStep(1.0f - min(abs(expected_view_distance - sample_view_distance), 1.0f));

		//If the expected hit distance is greater then the sample hit distance, there is occlusion.
		occlusion += float(expected_view_distance < sample_view_distance) * distance_falloff;
	}

	//Normalize the ambient occlusion.
	occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Write the fragment
    fragment = vec4(vec3(occlusion), 1.0f);
}