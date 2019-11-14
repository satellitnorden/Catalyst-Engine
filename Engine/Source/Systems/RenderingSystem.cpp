//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/Perceiver.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Entities.
#include <Entities/Types/Entity.h>

//Managers.
#include <Managers/EnvironmentManager.h>
#include <Managers/RenderingConfigurationManager.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Noise/HaltonSequence.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/RenderPasses/RenderPassManager.h>

//Resources.
#include <Resources/Creation/ResourceCreator.h>
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/CatalystEngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingSystem);

//Rendering system logic.
namespace RenderingSystemLogic
{

	/*
	*	Initializes all render passes.
	*/
	FORCE_INLINE void InitializeRenderPasses() NOEXCEPT
	{
		//Initialize all render passes.
		for (RenderPass *const RESTRICT renderPass : *RenderPassManager::GetRenderPasses())
		{
			renderPass->Initialize();
		}
	}

	/*
	*	Executes all render passes.
	*/
	FORCE_INLINE void ExecuteRenderPasses() NOEXCEPT
	{
		//Executes all render passes.
		for (RenderPass *const RESTRICT renderPass : *RenderPassManager::GetRenderPasses())
		{
			renderPass->Execute();
		}
	}

}

/*
*	Initializes the rendering system.
*/
void RenderingSystem::Initialize(const CatalystProjectRenderingConfiguration &configuration) NOEXCEPT
{
	//Set the resolution.
	_Resolution = configuration._Resolution;

	//Set the scaled resolution.
	_ScaledResolution = Resolution(static_cast<uint32>(static_cast<float>(_Resolution._Width) * configuration._ResolutionScale), static_cast<uint32>(static_cast<float>(_Resolution._Height) * configuration._ResolutionScale));

	//Set the far plane of the perceiver.
	Perceiver::Instance->SetFarPlane(configuration._ViewDistance * 2.0f);

	//Pre-initialize the rendering system.
	PreInitialize();

	//Initialize all render targets.
	InitializeRenderTargets();

	//Initialize all samplers.
	InitializeSamplers();

	//Initialize all common render data table layouts.
	InitializeCommonRenderDataTableLayouts();

	//Pre-initialize the global render data.
	PreInitializeGlobalRenderData();

	//Post-initialize the rendering system.
	PostInitialize();
}

/*
*	Post initializes the rendering system.
*/
void RenderingSystem::PostInitializeSystem()
{
	//Initialize all common materials.
	InitializeCommonMaterials();

	//Post-initialize the global render data.
	PostInitializeGlobalRenderData();

	//Initialize the noise textures.
	InitializeNoiseTextures();

	//Post-initialize the lighting system.
	_LightingSystem.PostInitialize();

	//Post-initialize the material system.
	_MaterialSystem.PostInitialize();

	//Post-initialize the model system.
	_ModelSystem.PostInitialize();

	//Initialize all render passes.
	RenderingSystemLogic::InitializeRenderPasses();
}

/*
*	Updates the rendering system during the pre update phase.
*/
void RenderingSystem::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the model system.
	_ModelSystem.PreUpdate(context);
}

/*
*	Updates the rendering system during the render update phase.
*/
void RenderingSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Begin the frame.
	BeginFrame();

	//Update the global render data.
	UpdateGlobalRenderData();

	//Update the lighting system.
	_LightingSystem.RenderUpdate(context);

	//Update the material system.
	_MaterialSystem.RenderUpdate(context);

	//Update the model system.
	_ModelSystem.RenderUpdate(context);

	//This shouldn't really be here, but let's have it here for now...
	AnimationSystem::Instance->RenderUpdate(context);

	//Execute all render passes.
	RenderingSystemLogic::ExecuteRenderPasses();

	//End the frame.
	EndFrame();
}

/*
*	Releases the rendering system.
*/
void RenderingSystem::Release() NOEXCEPT
{
	//Pre-release the rendering system.
	PreRelease();

	//Post-release the rendering system.
	PostRelease();
}

/*
*	Returns whether or not ray tracing is active.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingActive() const NOEXCEPT
{
	return RenderingConfigurationManager::Instance->GetShadowsMode() == RenderingConfigurationManager::ShadowsMode::RayTraced;
}

/*
*	Returns the global render data table.
*/
RenderDataTableHandle RenderingSystem::GetGlobalRenderDataTable() const NOEXCEPT
{
	return _GlobalRenderData._RenderDataTables[GetCurrentFramebufferIndex()];
}

/*
*	Adds a texture to the global render data and returns it's index.
*/
uint32 RenderingSystem::AddTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT
{
	//Lock global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	//Find the first available index and store it.
	uint32 index{ UINT32_MAXIMUM };

	for (uint32 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		//If this is available, grab it!
		if (!_GlobalRenderData._GlobalTextureSlots[i])
		{
			index = i;
			_GlobalRenderData._GlobalTextureSlots[i] = true;

			break;
		}
	}

	ASSERT(index != UINT32_MAXIMUM, "If no index could be found, then, well... This is bad. ):");

	//Add the global texture updates.
	for (DynamicArray<Pair<uint32, Texture2DHandle>> &globalTextureUpdate : _GlobalRenderData._AddGlobalTextureUpdates)
	{
		globalTextureUpdate.EmplaceSlow(index, texture);
	}

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();

	//Return the index.
	return index;
}

/*
*	Returns a texture to the global render data and marks it's index as available.
*/
void RenderingSystem::ReturnTextureToGlobalRenderData(const uint32 index) NOEXCEPT
{
	//Lock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	//Add the global texture updates.
	for (DynamicArray<uint32> &globalTextureUpdate : _GlobalRenderData._RemoveGlobalTextureUpdates)
	{
		globalTextureUpdate.EmplaceSlow(index);
	}

	//Mark the global texture slot as available.
	_GlobalRenderData._GlobalTextureSlots[index] = false;

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();
}

/*
*	Returns the given common material.
*/
Material RenderingSystem::GetCommonMaterial(const CommonMaterial material) const NOEXCEPT
{
	return _CommonMaterials[UNDERLYING(material)];
}

/*
*	Returns the given common render data table layout.
*/
RenderDataTableHandle RenderingSystem::GetCommonRenderDataTableLayout(const CommonRenderDataTableLayout commonRenderDataTableLayout) const NOEXCEPT
{
	//Return the given common render data table layout.
	return _CommonRenderDataTableLayouts[UNDERLYING(commonRenderDataTableLayout)];
}

/*
*	Pre-initializes the global render data.
*/
void RenderingSystem::PreInitializeGlobalRenderData() NOEXCEPT
{
	//Get the number of frame buffers.
	const uint8 numberOfFrameBuffers{ GetNumberOfFramebuffers() };

	//Upsize the buffers.
	_GlobalRenderData._RenderDataTables.UpsizeFast(numberOfFrameBuffers);
	_GlobalRenderData._DynamicUniformDataBuffers.UpsizeFast(numberOfFrameBuffers);
	_GlobalRenderData._RemoveGlobalTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._AddGlobalTextureUpdates.UpsizeSlow(numberOfFrameBuffers);

	for (uint8 i{ 0 }; i < numberOfFrameBuffers; ++i)
	{
		//Create the render data table.
		CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global), &_GlobalRenderData._RenderDataTables[i]);

		//Create the dynamic uniform data buffer.
		CreateBuffer(sizeof(DynamicUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &_GlobalRenderData._DynamicUniformDataBuffers[i]);

		//Bind the dynamic uniform data buffer to the render data table.
		BindUniformBufferToRenderDataTable(0, 0, &_GlobalRenderData._RenderDataTables[i], _GlobalRenderData._DynamicUniformDataBuffers[i]);
	}

	//Mark all global texture slots as free.
	for (uint32 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		_GlobalRenderData._GlobalTextureSlots[i] = false;
	}
}

/*
*	Initializes all render targets.
*/
void RenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all render targets.
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneFeatures1)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::SceneFeatures2)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneFeatures3)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::AmbientOcclusion)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Scene)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::TemporalAntiAliasingBuffer1)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::TemporalAntiAliasingBuffer2)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_R32G32B32A32_Float_1)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_R32G32B32A32_Float_2)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_R8_Byte)]);
	CreateRenderTarget(GetScaledResolution() / 2, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1)]);
	CreateRenderTarget(GetScaledResolution() / 2, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2)]);
	CreateRenderTarget(GetScaledResolution() / 4, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1)]);
	CreateRenderTarget(GetScaledResolution() / 4, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_2)]);
	CreateRenderTarget(GetScaledResolution() / 8, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate_Eighth_R32G32B32A32_Float_1)]);
}

/*
*	Initializes all samplers.
*/
void RenderingSystem::InitializeSamplers() NOEXCEPT
{
	CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::ClampToEdge), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Linear, AddressMode::Repeat), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat)]);
	CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::ClampToEdge), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::Linear, MipmapMode::Nearest, AddressMode::Repeat), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat)]);
	CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::Repeat), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat)]);
}

/*
*	Initializes all common materials.
*/
void RenderingSystem::InitializeCommonMaterials() NOEXCEPT
{
	{
		//Initialize the black common material.
		Material &material{ _CommonMaterials[UNDERLYING(CommonMaterial::Black)] };

		material._Properties = Material::Property::None;

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(0);
			data._Data[0].EmplaceFast(0);
			data._Data[0].EmplaceFast(0);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._AlbedoTextureIndex = texture._Index;

			//Make the albedo texture the default texture 2D.
			_DefaultTexture2D = texture._Texture2D;
		}

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._NormalMapTextureIndex = texture._Index;
		}

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(0);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._MaterialPropertiesTextureIndex = texture._Index;
		}

		material._LuminanceMultiplier = 0.0f;
	}

	{
		//Initialize the glass blue common material.
		Material &material{ _CommonMaterials[UNDERLYING(CommonMaterial::GlassBlue)] };

		material._Properties = Material::Property::None;

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(204);
			data._Data[0].EmplaceFast(230);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._AlbedoTextureIndex = texture._Index;

			//Make the albedo texture the default texture 2D.
			_DefaultTexture2D = texture._Texture2D;
		}

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._NormalMapTextureIndex = texture._Index;
		}

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(0);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._MaterialPropertiesTextureIndex = texture._Index;
		}

		material._LuminanceMultiplier = 0.0f;
	}

	{
		//Initialize the white porcelain common material.
		Material &material{ _CommonMaterials[UNDERLYING(CommonMaterial::WhitePorcelain)] };

		material._Properties = Material::Property::None;

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._AlbedoTextureIndex = texture._Index;

			//Make the albedo texture the default texture 2D.
			_DefaultTexture2D = texture._Texture2D;
		}

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._NormalMapTextureIndex = texture._Index;
		}

		{
			Texture2DData data;

			data._MipmapLevels = 1;
			data._Width = 1;
			data._Height = 1;
			data._Data.UpsizeSlow(1);
			data._Data[0].Reserve(4);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(127);
			data._Data[0].EmplaceFast(255);
			data._Data[0].EmplaceFast(255);

			GlobalTexture2D texture;

			ResourceCreator::CreateTexture2D(&data, &texture);

			material._MaterialPropertiesTextureIndex = texture._Index;
		}

		material._LuminanceMultiplier = 0.0f;
	}
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 5> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Compute | ShaderStage::Fragment | ShaderStage::Geometry | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::Vertex),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::SampledImage, RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::Vertex),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::Sampler, UNDERLYING(Sampler::NumberOfSamplers), ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::Vertex),
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Global)]);
	}
}

/*
*	Initializes the noise textures.
*/
void RenderingSystem::InitializeNoiseTextures() NOEXCEPT
{
	//Add all the noise textures.
	_NoiseTextures[0] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_0_Texture2D"));
	_NoiseTextures[1] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_1_Texture2D"));
	_NoiseTextures[2] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_2_Texture2D"));
	_NoiseTextures[3] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_3_Texture2D"));
	_NoiseTextures[4] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_4_Texture2D"));
	_NoiseTextures[5] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_5_Texture2D"));
	_NoiseTextures[6] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_6_Texture2D"));
	_NoiseTextures[7] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_7_Texture2D"));
	_NoiseTextures[8] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_8_Texture2D"));
	_NoiseTextures[9] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_9_Texture2D"));
	_NoiseTextures[10] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_10_Texture2D"));
	_NoiseTextures[11] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_11_Texture2D"));
	_NoiseTextures[12] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_12_Texture2D"));
	_NoiseTextures[13] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_13_Texture2D"));
	_NoiseTextures[14] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_14_Texture2D"));
	_NoiseTextures[15] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_15_Texture2D"));
	_NoiseTextures[16] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_16_Texture2D"));
	_NoiseTextures[17] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_17_Texture2D"));
	_NoiseTextures[18] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_18_Texture2D"));
	_NoiseTextures[19] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_19_Texture2D"));
	_NoiseTextures[20] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_20_Texture2D"));
	_NoiseTextures[21] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_21_Texture2D"));
	_NoiseTextures[22] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_22_Texture2D"));
	_NoiseTextures[23] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_23_Texture2D"));
	_NoiseTextures[24] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_24_Texture2D"));
	_NoiseTextures[25] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_25_Texture2D"));
	_NoiseTextures[26] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_26_Texture2D"));
	_NoiseTextures[27] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_27_Texture2D"));
	_NoiseTextures[28] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_28_Texture2D"));
	_NoiseTextures[29] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_29_Texture2D"));
	_NoiseTextures[30] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_30_Texture2D"));
	_NoiseTextures[31] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_31_Texture2D"));
	_NoiseTextures[32] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_32_Texture2D"));
	_NoiseTextures[33] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_33_Texture2D"));
	_NoiseTextures[34] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_34_Texture2D"));
	_NoiseTextures[35] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_35_Texture2D"));
	_NoiseTextures[36] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_36_Texture2D"));
	_NoiseTextures[37] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_37_Texture2D"));
	_NoiseTextures[38] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_38_Texture2D"));
	_NoiseTextures[39] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_39_Texture2D"));
	_NoiseTextures[40] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_40_Texture2D"));
	_NoiseTextures[41] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_41_Texture2D"));
	_NoiseTextures[42] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_42_Texture2D"));
	_NoiseTextures[43] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_43_Texture2D"));
	_NoiseTextures[44] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_44_Texture2D"));
	_NoiseTextures[45] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_45_Texture2D"));
	_NoiseTextures[46] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_46_Texture2D"));
	_NoiseTextures[47] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_47_Texture2D"));
	_NoiseTextures[48] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_48_Texture2D"));
	_NoiseTextures[49] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_49_Texture2D"));
	_NoiseTextures[50] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_50_Texture2D"));
	_NoiseTextures[51] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_51_Texture2D"));
	_NoiseTextures[52] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_52_Texture2D"));
	_NoiseTextures[53] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_53_Texture2D"));
	_NoiseTextures[54] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_54_Texture2D"));
	_NoiseTextures[55] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_55_Texture2D"));
	_NoiseTextures[56] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_56_Texture2D"));
	_NoiseTextures[57] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_57_Texture2D"));
	_NoiseTextures[58] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_58_Texture2D"));
	_NoiseTextures[59] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_59_Texture2D"));
	_NoiseTextures[60] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_60_Texture2D"));
	_NoiseTextures[61] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_61_Texture2D"));
	_NoiseTextures[62] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_62_Texture2D"));
	_NoiseTextures[63] = ResourceLoader::GetTexture2D(HashString("Blue_Noise_63_Texture2D"));
}

/*
*	Post-initializes the global render data.
*/
void RenderingSystem::PostInitializeGlobalRenderData() NOEXCEPT
{
	
	for (uint8 i{ 0 }; i < GetNumberOfFramebuffers(); ++i)
	{
		//Bind some default texture to the global textures, because... Validation layers tells me I need to do this. (:
		for (uint32 j{ 0 }; j < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++j)
		{
			BindSampledImageToRenderDataTable(1, j, &_GlobalRenderData._RenderDataTables[i], _DefaultTexture2D);
		}

		//Bind all the samplers to the render data table.
		for (uint32 j{ 0 }; j < UNDERLYING(Sampler::NumberOfSamplers); ++j)
		{
			BindSamplerToRenderDataTable(2, j, &_GlobalRenderData._RenderDataTables[i], _Samplers[j]);
		}

		//Bind the cloud texture.
		BindCombinedImageSamplerToRenderDataTable(4, 0, &_GlobalRenderData._RenderDataTables[i], ResourceLoader::GetTexture3D(HashString("Cloud_Texture3D")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));
	}
}

/*
*	Updates the global render data.
*/
void RenderingSystem::UpdateGlobalRenderData() NOEXCEPT
{
	//Retrieve the current frame buffer index.
	const uint8 current_framebuffer_index{ GetCurrentFramebufferIndex() };

	//Update the global uniform data.
	UpdateGlobalUniformData(current_framebuffer_index);

	//Update the global textures.
	UpdateGlobalTextures(current_framebuffer_index);

	//Update global materials.
	UpdateGlobalMaterials(current_framebuffer_index);
}

/*
*	Updates the global render data.
*/
void RenderingSystem::UpdateGlobalUniformData(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Define constants.
	constexpr float JITTER_SAMPLE_MULTIPLIER{ 0.5f };
	constexpr StaticArray<Vector2<float>, 16> JITTER_SAMPLES
	{
		(Vector2<float>(HaltonSequence::Generate(0, 3), HaltonSequence::Generate(1, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(2, 3), HaltonSequence::Generate(3, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(4, 3), HaltonSequence::Generate(5, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(6, 3), HaltonSequence::Generate(7, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(8, 3), HaltonSequence::Generate(9, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(10, 3), HaltonSequence::Generate(11, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(12, 3), HaltonSequence::Generate(13, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(14, 3), HaltonSequence::Generate(15, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(16, 3), HaltonSequence::Generate(17, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(18, 3), HaltonSequence::Generate(19, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(20, 3), HaltonSequence::Generate(21, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(22, 3), HaltonSequence::Generate(23, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(24, 3), HaltonSequence::Generate(25, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(26, 3), HaltonSequence::Generate(27, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(28, 3), HaltonSequence::Generate(29, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER,
		(Vector2<float>(HaltonSequence::Generate(30, 3), HaltonSequence::Generate(31, 3)) * 2.0f - 1.0f) * JITTER_SAMPLE_MULTIPLIER
	};
	constexpr float MINIMUM_CLOUD_DENSITY{ 0.25f };
	constexpr float MAXIMUM_CLOUD_DENSITY{ 0.5f };

	//Store the previous perceiver forward vector.
	const Vector3<float> previousPerceiverForwardVector{ Vector3<float>(_DynamicUniformData._PerceiverForwardVector._X, _DynamicUniformData._PerceiverForwardVector._Y, _DynamicUniformData._PerceiverForwardVector._Z) };

	//Jitter the projection matrix a bit.
	Vector2<float> currentFrameJitter{ 0.0f, 0.0f };

#if !defined(CATALYST_ENABLE_PATH_TRACING)
	Perceiver::Instance->SetProjectionMatrixJitter(JITTER_SAMPLES[_CurrentJitterIndex] * _DynamicUniformData._InverseScaledResolution);
#endif

	//Update matrices.
	_DynamicUniformData._ViewMatrixMinusOne = _DynamicUniformData._ViewMatrix;
	_DynamicUniformData._InversePerceiverMatrix = *Perceiver::Instance->GetInversePerceiverMatrix();
	_DynamicUniformData._InverseProjectionMatrix = *Perceiver::Instance->GetInverseProjectionMatrix();
	_DynamicUniformData._PerceiverMatrix = *Perceiver::Instance->GetPerceiverMatrix();
	_DynamicUniformData._ProjectionMatrix = *Perceiver::Instance->GetProjectionMatrix();
	_DynamicUniformData._ViewMatrix = *Perceiver::Instance->GetViewMatrix();

	//Update vectors.
	_DynamicUniformData._UpperSkyColor = EnvironmentManager::GetUpperSkyColor();
	_DynamicUniformData._LowerSkyColor = EnvironmentManager::GetLowerSkyColor();
	_DynamicUniformData._PerceiverWorldPositionMinusOne = _DynamicUniformData._PerceiverWorldPosition;
	_DynamicUniformData._PerceiverForwardVector = Perceiver::Instance->GetForwardVector();
	_DynamicUniformData._PerceiverWorldPosition = Perceiver::Instance->GetPosition();

	_DynamicUniformData._ScaledResolution = Vector2<float>(static_cast<float>(GetScaledResolution()._Width), static_cast<float>(GetScaledResolution()._Height));
	_DynamicUniformData._InverseScaledResolution = 1.0f / _DynamicUniformData._ScaledResolution;
	_DynamicUniformData._PreviousFrameJitter = _DynamicUniformData._CurrentFrameJitter;
	_DynamicUniformData._CurrentFrameJitter = currentFrameJitter;

	//Update floats.
	_DynamicUniformData._DeltaTime = CatalystEngineSystem::Instance->GetDeltaTime();
	_DynamicUniformData._StarStrength = EnvironmentManager::GetStarStrength();
	_DynamicUniformData._GlobalRandomSeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._PerceiverRotationVelocity = Vector3<float>::DotProduct(previousPerceiverForwardVector, Vector3<float>(_DynamicUniformData._PerceiverForwardVector._X, _DynamicUniformData._PerceiverForwardVector._Y, _DynamicUniformData._PerceiverForwardVector._Z));
	_DynamicUniformData._TotalTime = CatalystEngineSystem::Instance->GetTotalTime();
	_DynamicUniformData._WindSpeed = 10.0f;

	_DynamicUniformData._AmbientOcclusionMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetAmbientOcclusionMode());
	_DynamicUniformData._MotionBlurMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetMotionBlurMode());
	_DynamicUniformData._SpecularIrradianceMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetSpecularIrradianceMode());
	_DynamicUniformData._ShadowsMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetShadowsMode());
	_DynamicUniformData._VolumetricLightingMode = 0;

	_DynamicUniformData._AmbientIllumionationIntensity = RenderingConfigurationManager::Instance->GetAmbientIlluminationIntensity();
	_DynamicUniformData._BloomIntensity = RenderingConfigurationManager::Instance->GetBloomIntensity();
	_DynamicUniformData._ChromaticAberrationIntensity = RenderingConfigurationManager::Instance->GetChromaticAberrationIntensity();
	_DynamicUniformData._VolumetricLightingIntensity = RenderingConfigurationManager::Instance->GetVolumetricLightingIntensity();

	_DynamicUniformData._DepthOfFieldFocusDistance = RenderingConfigurationManager::Instance->GetDepthOfFieldFocusDistance();

	_DynamicUniformData._ActiveBlueNoiseTextureIndex = _NoiseTextures[_ActiveNoiseTextureIndex]._Index;
	_DynamicUniformData._ActiveBlueNoiseTextureOffsetX = static_cast<float>(CatalystRandomMath::RandomIntegerInRange<int32>(0, NUMBER_OF_NOISE_TEXTURES - 1)) / static_cast<float>(NOISE_TEXTURE_SIZE);
	_DynamicUniformData._ActiveBlueNoiseTextureOffsetY = static_cast<float>(CatalystRandomMath::RandomIntegerInRange<int32>(0, NUMBER_OF_NOISE_TEXTURES - 1)) / static_cast<float>(NOISE_TEXTURE_SIZE);
	_DynamicUniformData._ViewDistance = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance;
	_DynamicUniformData._CloudDensity = CatalystBaseMath::Scale(EnvironmentManager::GetCloudDensity(), 0.0f, 1.0f, MINIMUM_CLOUD_DENSITY, MAXIMUM_CLOUD_DENSITY);

	void *const RESTRICT dataChunks[]{ &_DynamicUniformData };
	const uint64 dataSizes[]{ sizeof(DynamicUniformData) };

	UploadDataToBuffer(dataChunks, dataSizes, 1, &_GlobalRenderData._DynamicUniformDataBuffers[current_framebuffer_index]);

	//Update the current jitter index.
	_CurrentJitterIndex = _CurrentJitterIndex == JITTER_SAMPLES.Size() - 1 ? 0 : _CurrentJitterIndex + 1;

	//Update the active noise texture index.
	_ActiveNoiseTextureIndex = _ActiveNoiseTextureIndex == NUMBER_OF_NOISE_TEXTURES - 1 ? 0 : _ActiveNoiseTextureIndex + 1;
}

/*
*	Updates the global textures.
*/
void RenderingSystem::UpdateGlobalTextures(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Lock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	for (const uint32 update : _GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(1, update, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _DefaultTexture2D);
	}

	_GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index].ClearFast();

	for (Pair<uint32, Texture2DHandle> &update : _GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(1, update._First, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], update._Second);
	}

	_GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index].ClearFast();

	//Unlock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();
}

/*
*	Updates the global materials.
*/
void RenderingSystem::UpdateGlobalMaterials(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Bind the current global material uniform buffer to the global render data table.
	BindUniformBufferToRenderDataTable(3, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _MaterialSystem.GetCurrentGlobalMaterialUnifomBuffer());
}