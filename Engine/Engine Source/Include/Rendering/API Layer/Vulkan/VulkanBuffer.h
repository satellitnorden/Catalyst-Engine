#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

class VulkanBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return vulkanBuffer; }

	/*
	*	Initializes this Vulkan buffer.
	*
	*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
	*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
	*	dataChunks - The number of data chunks that should be copied into the buffer.
	*/
	void Initialize(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT;

	/*
	*	Releases this Vulkan buffer.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

};