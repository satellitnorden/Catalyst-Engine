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
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (8)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_MINIMUM_BIAS (0.001f)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_MAXIMUM_BIAS (0.01f)

//The random samples.
vec4 RANDOM_SAMPLES[64] = vec4[]
(
	vec4(-0.124756f, 4.35971e-08f, 0.992188f, 0.25f),
	vec4(7.52156e-08f, -0.215234f, 0.976563f, 0.125f),
	vec4(-0.195703f, -0.195703f, 0.960938f, 0.375f),
	vec4(0.230634f, -0.230634f, 0.945313f, 0.0625f),
	vec4(-0.34031f, -0.140961f, 0.929688f, 0.3125f),
	vec4(0.155206f, -0.374701f, 0.914063f, 0.1875f),
	vec4(-0.168037f, -0.405677f, 0.898438f, 0.4375f),
	vec4(0.43397f, -0.179756f, 0.882813f, 0.03125f),
	vec4(-0.488413f, -0.0971513f, 0.867188f, 0.28125f),
	vec4(0.102277f, -0.51418f, 0.851563f, 0.15625f),
	vec4(-0.30491f, -0.456331f, 0.835938f, 0.40625f),
	vec4(0.475531f, -0.317739f, 0.820313f, 0.09375f),
	vec4(-0.493642f, -0.329841f, 0.804688f, 0.34375f),
	vec4(0.341294f, -0.510782f, 0.789063f, 0.21875f),
	vec4(-0.123662f, -0.621693f, 0.773438f, 0.46875f),
	vec4(0.639935f, -0.127291f, 0.757813f, 0.015625f),
	vec4(-0.666965f, -0.0656901f, 0.742188f, 0.265625f),
	vec4(0.0673477f, -0.683792f, 0.726563f, 0.140625f),
	vec4(-0.44614f, -0.543623f, 0.710938f, 0.390625f),
	vec4(0.555568f, -0.455943f, 0.695313f, 0.078125f),
	vec4(-0.646891f, -0.34577f, 0.679688f, 0.328125f),
	vec4(0.352453f, -0.659392f, 0.664063f, 0.203125f),
	vec4(-0.220984f, -0.728488f, 0.648438f, 0.453125f),
	vec4(0.740964f, -0.224769f, 0.632813f, 0.046875f),
	vec4(-0.752936f, -0.2284f, 0.617188f, 0.296875f),
	vec4(0.231887f, -0.764428f, 0.601563f, 0.171875f),
	vec4(-0.381999f, -0.71467f, 0.585938f, 0.421875f),
	vec4(0.724435f, -0.387218f, 0.570313f, 0.109375f),
	vec4(-0.64319f, -0.527852f, 0.554688f, 0.359375f),
	vec4(0.534328f, -0.65108f, 0.539063f, 0.234375f),
	vec4(-0.0835164f, -0.847961f, 0.523438f, 0.484375f),
	vec4(0.857319f, -0.0844381f, 0.507813f, 0.0078125f),
	vec4(-0.869441f, -0.0427126f, 0.492188f, 0.257813f),
	vec4(0.0431378f, -0.878082f, 0.476563f, 0.132813f),
	vec4(-0.595963f, -0.657544f, 0.460938f, 0.382813f),
	vec4(0.663429f, -0.601297f, 0.445313f, 0.0703125f),
	vec4(-0.816282f, -0.386072f, 0.429688f, 0.320313f),
	vec4(0.389181f, -0.822855f, 0.414063f, 0.195313f),
	vec4(-0.308994f, -0.86358f, 0.398438f, 0.445313f),
	vec4(0.869823f, -0.311227f, 0.382813f, 0.0390625f),
	vec4(-0.902271f, -0.226007f, 0.367188f, 0.289063f),
	vec4(0.227469f, -0.908109f, 0.351563f, 0.164063f),
	vec4(-0.484225f, -0.807881f, 0.335938f, 0.414063f),
	vec4(0.812536f, -0.487016f, 0.320313f, 0.101563f),
	vec4(-0.765017f, -0.567375f, 0.304688f, 0.351563f),
	vec4(0.570269f, -0.768918f, 0.289063f, 0.226563f),
	vec4(-0.141138f, -0.951478f, 0.273438f, 0.476563f),
	vec4(0.955737f, -0.14177f, 0.257813f, 0.0234375f),
	vec4(-0.959728f, -0.142362f, 0.242188f, 0.273438f),
	vec4(0.142915f, -0.963454f, 0.226563f, 0.148438f),
	vec4(-0.582295f, -0.785135f, 0.210938f, 0.398438f),
	vec4(0.787739f, -0.584226f, 0.195313f, 0.0859375f),
	vec4(-0.843768f, -0.505735f, 0.179688f, 0.335938f),
	vec4(0.507137f, -0.846106f, 0.164063f, 0.210938f),
	vec4(-0.240288f, -0.959285f, 0.148438f, 0.460938f),
	vec4(0.961438f, -0.240827f, 0.132813f, 0.0546875f),
	vec4(-0.935056f, -0.334568f, 0.117188f, 0.304688f),
	vec4(0.335148f, -0.936675f, 0.101563f, 0.179688f),
	vec4(-0.425973f, -0.900645f, 0.0859375f, 0.429688f),
	vec4(0.901752f, -0.426496f, 0.0703125f, 0.117188f),
	vec4(-0.739842f, -0.670554f, 0.0546875f, 0.367188f),
	vec4(0.671046f, -0.740385f, 0.0390625f, 0.242188f),
	vec4(-0.049054f, -0.998521f, 0.0234375f, 0.492188f),
	vec4(0.998765f, -0.049066f, 0.0078125f, 0.00390625f)
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
void SampleRandom(uint index, out vec3 direction, out float length)
{
	vec4 random_sample = RANDOM_SAMPLES[(index + uint(gl_FragCoord.x) + uint(gl_FragCoord.y)) & 63];

	direction = random_sample.xyz;
	length = random_sample.w;
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
	//float bias = 0.0f;

	//Calculate the noise texture coordinate.
	vec2 noise_texture_coordinate = gl_FragCoord.xy / 64.0f + vec2(activeNoiseTextureOffsetX, activeNoiseTextureOffsetY);

	//Calculate the random rotation matrix.
	vec4 random_sample = texture(sampler2D(GLOBAL_TEXTURES[activeNoiseTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_REPEAT_INDEX]), noise_texture_coordinate);

	vec3 random_normal = random_sample.xyz * 2.0f - 1.0f;
	vec3 random_tangent = normalize(random_normal - normal * dot(random_normal, normal));
	vec3 random_bitangent = cross(random_normal, random_tangent);

	mat3 random_rotation = mat3(random_tangent, random_bitangent, random_normal);

	//Calculate the random start index.
	uint random_start_index = uint(random_sample.w * 64.0f);

	//Calculate the occlusion.
	float occlusion = 0.0f;

	for (uint i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
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