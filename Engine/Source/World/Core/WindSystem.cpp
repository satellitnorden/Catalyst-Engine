//Header file.
#include <World/Core/WindSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes the wind system.
*/
void WindSystem::Initialize() NOEXCEPT
{
	//Initialize the wind direction.
	_WindDirection = Vector3<float32>::Normalize(Vector3<float32>(1.0f, 0.0f, 1.0f));

	//Initialize the wind speed.
	_WindSpeed = 1.0f;

	//Create the render data table layout.
	{
		StaticArray<RenderDataTableLayoutBinding, 1> bindings
		{
			RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::VERTEX)
		};

		RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
	}

	//Create the render data tables.
	_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle& render_data_table : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);
	}

	//Create the uniform buffers.
	_UniformBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (uint64 i{ 0 }; i < _UniformBuffers.Size(); ++i)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(WindRenderingData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &_UniformBuffers[i]);
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &_RenderDataTables[i], _UniformBuffers[i]);
	}

	//Reset the wind uniform data.
	Memory::Set(&_WindUniformData, 0, sizeof(WindUniformData));

	//Register the uniform data.
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
	(
		HashString("Wind"),
		&_WindUniformData,
		sizeof(WindUniformData)
	);
}

/*
*	Updates the wind system during render.
*/
void WindSystem::RenderUpdate() NOEXCEPT
{
	//Update the current wind rendering data.
	_CurrentWindRenderingData._WindDirection = _WindDirection;
	_CurrentWindRenderingData._WindSpeed = _WindSpeed;

	//Set the current render data table.
	_CurrentRenderDataTable = _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];

	//Upload the data to the buffer.
	BufferHandle &current_uniform_buffer{ _UniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	const void *const RESTRICT data_chunks[]{ &_CurrentWindRenderingData };
	const uint64 data_sizes[]{ sizeof(_CurrentWindRenderingData) };

	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_uniform_buffer);

	//Update the wind uniform data.
	_WindUniformData._PreviousWindDirectionSpeed = _WindUniformData._CurrentWindDirectionSpeed;
	_WindUniformData._PreviousWindTime = _WindUniformData._CurrentWindTime;
	_WindUniformData._CurrentWindDirectionSpeed = Vector4<float32>(_WindDirection, _WindSpeed);
	_WindUniformData._CurrentWindTime = fmodf(_WindUniformData._CurrentWindTime + CatalystEngineSystem::Instance->GetDeltaTime(), BaseMathConstants::DOUBLE_PI);
}