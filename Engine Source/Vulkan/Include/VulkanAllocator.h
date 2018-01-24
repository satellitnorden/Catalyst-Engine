#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanAllocation.h>

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
	*	Default constructor.
	*/
	VulkanAllocator() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanAllocator() CATALYST_NOEXCEPT;

	/*
	*	Initializes the Vulkan allocator.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Releases the Vulkan allocator.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns an allocation.
	*/
	VulkanAllocation GetAllocation(const MemoryProperties memoryProperties, const VkDeviceSize allocationSize) CATALYST_NOEXCEPT;

private:

	//Container for all device memories for all memory properties.
	DynamicArray<VkDeviceMemory> deviceMemories[MemoryProperties::NumberOfMemoryProperties];

	//Container for all offsets for all memory properties.
	uint32 offsets[MemoryProperties::NumberOfMemoryProperties];

	/*
	*	Given a type filter and memory properties, returns the memory index.
	*/
	uint32 FindMemoryTypeIndex(const uint32 typeFilter, const VkMemoryPropertyFlags memoryProperties) const CATALYST_NOEXCEPT;

};