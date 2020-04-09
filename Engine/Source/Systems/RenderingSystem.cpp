//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/Perceiver.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Entities.
#include <Entities/Types/Entity.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Noise/HaltonSequence.h>
#include <Math/Noise/HammersleySequence.h>

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
#include <Systems/TerrainSystem.h>
#include <Systems/WorldSystem.h>

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
	//Set the full resolution.
	_FullResolution = configuration._Resolution;

	//Set the scaled resolutions.
	_ScaledResolutions[0] = Resolution(static_cast<uint32>(static_cast<float32>(_FullResolution._Width) * configuration._ResolutionScale), static_cast<uint32>(static_cast<float32>(_FullResolution._Height) * configuration._ResolutionScale));
	_ScaledResolutions[0].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[1] = _ScaledResolutions[0] / 2;
	_ScaledResolutions[1].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[2] = _ScaledResolutions[1] / 2;
	_ScaledResolutions[2].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[3] = _ScaledResolutions[2] / 2;
	_ScaledResolutions[3].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[4] = _ScaledResolutions[3] / 2;
	_ScaledResolutions[4].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[5] = _ScaledResolutions[4] / 2;
	_ScaledResolutions[5].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[6] = _ScaledResolutions[5] / 2;
	_ScaledResolutions[6].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[7] = _ScaledResolutions[6] / 2;
	_ScaledResolutions[7].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[8] = _ScaledResolutions[7] / 2;
	_ScaledResolutions[8].RoundUpToNearestMultipleOfTwo();

	_ScaledResolutions[9] = _ScaledResolutions[8] / 2;
	_ScaledResolutions[9].RoundUpToNearestMultipleOfTwo();

	//Initialize the model system.
	_ModelSystem.Initialize();

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

	//Initialize the default texture.
	InitializeDefaultTexture();

	//Initialize the Hammersley hemisphere samples uniform buffer.
	InitializeHammersleyHemisphereSamplesUniformBuffer();

	//Pre-initialize the global render data.
	PreInitializeGlobalRenderData();

	//Post-initialize the rendering system.
	PostInitialize();
}

/*
*	Post initializes the rendering system.
*/
void RenderingSystem::PostInitializeSystem() NOEXCEPT
{
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

	//Post-initialize the ray tracing system.
	_RayTracingSystem.PostInitialize();

	//Initialize all render passes.
	RenderingSystemLogic::InitializeRenderPasses();
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

	//Update the ray tracing system.
	_RayTracingSystem.RenderUpdate(context);

#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
	_RenderingReferenceSystem.RenderUpdate(context);
#endif

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
	return	IsRayTracingSupported() &&
			(RenderingConfigurationManager::Instance->GetRenderingPath() == RenderingConfigurationManager::RenderingPath::PATH_TRACING
			|| RenderingConfigurationManager::Instance->GetAmbientOcclusionMode() == RenderingConfigurationManager::AmbientOcclusionMode::RAY_TRACED
			|| RenderingConfigurationManager::Instance->GetShadowsMode() == RenderingConfigurationManager::ShadowsMode::RAY_TRACED);
}

/*
*	Returns whether or not ray tracing is possible.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingPossible() const NOEXCEPT
{
	return _RayTracingSystem.DoesRayTracingDataExist();
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
	_GlobalRenderData._GlobalTexturesLock.Lock();

	//Find the first available index and store it.
	uint32 index{ UINT32_MAXIMUM };

	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
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
		globalTextureUpdate.Emplace(index, texture);
	}

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Unlock();

	//Return the index.
	return index;
}

/*
*	Returns a texture to the global render data and marks it's index as available.
*/
void RenderingSystem::ReturnTextureToGlobalRenderData(const uint32 index) NOEXCEPT
{
	//Lock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Lock();

	//Add the global texture updates.
	for (DynamicArray<uint32> &globalTextureUpdate : _GlobalRenderData._RemoveGlobalTextureUpdates)
	{
		globalTextureUpdate.Emplace(index);
	}

	//Mark the global texture slot as available.
	_GlobalRenderData._GlobalTextureSlots[index] = false;

	//Unlock the global texture slots.
	_GlobalRenderData._GlobalTexturesLock.Unlock();
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
	_GlobalRenderData._RenderDataTables.Upsize<false>(numberOfFrameBuffers);
	_GlobalRenderData._DynamicUniformDataBuffers.Upsize<false>(numberOfFrameBuffers);
	_GlobalRenderData._RemoveGlobalTextureUpdates.Upsize<true>(numberOfFrameBuffers);
	_GlobalRenderData._AddGlobalTextureUpdates.Upsize<true>(numberOfFrameBuffers);

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
	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
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
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_1)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_2)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_3)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RG_FLOAT16, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_4)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_2_HALF)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_3_HALF)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RG_FLOAT16, &_RenderTargets[UNDERLYING(RenderTarget::SCENE_FEATURES_4_HALF)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::R_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::AMBIENT_OCCLUSION)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::SCENE)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::R_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_AMBIENT_OCCLUSION_BUFFER_1)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::R_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_AMBIENT_OCCLUSION_BUFFER_2)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_1)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_2)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_1)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_2)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_1)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_2)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_UINT8)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::R_UINT8, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_R_UINT8_HALF)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2)]);
	CreateRenderTarget(GetScaledResolution(2), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER)]);
	CreateRenderTarget(GetScaledResolution(3), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH)]);
	CreateRenderTarget(GetScaledResolution(4), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH)]);
	CreateRenderTarget(GetScaledResolution(5), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND)]);
	CreateRenderTarget(GetScaledResolution(6), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH)]);
	CreateRenderTarget(GetScaledResolution(7), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HUNDREDTWENTYEIGHTH)]);
	CreateRenderTarget(GetScaledResolution(8), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_TWOHUNDREDFIFTYSIXTH)]);
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
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 8> bindings
		{
			//Global uniform data.
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Compute | ShaderStage::Fragment | ShaderStage::Geometry | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::RayMiss | ShaderStage::Vertex),
			
			//Global textures.
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::SampledImage, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::Compute | ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::Vertex),
			
			//Global samplers.
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::Sampler, UNDERLYING(Sampler::NumberOfSamplers), ShaderStage::Compute | ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::Vertex),
			
			//Global materials.
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration | ShaderStage::Vertex),

			//Cloud texture.
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Compute | ShaderStage::Fragment | ShaderStage::RayGeneration),
			
			//Sky images.
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::StorageImage, CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES, ShaderStage::Compute),

			//Sky textures.
			RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::CombinedImageSampler, CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES, ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration),

			//Hammersley hemisphere samples uniform buffer.
			RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment | ShaderStage::RayGeneration),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Global)]);
	}

	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::Compute)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::ParticleSystem)]);
	}
}

/*
*	Initializes the default texture.
*/
void RenderingSystem::InitializeDefaultTexture() NOEXCEPT
{
	//Create the default texture 2D.
	StaticArray<byte, 4> default_texture_2d_data{ static_cast<byte>(255), static_cast<byte>(0), static_cast<byte>(0), static_cast<byte>(255) };
	CreateTexture2D(TextureData(TextureDataContainer(default_texture_2d_data.Data(), 1, 1, 1, 4), TextureFormat::RGBA_UINT8), &_DefaultTexture2D);
}

/*
*	Initializes the Hammersley hemisphere samples uniform buffer.
*/
void RenderingSystem::InitializeHammersleyHemisphereSamplesUniformBuffer() NOEXCEPT
{
	//Calculate the hemisphere samples.
	StaticArray<Vector4<float32>, 64> hemisphere_samples;

	uint8 counter{ 0 };

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		for (uint8 j{ 0 }; j < 8; ++j)
		{
			hemisphere_samples[counter] = Vector4<float32>(HammersleySequence::CalculateCoordinateHemisphereCosinus(i + j * 8 + 1, 65), HammersleySequence::RadicalInverse(counter + 1));

			++counter;
		}
	}

	//Create the buffer and upload the data.
	const void* const RESTRICT data_chunks[]{ hemisphere_samples.Data() };
	const uint64 data_sizes[]{ hemisphere_samples.Size() * sizeof(Vector4<float32>) };
	CreateBuffer(data_sizes[0], BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &_HammersleyHemisphereSamplesUniformBuffer);
	UploadDataToBuffer(data_chunks, data_sizes, 1, &_HammersleyHemisphereSamplesUniformBuffer);
}

/*
*	Initializes the noise textures.
*/
void RenderingSystem::InitializeNoiseTextures() NOEXCEPT
{
	//Add all the noise textures.
	_NoiseTextures[0] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_0_Texture2D"));
	_NoiseTextures[1] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_1_Texture2D"));
	_NoiseTextures[2] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_2_Texture2D"));
	_NoiseTextures[3] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_3_Texture2D"));
	_NoiseTextures[4] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_4_Texture2D"));
	_NoiseTextures[5] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_5_Texture2D"));
	_NoiseTextures[6] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_6_Texture2D"));
	_NoiseTextures[7] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_7_Texture2D"));
	_NoiseTextures[8] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_8_Texture2D"));
	_NoiseTextures[9] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_9_Texture2D"));
	_NoiseTextures[10] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_10_Texture2D"));
	_NoiseTextures[11] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_11_Texture2D"));
	_NoiseTextures[12] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_12_Texture2D"));
	_NoiseTextures[13] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_13_Texture2D"));
	_NoiseTextures[14] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_14_Texture2D"));
	_NoiseTextures[15] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_15_Texture2D"));
	_NoiseTextures[16] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_16_Texture2D"));
	_NoiseTextures[17] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_17_Texture2D"));
	_NoiseTextures[18] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_18_Texture2D"));
	_NoiseTextures[19] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_19_Texture2D"));
	_NoiseTextures[20] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_20_Texture2D"));
	_NoiseTextures[21] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_21_Texture2D"));
	_NoiseTextures[22] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_22_Texture2D"));
	_NoiseTextures[23] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_23_Texture2D"));
	_NoiseTextures[24] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_24_Texture2D"));
	_NoiseTextures[25] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_25_Texture2D"));
	_NoiseTextures[26] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_26_Texture2D"));
	_NoiseTextures[27] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_27_Texture2D"));
	_NoiseTextures[28] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_28_Texture2D"));
	_NoiseTextures[29] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_29_Texture2D"));
	_NoiseTextures[30] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_30_Texture2D"));
	_NoiseTextures[31] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_31_Texture2D"));
	_NoiseTextures[32] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_32_Texture2D"));
	_NoiseTextures[33] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_33_Texture2D"));
	_NoiseTextures[34] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_34_Texture2D"));
	_NoiseTextures[35] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_35_Texture2D"));
	_NoiseTextures[36] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_36_Texture2D"));
	_NoiseTextures[37] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_37_Texture2D"));
	_NoiseTextures[38] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_38_Texture2D"));
	_NoiseTextures[39] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_39_Texture2D"));
	_NoiseTextures[40] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_40_Texture2D"));
	_NoiseTextures[41] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_41_Texture2D"));
	_NoiseTextures[42] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_42_Texture2D"));
	_NoiseTextures[43] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_43_Texture2D"));
	_NoiseTextures[44] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_44_Texture2D"));
	_NoiseTextures[45] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_45_Texture2D"));
	_NoiseTextures[46] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_46_Texture2D"));
	_NoiseTextures[47] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_47_Texture2D"));
	_NoiseTextures[48] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_48_Texture2D"));
	_NoiseTextures[49] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_49_Texture2D"));
	_NoiseTextures[50] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_50_Texture2D"));
	_NoiseTextures[51] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_51_Texture2D"));
	_NoiseTextures[52] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_52_Texture2D"));
	_NoiseTextures[53] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_53_Texture2D"));
	_NoiseTextures[54] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_54_Texture2D"));
	_NoiseTextures[55] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_55_Texture2D"));
	_NoiseTextures[56] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_56_Texture2D"));
	_NoiseTextures[57] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_57_Texture2D"));
	_NoiseTextures[58] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_58_Texture2D"));
	_NoiseTextures[59] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_59_Texture2D"));
	_NoiseTextures[60] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_60_Texture2D"));
	_NoiseTextures[61] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_61_Texture2D"));
	_NoiseTextures[62] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_62_Texture2D"));
	_NoiseTextures[63] = &ResourceLoader::GetTexture2DResource(HashString("Blue_Noise_63_Texture2D"));
}

/*
*	Post-initializes the global render data.
*/
void RenderingSystem::PostInitializeGlobalRenderData() NOEXCEPT
{
	for (uint8 i{ 0 }; i < GetNumberOfFramebuffers(); ++i)
	{
		//Bind some default texture to the global textures, because... Validation layers tells me I need to do this. (:
		for (uint32 j{ 0 }; j < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++j)
		{
			BindSampledImageToRenderDataTable(1, j, &_GlobalRenderData._RenderDataTables[i], _DefaultTexture2D);
		}

		//Bind all the samplers to the render data table.
		for (uint32 j{ 0 }; j < UNDERLYING(Sampler::NumberOfSamplers); ++j)
		{
			BindSamplerToRenderDataTable(2, j, &_GlobalRenderData._RenderDataTables[i], _Samplers[j]);
		}

		//Bind the cloud texture.
		BindCombinedImageSamplerToRenderDataTable(4, 0, &_GlobalRenderData._RenderDataTables[i], ResourceLoader::GetTexture3DResource(HashString("Cloud_Texture3D")), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));

		//Bind the sky images.
		for (uint32 j{ 0 }; j < CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES; ++j)
		{
			BindStorageImageToRenderDataTable(5, j, &_GlobalRenderData._RenderDataTables[i], WorldSystem::Instance->GetSkySystem()->GetSkyTexture(j));
		}

		//Bind the sky textures.
		for (uint32 j{ 0 }; j < CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES; ++j)
		{
			BindCombinedImageSamplerToRenderDataTable(6, j, &_GlobalRenderData._RenderDataTables[i], WorldSystem::Instance->GetSkySystem()->GetSkyTexture(j), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
		}

		//Bind the Hammersley hemisphere samples uniform buffer.
		BindUniformBufferToRenderDataTable(7, 0, &_GlobalRenderData._RenderDataTables[i], _HammersleyHemisphereSamplesUniformBuffer);
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

	//Jitter the projection matrix a bit.
	Vector2<float> current_frame_jitter{ JITTER_SAMPLES[_CurrentJitterIndex] * _DynamicUniformData._InverseScaledResolution };
	Perceiver::Instance->SetProjectionMatrixJitter(current_frame_jitter);

	//Update matrices.
	_DynamicUniformData._ViewMatrixMinusOne = _DynamicUniformData._ViewMatrix;
	_DynamicUniformData._InversePerceiverMatrix = *Perceiver::Instance->GetInversePerceiverMatrix();
	_DynamicUniformData._InverseProjectionMatrix = *Perceiver::Instance->GetInverseProjectionMatrix();
	_DynamicUniformData._PerceiverMatrix = *Perceiver::Instance->GetPerceiverMatrix();
	_DynamicUniformData._ProjectionMatrix = *Perceiver::Instance->GetProjectionMatrix();
	_DynamicUniformData._ViewMatrix = *Perceiver::Instance->GetViewMatrix();

	//Update vectors.
	_DynamicUniformData._PerceiverForwardVector = Perceiver::Instance->GetForwardVector();
	_DynamicUniformData._PerceiverWorldPosition = Perceiver::Instance->GetPosition();

	_DynamicUniformData._ScaledResolution = Vector2<float>(static_cast<float>(GetScaledResolution(0)._Width), static_cast<float>(GetScaledResolution(0)._Height));
	_DynamicUniformData._InverseScaledResolution = 1.0f / _DynamicUniformData._ScaledResolution;
	_DynamicUniformData._PreviousFrameJitter = _DynamicUniformData._CurrentFrameJitter;
	_DynamicUniformData._CurrentFrameJitter = current_frame_jitter;

	//Update floats.
	_DynamicUniformData._DeltaTime = CatalystEngineSystem::Instance->GetDeltaTime();
	_DynamicUniformData._GlobalRandomSeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._UNUSED5 = 0.0f;
	_DynamicUniformData._TotalTime = CatalystEngineSystem::Instance->GetTotalTime();
	_DynamicUniformData._WindSpeed = 2.5f;

	_DynamicUniformData._AmbientOcclusionMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetAmbientOcclusionMode());
	_DynamicUniformData._MotionBlurMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetMotionBlurMode());
	_DynamicUniformData._UNUSED3 = 0;
	_DynamicUniformData._ShadowsMode = static_cast<int32>(RenderingConfigurationManager::Instance->GetShadowsMode());
	_DynamicUniformData._UNUSED4 = 0;

	_DynamicUniformData._UNUSED2 = 0.0f;
	_DynamicUniformData._BloomIntensity = RenderingConfigurationManager::Instance->GetBloomIntensity();
	_DynamicUniformData._ChromaticAberrationIntensity = RenderingConfigurationManager::Instance->GetChromaticAberrationIntensity();

	_DynamicUniformData._DepthOfFieldFocusDistance = RenderingConfigurationManager::Instance->GetDepthOfFieldFocusDistance();

	_DynamicUniformData._ActiveBlueNoiseTextureIndex = _NoiseTextures[_ActiveNoiseTextureIndex]->_Index;
	_DynamicUniformData._ActiveBlueNoiseTextureOffsetX = static_cast<float>(CatalystRandomMath::RandomIntegerInRange<int32>(0, NUMBER_OF_NOISE_TEXTURES - 1)) / static_cast<float>(NOISE_TEXTURE_SIZE);
	_DynamicUniformData._ActiveBlueNoiseTextureOffsetY = static_cast<float>(CatalystRandomMath::RandomIntegerInRange<int32>(0, NUMBER_OF_NOISE_TEXTURES - 1)) / static_cast<float>(NOISE_TEXTURE_SIZE);
	_DynamicUniformData._ViewDistance = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance;
	_DynamicUniformData._CloudDensity = WorldSystem::Instance->GetSkySystem()->GetCloudDensity();
	_DynamicUniformData._Wetness = WorldSystem::Instance->GetWetness();
	_DynamicUniformData._NearPlane = Perceiver::Instance->GetNearPlane();
	_DynamicUniformData._FarPlane = Perceiver::Instance->GetFarPlane();
	_DynamicUniformData._TerrainHeightMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_HeightMapTextureIndex;
	_DynamicUniformData._TerrainIndexMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_IndexMapTextureIndex;
	_DynamicUniformData._TerrainBlendMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_BlendMapTextureIndex;
	_DynamicUniformData._TerrainMapResolution = static_cast<float32>(TerrainSystem::Instance->GetTerrainProperties()->_HeightMap.GetWidth());

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
	_GlobalRenderData._GlobalTexturesLock.Lock();

	for (const uint32 update : _GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(1, update, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _DefaultTexture2D);
	}

	_GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index].Clear();

	for (Pair<uint32, Texture2DHandle> &update : _GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(1, update._First, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], update._Second);
	}

	_GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index].Clear();

	//Unlock the global textures.
	_GlobalRenderData._GlobalTexturesLock.Unlock();
}

/*
*	Updates the global materials.
*/
void RenderingSystem::UpdateGlobalMaterials(const uint8 current_framebuffer_index) NOEXCEPT
{
	//Bind the current global material uniform buffer to the global render data table.
	BindUniformBufferToRenderDataTable(3, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _MaterialSystem.GetCurrentGlobalMaterialUnifomBuffer());
}