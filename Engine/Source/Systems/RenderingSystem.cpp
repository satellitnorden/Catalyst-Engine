//Header file.
#include <Systems/RenderingSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Entities.
#include <Entities/Types/Entity.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Core/CatalystBaseMath.h>
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/DynamicUniformData.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/Pipelines/Core/Pipelines.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/RenderPasses/RenderPassManager.h>

//Systems.
#include <Systems/InputSystem.h>
#include <Systems/LightingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TerrainSystem.h>

//Vegetation.
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationModel.h>
#include <Vegetation/TreeVegetationMaterial.h>
#include <Vegetation/TreeVegetationModel.h>

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

	//Set the directional shadow map resolution.
	_DirectionalShadowMapResolution = Resolution(configuration._ShadowMapResolution, configuration._ShadowMapResolution);

	//Pre-initialize the rendering system.
	PreInitialize();

	//Initialize all depth buffers.
	InitializeDepthBuffers();

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
*	Post-initializes the rendering system.
*/
void RenderingSystem::PostInitializeSystem()
{
	//Initialize all render passes.
	RenderingSystemLogic::InitializeRenderPasses();

	//Register all pipelines.
	RegisterPipelines();
}

/*
*	Updates the rendering system.
*/
void RenderingSystem::UpdateSystem(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Begin the frame.
	BeginFrame();

	//Update the global render data.
	UpdateGlobalRenderData();

	//Render all render passes.
	for (Pipeline *const RESTRICT pipeline : _Pipelines)
	{
		pipeline->Execute();
	}

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
*	Returns the global render data table.
*/
RenderDataTableHandle RenderingSystem::GetGlobalRenderDataTable() const NOEXCEPT
{
	return _GlobalRenderData._RenderDataTables[GetCurrentFramebufferIndex()];
}

/*
*	Adds a terrain height texture to the global render data and returns it's index.
*/
uint8 RenderingSystem::AddTerrainHeightTextureToGlobalRenderData(Texture2DHandle texture) NOEXCEPT
{
	//Lock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteLock();

	//Find the first available index and store it.
	uint8 index{ UINT8_MAXIMUM };

	for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES; ++i)
	{
		//If this is available, grab it!
		if (!_GlobalRenderData._TerrainHeightTextureSlots[i])
		{
			index = i;
			_GlobalRenderData._TerrainHeightTextureSlots[i] = true;

			break;
		}
	}

	ASSERT(index != UINT8_MAXIMUM, "If no index could be found, then, well... This is bad. ):");

	//Add the terrain height texture updates.
	for (DynamicArray<Pair<uint8, Texture2DHandle>> &terrainHeightTextureUpdate : _GlobalRenderData._AddTerrainHeightTextureUpdates)
	{
		terrainHeightTextureUpdate.EmplaceSlow(index, texture);
	}

	//Unlock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteUnlock();

	//Return the index.
	return index;
}

/*
*	Returns a terrain height texture to the global render data and marks it's index as available.
*/
void RenderingSystem::ReturnTerrainHeightTextureToGlobalRenderData(const uint8 index) NOEXCEPT
{
	//Lock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteLock();

	//Add the terrain height texture updates.
	for (DynamicArray<uint8> &terrainHeightTextureUpdate : _GlobalRenderData._RemoveTerrainHeightTextureUpdates)
	{
		terrainHeightTextureUpdate.EmplaceSlow(index);
	}

	//Mark the terrain height texture slot as available.
	_GlobalRenderData._TerrainHeightTextureSlots[index] = false;

	//Unlock the terrain height texture slots.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteUnlock();
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
	_GlobalRenderData._RemoveTerrainHeightTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._AddTerrainHeightTextureUpdates.UpsizeSlow(numberOfFrameBuffers);
	_GlobalRenderData._TerrainPatchDataBuffers.UpsizeFast(numberOfFrameBuffers);
	_GlobalRenderData._TerrainMaterialDataBuffers.UpsizeFast(numberOfFrameBuffers);

	for (uint8 i{ 0 }; i < numberOfFrameBuffers; ++i)
	{
		//Create the render data table.
		CreateRenderDataTable(GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global), &_GlobalRenderData._RenderDataTables[i]);

		//Create the dynamic uniform data buffer.
		CreateUniformBuffer(sizeof(DynamicUniformData), BufferUsage::UniformBuffer, &_GlobalRenderData._DynamicUniformDataBuffers[i]);

		//Bind the dynamic uniform data buffer to the render data table.
		BindUniformBufferToRenderDataTable(0, 0, &_GlobalRenderData._RenderDataTables[i], _GlobalRenderData._DynamicUniformDataBuffers[i]);

		//Bind all samplers.
		for (uint8 j{ 0 }; j < UNDERLYING(Sampler::NumberOfSamplers); ++j)
		{
			BindSamplerToRenderDataTable(1, j, &_GlobalRenderData._RenderDataTables[i], _Samplers[j]);
		}

		//Create the terrain patch data buffer.
		CreateUniformBuffer(sizeof(TerrainPatchInstanceRenderInformation) * RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, BufferUsage::UniformBuffer, &_GlobalRenderData._TerrainPatchDataBuffers[i]);
	
		//Bind the terrain patch data buffer to the render data table.
		BindUniformBufferToRenderDataTable(4, 0, &_GlobalRenderData._RenderDataTables[i], _GlobalRenderData._TerrainPatchDataBuffers[i]);

		//Create the terrain material data buffer.
		CreateUniformBuffer(sizeof(TerrainMaterial) * RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, BufferUsage::UniformBuffer, &_GlobalRenderData._TerrainMaterialDataBuffers[i]);

		//Bind the terrain material data buffer to the render data table.
		BindUniformBufferToRenderDataTable(5, 0, &_GlobalRenderData._RenderDataTables[i], _GlobalRenderData._TerrainMaterialDataBuffers[i]);
	}

	//Mark all global texture slots as free.
	for (uint32 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES; ++i)
	{
		_GlobalRenderData._GlobalTextureSlots[i] = false;
	}

	//Mark all terrain height texture slots as free.
	for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES; ++i)
	{
		_GlobalRenderData._TerrainHeightTextureSlots[i] = false;
	}
}

/*
*	Initializes all depth buffers.
*/
void RenderingSystem::InitializeDepthBuffers() NOEXCEPT
{
	CreateDepthBuffer(GetDirectionalShadowMapResolution(), &_DepthBuffers[UNDERLYING(DepthBuffer::DirectionalLight)]);
	CreateDepthBuffer(GetScaledResolution(), &_DepthBuffers[UNDERLYING(DepthBuffer::SceneBuffer)]);
}

/*
*	Initializes all render targets.
*/
void RenderingSystem::InitializeRenderTargets() NOEXCEPT
{
	//Initialize all render targets.
	CreateRenderTarget(GetDirectionalShadowMapResolution(), TextureFormat::R32_Float, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadowMap)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::DirectionalShadow)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate)]);
	CreateRenderTarget(GetScaledResolution() / 2, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::IntermediateHalf)]);
	CreateRenderTarget(GetScaledResolution() / 4, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::IntermediateQuarter)]);
	CreateRenderTarget(GetScaledResolution() / 8, TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::IntermediateEighth)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferAlbedo)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferNormalDepth)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneBufferMaterialProperties)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Scene)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneProperties)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::ScreenSpaceAmbientOcclusion)]);
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
	CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToBorder), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToBorder)]);
	CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge)]);
}

/*
*	Registers all pipelines.
*/
void RenderingSystem::RegisterPipelines() NOEXCEPT
{
	//Register all render passes.
	_Pipelines[UNDERLYING(PipelineSubStage::ToneMapping)] = ToneMappingGraphicsPipeline::Instance;
	_Pipelines[UNDERLYING(PipelineSubStage::AntiAliasing)] = AntiAliasingGraphicsPipeline::Instance;
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
	_Pipelines[UNDERLYING(PipelineSubStage::RenderOverride)] = RenderOverrideGraphicsPipeline::Instance;
#endif
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 6> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Vertex | ShaderStage::TessellationControl | ShaderStage::TessellationEvaluation | ShaderStage::Geometry | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::Sampler, UNDERLYING(Sampler::NumberOfSamplers), ShaderStage::Vertex | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::SampledImage, RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::Vertex | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES, ShaderStage::Vertex),
			RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Vertex | ShaderStage::Fragment),
			RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Global)]);
	}

	{
		//Initialize the ocean render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::OceanMaterial)]);
	}

	{
		//Initialize the particle system render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::ParticleSystem)]);
	}

	{
		//Initialize the bloom render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::GaussianBlur)]);
	}
}

/*
*	Updates the global render data.
*/
void RenderingSystem::UpdateGlobalRenderData() NOEXCEPT
{
	//Retrieve the current frame buffer index.
	const uint8 currentFrameBufferIndex{ GetCurrentFramebufferIndex() };

	//Update the dynamic uniform data.
	UpdateDynamicUniformData(currentFrameBufferIndex);

	//Update the global textures.
	UpdateGlobalTextures(currentFrameBufferIndex);

	//Update the terrain height textures.
	UpdateTerrainHeightTextures(currentFrameBufferIndex);

	//Update the terrain patch data.
	UpdateTerrainPatchData(currentFrameBufferIndex);

	//Update the terrain material data.
	UpdateTerrainMaterialData(currentFrameBufferIndex);
}

/*
*	Updates the dynamic uniform data.
*/
void RenderingSystem::UpdateDynamicUniformData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	DynamicUniformData data;

	//Update matrices.
	data._DirectionalLightViewMatrix = *LightingSystem::Instance->GetDirectionalLight()->GetViewMatrix();
	data._InversePerceiverMatrix = *Perceiver::Instance->GetInversePerceiverMatrix();
	data._InverseProjectionMatrix = *Perceiver::Instance->GetInverseProjectionMatrix();
	data._PerceiverMatrix = *Perceiver::Instance->GetPerceiverMatrix();
	data._ProjectionMatrix = *Perceiver::Instance->GetProjectionMatrix();
	data._ViewMatrix = *Perceiver::Instance->GetViewMatrix();

	//Update vectors.
	data._DirectionalLightColor = LightingSystem::Instance->GetDirectionalLight()->GetColor();
	data._DirectionalLightDirection = LightingSystem::Instance->GetDirectionalLight()->GetDirection();
	data._PerceiverWorldPosition = Perceiver::Instance->GetPosition();
	data._WindDirection = PhysicsSystem::Instance->GetWindDirection();

	//Update floats.
	data._DeltaTime = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_DeltaTime;
	data._DirectionalLightIntensity = LightingSystem::Instance->GetDirectionalLight()->GetIntensity();
	data._EnvironmentBlend = EnvironmentManager::Instance->GetEnvironmentBlend();
	data._GlobalRandomSeed = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	data._TotalTime = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_TotalTime;
	data._WindSpeed = PhysicsSystem::Instance->GetWindSpeed();

	UploadDataToUniformBuffer(&data, &_GlobalRenderData._DynamicUniformDataBuffers[currentFrameBufferIndex]);
}

/*
*	Updates the global textures.
*/
void RenderingSystem::UpdateGlobalTextures(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	//Lock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteLock();

	_GlobalRenderData._RemoveGlobalTextureUpdates[currentFrameBufferIndex].ClearFast();

	for (Pair<uint32, Texture2DHandle> &update : _GlobalRenderData._AddGlobalTextureUpdates[currentFrameBufferIndex])
	{
		BindSampledImageToRenderDataTable(2, update._First, &_GlobalRenderData._RenderDataTables[currentFrameBufferIndex], update._Second);
	}

	_GlobalRenderData._AddGlobalTextureUpdates[currentFrameBufferIndex].ClearFast();

	//Unlock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();
}

/*
*	Updates the terrain height textures.
*/
void RenderingSystem::UpdateTerrainHeightTextures(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	//Lock the terrain height textures.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteLock();

	_GlobalRenderData._RemoveTerrainHeightTextureUpdates[currentFrameBufferIndex].ClearFast();

	for (Pair<uint8, Texture2DHandle> &update : _GlobalRenderData._AddTerrainHeightTextureUpdates[currentFrameBufferIndex])
	{
		BindCombinedImageSamplerToRenderDataTable(3, static_cast<uint32>(update._First), &_GlobalRenderData._RenderDataTables[currentFrameBufferIndex], update._Second, GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	}

	_GlobalRenderData._AddTerrainHeightTextureUpdates[currentFrameBufferIndex].ClearFast();

	//Unlock the terrain height textures.
	_GlobalRenderData._TerrainHeightTexturesLock.WriteUnlock();
}

/*
*	Updates the terrain patch data.
*/
void RenderingSystem::UpdateTerrainPatchData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	StaticArray<TerrainPatchInstanceRenderInformation, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES> terrainUniformData;

	const DynamicArray<TerrainPatchRenderInformation> *const RESTRICT informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

	uint64 counter{ 0 };

	for (uint64 i{ 0 }, size{ informations->Size() }; i < size; ++i)
	{
		if (!TEST_BIT(informations->At(i)._Visibility, VisibilityFlag::Perceiver))
		{
			continue;
		}

		terrainUniformData[counter++] = informations->At(i)._InstanceInformation;
	}

	UploadDataToUniformBuffer(terrainUniformData.Data(), &_GlobalRenderData._TerrainPatchDataBuffers[currentFrameBufferIndex]);
}

/*
*	Updates the terrain material data.
*/
void RenderingSystem::UpdateTerrainMaterialData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	UploadDataToUniformBuffer(TerrainSystem::Instance->GetTerrainMaterials()->Data(), &_GlobalRenderData._TerrainMaterialDataBuffers[currentFrameBufferIndex]);
}