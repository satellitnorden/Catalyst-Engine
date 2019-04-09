//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

/*
*	Functions definitions.
*/
VKAPI_ATTR VkResult VKAPI_CALL vkBindAccelerationStructureMemoryNV(
	VkDevice                                    device,
	uint32_t                                    bindInfoCount,
	const VkBindAccelerationStructureMemoryInfoNV* pBindInfos)
{
	static const auto function{ reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkBindAccelerationStructureMemoryNV")) };

	return function(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructureNV(
	VkCommandBuffer                             commandBuffer,
	const VkAccelerationStructureInfoNV*        pInfo,
	VkBuffer                                    instanceData,
	VkDeviceSize                                instanceOffset,
	VkBool32                                    update,
	VkAccelerationStructureNV                   dst,
	VkAccelerationStructureNV                   src,
	VkBuffer                                    scratch,
	VkDeviceSize                                scratchOffset)
{
	static const auto function{ reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkCmdBuildAccelerationStructureNV")) };

	return function(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
}

VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysNV(
	VkCommandBuffer                             commandBuffer,
	VkBuffer                                    raygenShaderBindingTableBuffer,
	VkDeviceSize                                raygenShaderBindingOffset,
	VkBuffer                                    missShaderBindingTableBuffer,
	VkDeviceSize                                missShaderBindingOffset,
	VkDeviceSize                                missShaderBindingStride,
	VkBuffer                                    hitShaderBindingTableBuffer,
	VkDeviceSize                                hitShaderBindingOffset,
	VkDeviceSize                                hitShaderBindingStride,
	VkBuffer                                    callableShaderBindingTableBuffer,
	VkDeviceSize                                callableShaderBindingOffset,
	VkDeviceSize                                callableShaderBindingStride,
	uint32_t                                    width,
	uint32_t                                    height,
	uint32_t                                    depth)
{
	static const auto function{ reinterpret_cast<PFN_vkCmdTraceRaysNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkCmdTraceRaysNV")) };

	return function(commandBuffer,
					raygenShaderBindingTableBuffer,
					raygenShaderBindingOffset,
					missShaderBindingTableBuffer,
					missShaderBindingOffset,
					missShaderBindingStride,
					hitShaderBindingTableBuffer,
					hitShaderBindingOffset,
					hitShaderBindingStride,
					callableShaderBindingTableBuffer,
					callableShaderBindingOffset,
					callableShaderBindingStride,
					width,
					height,
					depth);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureNV(
	VkDevice                                    device,
	const VkAccelerationStructureCreateInfoNV*  pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkAccelerationStructureNV*                  pAccelerationStructure)
{
	static const auto function{ reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkCreateAccelerationStructureNV")) };

	return function(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

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

VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureNV(
	VkDevice                                    device,
	VkAccelerationStructureNV                   accelerationStructure,
	const VkAllocationCallbacks*                pAllocator)
{
	static const auto function{ reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkDestroyAccelerationStructureNV")) };

	return function(device, accelerationStructure, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetAccelerationStructureHandleNV(
	VkDevice                                    device,
	VkAccelerationStructureNV                   accelerationStructure,
	size_t                                      dataSize,
	void*                                       pData)
{
	static const auto function{ reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkGetAccelerationStructureHandleNV")) };

	return function(device, accelerationStructure, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureMemoryRequirementsNV(
	VkDevice                                    device,
	const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
	VkMemoryRequirements2KHR*                   pMemoryRequirements)
{
	static const auto function{ reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkGetAccelerationStructureMemoryRequirementsNV")) };

	return function(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesNV(
	VkDevice                                    device,
	VkPipeline                                  pipeline,
	uint32_t                                    firstGroup,
	uint32_t                                    groupCount,
	size_t                                      dataSize,
	void*                                       pData)
{
	static const auto function{ reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(vkGetDeviceProcAddr(VulkanInterface::Instance->GetLogicalDevice().Get(), "vkGetRayTracingShaderGroupHandlesNV")) };

	return function(device, pipeline, firstGroup, groupCount, dataSize, pData);
}