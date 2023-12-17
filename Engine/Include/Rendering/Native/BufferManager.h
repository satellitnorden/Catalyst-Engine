#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Type aliases.
using StorageBufferFunction = void(*)(DynamicArray<byte> *const RESTRICT data, void *const RESTRICT /*arguments*/);

class BufferManager final
{

public:

	/*
	*	Updates the buffer manager during the render update phase.
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
	*	Registers a storage buffer.
	*/
	void RegisterStorageBuffer
	(
		const HashString identifier,
		const uint64 initial_size,
		const StorageBufferFunction storage_buffer_function,
		void *const RESTRICT arguments
	) NOEXCEPT;

	/*
	*	Retrieves the uniform buffer with the given identifier.
	*/
	NO_DISCARD BufferHandle GetUniformBuffer(const HashString identifier, const uint8 framebuffer_index) NOEXCEPT;

	/*
	*	Retrieves the storage buffer with the given identifier.
	*/
	NO_DISCARD BufferHandle GetStorageBuffer(const HashString identifier, const uint8 framebuffer_index) NOEXCEPT;

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

	/*
	*	Registered storage buffer class definition.
	*/
	class RegisteredStorageBuffer final
	{

	public:

		//The buffers.
		DynamicArray<BufferHandle> _Buffers;

		//The identifier.
		HashString _Identifier;

		//The storage buffer function.
		StorageBufferFunction _StorageBufferFunction;

		//The arguments.
		void *RESTRICT _Arguments;

		//The data.
		DynamicArray<byte> _Data;

	};

	//The registered uniform buffers.
	DynamicArray<RegisteredUniformBuffer> _RegisteredUniformBuffers;

	//The registered storage buffers.
	DynamicArray<RegisteredStorageBuffer> _RegisteredStorageBuffers;

};