//Includes.
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define FIREFLY_REDUCTION_SIZE (3.0f)
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
layout (set = 1, binding = 0) uniform sampler2D diffuseIrradianceTexture;
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures2Texture;

//Out parameters.
layout (location = 0) out vec4 diffuseIrradiance;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);

	SceneFeatures features;

	features.normal = sceneFeatures2.xyz;
	features.hitPosition = PERCEIVER_WORLD_POSITION + CalculateRayDirection(coordinate) * sceneFeatures2.w;

	return features;
}

void CatalystShaderMain()
{
	//Sample the scene features at the current fragment.
	vec3 currentDiffuseIrradiance = texture(diffuseIrradianceTexture, fragmentTextureCoordinate).rgb;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample neighboring fragments.
	vec3 minimum = vec3(0.0f);

	for (float x = -FIREFLY_REDUCTION_START_END; x <= FIREFLY_REDUCTION_START_END; ++x)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;
	
		vec3 sampleDiffuseIrradiance = texture(diffuseIrradianceTexture, sampleCoordinate).rgb;
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

		minimum = mix(minimum, min(minimum, sampleDiffuseIrradiance), sampleWeight);
	}
					
	//Calculate the variance.
	float variance = min(abs(CalculateAverage(minimum) - CalculateAverage(currentDiffuseIrradiance)) * FIREFLY_REDUCTION_SCALE, 1.0f);

	//Write the fragment.
	diffuseIrradiance = vec4(mix(currentDiffuseIrradiance, minimum, variance), 1.0f);
}