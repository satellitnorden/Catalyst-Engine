#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanAllocation.h>

/*
*	Enumeration covering all different memory properties.
*/
enum MemoryProperties : uint8
{
	DeviceLocal,
	HostVisibleAndCoherent,
	NumberOfMemoryProperties
};

class VulkanAllocator final
{

public:

	/*
	*	Initializes the Vulkan allocator.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Releases the Vulkan allocator.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns an allocation.
	*/
	VulkanAllocation GetAllocation(const MemoryProperties memoryProperties, const VkDeviceSize allocationSize) NOEXCEPT;

private:

	//Container for all device memories for all memory properties.
	DynamicArray<VkDeviceMemory> _DeviceMemories[MemoryProperties::NumberOfMemoryProperties];

	//Container for all offsets for all memory properties.
	uint32 _Offsets[MemoryProperties::NumberOfMemoryProperties];

	/*
	*	Given a type filter and memory properties, returns the memory index.
	*/
	uint32 FindMemoryTypeIndex(const uint32 typeFilter, const VkMemoryPropertyFlags memoryProperties) const NOEXCEPT;

};
#endif