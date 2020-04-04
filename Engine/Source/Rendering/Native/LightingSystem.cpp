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
	uint32 _NumberOfLights;

	//The maximum number of shadow casting lights.
	uint32 _MaximumNumberOfShadowCastingLights;

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

	//Create the light uniform data buffers.
	CreateLightUniformDataBuffers();

	//Create the light data buffers.
	CreateLightDataBuffers();
}

/*
*	Updates the lighting system during the render update phase.
*/
void LightingSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Cache relevant data.
	const uint64 number_of_lights{ ComponentManager::GetNumberOfLightComponents() };

	//Update the current render data table.
	RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Update the current light uniform data buffer.
	{
		BufferHandle &current_light_uniform_data_buffer{ _LightUniformDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		LightUniformData light_uniform_data;

		light_uniform_data._NumberOfLights = static_cast<uint32>(number_of_lights);
		light_uniform_data._MaximumNumberOfShadowCastingLights = LightingConstants::MAXIMUM_NUMBER_OF_SHADOW_CASTING_LIGHTS;

		const void *const RESTRICT data_chunks[]{ &light_uniform_data };
		const uint64 data_sizes[]{ sizeof(LightUniformData) };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_light_uniform_data_buffer);

		//Bind the light uniform data buffer.
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &current_render_data_table, current_light_uniform_data_buffer);
	}

	//Update the current light data buffer.
	{
		BufferHandle &current_light_data_buffer{ _LightDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };
		uint64 &current_light_data_buffer_size{ _LightDataBufferSizes[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		//Does the light data buffer need to be recreated?
		if (current_light_data_buffer_size != CatalystBaseMath::Maximum<uint64>(number_of_lights, 1))
		{
			if (current_light_data_buffer)
			{
				RenderingSystem::Instance->DestroyBuffer(&current_light_data_buffer);
			}

			RenderingSystem::Instance->CreateBuffer(sizeof(LightComponent) * number_of_lights, BufferUsage::StorageBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &current_light_data_buffer);
		
			current_light_data_buffer_size = number_of_lights;
		}

		//Upload the light data to the buffer.
		const void *const RESTRICT data_chunks[]{ ComponentManager::GetLightLightComponents() };
		const uint64 data_sizes[]{ sizeof(LightComponent) * number_of_lights };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_light_data_buffer);

		//Bind the light data buffer.
		RenderingSystem::Instance->BindStorageBufferToRenderDataTable(1, 0, &current_render_data_table, current_light_data_buffer);
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
		StaticArray<RenderDataTableLayoutBinding, 2> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
			RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::Fragment | ShaderStage::RayClosestHit | ShaderStage::RayGeneration)
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
	_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);
	}
}

/*
*	Creates the light uniform data buffers.
*/
void LightingSystem::CreateLightUniformDataBuffers() NOEXCEPT
{
	//Create the light uniform data buffers.
	_LightUniformDataBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &light_uniform_data_buffer : _LightUniformDataBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(LightUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &light_uniform_data_buffer);
	}
}

/*
*	Creates the light data buffers.
*/
void LightingSystem::CreateLightDataBuffers() NOEXCEPT
{
	//Create the light data buffers.
	_LightDataBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &light_data_buffer : _LightDataBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(LightComponent), BufferUsage::StorageBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &light_data_buffer);
	}

	//Set the light data buffer sizes.
	_LightDataBufferSizes.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (uint64 &light_data_buffer_size : _LightDataBufferSizes)
	{
		light_data_buffer_size = 1;
	}
}