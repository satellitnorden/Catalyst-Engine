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
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Create the uniform buffers.
	CreateUniformBuffers();
}

/*
*	Updates the lighting system during the render update phase.
*/
void LightingSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

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

		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &currentRenderDataTable, currentUniformBuffer);

		//Store the number of active lights.
		_NumberOfActiveLights = lightUniformData._NumberOfLights;
	}
}

/*
*	Returns the current lighting data render data table.
*/
RenderDataTableHandle LightingSystem::GetCurrentLightingDataRenderDataTable() const NOEXCEPT
{
	//Return the current lighting data render data table.
	return _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Creates the render data table layout.
*/
void LightingSystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Create the render data table layout.
	{
		StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration)
		};

		RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
	}
}

/*
*	Creates the render data tables.
*/
void LightingSystem::CreateRenderDataTables() NOEXCEPT
{
	//Create the render data tables.
	_RenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);
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