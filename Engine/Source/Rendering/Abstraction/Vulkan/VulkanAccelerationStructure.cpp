//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanAccelerationStructure.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanUtilities.h>

//Vulkan acceleration structure logic.
namespace VulkanAccelerationStructureLogic
{

	/*
	*	Binds the memory to the acceleration structure.
	*/
	FORCE_INLINE void BindAccelerationStructureMemory(const VkAccelerationStructureNV accelerationStructure, const VkDeviceMemory memory) NOEXCEPT
	{
		VkBindAccelerationStructureMemoryInfoNV bindAccelerationStructureMemoryInfo;

		bindAccelerationStructureMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bindAccelerationStructureMemoryInfo.pNext = nullptr;
		bindAccelerationStructureMemoryInfo.accelerationStructure = accelerationStructure;
		bindAccelerationStructureMemoryInfo.memory = memory;
		bindAccelerationStructureMemoryInfo.memoryOffset = 0;
		bindAccelerationStructureMemoryInfo.deviceIndexCount = 0;
		bindAccelerationStructureMemoryInfo.pDeviceIndices = nullptr;

		VULKAN_ERROR_CHECK(vkBindAccelerationStructureMemoryNV(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &bindAccelerationStructureMemoryInfo));
	}

	/*
	*	Creates the acceleration structure create info.
	*/
	FORCE_INLINE void CreateAccelerationStructureCreateInfo(const VkAccelerationStructureTypeNV type,
															const uint32 instanceCount,
															const ArrayProxy<VkGeometryNV> &geometry,
															VkAccelerationStructureCreateInfoNV *const RESTRICT accelerationStructureCreateInfo) NOEXCEPT
	{
		accelerationStructureCreateInfo->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
		accelerationStructureCreateInfo->pNext = nullptr;
		accelerationStructureCreateInfo->compactedSize = 0;
		accelerationStructureCreateInfo->info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		accelerationStructureCreateInfo->info.pNext = nullptr;
		accelerationStructureCreateInfo->info.type = type;
		accelerationStructureCreateInfo->info.flags = 0;
		accelerationStructureCreateInfo->info.instanceCount = instanceCount;
		accelerationStructureCreateInfo->info.geometryCount = static_cast<uint32>(geometry.Size());
		accelerationStructureCreateInfo->info.pGeometries = geometry.Data();
	}
}

/*
*	Initializes this Vulkan acceleration structure.
*/
void VulkanAccelerationStructure::Initialize(const VkAccelerationStructureTypeNV type, const uint32 instanceCount, const ArrayProxy<VkGeometryNV> &geometry, const VkBuffer instanceData) NOEXCEPT
{
	//Create the acceleration structure create info.
	VkAccelerationStructureCreateInfoNV accelerationStructureCreateInfo;
	VulkanAccelerationStructureLogic::CreateAccelerationStructureCreateInfo(type, instanceCount, geometry, &accelerationStructureCreateInfo);

	//Create the acceleration structure!
	VULKAN_ERROR_CHECK(vkCreateAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &accelerationStructureCreateInfo, nullptr, &_VulkanAccelerationStructure));

	//Allocate the acceleration structure memory.
	VulkanUtilities::AllocateAccelerationStructureMemory(_VulkanAccelerationStructure, &_VulkanDeviceMemory);

	//Bind the memory to the acceleration structure.
	VulkanAccelerationStructureLogic::BindAccelerationStructureMemory(_VulkanAccelerationStructure, _VulkanDeviceMemory);

	//Create the scratch buffer and memory.
	VkBuffer scratchBuffer;
	VkDeviceMemory scratchMemory;
	VulkanUtilities::CreateAccelerationStructureScratchBuffer(_VulkanAccelerationStructure, false, &scratchBuffer, &scratchMemory);

	//Build the acceleration structure.
	//if (type == VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV)
	{
		VulkanUtilities::BuildAccelerationStructure(type, instanceCount, geometry, instanceData, _VulkanAccelerationStructure, scratchBuffer);
	}

	//Destroy the scratch buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), scratchBuffer, nullptr);

	//Free the scratch memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), scratchMemory, nullptr);
}

/*
*	Releases this Vulkan acceleration structure.
*/
void VulkanAccelerationStructure::Release() NOEXCEPT
{
	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the acceleration structure.
	vkDestroyAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanAccelerationStructure, nullptr);
}