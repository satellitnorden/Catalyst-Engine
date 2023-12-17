//Header file.
#include <Rendering/Native/BufferManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Updates the  buffer manager during the render update phase.
*/
void BufferManager::RenderUpdate() NOEXCEPT
{
	//Cache the current framebuffer index.
	const uint8 current_framebuffer_index{ RenderingSystem::Instance->GetCurrentFramebufferIndex() };

	//Upload data to all uniform buffers.
	for (RegisteredUniformBuffer &registered_uniform_buffer : _RegisteredUniformBuffers)
	{
		BufferHandle &current_buffer{ registered_uniform_buffer._Buffers[current_framebuffer_index] };

		RenderingSystem::Instance->UploadDataToBuffer
		(
			&registered_uniform_buffer._Data,
			&registered_uniform_buffer._DataSize,
			1,
			&current_buffer
		);
	}

	//Upload data to all storage buffers.
	for (RegisteredStorageBuffer &registered_storage_buffer : _RegisteredStorageBuffers)
	{
		BufferHandle& current_buffer{ registered_storage_buffer._Buffers[current_framebuffer_index] };
		registered_storage_buffer._StorageBufferFunction(&registered_storage_buffer._Data, registered_storage_buffer._Arguments);

		const void *const RESTRICT data{ registered_storage_buffer._Data.Data() };
		const uint64 data_size{ registered_storage_buffer._Data.Size() };

		RenderingSystem::Instance->UploadDataToBuffer
		(
			&data,
			&data_size,
			1,
			&current_buffer
		);
	}
}

/*
*	Registers a uniform buffer.
*/
void BufferManager::RegisterUniformBuffer
(
	const HashString identifier,
	const void *const RESTRICT data,
	const uint64 data_size
) NOEXCEPT
{
	//Add the new registered uniform buffer.
	_RegisteredUniformBuffers.Emplace();
	RegisteredUniformBuffer &new_registered_uniform_buffer{ _RegisteredUniformBuffers.Back() };

	new_registered_uniform_buffer._Identifier = identifier;
	new_registered_uniform_buffer._Data = data;
	new_registered_uniform_buffer._DataSize = data_size;

	new_registered_uniform_buffer._Buffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &buffer : new_registered_uniform_buffer._Buffers)
	{
		RenderingSystem::Instance->CreateBuffer
		(
			new_registered_uniform_buffer._DataSize,
			BufferUsage::UniformBuffer,
			MemoryProperty::HostCoherent | MemoryProperty::HostVisible,
			&buffer
		);
	}
}

/*
*	Registers a storage buffer.
*/
void BufferManager::RegisterStorageBuffer
(
	const HashString identifier,
	const uint64 initial_size,
	const StorageBufferFunction storage_buffer_function,
	void *const RESTRICT arguments
) NOEXCEPT
{
	//Add the new registered storage buffer.
	_RegisteredStorageBuffers.Emplace();
	RegisteredStorageBuffer &new_registered_storage_buffer{ _RegisteredStorageBuffers.Back() };

	new_registered_storage_buffer._Identifier = identifier;
	new_registered_storage_buffer._StorageBufferFunction = storage_buffer_function;
	new_registered_storage_buffer._Arguments = arguments;

	new_registered_storage_buffer._Buffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &buffer : new_registered_storage_buffer._Buffers)
	{
		RenderingSystem::Instance->CreateBuffer
		(
			initial_size,
			BufferUsage::StorageBuffer,
			MemoryProperty::HostCoherent | MemoryProperty::HostVisible,
			&buffer
		);
	}
}

/*
*	Retrieves the uniform buffer with the given identifier.
*/
NO_DISCARD BufferHandle BufferManager::GetUniformBuffer(const HashString identifier, const uint8 framebuffer_index) NOEXCEPT
{
	for (RegisteredUniformBuffer &registered_uniform_buffer : _RegisteredUniformBuffers)
	{
		if (registered_uniform_buffer._Identifier == identifier)
		{
			return registered_uniform_buffer._Buffers[framebuffer_index];
		}
	}

	ASSERT(false, "Couldn't find uniform buffer!");

	return EMPTY_HANDLE;
}

/*
*	Retrieves the storage buffer with the given identifier.
*/
NO_DISCARD BufferHandle BufferManager::GetStorageBuffer(const HashString identifier, const uint8 framebuffer_index) NOEXCEPT
{
	for (RegisteredStorageBuffer  registered_storage_buffer : _RegisteredStorageBuffers)
	{
		if (registered_storage_buffer._Identifier == identifier)
		{
			return registered_storage_buffer._Buffers[framebuffer_index];
		}
	}

	ASSERT(false, "Couldn't find storage buffer!");

	return EMPTY_HANDLE;
}