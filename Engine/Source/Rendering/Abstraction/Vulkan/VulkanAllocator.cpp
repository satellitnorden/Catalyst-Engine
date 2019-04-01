#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanAllocator.h>

/*
*	Initializes the Vulkan allocator.
*/
void VulkanAllocator::Initialize() NOEXCEPT
{

}

/*
*	Releases the Vulkan allocator.
*/
void VulkanAllocator::Release() NOEXCEPT
{

}

/*
*	Given a type filter and memory properties, returns the memory index.
*/
uint32 VulkanAllocator::FindMemoryTypeIndex(const uint32 typeFilter, const VkMemoryPropertyFlags memoryProperties) const NOEXCEPT
{
	return 0;
}
#endif