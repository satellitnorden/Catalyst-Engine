//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRandomUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_MINIMUM_BIAS (0.001f)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_MAXIMUM_BIAS (0.01f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint AMBIENT_OCCLUSION_NUMBER_OF_SAMPLES;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_2_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	The probability density function.
*/
float ProbabilityDensityFunction(vec3 pre_rotation_hemisphere_direction, vec2 sample_screen_coordinate)
{
	return max(dot(pre_rotation_hemisphere_direction, vec3(0.0f, 0.0f, 1.0f)), 0.0f) * float(ValidCoordinate(sample_screen_coordinate));
}

void CatalystShaderMain()
{
	//Sample the scene features.
	vec4 scene_features = texture(scene_features_2_texture, fragment_texture_coordinate);
	vec3 normal = scene_features.xyz;
	float depth = scene_features.w;

	//Calculate the world position at this fragment.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, depth);

	//Calculate the bias.
	float bias = mix(SCREEN_SPACE_AMBIENT_OCCLUSION_MAXIMUM_BIAS, SCREEN_SPACE_AMBIENT_OCCLUSION_MINIMUM_BIAS, depth);

	//Sample the noise texture.
	vec4 noise_texture_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

	//Calculate the random rotation matrix.
	mat3 random_rotation = CalculateGramSchmidtRotationMatrix(normal, noise_texture_sample.xyz * 2.0f - 1.0f);

	//Calculate the random hemisphere sample start index.
	uint random_hemisphere_sample_start_index = uint(noise_texture_sample.w * 64.0f);

	//Keep track of the total weight.
	float total_weight = 0.0f;

	//Calculate the occlusion.
	float occlusion = 0.0f;

	for (uint i = 0; i < AMBIENT_OCCLUSION_NUMBER_OF_SAMPLES; ++i)
	{
		//Sample the random direction and length.
		vec3 pre_rotation_hemisphere_direction;
		float random_length;

		SampleHammersleyHemisphereSample(random_hemisphere_sample_start_index + i + uint(gl_FragCoord.x) + uint(gl_FragCoord.y), pre_rotation_hemisphere_direction, random_length);

		//Rotate the random direction.
		vec3 random_direction = random_rotation * pre_rotation_hemisphere_direction;

		//Flip the direction, if needed.
		random_direction = dot(random_direction, normal) >= 0.0f ? random_direction : -random_direction;

		//Calculate the sample position.
		vec3 sample_position = world_position + random_direction * random_length * AMBIENT_OCCLUSION_RADIUS;

		//Calculate the sample screen coordinate.
		vec4 sample_view_space_position = WORLD_TO_CLIP_MATRIX * vec4(sample_position, 1.0f);
		float sample_screen_coordinate_inverse_denominator = 1.0f / sample_view_space_position.w;
		vec2 sample_screen_coordinate = sample_view_space_position.xy * sample_screen_coordinate_inverse_denominator * 0.5f + 0.5f;
		float expected_view_distance = CalculateViewSpacePosition(fragment_texture_coordinate, sample_view_space_position.z * sample_screen_coordinate_inverse_denominator).z;

		//Sample the samplescene features.
		vec4 sample_scene_features = texture(scene_features_2_texture, sample_screen_coordinate);
		float sample_view_distance = CalculateViewSpacePosition(fragment_texture_coordinate, sample_scene_features.w).z - bias;

		//Calculate the distance falloff.
		float distance_falloff = SmoothStep(1.0f - min(abs(expected_view_distance - sample_view_distance), 1.0f));

		//Calculate the sample weight.
		float sample_weight = ProbabilityDensityFunction(pre_rotation_hemisphere_direction, sample_screen_coordinate);

		//If the expected hit distance is greater then the sample hit distance, there is occlusion.
		occlusion += float(expected_view_distance < sample_view_distance) * distance_falloff * sample_weight;
		total_weight += sample_weight;
	}

	//Normalize the ambient occlusion.
	occlusion = total_weight != 0.0f ? 1.0f - (occlusion / total_weight) : 1.0f;

	//Bias the occlusion.
	occlusion *= occlusion;

    //Write the fragment
    fragment = vec4(vec3(occlusion), 1.0f);
}