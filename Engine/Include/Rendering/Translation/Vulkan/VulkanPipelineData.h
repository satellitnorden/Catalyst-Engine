#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanQueryPool.h>

class VulkanPipelineData
{

public:

#if VULKAN_TIMESTAMPS_ENABLED
	//The query pools.
	DynamicArray<VulkanQueryPool *RESTRICT> _QueryPools;
#endif

};
#endif