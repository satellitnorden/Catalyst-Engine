#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class UniformBufferManager final
{

public:

	/*
	*	Updates the uniform buffer manager during the render update phase.
	*/
	void RenderUpdate() NOEXCEPT;

	/*
	*	Registers a uniform buffer.
	*/
	void RegisterUniformBuffer
	(
		const HashString identifier,
		const void *const RESTRICT data,
		const uint64 data_size
	) NOEXCEPT;

	/*
	*	Retrieves the uniform buffer with the given identifier.
	*/
	NO_DISCARD BufferHandle GetUniformBuffer(const HashString identifier, const uint8 framebuffer_index) NOEXCEPT;

private:

	/*
	*	Registered uniform buffer class definition.
	*/
	class RegisteredUniformBuffer final
	{

	public:

		//The buffers.
		DynamicArray<BufferHandle> _Buffers;

		//The identifier.
		HashString _Identifier;

		//The data.
		const void *RESTRICT _Data;

		//The data size.
		uint64 _DataSize;
	
	};

	//The registered uniform buffers.
	DynamicArray<RegisteredUniformBuffer> _RegisteredUniformBuffers;

};