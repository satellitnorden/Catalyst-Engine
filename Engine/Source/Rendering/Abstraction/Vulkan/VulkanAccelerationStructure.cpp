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
	FORCE_INLINE void BindAccelerationStructureMemory(const VkAccelerationStructureNV acceleration_structure, const VkDeviceMemory memory, const VkDeviceSize memory_offset) NOEXCEPT
	{
		VkBindAccelerationStructureMemoryInfoNV bindAccelerationStructureMemoryInfo;

		bindAccelerationStructureMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bindAccelerationStructureMemoryInfo.pNext = nullptr;
		bindAccelerationStructureMemoryInfo.accelerationStructure = acceleration_structure;
		bindAccelerationStructureMemoryInfo.memory = memory;
		bindAccelerationStructureMemoryInfo.memoryOffset = memory_offset;
		bindAccelerationStructureMemoryInfo.deviceIndexCount = 0;
		bindAccelerationStructureMemoryInfo.pDeviceIndices = nullptr;

		VULKAN_ERROR_CHECK(vkBindAccelerationStructureMemoryNV(VulkanInterface::Instance->GetLogicalDevice().Get(), 1, &bindAccelerationStructureMemoryInfo));
	}

	/*
	*	Retrieves the memory requirements for the given acceleration structure.
	*/
	FORCE_INLINE void GetMemoryRequirements(const VkAccelerationStructureNV acceleration_structure, VkMemoryRequirements *const RESTRICT memory_requirements) NOEXCEPT
	{
		//Query the memory requirements.
		VkAccelerationStructureMemoryRequirementsInfoNV acceleration_structure_memory_requirements_info;

		acceleration_structure_memory_requirements_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		acceleration_structure_memory_requirements_info.pNext = nullptr;
		acceleration_structure_memory_requirements_info.accelerationStructure = acceleration_structure;
		acceleration_structure_memory_requirements_info.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;

		VkMemoryRequirements2 _memory_requirements;
		vkGetAccelerationStructureMemoryRequirementsNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &acceleration_structure_memory_requirements_info, &_memory_requirements);

		*memory_requirements = _memory_requirements.memoryRequirements;
	}

	/*
	*	Retrieves the scratch memory requirements for the given acceleration structure.
	*/
	FORCE_INLINE void GetScratchMemoryRequirements(const VkAccelerationStructureNV acceleration_structure, VkMemoryRequirements *const RESTRICT memory_requirements) NOEXCEPT
	{
		//Query the memory requirements.
		VkAccelerationStructureMemoryRequirementsInfoNV acceleration_structure_memory_requirements_info;

		acceleration_structure_memory_requirements_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		acceleration_structure_memory_requirements_info.pNext = nullptr;
		acceleration_structure_memory_requirements_info.accelerationStructure = acceleration_structure;
		acceleration_structure_memory_requirements_info.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV;

		VkMemoryRequirements2 _memory_requirements;
		vkGetAccelerationStructureMemoryRequirementsNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &acceleration_structure_memory_requirements_info, &_memory_requirements);

		*memory_requirements = _memory_requirements.memoryRequirements;
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
void VulkanAccelerationStructure::Initialize
(
	const VkAccelerationStructureTypeNV type,
	const ArrayProxy<VulkanGeometryInstance> &instances,
	const ArrayProxy<VkGeometryNV> &geometry,
	VulkanCommandBuffer *const RESTRICT command_buffer
) NOEXCEPT
{
	//Statics.
	static thread_local VulkanCommandPool *const RESTRICT COMMAND_POOL{ VulkanInterface::Instance->CreateAsyncComputeCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

	//TODO: Engine crashes without this. Doesn't seem to have a huge performance impact, but plz rewrite resource destruction code ASAP. :c
	VulkanInterface::Instance->GetMainQueue()->WaitIdle();

	//Set the type.
	_Type = type;

	//Retrieve the command buffer.
	VulkanCommandBuffer temporary_command_buffer;

	VulkanCommandBuffer *RESTRICT _command_buffer{ command_buffer ? command_buffer : nullptr };

	if (command_buffer)
	{
		_command_buffer = command_buffer;
	}

	else
	{
		
		
		COMMAND_POOL->AllocatePrimaryCommandBuffer(temporary_command_buffer);
		_command_buffer = &temporary_command_buffer;

		_command_buffer->BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}

	//Create the acceleration structure create info.
	VkAccelerationStructureCreateInfoNV acceleration_structure_create_info;
	VulkanAccelerationStructureLogic::CreateAccelerationStructureCreateInfo(type, static_cast<uint32>(instances.Size()), geometry, &acceleration_structure_create_info);

	//Create the acceleration structure!
	VULKAN_ERROR_CHECK(vkCreateAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &acceleration_structure_create_info, nullptr, &_VulkanAccelerationStructure));

	//Query the memory requirements.
	VkMemoryRequirements memory_requirements;

	VulkanAccelerationStructureLogic::GetMemoryRequirements(_VulkanAccelerationStructure, &memory_requirements);

	//Allocate the acceleration structure memory.
	VmaAllocationCreateInfo allocation_create_info = { };

	allocation_create_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
	allocation_create_info.requiredFlags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	vmaAllocateMemory(VULKAN_MEMORY_ALLOCATOR, &memory_requirements, &allocation_create_info, &_AccelerationStructureMemoryAllocation, &_AccelerationStructureMemoryAllocationInfo);

	//Bind the memory to the acceleration structure.
	VulkanAccelerationStructureLogic::BindAccelerationStructureMemory(_VulkanAccelerationStructure, _AccelerationStructureMemoryAllocationInfo.deviceMemory, _AccelerationStructureMemoryAllocationInfo.offset);

	//Create the instance data, if necessary.
	if (!instances.Empty())
	{
		//Create the instance data buffers.
		_InstanceDataHostBuffer.Initialize(sizeof(VulkanGeometryInstance) * instances.Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		const void *const RESTRICT data_chunks[]{ instances.Data() };
		const uint64 data_sizes[]{ sizeof(VulkanGeometryInstance) * instances.Size() };

		_InstanceDataHostBuffer.UploadData(data_chunks, data_sizes, 1);

		_InstanceDataDeviceBuffer.Initialize(sizeof(VulkanGeometryInstance) * instances.Size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		_command_buffer->CommandCopyBuffer(_InstanceDataHostBuffer.Get(), _InstanceDataDeviceBuffer.Get(), sizeof(VulkanGeometryInstance) * instances.Size());

		//Set the number of instances.
		_NumberOfInstances = static_cast<uint32>(instances.Size());
	}

	//Query the scratch memory requirements.
	VkMemoryRequirements scratch_memory_requirements;

	VulkanAccelerationStructureLogic::GetScratchMemoryRequirements(_VulkanAccelerationStructure, &scratch_memory_requirements);

	//Allocate the scratch buffer.
	_ScratchBuffer.Initialize(scratch_memory_requirements.size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//Record the build command!
	_command_buffer->CommandBuildAccelerationStructure
	(
		_Type,
		_NumberOfInstances,
		_NumberOfInstances > 0 ? _InstanceDataDeviceBuffer.Get() : VK_NULL_HANDLE,
		static_cast<uint32>(geometry.Size()),
		geometry.Data(),
		_VulkanAccelerationStructure,
		_ScratchBuffer.Get()
	);

	//If no external command buffer was provided, wait for the temporary one to finish.
	if (!command_buffer)
	{
		//End the command buffer.
		_command_buffer->End();

		//Submit the command buffer to the transfer queue.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetAsyncComputeQueue()->Submit(*_command_buffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the copy command buffer.
		COMMAND_POOL->FreeCommandBuffer(*_command_buffer);
	}

	//No need to keep the scratch buffer around for bottom level acceleration structures.
	if (_Type == VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV)
	{
		_ScratchBuffer.Release();
	}
}

/*
*	Releases this Vulkan acceleration structure.
*/
void VulkanAccelerationStructure::Release() NOEXCEPT
{
	//Destroy the acceleration structure.
	vkDestroyAccelerationStructureNV(VulkanInterface::Instance->GetLogicalDevice().Get(), _VulkanAccelerationStructure, nullptr);

	//Free the acceleration structure memory allocation.
	vmaFreeMemory(VULKAN_MEMORY_ALLOCATOR, _AccelerationStructureMemoryAllocation);

	//Free the instance data, if necessary.
	if (_NumberOfInstances > 0)
	{
		//Release the instance data buffers.
		_InstanceDataHostBuffer.Release();
		_InstanceDataDeviceBuffer.Release();
	}

	//Free the scratch buffer, if necessary.
	if (_Type == VkAccelerationStructureTypeNV::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV)
	{
		_ScratchBuffer.Release();
	}
}