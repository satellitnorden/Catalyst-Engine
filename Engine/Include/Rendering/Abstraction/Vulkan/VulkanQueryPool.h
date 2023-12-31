#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

class VulkanQueryPool final
{

public:

	/*
	*	Returns the underlying Vulkan query pool.
	*/
	const VkQueryPool& Get() const NOEXCEPT { return _VulkanQueryPool; }

	/*
	*	Initializes this Vulkan query pool.
	*/
	void Initialize(const VkQueryType query_type, const uint32 query_count) NOEXCEPT;

	/*
	*	Releases this Vulkan query pool.
	*/
	void Release() NOEXCEPT;

	/*
	*	Resets this query pool.
	*/
	void Reset() NOEXCEPT;

private:

	//The underlying Vulkan query pool.
	VkQueryPool _VulkanQueryPool;

	//The query count.
	uint32 _QueryCount;

	/*
	*	Creates a query pool create info.
	*/
	void CreateQueryPoolCreateInfo(VkQueryPoolCreateInfo &query_pool_create_info, const VkQueryType query_type, const uint32 query_count) const NOEXCEPT;

};
#endif