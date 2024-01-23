//Header file.
#include <Rendering/Native/LightingSystem.h>

//Core.
#include <Core/General/Padding.h>

//Components.
#include <Components/Components/LightComponent.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Light header data definition.
*/
class LightHeaderData final
{

public:

	//The number of lights.
	uint32 _NumberOfLights;

	//The maximum number of shadow casting lights.
	uint32 _MaximumNumberOfShadowCastingLights;

	//Padding.
	Padding<8> _Padding;

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

	//Create the light data buffers.
	CreateLightDataBuffers();

	//Register the storage buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterStorageBuffer
	(
		HashString("Lighting"),
		sizeof(LightHeaderData) + sizeof(ShaderLightComponent) * 64,
		[](DynamicArray<byte> *const RESTRICT data, void* const RESTRICT arguments)
		{
			data->Clear();
			LightingSystem *const RESTRICT lighting_system{ static_cast<LightingSystem *const RESTRICT>(arguments) };

			LightHeaderData header_data;

			header_data._NumberOfLights = static_cast<uint32>(LightComponent::Instance->NumberOfInstances());
			header_data._MaximumNumberOfShadowCastingLights = LightingConstants::MAXIMUM_NUMBER_OF_SHADOW_CASTING_LIGHTS;

			for (uint64 i{ 0 }; i < sizeof(LightHeaderData); ++i)
			{
				data->Emplace(((const byte *const RESTRICT)&header_data)[i]);
			}

			for (const ShaderLightComponent &shader_light_component : lighting_system->_ShaderLightComponents)
			{
				for (uint64 i{ 0 }; i < sizeof(ShaderLightComponent); ++i)
				{
					data->Emplace(((const byte *const RESTRICT)&shader_light_component)[i]);
				}
			}
		},
		this
	);
}

/*
*	Updates the lighting system during the render update phase.
*/
void LightingSystem::RenderUpdate() NOEXCEPT
{
	//Cache relevant data.
	const uint32 number_of_lights{ static_cast<uint32>(LightComponent::Instance->NumberOfInstances()) };

	//Update the current render data table.
	RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Update the current light data buffer.
	{
		BufferHandle &current_light_data_buffer{ _LightDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };
		uint64 &current_light_data_buffer_size{ _LightDataBufferSizes[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

		//Does the light data buffer need to be recreated?
		if (current_light_data_buffer_size != CatalystBaseMath::Maximum<uint32>(number_of_lights, 1))
		{
			if (current_light_data_buffer)
			{
				RenderingSystem::Instance->DestroyBuffer(&current_light_data_buffer);
			}

			RenderingSystem::Instance->CreateBuffer(sizeof(uint32) + sizeof(LightComponent) * number_of_lights, BufferUsage::StorageBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &current_light_data_buffer);
		
			current_light_data_buffer_size = number_of_lights;
		}

		//Update the shader light components.
		_ShaderLightComponents.Clear();
		
		for (uint64 i{ 0 }; i < number_of_lights; ++i)
		{
			_ShaderLightComponents.Emplace(LightComponent::Instance->InstanceToEntity(i));
		}

		//Fill in the header data.
		LightHeaderData header_data;

		header_data._NumberOfLights = number_of_lights;
		header_data._MaximumNumberOfShadowCastingLights = LightingConstants::MAXIMUM_NUMBER_OF_SHADOW_CASTING_LIGHTS;

		//Upload the light data to the buffer.
		const void *const RESTRICT data_chunks[]{ &header_data, _ShaderLightComponents.Data() };
		const uint64 data_sizes[]{ sizeof(LightHeaderData), sizeof(ShaderLightComponent) * number_of_lights };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 2, &current_light_data_buffer);

		//Bind the light data buffer.
		RenderingSystem::Instance->BindStorageBufferToRenderDataTable(0, 0, &current_render_data_table, current_light_data_buffer);
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
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::FRAGMENT | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS)
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
*	Creates the light data buffers.
*/
void LightingSystem::CreateLightDataBuffers() NOEXCEPT
{
	//Create the light data buffers.
	_LightDataBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &light_data_buffer : _LightDataBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(LightHeaderData) + sizeof(ShaderLightComponent), BufferUsage::StorageBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &light_data_buffer);
	}

	//Set the light data buffer sizes.
	_LightDataBufferSizes.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (uint64 &light_data_buffer_size : _LightDataBufferSizes)
	{
		light_data_buffer_size = 1;
	}
}