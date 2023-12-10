//Header file.
#include <Rendering/Native/UniformBufferManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Updates the uniform buffer manager during the render update phase.
*/
void UniformBufferManager::RenderUpdate() NOEXCEPT
{
	//Upload data to all uniform buffers.
	const uint8 current_framebuffer_index{ RenderingSystem::Instance->GetCurrentFramebufferIndex() };

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
}

/*
*	Registers a uniform buffer.
*/
void UniformBufferManager::RegisterUniformBuffer
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
*	Retrieves the uniform buffer with the given identifier.
*/
NO_DISCARD BufferHandle UniformBufferManager::GetUniformBuffer(const HashString identifier, const uint8 framebuffer_index) NOEXCEPT
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