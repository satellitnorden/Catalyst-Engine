//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/Perceiver.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Entities.
#include <Entities/Types/Entity.h>

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

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TaskSystem.h>
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Initialize the debug rendering system.
	_DebugRenderingSystem.Initialize();
#endif

	//Initialize the model system.
	_ModelSystem.Initialize();

	//Set the far plane of the perceiver.
	Perceiver::Instance->SetFarPlane(configuration._ViewDistance);

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
}

/*
*	Post initializes the rendering system.
*/
void RenderingSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize the global render data.
	PostInitializeGlobalRenderData();

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
*	Returns whether or not ray tracing is active.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingActive() const NOEXCEPT
{
	return	IsRayTracingSupported() &&
			(_RenderingConfiguration.GetRenderingPath() == RenderingConfiguration::RenderingPath::PATH_TRACING
			|| _RenderingConfiguration.GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::RAY_TRACED
			|| _RenderingConfiguration.GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED
			|| _RenderingConfiguration.GetVolumetricShadowsMode() == RenderingConfiguration::VolumetricShadowsMode::RAY_TRACED);
}

/*
*	Returns whether or not ray tracing is possible.
*/
NO_DISCARD bool RenderingSystem::IsRayTracingPossible() const NOEXCEPT
{
	return _RayTracingSystem.DoesRayTracingDataExist();
}

/*
*	Returns a global command buffer from the global command pool.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT RenderingSystem::GetGlobalCommandBuffer(const CommandBufferLevel level) NOEXCEPT
{
	//Retrieve the thread data for the current frame.
	GlobalCommandPoolData::ThreadData &thread_data{ _GlobalRenderData._GlobalCommandPoolData[GetCurrentFramebufferIndex()]._ThreadData[Concurrency::CurrentThread::Index()] };

	//Retrieve the command buffer.
	switch (level)
	{
		case CommandBufferLevel::PRIMARY:
		{
			//Does a new command buffer need to be allocated?
			if (thread_data._NumberOfPrimaryCommandBuffersUsed >= thread_data._PrimaryCommandBuffers.Size())
			{
				thread_data._PrimaryCommandBuffers.Emplace(AllocateCommandBuffer(thread_data._CommandPool, CommandBufferLevel::PRIMARY));
			}

			return thread_data._PrimaryCommandBuffers[thread_data._NumberOfPrimaryCommandBuffersUsed++];
		}

		case CommandBufferLevel::SECONDARY:
		{
			//Does a new command buffer need to be allocated?
			if (thread_data._NumberOfSecondaryCommandBuffersUsed >= thread_data._SecondaryCommandBuffers.Size())
			{
				thread_data._SecondaryCommandBuffers.Emplace(AllocateCommandBuffer(thread_data._CommandPool, CommandBufferLevel::SECONDARY));
			}

			return thread_data._SecondaryCommandBuffers[thread_data._NumberOfSecondaryCommandBuffersUsed++];
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return nullptr;
		}
	}
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
*	Starts taking a screenshot.
*	This will pause the game, turn off denoising for all effects and start accumulating results across frames.
*	This can be used to, for example, take reference screenshots to tweak visual effects, or to take "prettier" screenshots of the game.
*/
void RenderingSystem::StartTakingScreenshot() NOEXCEPT
{
	//Pause the game.
	CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);

	//Signal to other systems that the rendering system is taking a screenshot.
	_IsTakingScreenshot = true;
}

/*
*	Returns if the rendering system is currently taking a screenshot.
*/
NO_DISCARD bool RenderingSystem::IsTakingScreenshot() NOEXCEPT
{
	return _IsTakingScreenshot;
}

/*
*	Stops taking a screenshot and writes the result to the given file path.
*/
void RenderingSystem::StopTakingScreenshot(const char *const RESTRICT file_path) NOEXCEPT
{
	//Signal to other systems that the rendering system is no longertaking a screenshot.
	_IsTakingScreenshot = false;

	//Take the screenshot.
	TakeImmediateScreenshot(file_path);

	//Unpause the game.
	CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);
}

/*
*	Pre-initializes the global render data.
*/
void RenderingSystem::PreInitializeGlobalRenderData() NOEXCEPT
{
	//Get the number of frame buffers.
	const uint8 number_of_framebuffers{ GetNumberOfFramebuffers() };

	//Upsize the buffers.
	_GlobalRenderData._RenderDataTables.Upsize<false>(number_of_framebuffers);
	_GlobalRenderData._DynamicUniformDataBuffers.Upsize<false>(number_of_framebuffers);
	_GlobalRenderData._RemoveGlobalTextureUpdates.Upsize<true>(number_of_framebuffers);
	_GlobalRenderData._AddGlobalTextureUpdates.Upsize<true>(number_of_framebuffers);
	_GlobalRenderData._GlobalCommandPoolData.Upsize<true>(number_of_framebuffers);

	for (uint8 i{ 0 }; i < number_of_framebuffers; ++i)
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
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_HALF_1)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_HALF_2)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_FULL_1)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_FULL_2)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_1)]);
	CreateRenderTarget(GetScaledResolution(1), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_VOLUMETRIC_LIGHTING_BUFFER_2)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_1)]);
	CreateRenderTarget(GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_2)]);
	CreateRenderTarget(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), TextureFormat::R_FLOAT32, &_RenderTargets[UNDERLYING(RenderTarget::SHADOW_MAP)]);
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
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::LINEAR, AddressMode::CLAMP_TO_EDGE, 0), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::LINEAR, AddressMode::REPEAT, 8), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat)]);
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::NEAREST, AddressMode::CLAMP_TO_EDGE, 0), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::LINEAR, MipmapMode::NEAREST, AddressMode::REPEAT, 0), &_Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat)]);
	CreateSampler(SamplerProperties(TextureFilter::NEAREST, MipmapMode::NEAREST, AddressMode::CLAMP_TO_EDGE, 0), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)]);
	CreateSampler(SamplerProperties(TextureFilter::NEAREST, MipmapMode::NEAREST, AddressMode::REPEAT, 0), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat)]);
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 9> bindings
		{
			//Global uniform data.
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::GEOMETRY | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS | ShaderStage::VERTEX),
			
			//Render targets.
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::SampledImage, CatalystShaderConstants::NUMBER_OF_RENDER_TARGETS, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::GEOMETRY | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS | ShaderStage::VERTEX),

			//Global textures.
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::SampledImage, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),
			
			//Global samplers.
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::Sampler, UNDERLYING(Sampler::NumberOfSamplers), ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),
			
			//Global materials.
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::VERTEX),

			//Blue noise textures.
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION),

			//Cloud texture.
			RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::COMPUTE | ShaderStage::FRAGMENT | ShaderStage::RAY_GENERATION),
			
			//Sky texture.
			RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION),

			//Hammersley hemisphere samples uniform buffer.
			RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_GENERATION),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Global)]);
	}

	{
		//Initialize the particle system data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::COMPUTE)
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
*	Post-initializes the global render data.
*/
void RenderingSystem::PostInitializeGlobalRenderData() NOEXCEPT
{
	for (uint8 i{ 0 }; i < GetNumberOfFramebuffers(); ++i)
	{
		//Bind the render targets
		for (uint32 j{ 0 }; j < CatalystShaderConstants::NUMBER_OF_RENDER_TARGETS; ++j)
		{
			BindSampledImageToRenderDataTable(1, j, &_GlobalRenderData._RenderDataTables[i], _RenderTargets[j]);
		}

		//Bind some default texture to the global textures, because... Validation layers tells me I need to do this. (:
		for (uint32 j{ 0 }; j < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++j)
		{
			BindSampledImageToRenderDataTable(2, j, &_GlobalRenderData._RenderDataTables[i], _DefaultTexture2D);
		}

		//Bind all the samplers to the render data table.
		for (uint32 j{ 0 }; j < UNDERLYING(Sampler::NumberOfSamplers); ++j)
		{
			BindSamplerToRenderDataTable(3, j, &_GlobalRenderData._RenderDataTables[i], _Samplers[j]);
		}

		//Bind the cloud texture.
		BindCombinedImageSamplerToRenderDataTable(6, 0, &_GlobalRenderData._RenderDataTables[i], ResourceSystem::Instance->GetTexture3DResource(HashString("Cloud_Texture3D"))->_Texture3DHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeRepeat));

		//Bind the Hammersley hemisphere samples uniform buffer.
		BindUniformBufferToRenderDataTable(8, 0, &_GlobalRenderData._RenderDataTables[i], _HammersleyHemisphereSamplesUniformBuffer);
	}

	//Bind all the blue noise textures to the global render data tables.
	for (uint8 i{ 0 }; i < CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES; ++i)
	{
		//Create the identifier.
		char buffer[32];
		sprintf_s(buffer, "Blue_Noise_%u_Texture2D", i);
		const HashString identifier{ buffer };

		//Retrieve the texture 2D handle.
		const Texture2DHandle texture_2D_handle{ ResourceSystem::Instance->GetTexture2DResource(identifier)->_Texture2DHandle };

		//Bind the texture to the global render data tables.
		for (uint8 j{ 0 }; j < GetNumberOfFramebuffers(); ++j)
		{
			BindCombinedImageSamplerToRenderDataTable(5, i, &_GlobalRenderData._RenderDataTables[j], texture_2D_handle, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat));
		}
	}

	//Initialize the global command pool data.
	for (GlobalCommandPoolData &global_command_pool_data : _GlobalRenderData._GlobalCommandPoolData)
	{
		//Allocate enough thread data for all threads (main thread + task executor threads).
		global_command_pool_data._ThreadData.Upsize<true>(1 + TaskSystem::Instance->GetNumberOfTaskExecutors());

		//Initialize all thread data.
		for (GlobalCommandPoolData::ThreadData &thread_data : global_command_pool_data._ThreadData)
		{
			//Create the command pool.
			CreateCommandPool(Pipeline::Type::Compute, &thread_data._CommandPool);
		}
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

	//Update the global command pool data.
	UpdateGlobalCommandPoolData(current_framebuffer_index);

	//Bind the sky texture.
	BindCombinedImageSamplerToRenderDataTable(7, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], WorldSystem::Instance->GetSkySystem()->GetSkyTexture()->_TextureCubeHandle, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
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

	//Update the previous Perceiver world transform.
	_PreviousPerceiverWorldTransform = _CurrentPerceiverWorldTransform;
	_CurrentPerceiverWorldTransform = Perceiver::Instance->GetWorldTransform();

	//Calculate the previous and current Perceiver matrices, as well as their inverses.
	const Matrix4x4 previous_perceiver_matrix{ Matrix4x4::LookAt(_PreviousPerceiverWorldTransform.GetRelativePosition(_CurrentPerceiverWorldTransform.GetCell()), _PreviousPerceiverWorldTransform.GetRelativePosition(_CurrentPerceiverWorldTransform.GetCell()) + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_PreviousPerceiverWorldTransform.GetRotation()), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_PreviousPerceiverWorldTransform.GetRotation())) };
	const Matrix4x4 current_perceiver_matrix{ Matrix4x4::LookAt(_CurrentPerceiverWorldTransform.GetLocalPosition(), _CurrentPerceiverWorldTransform.GetLocalPosition() + CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_CurrentPerceiverWorldTransform.GetRotation()), CatalystCoordinateSpacesUtilities::RotatedWorldUpVector(_CurrentPerceiverWorldTransform.GetRotation())) };

	//Jitter the projection matrix a bit.
	Vector2<float> current_frame_jitter{ JITTER_SAMPLES[_CurrentJitterIndex] * _DynamicUniformData._InverseScaledResolution };
	Perceiver::Instance->SetProjectionMatrixJitter(current_frame_jitter);

	//Update matrices.
	_DynamicUniformData._PreviousWorldToClipMatrix = *Perceiver::Instance->GetProjectionMatrix() * previous_perceiver_matrix;
	_DynamicUniformData._InverseWorldToPerceiverMatrix = *Perceiver::Instance->GetInversePerceiverMatrix();
	_DynamicUniformData._InversePerceiverToClipMatrix = *Perceiver::Instance->GetInverseProjectionMatrix();
	_DynamicUniformData._WorldToPerceiverMatrix = *Perceiver::Instance->GetPerceiverMatrix();
	_DynamicUniformData._WorldToClipMatrix = *Perceiver::Instance->GetProjectionMatrix() * current_perceiver_matrix;

	//Update vectors.
	_DynamicUniformData._PerceiverForwardVector = Perceiver::Instance->GetForwardVector();
	_DynamicUniformData._PerceiverWorldPosition = Perceiver::Instance->GetWorldTransform().GetLocalPosition();

	_DynamicUniformData._ScaledResolution = Vector2<float32>(static_cast<float32>(GetScaledResolution(0)._Width), static_cast<float32>(GetScaledResolution(0)._Height));
	_DynamicUniformData._InverseScaledResolution = 1.0f / Vector2<float32>(static_cast<float32>(GetScaledResolution(0)._Width), static_cast<float32>(GetScaledResolution(0)._Height));
	_DynamicUniformData._PreviousFramejitter = _DynamicUniformData._CurrentFrameJitter;
	_DynamicUniformData._CurrentFrameJitter = current_frame_jitter;

	//Update floats.
	_DynamicUniformData._DeltaTime = CatalystEngineSystem::Instance->GetDeltaTime();
	_DynamicUniformData._GlobalRandomSeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._TotalTime = CatalystEngineSystem::Instance->GetTotalTime();
	_DynamicUniformData._WindSpeed = 2.5f;

	_DynamicUniformData._BloomIntensity = _RenderingConfiguration.GetBloomIntensity();

	_DynamicUniformData._AspectRatio = static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Width) / static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(0)._Height);

	_DynamicUniformData._CurrentBlueNoiseTextureIndex = _CurrentBlueNoiseTextureIndex;
	_DynamicUniformData._CurrentBlueNoiseTextureOffsetX = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._CurrentBlueNoiseTextureOffsetY = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._ViewDistance = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance;
	_DynamicUniformData._MaximumSkyTextureMipmapLevel = static_cast<float32>(WorldSystem::Instance->GetSkySystem()->GetSkyTexture()->_MipmapLevels);
	_DynamicUniformData._Wetness = WorldSystem::Instance->GetWetness();
	_DynamicUniformData._NearPlane = Perceiver::Instance->GetNearPlane();
	_DynamicUniformData._FarPlane = Perceiver::Instance->GetFarPlane();
	{
		SCOPED_LOCK(TerrainSystem::Instance->GetTerrainProperties()->_HeightMapLock);
		_DynamicUniformData._TerrainHeightMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_HeightMapTextureIndex;
	}
	{
		SCOPED_LOCK(TerrainSystem::Instance->GetTerrainProperties()->_IndexMapLock);
		_DynamicUniformData._TerrainIndexMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_IndexMapTextureIndex;
	}
	{
		SCOPED_LOCK(TerrainSystem::Instance->GetTerrainProperties()->_BlendMapLock);
		_DynamicUniformData._TerrainBlendMapTextureIndex = TerrainSystem::Instance->GetTerrainProperties()->_BlendMapTextureIndex;
	}
	{
		SCOPED_LOCK(TerrainSystem::Instance->GetTerrainProperties()->_HeightMapLock);
		_DynamicUniformData._TerrainMapResolution = static_cast<float32>(TerrainSystem::Instance->GetTerrainProperties()->_HeightMap.GetWidth());
	}

	_DynamicUniformData._SkyIntensity = WorldSystem::Instance->GetSkySystem()->GetSkyIntensity();

	_DynamicUniformData._VolumetricLightingDistance = WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Distance;
	_DynamicUniformData._VolumetricLightingHeight = WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Height;
	_DynamicUniformData._VolumetricLightingThickness = WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Thickness;

	_DynamicUniformData._CurrentWorldGridCell = WorldSystem::Instance->GetCurrentWorldGridCell();
	_DynamicUniformData._WorldGridSize = WorldSystem::Instance->GetWorldGridSize();

	void *const RESTRICT dataChunks[]{ &_DynamicUniformData };
	const uint64 dataSizes[]{ sizeof(DynamicUniformData) };

	UploadDataToBuffer(dataChunks, dataSizes, 1, &_GlobalRenderData._DynamicUniformDataBuffers[current_framebuffer_index]);

	//Update the current jitter index.
	_CurrentJitterIndex = _CurrentJitterIndex == JITTER_SAMPLES.Size() - 1 ? 0 : _CurrentJitterIndex + 1;

	//Update the active noise texture index.
	_CurrentBlueNoiseTextureIndex = _CurrentBlueNoiseTextureIndex == CatalystShaderConstants::NUMBER_OF_BLUE_NOISE_TEXTURES - 1 ? 0 : _CurrentBlueNoiseTextureIndex + 1;
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
		BindSampledImageToRenderDataTable(2, update, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _DefaultTexture2D);
	}

	_GlobalRenderData._RemoveGlobalTextureUpdates[current_framebuffer_index].Clear();

	for (Pair<uint32, Texture2DHandle> &update : _GlobalRenderData._AddGlobalTextureUpdates[current_framebuffer_index])
	{
		BindSampledImageToRenderDataTable(2, update._First, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], update._Second);
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
	BindUniformBufferToRenderDataTable(4, 0, &_GlobalRenderData._RenderDataTables[current_framebuffer_index], _MaterialSystem.GetCurrentMaterialUnifomBuffer());
}

/*
*	Updates the global command pool data.
*/
void RenderingSystem::UpdateGlobalCommandPoolData(const uint8 current_framebuffer_index) NOEXCEPT
{
	for (GlobalCommandPoolData::ThreadData &thread_data : _GlobalRenderData._GlobalCommandPoolData[current_framebuffer_index]._ThreadData)
	{
		//Reset the command pool.
		ResetCommandPool(&thread_data._CommandPool);

		//Reset the number of primary command buffers used.
		thread_data._NumberOfPrimaryCommandBuffersUsed = 0;

		//Reset the number of secondary command buffers used.
		thread_data._NumberOfSecondaryCommandBuffersUsed = 0;
	}
}