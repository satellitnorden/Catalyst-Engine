//Includes.
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	float view_distance;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) int STRIDE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D DEPTH_TEXTURE;
layout (set = 1, binding = 1) uniform sampler2D SOURCE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	float depth = texture(DEPTH_TEXTURE, coordinate)[0];

	SceneFeatures features;

	features.view_distance = CalculateViewSpacePosition(fragment_texture_coordinate, depth).z;

	return features;
}

void CatalystShaderMain()
{
	//Sample the shadows and scene features at the current fragment.
	vec4 current_shadows = texture(SOURCE_TEXTURE, fragment_texture_coordinate);
	SceneFeatures current_features = SampleSceneFeatures(fragment_texture_coordinate);

	//Sample neighboring fragments.
	vec4 denoised_shadows = vec4(0.0f);
	float total = 0.0f;

	for (int y = -STRIDE; y <= STRIDE; y += STRIDE)
	{
		for (int x = -STRIDE; x <= STRIDE; x += STRIDE)
		{
			vec2 sample_coordinate = fragment_texture_coordinate + vec2(float(x), float(y)) * (INVERSE_SCALED_RESOLUTION * 1.0f);

			vec4 sample_shadows = texture(SOURCE_TEXTURE, sample_coordinate);
			SceneFeatures sample_features = SampleSceneFeatures(sample_coordinate);

			/*
			*	Calculate the sample weight based on certain criteria;
			*	
			*	1. Is the sample coordinate valid?
			*	2. How closely aligned are the view distances to each other?
			*/
			float sample_weight = 1.0f;

			sample_weight *= float(ValidCoordinate(sample_coordinate));
			sample_weight *= 1.0f - min(abs(current_features.view_distance - sample_features.view_distance), 1.0f);

			denoised_shadows += sample_shadows * sample_weight;
			total += sample_weight;
		}
	}
	
	//Normalize the denoised shadows
	denoised_shadows = total == 0.0f ? current_shadows : denoised_shadows / total;

	//Write the fragment.
	fragment = denoised_shadows;
}