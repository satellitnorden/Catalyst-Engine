#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class Vertex;

class VulkanVertexBuffer
{

public:

	/*
	*	Default constructor.
	*/
	VulkanVertexBuffer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanVertexBuffer() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	CATALYST_FORCE_INLINE const VkBuffer& Get() const CATALYST_NOEXCEPT { return vulkanBuffer; }

	/*
	*	Initializes this Vulkan vertex buffer.
	*/
	void Initialize(const DynamicArray<Vertex> &vertices) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan vertex buffer.
	*/
	void Release() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	/*
	*	Creates a buffer create info.
	*/
	void CreateBufferCreateInfo(VkBufferCreateInfo &bufferCreateInfo, const DynamicArray<Vertex> &vertices, const VkBufferUsageFlags &bufferUsageFlags, const uint32 *CATALYST_RESTRICT queueFamilyIndices) const CATALYST_NOEXCEPT;

	/*
	*	Finds the memory type index.
	*/
	uint32 FindMemoryTypeIndex(const uint32 typeFilter, const VkMemoryPropertyFlags memoryPropertyFlags) const CATALYST_NOEXCEPT;

	/*
	*	Creates a memory allocate info.
	*/
	void CreateMemoryAllocateInfo(VkMemoryAllocateInfo &memoryAllocateInfo, const uint64 allocationSize, const uint32 memoryTypeIndex) const CATALYST_NOEXCEPT;

	/*
	*	Creates a buffer copy.
	*/
	void CreateBufferCopy(VkBufferCopy &bufferCopy, const uint64 size) CATALYST_NOEXCEPT;

};