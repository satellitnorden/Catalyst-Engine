//Header file.
#include <Rendering/Native/LightingSystem.h>

//Core.
#include <Core/General/Padding.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Light uniform data definition.
*/
class LightUniformData final
{

public:

	//The number of lights.
	int32 _NumberOfLights;

	//Some padding.
	Padding<12> _Padding;

	//The light data.
	StaticArray<LightComponent, LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS> _LightData;

};

/*
*	Post-initializes the lighting system.
*/
void LightingSystem::PostInitialize() NOEXCEPT
{
	//Create the render data table layouts.
	CreateRenderDataTableLayouts();

	//Create the render data tables.
	CreateRenderDataTables();

	//Create the uniform buffers.
	CreateUniformBuffers();

	//Create the render targets.
	CreateRenderTargets();
}

/*
*	Updates the lighting system.
*/
void LightingSystem::Update(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	{
		//Update the current compute render data table.
		RenderDataTableHandle &currentComputeRenderDataTable{ _ComputeRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		//Update the light uniform data.
		{
			BufferHandle &currentUniformBuffer{ _UniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

			LightUniformData lightUniformData;

			lightUniformData._NumberOfLights = static_cast<int32>(ComponentManager::GetNumberOfLightComponents());

			for (int32 i{ 0 }; i < lightUniformData._NumberOfLights; ++i)
			{
				lightUniformData._LightData[i] = ComponentManager::GetLightLightComponents()[i];
			}

			const void *const RESTRICT dataChunks[]{ &lightUniformData };
			const uint64 dataSizes[]{ sizeof(LightUniformData) };

			RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &currentUniformBuffer);

			RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &currentComputeRenderDataTable, currentUniformBuffer);
		}

		//Bind the directional light visibility render target to the current render data table.
		RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &currentComputeRenderDataTable, _DirectionalLightVisibilityRenderTarget);

		//Bind the light visibility render targets to the current render data table.
		for (uint64 i{ 0 }, size{ _LightVisibilityRenderTargets.Size() }; i < size; ++i)
		{
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, static_cast<uint32>(i), &currentComputeRenderDataTable, _LightVisibilityRenderTargets[i]);
		}
	}

	{
		//Update the current fragment render data table.
		RenderDataTableHandle &currentFragmentRenderDataTable{ _FragmentRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		//Update the light uniform data.
		{
			BufferHandle &currentUniformBuffer{ _UniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

			LightUniformData lightUniformData;

			lightUniformData._NumberOfLights = static_cast<int32>(ComponentManager::GetNumberOfLightComponents());

			for (int32 i{ 0 }; i < lightUniformData._NumberOfLights; ++i)
			{
				lightUniformData._LightData[i] = ComponentManager::GetLightLightComponents()[i];
			}

			const void *const RESTRICT dataChunks[]{ &lightUniformData };
			const uint64 dataSizes[]{ sizeof(LightUniformData) };

			RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &currentUniformBuffer);

			RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &currentFragmentRenderDataTable, currentUniformBuffer);
		}

		//Bind the directional light visibility render target to the current render data table.
		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &currentFragmentRenderDataTable, _DirectionalLightVisibilityRenderTarget, RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));

		//Bind the light visibility render targets to the current render data table.
		for (uint64 i{ 0 }, size{ _LightVisibilityRenderTargets.Size() }; i < size; ++i)
		{
			RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(2, static_cast<uint32>(i), &currentFragmentRenderDataTable, _LightVisibilityRenderTargets[i], RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
		}
	}
}

/*
*	Returns the current lighting data compute render data table.
*/
RenderDataTableHandle LightingSystem::GetCurrentLightingDataComputeRenderDataTable() const NOEXCEPT
{
	//Return the current lighting data compute render data table.
	return _ComputeRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Returns the current lighting data fragment render data table.
*/
RenderDataTableHandle LightingSystem::GetCurrentLightingDataFragmentRenderDataTable() const NOEXCEPT
{
	//Return the current lighting data fragment render data table.
	return _FragmentRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Creates the render data table layouts.
*/
void LightingSystem::CreateRenderDataTableLayouts() NOEXCEPT
{
	//Create the compute render data table layout.
	{
		StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Compute | ShaderStage::RayClosestHit),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute | ShaderStage::RayClosestHit),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS, ShaderStage::Compute | ShaderStage::RayClosestHit)
		};

		RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_ComputeRenderDataTableLayout);
	}

	//Create the fragment render data table layout.
	{
		StaticArray<RenderDataTableLayoutBinding, 3> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
			RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS, ShaderStage::Fragment)
		};

		RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_FragmentRenderDataTableLayout);
	}
}

/*
*	Creates the render data tables.
*/
void LightingSystem::CreateRenderDataTables() NOEXCEPT
{
	//Create the compute render data tables.
	_ComputeRenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _ComputeRenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_ComputeRenderDataTableLayout, &renderDataTable);
	}

	//Create the fragment render data tables.
	_FragmentRenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _FragmentRenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_FragmentRenderDataTableLayout, &renderDataTable);
	}
}

/*
*	Creates the uniform buffers.
*/
void LightingSystem::CreateUniformBuffers() NOEXCEPT
{
	//Create the uniform buffers.
	_UniformBuffers.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &uniformBuffer : _UniformBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(LightUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &uniformBuffer);
	}
}

/*
*	Creates the render targets.
*/
void LightingSystem::CreateRenderTargets() NOEXCEPT
{
	//Create the directional light visibility render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(), TextureFormat::R32_Float, &_DirectionalLightVisibilityRenderTarget);

	//Create the light visibility render targets.
	for (RenderTargetHandle &lightVisibilityRenderTarget : _LightVisibilityRenderTargets)
	{
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(), TextureFormat::R32_Float, &lightVisibilityRenderTarget);
	}
}