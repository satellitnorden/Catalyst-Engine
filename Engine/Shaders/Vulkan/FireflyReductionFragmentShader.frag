//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define FIREFLY_REDUCTION_SIZE (5.0f)
#define FIREFLY_REDUCTION_START_END ((FIREFLY_REDUCTION_SIZE - 1.0f) * 0.5f)
#define FIREFLY_REDUCTION_SCALE (0.01f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 normal;
	vec3 hitPosition;
};

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 direction;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 2) uniform sampler2D temporalAccumulationDescriptionBufferTexture;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Returns if a coordinate is valid.
*/
bool ValidCoordinate(vec2 coordinate)
{
	return 	coordinate.x >= 0.0f
			&& coordinate.x < 1.0f
			&& coordinate.y >= 0.0f
			&& coordinate.y < 1.0f;
}

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.normal = UnpackNormal(sceneFeatures2.y);
	features.hitPosition = perceiverWorldPosition + CalculateRayDirection(coordinate) * sceneFeatures2.z;

	return features;
}

void main()
{
	//Sample the scene features at the current fragment.
	vec3 currentScene = texture(sceneTexture, fragmentTextureCoordinate).rgb;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample neighboring fragments.
	vec3 minimum = vec3(0.0f);

	for (float x = -FIREFLY_REDUCTION_START_END; x <= FIREFLY_REDUCTION_START_END; ++x)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;
	
		vec3 sampleScene = texture(sceneTexture, sampleCoordinate).rgb;
		SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. How closely aligned are the normals to each other?
		*	2. How closely aligned are the hit positions to each other?
		*	3. Is the sample coordinate a valid coordinate?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= max(dot(currentFeatures.normal, sampleFeatures.normal), 0.0f);
		sampleWeight *= 1.0f - min(length(currentFeatures.hitPosition - sampleFeatures.hitPosition), 1.0f);
		sampleWeight *= float(ValidCoordinate(sampleCoordinate));

		minimum = mix(minimum, min(minimum, sampleScene), sampleWeight);
	}
					
	//Calculate the variance.
	float variance = min(abs(CalculateAverage(minimum) - CalculateAverage(currentScene)) * FIREFLY_REDUCTION_SCALE, 1.0f);

	//Write the fragment.
	scene = vec4(mix(currentScene, minimum, variance), 1.0f);
}