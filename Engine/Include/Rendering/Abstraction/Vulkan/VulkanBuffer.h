#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBuffer.h>

class VulkanBuffer final
{

public:

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return _VulkanBuffer; }

	/*
	*	Initializes this Vulkan buffer.
	*/
	void Initialize(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperties) NOEXCEPT;

	/*
	*	Releases this Vulkan buffer.
	*/
	void Release() NOEXCEPT;

	/*
	*	Uploads data to this Vulkan buffer.
	*
	*	data - Pointer to an array of pointers to the data that should be copied into the buffer.
	*	dataSizes - Pointer to an array of offsets for the data that should be copied into the buffer.
	*	dataChunks - The number of data chunks that should be copied into the buffer.
	*/
	void UploadData(const void *const RESTRICT *const RESTRICT data, const VkDeviceSize *const RESTRICT dataSizes, const uint32 dataChunks, VulkanCommandBuffer *const RESTRICT commandBuffer) NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer _VulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory _VulkanDeviceMemory;

	//The memory properties.
	VkMemoryPropertyFlags _MemoryProperties;

};
#endif