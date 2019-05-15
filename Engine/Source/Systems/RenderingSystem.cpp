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
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/RenderPasses/RenderPassManager.h>

//Resources.
#include <Resources/Creation/ResourceCreator.h>

//Systems.
#include <Systems/PhysicsSystem.h>

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

	//Initialize all common materials.
	InitializeCommonMaterials();
}

/*
*	Post-initializes the rendering system.
*/
void RenderingSystem::PostInitializeSystem()
{
	//Post-initialize the lighting system.
	_LightingSystem.PostInitialize();

	//Post-initialize the model system.
	_ModelSystem.PostInitialize();

	//Initialize all render passes.
	RenderingSystemLogic::InitializeRenderPasses();
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

	//Update the lighting system.
	_LightingSystem.Update(context);

	//Update the model system.
	_ModelSystem.Update(context);

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
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::DiffuseIrradiance)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::SpecularIrradiance)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Intermediate)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::PreviousRadiance)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::Scene)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneFeatures1)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::SceneFeatures2)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R8G8B8A8_Byte, &_RenderTargets[UNDERLYING(RenderTarget::SceneFeatures3)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::TemporalAccumulationColorBuffer1)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::TemporalAccumulationDescriptionBuffer1)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::TemporalAccumulationColorBuffer2)]);
	CreateRenderTarget(GetScaledResolution(), TextureFormat::R32G32B32A32_Float, &_RenderTargets[UNDERLYING(RenderTarget::TemporalAccumulationDescriptionBuffer2)]);
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
	CreateSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::Repeat), &_Samplers[UNDERLYING(Sampler::FilterNearest_MipmapModeNearest_AddressModeRepeat)]);
}

/*
*	Initializes all common materials.
*/
void RenderingSystem::InitializeCommonMaterials() NOEXCEPT
{
	//Initialize the gray common material.
	Material &material{ _CommonMaterials[UNDERLYING(CommonMaterial::Gray)] };

	material._Type = Material::Type::Opaque;

	{
		Texture2DData data;

		data._MipmapLevels = 1;
		data._Width = 1;
		data._Height = 1;
		data._Data.UpsizeSlow(1);
		data._Data[0].Reserve(4);
		data._Data[0].EmplaceFast(127);
		data._Data[0].EmplaceFast(127);
		data._Data[0].EmplaceFast(127);
		data._Data[0].EmplaceFast(255);

		GlobalTexture2D texture;

		ResourceCreator::CreateTexture2D(&data, &texture);

		material._FirstTextureIndex = texture._Index;
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

		material._SecondTextureIndex = texture._Index;
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
		data._Data[0].EmplaceFast(0);

		GlobalTexture2D texture;

		ResourceCreator::CreateTexture2D(&data, &texture);

		material._ThirdTextureIndex = texture._Index;
	}
}

/*
*	Initializes all common render data table layouts.
*/
void RenderingSystem::InitializeCommonRenderDataTableLayouts() NOEXCEPT
{
	{
		//Initialize the dynamic uniform data render data table layout.
		constexpr StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Compute | ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES, ShaderStage::RayClosestHit)
		};

		CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_CommonRenderDataTableLayouts[UNDERLYING(CommonRenderDataTableLayout::Global)]);
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
}

/*
*	Updates the dynamic uniform data.
*/
void RenderingSystem::UpdateDynamicUniformData(const uint8 currentFrameBufferIndex) NOEXCEPT
{
	//Update matrices.
	_DynamicUniformData._PerceiverMatrixMinusOne = _DynamicUniformData._PerceiverMatrix;
	_DynamicUniformData._ProjectionMatrixMinusOne = _DynamicUniformData._ProjectionMatrix;
	_DynamicUniformData._InversePerceiverMatrix = *Perceiver::Instance->GetInversePerceiverMatrix();
	_DynamicUniformData._InverseProjectionMatrix = *Perceiver::Instance->GetInverseProjectionMatrix();
	_DynamicUniformData._PerceiverMatrix = *Perceiver::Instance->GetPerceiverMatrix();
	_DynamicUniformData._ProjectionMatrix = *Perceiver::Instance->GetProjectionMatrix();
	_DynamicUniformData._ViewMatrix = *Perceiver::Instance->GetViewMatrix();

	//Update vectors.
	_DynamicUniformData._DirectionalLightColor = _LightingSystem.GetDirectionalLight()->GetColor();
	_DynamicUniformData._DirectionalLightDirection = _LightingSystem.GetDirectionalLight()->GetDirection();
	_DynamicUniformData._PerceiverForwardVector = Perceiver::Instance->GetForwardVector();
	_DynamicUniformData._PerceiverVelocity = Perceiver::Instance->GetPosition() - Vector3<float>(_DynamicUniformData._PerceiverWorldPosition._X, _DynamicUniformData._PerceiverWorldPosition._Y, _DynamicUniformData._PerceiverWorldPosition._Z);
	_DynamicUniformData._PerceiverWorldPosition = Perceiver::Instance->GetPosition();

	//Update floats.
	_DynamicUniformData._DeltaTime = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_DeltaTime;
	_DynamicUniformData._DirectionalLightIntensity = _LightingSystem.GetDirectionalLight()->GetIntensity();
	_DynamicUniformData._EnvironmentBlend = EnvironmentManager::Instance->GetEnvironmentBlend();
	_DynamicUniformData._GlobalRandomSeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._GlobalRandomSeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	_DynamicUniformData._TotalTime = ComponentManager::ReadSingletonComponent<CatalystEngineComponent>()->_TotalTime;
	_DynamicUniformData._WindSpeed = PhysicsSystem::Instance->GetWindSpeed();

	void *const RESTRICT dataChunks[]{ &_DynamicUniformData };
	const uint64 dataSizes[]{ sizeof(DynamicUniformData) };

	UploadDataToBuffer(dataChunks, dataSizes, 1, &_GlobalRenderData._DynamicUniformDataBuffers[currentFrameBufferIndex]);
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
		BindCombinedImageSamplerToRenderDataTable(1, update._First, &_GlobalRenderData._RenderDataTables[currentFrameBufferIndex], update._Second, _Samplers[UNDERLYING(Sampler::FilterLinear_MipmapModeLinear_AddressModeRepeat)]);
	}

	_GlobalRenderData._AddGlobalTextureUpdates[currentFrameBufferIndex].ClearFast();

	//Unlock the global textures.
	_GlobalRenderData._GlobalTexturesLock.WriteUnlock();
}