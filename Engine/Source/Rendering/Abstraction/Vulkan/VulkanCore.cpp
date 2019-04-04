//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Functions definitions.
*/
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesNV(
	VkDevice                                    device,
	VkPipelineCache                             pipelineCache,
	uint32_t                                    createInfoCount,
	const VkRayTracingPipelineCreateInfoNV*     pCreateInfos,
	const VkAllocationCallbacks*                pAllocator,
	VkPipeline*                                 pPipelines)
{
	static const auto function{ reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkCreateRayTracingPipelinesNV")) };

	return function(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}