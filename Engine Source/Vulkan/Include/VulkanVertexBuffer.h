#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class Vertex;

class VulkanVertexBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanVertexBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanVertexBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan buffer.
	*/
	const VkBuffer& Get() const NOEXCEPT { return vulkanBuffer; }

	/*
	*	Initializes this Vulkan vertex buffer.
	*/
	void Initialize(const DynamicArray<Vertex> &vertices) NOEXCEPT;

	/*
	*	Releases this Vulkan vertex buffer.
	*/
	void Release() NOEXCEPT;

private:

	//The underlying Vulkan buffer.
	VkBuffer vulkanBuffer;

	//The Vulkan device memory.
	VkDeviceMemory vulkanDeviceMemory;

	/*
	*	Creates a buffer create info.
	*/
	void CreateBufferCreateInfo(VkBufferCreateInfo &bufferCreateInfo, const DynamicArray<Vertex> &vertices, const VkBufferUsageFlags &bufferUsageFlags, const uint32 *RESTRICT queueFamilyIndices) const NOEXCEPT;

	/*
	*	Finds the memory type index.
	*/
	uint32 FindMemoryTypeIndex(const uint32 typeFilter, const VkMemoryPropertyFlags memoryPropertyFlags) const NOEXCEPT;

	/*
	*	Creates a memory allocate info.
	*/
	void CreateMemoryAllocateInfo(VkMemoryAllocateInfo &memoryAllocateInfo, const uint64 allocationSize, const uint32 memoryTypeIndex) const NOEXCEPT;

	/*
	*	Creates a buffer copy.
	*/
	void CreateBufferCopy(VkBufferCopy &bufferCopy, const uint64 size) NOEXCEPT;

};