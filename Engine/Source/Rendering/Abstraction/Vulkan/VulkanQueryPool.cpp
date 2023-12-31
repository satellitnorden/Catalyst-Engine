#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanQueryPool.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Initializes this Vulkan query pool.
*/
void VulkanQueryPool::Initialize(const VkQueryType query_type, const uint32 query_count) NOEXCEPT
{
	//Set the query count.
	_QueryCount = query_count;

	//Create the query pool create info.
	VkQueryPoolCreateInfo query_pool_create_info;
	CreateQueryPoolCreateInfo(query_pool_create_info, query_type, query_count);

	//Create the query pool!
	VULKAN_ERROR_CHECK(vkCreateQueryPool(VulkanInterface::Instance->GetLogicalDevice().Get(), &query_pool_create_info, nullptr, &_VulkanQueryPool));
}

/*
*	Releases this Vulkan query pool.
*/
void VulkanQueryPool::Release() NOEXCEPT
{
	//Destroy the Vulkan query pool.
	vkDestroyQueryPool(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanQueryPool, nullptr);
}

/*
*	Resets this query pool.
*/
void VulkanQueryPool::Reset() NOEXCEPT
{
	vkResetQueryPool(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanQueryPool, 0, _QueryCount);
}

/*
*	Creates a query pool create info.
*/
void VulkanQueryPool::CreateQueryPoolCreateInfo(VkQueryPoolCreateInfo& query_pool_create_info, const VkQueryType query_type, const uint32 query_count) const NOEXCEPT
{
	query_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	query_pool_create_info.pNext = nullptr;
	query_pool_create_info.flags = 0;
	query_pool_create_info.queryType = query_type;
	query_pool_create_info.queryCount = query_count;
	query_pool_create_info.pipelineStatistics = 0;
}
#endif