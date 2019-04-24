//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define INVERSE_WIDTH (1.0f / 1920.0f)
#define INVERSE_HEIGHT (1.0f / 1080.0f)
#define MAXIMUM_VARIANCE (1.0f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) bool enabled;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	if (enabled)
	{
		//Sample the current fragment.
		vec3 currentFragment = texture(sceneTexture, fragmentTextureCoordinate).rgb;

		//Sample the adjacent fragments.
		vec3 adjacentFragments[8] = vec3[]
		(
			texture(sceneTexture, fragmentTextureCoordinate + vec2(-INVERSE_WIDTH, 0.0f)).rgb,
			texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, -INVERSE_HEIGHT)).rgb,
			texture(sceneTexture, fragmentTextureCoordinate + vec2(INVERSE_WIDTH, 0.0f)).rgb,
			texture(sceneTexture, fragmentTextureCoordinate + vec2(0.0f, INVERSE_HEIGHT)).rgb,

			texture(sceneTexture, fragmentTextureCoordinate + vec2(-INVERSE_WIDTH, -INVERSE_HEIGHT)).rgb,
			texture(sceneTexture, fragmentTextureCoordinate + vec2(-INVERSE_WIDTH, INVERSE_HEIGHT)).rgb,
			texture(sceneTexture, fragmentTextureCoordinate + vec2(INVERSE_WIDTH, -INVERSE_HEIGHT)).rgb,
			texture(sceneTexture, fragmentTextureCoordinate + vec2(INVERSE_WIDTH, INVERSE_HEIGHT)).rgb
		);

		//Calculate the current average.
		float currentAverage = CalculateAverage(currentFragment);

		//Determine the minimum and maximum averages for the adjacent fragments.
		float minimumAverage;
		float maximumAverage;

		minimumAverage = maximumAverage = currentAverage;

		for (int i = 0; i < 8; ++i)
		{
			minimumAverage = min(minimumAverage, CalculateAverage(adjacentFragments[i]));
			maximumAverage = max(maximumAverage, CalculateAverage(adjacentFragments[i]));
		}

		//Calculate the variance.
		float variance = min((maximumAverage - minimumAverage) * 1.0f, 1.0f);

		//Calculate the weights based on the variance.
		float currentWeight = mix(1.0f, 0.1111111111111111f, variance);
		float indirectWeight = mix(0.0f, 0.1111111111111111f, variance);

		//Average the current fragment.
		currentFragment = 	currentFragment * currentWeight
							+ adjacentFragments[0] * indirectWeight
							+ adjacentFragments[1] * indirectWeight
							+ adjacentFragments[2] * indirectWeight
							+ adjacentFragments[3] * indirectWeight
							+ adjacentFragments[4] * indirectWeight
							+ adjacentFragments[5] * indirectWeight
							+ adjacentFragments[6] * indirectWeight
							+ adjacentFragments[7] * indirectWeight;

		//Write the fragment.
		if (gl_FragCoord.x < 100.0f && gl_FragCoord.y < 100.0f)
		{
			fragment = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		else
		{
			fragment = vec4(currentFragment, 1.0f);
		}
	}

	else
	{
		//Write the fragment.
		if (gl_FragCoord.x < 100.0f && gl_FragCoord.y < 100.0f)
		{
			fragment = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		else
		{
			fragment = texture(sceneTexture, fragmentTextureCoordinate);
		}
	}
}