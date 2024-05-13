//Header file.
#include <Rendering/Native/SharedRenderTargetManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes the shared render target manager.
*/
void SharedRenderTargetManager::Initialize(const RenderingPath initial_rendering_path) NOEXCEPT
{
	//Reset the shared render targets.
	for (SharedRenderTargetInformation &information : _SharedRenderTargetInformations)
	{
		information._Handle = EMPTY_HANDLE;
	}

	//Set up the shared render target informations.
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._Identifier = HashString("SceneFeatures1");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._Identifier = HashString("SceneFeatures2");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._Identifier = HashString("SceneFeatures3");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._Identifier = HashString("SceneFeatures4");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._TextureFormat = TextureFormat::RG_FLOAT16;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._Identifier = HashString("SceneFeatures1Half");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_1_HALF)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._Identifier = HashString("SceneFeatures2Half");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_2_HALF)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._Identifier = HashString("SceneFeatures3Half");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_3_HALF)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._Identifier = HashString("SceneFeatures4Half");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._TextureFormat = TextureFormat::RG_FLOAT16;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_FEATURES_4_HALF)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._Identifier = HashString("DiffuseIrradiance");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::DIFFUSE_IRRADIANCE)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._Identifier = HashString("SpecularIrradiance");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SPECULAR_IRRADIANCE)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._Identifier = HashString("Scene");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._Identifier = HashString("BloomHalf");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_HALF)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._Identifier = HashString("BloomQuarter");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._Resolution = RenderingSystem::Instance->GetScaledResolution(2);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_QUARTER)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._Identifier = HashString("BloomEighth");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._Resolution = RenderingSystem::Instance->GetScaledResolution(3);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_EIGHTH)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._Identifier = HashString("BloomSixteenth");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._Resolution = RenderingSystem::Instance->GetScaledResolution(4);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_SIXTEENTH)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._Identifier = HashString("BloomThirtySecond");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._Resolution = RenderingSystem::Instance->GetScaledResolution(5);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::BLOOM_THIRTYSECOND)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._Identifier = HashString("PreviousScene");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::PREVIOUS_SCENE)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._Identifier = HashString("SceneLowDynamicRange1");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._Identifier = HashString("SceneLowDynamicRange2");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._Identifier = HashString("INTERMEDIATE_RGBA_FLOAT32_1");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._Identifier = HashString("INTERMEDIATE_RGBA_FLOAT32_2");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._Identifier = HashString("INTERMEDIATE_RGBA_FLOAT32_HALF_1");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._Resolution = RenderingSystem::Instance->GetScaledResolution(1);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._Identifier = HashString("INTERMEDIATE_RGBA_FLOAT32_QUARTER_1");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._Resolution = RenderingSystem::Instance->GetScaledResolution(2);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._Identifier = HashString("INTERMEDIATE_RGBA_FLOAT32_QUARTER_2");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._Resolution = RenderingSystem::Instance->GetScaledResolution(2);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._TextureFormat = TextureFormat::RGBA_FLOAT32;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_2)]._IsDepthBuffer = false;

	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._Identifier = HashString("SceneDepthBuffer");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._TextureFormat = TextureFormat::D_UINT24_S_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::SCENE_DEPTH_BUFFER)]._IsDepthBuffer = true;

#if defined(CATALYST_EDITOR)
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._Identifier = HashString("EditorViewport");
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._Resolution = RenderingSystem::Instance->GetScaledResolution(0);
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._TextureFormat = TextureFormat::RGBA_UINT8;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._IsNeeded[UNDERLYING(RenderingPath::DEFAULT)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._IsNeeded[UNDERLYING(RenderingPath::MOBILE)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._IsNeeded[UNDERLYING(RenderingPath::VIRTUAL_REALITY)] = true;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._IsNeeded[UNDERLYING(RenderingPath::CUSTOM)] = false;
	_SharedRenderTargetInformations[UNDERLYING(SharedRenderTarget::EDITOR_VIEWPORT)]._IsDepthBuffer = false;
#endif

	//Create the required shared render targets.
	for (uint8 i{ 0 }; i < UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS); ++i)
	{
		if (_SharedRenderTargetInformations[i]._IsNeeded[UNDERLYING(initial_rendering_path)])
		{
			if (_SharedRenderTargetInformations[i]._IsDepthBuffer)
			{
				RenderingSystem::Instance->CreateDepthBuffer
				(
					_SharedRenderTargetInformations[i]._Resolution,
					_SharedRenderTargetInformations[i]._TextureFormat,
					SampleCount::SAMPLE_COUNT_1,
					&_SharedRenderTargetInformations[i]._Handle
				);
			}
			
			else
			{
				RenderingSystem::Instance->CreateRenderTarget
				(
					_SharedRenderTargetInformations[i]._Resolution,
					_SharedRenderTargetInformations[i]._TextureFormat,
					SampleCount::SAMPLE_COUNT_1,
					&_SharedRenderTargetInformations[i]._Handle
				);
			}
		}
	}
}

/*
*	Called when switching rendering path.
*/
void SharedRenderTargetManager::OnSwitchRenderingPath(const RenderingPath new_rendering_path) NOEXCEPT
{
	//Are any shared render targets no longer needed?
	for (uint8 i{ 0 }; i < UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS); ++i)
	{
		if (_SharedRenderTargetInformations[i]._Handle
			&& !_SharedRenderTargetInformations[i]._IsNeeded[UNDERLYING(new_rendering_path)])
		{
			if (_SharedRenderTargetInformations[i]._IsDepthBuffer)
			{
				RenderingSystem::Instance->DestroyDepthBuffer(&_SharedRenderTargetInformations[i]._Handle);
			}
			
			else
			{
				RenderingSystem::Instance->DestroyRenderTarget(&_SharedRenderTargetInformations[i]._Handle);
			}
		}
	}

	//Are any shared render targets needed?
	for (uint8 i{ 0 }; i < UNDERLYING(SharedRenderTarget::NUMBER_OF_SHARED_RENDER_TARGETS); ++i)
	{
		if (!_SharedRenderTargetInformations[i]._Handle
			&& _SharedRenderTargetInformations[i]._IsNeeded[UNDERLYING(new_rendering_path)])
		{
			if (_SharedRenderTargetInformations[i]._IsDepthBuffer)
			{
				RenderingSystem::Instance->CreateDepthBuffer
				(
					_SharedRenderTargetInformations[i]._Resolution,
					_SharedRenderTargetInformations[i]._TextureFormat,
					SampleCount::SAMPLE_COUNT_1,
					&_SharedRenderTargetInformations[i]._Handle
				);
			}

			else
			{
				RenderingSystem::Instance->CreateRenderTarget
				(
					_SharedRenderTargetInformations[i]._Resolution,
					_SharedRenderTargetInformations[i]._TextureFormat,
					SampleCount::SAMPLE_COUNT_1,
					&_SharedRenderTargetInformations[i]._Handle
				);
			}
		}
	}
}

/*
*	Returns the shared render target with the given identifier..
*/
NO_DISCARD RenderTargetHandle SharedRenderTargetManager::GetSharedRenderTarget(const HashString identifier) NOEXCEPT
{
	for (const SharedRenderTargetInformation &information : _SharedRenderTargetInformations)
	{
		if (information._Identifier == identifier)
		{
			return information._Handle;
		}
	}
	
	ASSERT(false, "Invalid identifier!");

	return EMPTY_HANDLE;
}