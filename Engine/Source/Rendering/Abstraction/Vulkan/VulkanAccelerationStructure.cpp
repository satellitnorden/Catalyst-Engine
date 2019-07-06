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
		accelerationStructureCreateInfo->info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;
		accelerationStructureCreateInfo->info.instanceCount = instanceCount;
		accelerationStructureCreateInfo->info.geometryCount = static_cast<uint32>(geometry.Size());
		accelerationStructureCreateInfo->info.pGeometries = geometry.Data();
	}
}

/*
*	Initializes this Vulkan acceleration structure.
*/
void VulkanAccelerationStructure::Initialize(const VkAccelerationStructureTypeNV type, const ArrayProxy<VulkanGeometryInstance> &instances, const ArrayProxy<VkGeometryNV> &geometry) NOEXCEPT
{
	//Create the instance data, if necessary.
	if (!instances.Empty())
	{
		//Compute the size.
		VkDeviceSize size{ sizeof(VulkanGeometryInstance) * instances.Size() };

		//Create the buffer.
		VulkanUtilities::CreateVulkanBuffer(size, VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, _InstanceDataBuffer, _InstanceDataDeviceMemory);
	
		//Copy the data into the buffer.
		void *mappedMemory;
		VULKAN_ERROR_CHECK(vkMapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _InstanceDataDeviceMemory, 0, VK_WHOLE_SIZE, 0, &mappedMemory));

		Memory::CopyMemory(mappedMemory, instances.Data(), instances.Size());

		vkUnmapMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _InstanceDataDeviceMemory);

		//Remember that this acceleration structure has instance data.
		_HasInstanceData = true;
	}

	//Create the acceleration structure create info.
	VkAccelerationStructureCreateInfoNV accelerationStructureCreateInfo;
	VulkanAccelerationStructureLogic::CreateAccelerationStructureCreateInfo(type, static_cast<uint32>(instances.Size()), geometry, &accelerationStructureCreateInfo);

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
	VulkanUtilities::BuildAccelerationStructure(type, static_cast<uint32>(instances.Size()), geometry, _HasInstanceData ? _InstanceDataBuffer : VK_NULL_HANDLE, _VulkanAccelerationStructure, scratchBuffer);

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
	//Free the instance data, if necessary.
	if (_HasInstanceData)
	{
		//Destroy the instance data buffer.
		vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), _InstanceDataBuffer, nullptr);

		//Free the instance data device memory.
		vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _InstanceDataDeviceMemory, nullptr);
	}

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the acceleration structure.
	vkDestroyAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanAccelerationStructure, nullptr);
}