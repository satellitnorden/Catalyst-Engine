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
	FORCE_INLINE void BindAccelerationStructureMemory(const VkAccelerationStructureNV acceleration_structure, const VkDeviceMemory memory) NOEXCEPT
	{
		VkBindAccelerationStructureMemoryInfoNV bindAccelerationStructureMemoryInfo;

		bindAccelerationStructureMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bindAccelerationStructureMemoryInfo.pNext = nullptr;
		bindAccelerationStructureMemoryInfo.accelerationStructure = acceleration_structure;
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
															const uint32 instance_count,
															const ArrayProxy<VkGeometryNV> &geometry,
															VkAccelerationStructureCreateInfoNV *const RESTRICT acceleration_structure_create_info) NOEXCEPT
	{
		acceleration_structure_create_info->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
		acceleration_structure_create_info->pNext = nullptr;
		acceleration_structure_create_info->compactedSize = 0;
		acceleration_structure_create_info->info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		acceleration_structure_create_info->info.pNext = nullptr;
		acceleration_structure_create_info->info.type = type;
		acceleration_structure_create_info->info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;
		acceleration_structure_create_info->info.instanceCount = instance_count;
		acceleration_structure_create_info->info.geometryCount = static_cast<uint32>(geometry.Size());
		acceleration_structure_create_info->info.pGeometries = geometry.Data();
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
		//Create the instance buffer.
		_InstanceDataBuffer.Initialize(sizeof(VulkanGeometryInstance) * instances.Size(), VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		const void *const RESTRICT data_chunks[]{ instances.Data() };
		const uint64 data_sizes[]{ sizeof(VulkanGeometryInstance) * instances.Size() };

		_InstanceDataBuffer.UploadData(data_chunks, data_sizes, 1);

		//Remember that this acceleration structure has instance data.
		_HasInstanceData = true;
	}

	//Create the acceleration structure create info.
	VkAccelerationStructureCreateInfoNV acceleration_structure_create_info;
	VulkanAccelerationStructureLogic::CreateAccelerationStructureCreateInfo(type, static_cast<uint32>(instances.Size()), geometry, &acceleration_structure_create_info);

	//Create the acceleration structure!
	VULKAN_ERROR_CHECK(vkCreateAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &acceleration_structure_create_info, nullptr, &_VulkanAccelerationStructure));

	//Allocate the acceleration structure memory.
	VulkanUtilities::AllocateAccelerationStructureMemory(_VulkanAccelerationStructure, &_VulkanDeviceMemory);

	//Bind the memory to the acceleration structure.
	VulkanAccelerationStructureLogic::BindAccelerationStructureMemory(_VulkanAccelerationStructure, _VulkanDeviceMemory);

	//Create the scratch buffer and memory.
	VkBuffer scratch_buffer;
	VkDeviceMemory scratch_memory;
	VulkanUtilities::CreateAccelerationStructureScratchBuffer(_VulkanAccelerationStructure, &scratch_buffer, &scratch_memory);

	//Build the acceleration structure.
	VulkanUtilities::BuildAccelerationStructure(type, static_cast<uint32>(instances.Size()), geometry, _HasInstanceData ? _InstanceDataBuffer.Get() : VK_NULL_HANDLE, _VulkanAccelerationStructure, scratch_buffer);

	//Destroy the scratch buffer.
	vkDestroyBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), scratch_buffer, nullptr);

	//Free the scratch memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), scratch_memory, nullptr);
}

/*
*	Releases this Vulkan acceleration structure.
*/
void VulkanAccelerationStructure::Release() NOEXCEPT
{
	//Free the instance data, if necessary.
	if (_HasInstanceData)
	{
		//Release the instance data buffer.
		_InstanceDataBuffer.Release();
	}

	//Free the Vulkan device memory.
	vkFreeMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanDeviceMemory, nullptr);

	//Destroy the acceleration structure.
	vkDestroyAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanAccelerationStructure, nullptr);
}