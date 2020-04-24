//Includes.
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystRayTracingCore.glsl"

//Constants.
#define DENOISING_SIZE (32.0f)
#define DENOISING_START_END (DENOISING_SIZE * 0.5f)

/*
*	Scene features struct definition.
*/
struct SceneFeatures
{
	vec3 albedo;
	float hitDistance;
	vec3 shadingNormal;
	float roughness;
	float metallic;
	float ambientOcclusion;
	float luminance;
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
layout (set = 1, binding = 1) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures3Texture;
layout (set = 1, binding = 4) uniform sampler2D sceneFeatures4Texture;

//Out parameters.
layout (location = 0) out vec4 scene;

/*
*	Samples the scene features at the specified coordinates.
*/
SceneFeatures SampleSceneFeatures(vec2 coordinate)
{
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, coordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, coordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, coordinate);
	vec4 sceneFeatures4 = texture(sceneFeatures4Texture, coordinate);

	SceneFeatures features;

	features.albedo = sceneFeatures1.rgb;
	features.hitDistance = sceneFeatures2.w;
	features.shadingNormal = sceneFeatures3.xyz;
	features.roughness = sceneFeatures4.x;
	features.metallic = sceneFeatures4.y;
	features.ambientOcclusion = sceneFeatures4.z;
	features.luminance = sceneFeatures4.w;

	return features;
}

void CatalystShaderMain()
{
	//Sample the scene and scene features at the current fragment.
	vec3 currentScene = texture(sceneTexture, fragmentTextureCoordinate).rgb;
	SceneFeatures currentFeatures = SampleSceneFeatures(fragmentTextureCoordinate);

	//Sample neighboring fragments.
	vec3 denoisedScene = vec3(0.0f);
	float sceneWeightSum = 0.0f;

	for (float x = -DENOISING_START_END; x <= DENOISING_START_END; ++x)
	{
		vec2 sampleCoordinate = fragmentTextureCoordinate + vec2(x, x) * direction;

		vec3 sampleScene = texture(sceneTexture, sampleCoordinate).rgb;
		SceneFeatures sampleFeatures = SampleSceneFeatures(sampleCoordinate);

		/*
		*	Calculate the sample weight based on certain criteria;
		*	
		*	1. How closely aligned are the albedos to each other?
		*	2. How closely aligned are the hit distances to each other?
		*	3. How closely aligned are the shading normals to each other?
		*	4. How closely aligned are the roughness terms to each other?
		*	5. How closely aligned are the metallic terms to each other?
		*	6. How closely aligned are the ambient occlusion terms to each other?
		*	7. How closely aligned are the luminance terms to each other?
		*	8. Is the sample coordinate valid?
		*/
		float sampleWeight = 1.0f;

		sampleWeight *= 1.0f - min(abs(CalculateAverage(currentFeatures.albedo) - CalculateAverage(sampleFeatures.albedo)), 1.0f);
		sampleWeight *= 1.0f - min(abs(currentFeatures.hitDistance - sampleFeatures.hitDistance), 1.0f);
		sampleWeight *= max(dot(currentFeatures.shadingNormal, sampleFeatures.shadingNormal), 1.0f);
		sampleWeight *= 1.0f - min(abs(currentFeatures.roughness - sampleFeatures.roughness), 1.0f);
		sampleWeight *= 1.0f - min(abs(currentFeatures.metallic - sampleFeatures.metallic), 1.0f);
		sampleWeight *= 1.0f - min(abs(currentFeatures.ambientOcclusion - sampleFeatures.ambientOcclusion), 1.0f);
		sampleWeight *= 1.0f - min(abs(currentFeatures.luminance - sampleFeatures.luminance), 1.0f);
		sampleWeight *= float(ValidCoordinate(sampleCoordinate));

		denoisedScene += sampleScene * sampleWeight;
		sceneWeightSum += sampleWeight;
	}

	//Normalize the denoised scene. 
	denoisedScene = sceneWeightSum == 0.0f ? currentScene : denoisedScene / sceneWeightSum;

	//Write the fragment.
	scene = vec4(denoisedScene, 1.0f);
}