#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBufferAbstraction.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>

class VulkanUtilities
{

public:

	/*
	*	Builds an acceleration structure.
	*/
	static void BuildAccelerationStructure(const VkAccelerationStructureTypeNV type, const uint32 instanceCount, const ArrayProxy<VkGeometryNV> &geometry, const VkBuffer instanceData, const VkAccelerationStructureNV accelerationStructure, const VkBuffer scratchBuffer) NOEXCEPT
	{
		//Create the command pool.
		static thread_local VulkanCommandPool *const RESTRICT command_pool{ VulkanInterface::Instance->CreateComputeCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

		//Create a command buffer.
		VulkanCommandBuffer command_buffer;
		command_pool->AllocatePrimaryCommandBuffer(command_buffer);

		//Begin the command buffer.
		command_buffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Issue the build command!
		VkAccelerationStructureInfoNV accelerationStructureInfo;

		accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		accelerationStructureInfo.pNext = nullptr;
		accelerationStructureInfo.type = type;
		accelerationStructureInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_NV;
		accelerationStructureInfo.instanceCount = instanceCount;
		accelerationStructureInfo.geometryCount = static_cast<uint32>(geometry.Size());
		accelerationStructureInfo.pGeometries = geometry.Data();

		vkCmdBuildAccelerationStructureNV(	command_buffer.Get(),
											&accelerationStructureInfo,
											instanceData,
											0,
											VK_FALSE,
											accelerationStructure,
											VK_NULL_HANDLE,
											scratchBuffer,
											0);

		//End the command buffer.
		command_buffer.End();

		//Submit the command buffer to the transfer queue.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetComputeQueue()->Submit(command_buffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the copy command buffer.
		command_pool->FreeCommandBuffer(command_buffer);
	}

	/*
	*	Copies a Vulkan buffer to another Vulkan buffer.
	*/
	static void CopyBufferToBuffer(const VkDeviceSize &size, const VkBuffer &sourceBuffer, VkBuffer &destinationBuffer) NOEXCEPT
	{
		//Create the command pool.
		static thread_local VulkanCommandPool *const RESTRICT commandPool{ VulkanInterface::Instance->CreateTransferCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

		//Create a command buffer for the copy operation.
		VulkanCommandBuffer copyCommandBuffer;
		commandPool->AllocatePrimaryCommandBuffer(copyCommandBuffer);

		//Begin the command buffer.
		copyCommandBuffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Create the buffer copy.
		VkBufferCopy bufferCopy;

		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = 0;
		bufferCopy.size = size;

		//Copy the buffer.
		vkCmdCopyBuffer(copyCommandBuffer.Get(), sourceBuffer, destinationBuffer, 1, &bufferCopy);

		//End the copy command buffer.
		copyCommandBuffer.End();

		//Submit the command buffer to the transfer queue.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetTransferQueue()->Submit(copyCommandBuffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the copy command buffer.
		commandPool->FreeCommandBuffer(copyCommandBuffer);
	}

	/*
	*	Copies a Vulkan buffer to a Vulkan image.
	*/
	static void CopyBufferToImage(const VkBuffer &vulkanBuffer, VkImage &vulkanImage, const uint32 mipLevels, const uint32 layerCount, const uint32 width, const uint32 height, const uint32 depth, const uint32 texture_channels) NOEXCEPT
	{
		//Create the command pool.
		static thread_local VulkanCommandPool *const RESTRICT commandPool{ VulkanInterface::Instance->CreateTransferCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

		//Create the transfer command buffer.
		VulkanCommandBuffer transferCommandBuffer;
		commandPool->AllocatePrimaryCommandBuffer(transferCommandBuffer);

		//Create the buffer image copy.
		DynamicArray<VkBufferImageCopy> bufferImageCopies;
		bufferImageCopies.Reserve(mipLevels);

		VkDeviceSize currentOffset{ 0 };

		for (uint32 i = 0; i < mipLevels; ++i)
		{
			VkBufferImageCopy bufferImageCopy;

			bufferImageCopy.bufferOffset = currentOffset;
			bufferImageCopy.bufferRowLength = 0;
			bufferImageCopy.bufferImageHeight = 0;
			bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferImageCopy.imageSubresource.mipLevel = i;
			bufferImageCopy.imageSubresource.baseArrayLayer = 0;
			bufferImageCopy.imageSubresource.layerCount = layerCount;
			bufferImageCopy.imageOffset = { 0, 0, 0 };
			bufferImageCopy.imageExtent = { width >> i, height >> i, CatalystBaseMath::Maximum<uint32>(depth >> i, 1) };

			bufferImageCopies.Emplace(bufferImageCopy);

			currentOffset += (width >> i) * (height >> i) * CatalystBaseMath::Maximum<uint32>(depth >> i, 1) * texture_channels * sizeof(byte);
		}

		//Begin the transfer ommand buffer.
		transferCommandBuffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the copy command to the transfer command buffer.
		vkCmdCopyBufferToImage(transferCommandBuffer.Get(), vulkanBuffer, vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(bufferImageCopies.Size()), bufferImageCopies.Data());

		//End the transfer command buffer.
		transferCommandBuffer.End();

		//Submit the command buffer.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetTransferQueue()->Submit(transferCommandBuffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the transfer command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the transfer command buffer,
		commandPool->FreeCommandBuffer(transferCommandBuffer);
	}

	/*
	*	Copies a Vulkan image to a Vulkan buffer.
	*/
	static void CopyImageToBuffer(const uint32 imageWidth, const uint32 imageHeight, const VkImage vulkanImage, const VkBuffer vulkanBuffer) NOEXCEPT
	{
		//Create the command pool.
		static thread_local VulkanCommandPool *const RESTRICT commandPool{ VulkanInterface::Instance->CreateTransferCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

		//Create the buffer image copy.
		VkBufferImageCopy bufferImageCopy;

		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = 0;
		bufferImageCopy.bufferImageHeight = 0;
		bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferImageCopy.imageSubresource.baseArrayLayer = 0;
		bufferImageCopy.imageSubresource.layerCount = 1;
		bufferImageCopy.imageSubresource.mipLevel = 0;
		bufferImageCopy.imageOffset = { 0, 0, 0 };
		bufferImageCopy.imageExtent = { imageWidth, imageHeight, 1 };

		//Create the transfer command buffer.
		VulkanCommandBuffer transferCommandBuffer;
		commandPool->AllocatePrimaryCommandBuffer(transferCommandBuffer);

		//Begin the transfer ommand buffer.
		transferCommandBuffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the copy command to the transfer command buffer.
		vkCmdCopyImageToBuffer(transferCommandBuffer.Get(), vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vulkanBuffer, 1, &bufferImageCopy);

		//End the transfer command buffer.
		transferCommandBuffer.End();

		//Submit the command buffer.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetTransferQueue()->Submit(transferCommandBuffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the transfer command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the transfer command buffer,
		commandPool->FreeCommandBuffer(transferCommandBuffer);
	}

	/*
	*	Copies a Vulkan image to a Vulkan image.
	*/
	static void CopyImageToImage(const uint32 imageWidth, const uint32 imageHeight, const VkImage sourceImage, const VkImage destinationImage) NOEXCEPT
	{
		//Create the command pool.
		static thread_local VulkanCommandPool *const RESTRICT commandPool{ VulkanInterface::Instance->CreateTransferCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

		//Create the image copy.
		VkImageCopy imageCopy;
		imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopy.srcSubresource.baseArrayLayer = 0;
		imageCopy.srcSubresource.layerCount = 1;
		imageCopy.srcSubresource.mipLevel = 0;
		imageCopy.srcOffset = { 0, 0, 0 };
		imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopy.dstSubresource.baseArrayLayer = 0;
		imageCopy.dstSubresource.layerCount = 1;
		imageCopy.dstSubresource.mipLevel = 0;
		imageCopy.dstOffset = { 0, 0, 0 };
		imageCopy.extent = { imageWidth, imageHeight, 1 };

		//Create the transfer command buffer.
		VulkanCommandBuffer transferCommandBuffer;
		commandPool->AllocatePrimaryCommandBuffer(transferCommandBuffer);

		//Begin the transfer ommand buffer.
		transferCommandBuffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the copy command to the transfer command buffer.
		vkCmdCopyImage(transferCommandBuffer.Get(), sourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destinationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

		//End the transfer command buffer.
		transferCommandBuffer.End();

		//Submit the command buffer.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetTransferQueue()->Submit(transferCommandBuffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the transfer command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the transfer command buffer,
		commandPool->FreeCommandBuffer(transferCommandBuffer);
	}

	/*
	*	Creates an acceleration structure result buffer.
	*/
	static void AllocateAccelerationStructureMemory(const VkAccelerationStructureNV accelerationStructure, VkDeviceMemory *const RESTRICT vulkanDeviceMemory) NOEXCEPT
	{
		//Query the memory requirements.
		VkAccelerationStructureMemoryRequirementsInfoNV accelerationStructureMemoryRequirementsInfo;

		accelerationStructureMemoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		accelerationStructureMemoryRequirementsInfo.pNext = nullptr;
		accelerationStructureMemoryRequirementsInfo.accelerationStructure = accelerationStructure;
		accelerationStructureMemoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;

		VkMemoryRequirements2 memoryRequirements;
		vkGetAccelerationStructureMemoryRequirementsNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &accelerationStructureMemoryRequirementsInfo, &memoryRequirements);

		//Iterate over all memory types and find the proper memory type index.
		uint32 memoryTypeIndex{ FindMemoryTypeIndex(memoryRequirements.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) };

		//Create the memory allocate info.
		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		//Allocate the memory!
		VULKAN_ERROR_CHECK(vkAllocateMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), &memoryAllocateInfo, nullptr, vulkanDeviceMemory));
	}

	/*
	*	Creates an acceleration structure scratch buffer.
	*/
	static void CreateAccelerationStructureScratchBuffer(const VkAccelerationStructureNV accelerationStructure, VkBuffer *const RESTRICT vulkanBuffer, VkDeviceMemory *const RESTRICT vulkanDeviceMemory) NOEXCEPT
	{
		//Query the memory requirements.
		VkDeviceSize memorySizeRequirement{ 0 };

		VkAccelerationStructureMemoryRequirementsInfoNV accelerationStructureMemoryRequirementsInfo;

		accelerationStructureMemoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		accelerationStructureMemoryRequirementsInfo.pNext = nullptr;
		accelerationStructureMemoryRequirementsInfo.accelerationStructure = accelerationStructure;
		accelerationStructureMemoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV;

		VkMemoryRequirements2 memoryRequirements2;
		vkGetAccelerationStructureMemoryRequirementsNV(VulkanInterface::Instance->GetLogicalDevice().Get(), &accelerationStructureMemoryRequirementsInfo, &memoryRequirements2);

		memorySizeRequirement = memoryRequirements2.memoryRequirements.size;

		//Create the buffer create info.
		VkBufferCreateInfo bufferCreateInfo;

		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = memorySizeRequirement;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		//Create the Vulkan buffer!
		VULKAN_ERROR_CHECK(vkCreateBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), &bufferCreateInfo, nullptr, vulkanBuffer));

		//Get the memory requirements.
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(VulkanInterface::Instance->GetLogicalDevice().Get(), *vulkanBuffer, &memoryRequirements);

		//Iterate over all memory types and find the proper memory type index.
		uint32 memoryTypeIndex{ FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) };

		//Create the memory allocate info.
		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		//Allocate the memory!
		VULKAN_ERROR_CHECK(vkAllocateMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), &memoryAllocateInfo, nullptr, vulkanDeviceMemory));

		//Bind the buffer to the memory.
		VULKAN_ERROR_CHECK(vkBindBufferMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), *vulkanBuffer, *vulkanDeviceMemory, 0));
	}

	/*
	*	Creates an attachment description.
	*/
	static constexpr VkAttachmentDescription CreateAttachmentDescription(	const VkFormat format,
																			const VkAttachmentLoadOp loadOp,
																			const VkAttachmentStoreOp storeOp,
																			const VkAttachmentLoadOp stencilLoadOp,
																			const VkAttachmentStoreOp stencilStoreOp,
																			const VkImageLayout initialLayout,
																			const VkImageLayout finalLayout) NOEXCEPT
	{
		return VkAttachmentDescription
		{
			0, //flags
			format,
			VK_SAMPLE_COUNT_1_BIT, //samples
			loadOp,
			storeOp,
			stencilLoadOp,
			stencilStoreOp,
			initialLayout,
			finalLayout
		};
	}

	/*
	*	Creates an attachment reference.
	*/
	static constexpr VkAttachmentReference CreateAttachmentReference(const uint32 attachment, const VkImageLayout layout) NOEXCEPT
	{
		return VkAttachmentReference
		{
			attachment,
			layout
		};
	}

	/*
	*	Creates a descriptor set layout binding.
	*/
	static constexpr VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(const uint32 binding, const VkDescriptorType descriptorType, const uint32 descriptorCount, const VkShaderStageFlags stageFlags) NOEXCEPT
	{
		return VkDescriptorSetLayoutBinding{ binding, descriptorType, descriptorCount, stageFlags, nullptr };
	}

	/*
	*	Creates a subpass dependency.
	*/
	static constexpr VkSubpassDependency CreateSubpassDependency(	const uint32 sourceSubpass,
																	const uint32 destinationSubpass,
																	const VkPipelineStageFlags sourceStageMask,
																	const VkPipelineStageFlags destinationStageMask,
																	const VkAccessFlags sourceAccessMask,
																	const VkAccessFlags destinationAccessMask,
																	const VkDependencyFlags dependencyFlags) NOEXCEPT
	{
		return VkSubpassDependency
		{
			sourceSubpass,
			destinationSubpass,
			sourceStageMask,
			destinationStageMask,
			sourceAccessMask,
			destinationAccessMask,
			dependencyFlags
		};
	}

	/*
	*	Creates a subpass description.
	*/
	static constexpr VkSubpassDescription CreateSubpassDescription(	const uint32 inputAttachmentCount,
																	const VkAttachmentReference *RESTRICT const inputAttachments,
																	const uint32 colorAttachmentCount,
																	const VkAttachmentReference *RESTRICT const colorAttachments,
																	const VkAttachmentReference *RESTRICT const depthStencilAttachment,
																	const uint32 preserveAttachmentCount,
																	const uint32* const preserveAttachments) NOEXCEPT
	{
		return VkSubpassDescription
		{
			0, //flags
			VK_PIPELINE_BIND_POINT_GRAPHICS, //pipelineBindPoint
			inputAttachmentCount,
			inputAttachments,
			colorAttachmentCount,
			colorAttachments,
			nullptr, //pResolveAttachments
			depthStencilAttachment,
			preserveAttachmentCount,
			preserveAttachments
		};
	}

	/*
	*	Creates a Vulkan buffer.
	*/
	static void CreateVulkanBuffer(const VkDeviceSize bufferSize, const VkBufferUsageFlags bufferUsageFlags, const VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer &vulkanBuffer, VkDeviceMemory &vulkanDeviceMemory) NOEXCEPT
	{
		//Create the buffer create info.
		VkBufferCreateInfo bufferCreateInfo;

		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = bufferSize;
		bufferCreateInfo.usage = bufferUsageFlags;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		//Create the Vulkan buffer!
		VULKAN_ERROR_CHECK(vkCreateBuffer(VulkanInterface::Instance->GetLogicalDevice().Get(), &bufferCreateInfo, nullptr, &vulkanBuffer));

		//Get the memory requirements.
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanBuffer, &memoryRequirements);

		//Find the memory type index.
		const VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties{ VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceMemoryProperties() };

		//Iterate over all memory types and find the proper memory type index.
		uint32 memoryTypeIndex{ 0 };

		for (uint32 i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((memoryRequirements.memoryTypeBits & (1 << i)) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags))
			{
				memoryTypeIndex = i;

				break;
			}
		}

		//Create the memory allocate info.
		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		//Allocate the memory!
		VULKAN_ERROR_CHECK(vkAllocateMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), &memoryAllocateInfo, nullptr, &vulkanDeviceMemory));

		//Bind the buffer to the memory.
		VULKAN_ERROR_CHECK(vkBindBufferMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanBuffer, vulkanDeviceMemory, 0));
	}

	/*
	*	Creates a Vulkan image.
	*/
	static void CreateVulkanImage(const VkImageCreateFlags flags, const VkImageType image_type, const VkFormat format, const uint32 width, const uint32 height, const uint32 depth, const uint32 mipLevels, const uint32 arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags memoryPropertyFlags, VkImage &vulkanImage, VkDeviceMemory &vulkanDeviceMemory) NOEXCEPT
	{
		//Create the image create info.
		VkImageCreateInfo imageCreateInfo;
		
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = flags;
		imageCreateInfo.imageType = image_type;
		imageCreateInfo.format = format;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = depth;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = arrayLayers;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = tiling;
		imageCreateInfo.usage = usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		//Create the image!
		VULKAN_ERROR_CHECK(vkCreateImage(VulkanInterface::Instance->GetLogicalDevice().Get(), &imageCreateInfo, nullptr, &vulkanImage));

		//Get the memory requirements.
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanImage, &memoryRequirements);

		//Find the memory type index.
		const VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties = VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceMemoryProperties();

		//Iterate over all memory types and find the proper memory type index.
		uint32 memoryTypeIndex = 0;

		for (uint32 i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((memoryRequirements.memoryTypeBits & (1 << i)) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags))
			{
				memoryTypeIndex = i;

				break;
			}
		}

		//Create the memory allocate info.
		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		//Allocate the memory!
		VULKAN_ERROR_CHECK(vkAllocateMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), &memoryAllocateInfo, nullptr, &vulkanDeviceMemory));

		//Bind the image to the memory.
		VULKAN_ERROR_CHECK(vkBindImageMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanImage, vulkanDeviceMemory, 0));
	}

	/*
	*	Creates a Vulkan image view.
	*/
	static void CreateVulkanImageView(const VkImage &vulkanImage, const VkImageViewType viewType, const VkFormat format, const VkImageAspectFlags aspectMask, const uint32 levelCount, const uint32 layerCount, VkImageView &vulkanImageView) NOEXCEPT
	{
		//Create the image view create info.
		VkImageViewCreateInfo imageViewCreateInfo;

		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = vulkanImage;
		imageViewCreateInfo.viewType = viewType;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = levelCount;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = layerCount;

		//Create the image view!
		VULKAN_ERROR_CHECK(vkCreateImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), &imageViewCreateInfo, nullptr, &vulkanImageView));
	}

	/*
	*	Finds a memory type index.
	*/
	static uint32 FindMemoryTypeIndex(const uint32 memoryTypeBits, const VkMemoryPropertyFlags memoryProperties) NOEXCEPT
	{
		const VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties{ VulkanInterface::Instance->GetPhysicalDevice().GetPhysicalDeviceMemoryProperties() };

		for (uint32 i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((memoryTypeBits & (1 << i)) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties))
			{
				return i;
			}
		}

		return 0;
	}

	/*
	*	Transitions a Vulkan image to a layout.
	*/
	static void TransitionImageToLayout(const VkAccessFlags sourceAccessMask, const VkAccessFlags destinationAccessMask, const VkImageAspectFlags aspectMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32 mipLevels, const uint32 layerCount, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask, VkImage &vulkanImage) NOEXCEPT
	{
		//Create the command pool.
		static thread_local VulkanCommandPool *const RESTRICT commandPool{ VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };

		//Create the transition command buffer.
		VulkanCommandBuffer transitionCommandBuffer;
		commandPool->AllocatePrimaryCommandBuffer(transitionCommandBuffer);

		//Create the image memory barrier.
		VkImageMemoryBarrier imageMemoryBarrier;

		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;
		imageMemoryBarrier.srcAccessMask = sourceAccessMask;
		imageMemoryBarrier.dstAccessMask = destinationAccessMask;
		imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;

		imageMemoryBarrier.oldLayout = oldLayout;
		imageMemoryBarrier.newLayout = newLayout;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = vulkanImage;
		
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = layerCount;

		//Begin the transition command buffer.
		transitionCommandBuffer.BeginPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the pipeline barrier command.
		vkCmdPipelineBarrier(transitionCommandBuffer.Get(), sourceStageMask, destinationStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		//End the transition command buffer.
		transitionCommandBuffer.End();

		//Submit the command buffer.
		VulkanFence fence;
		fence.Initialize(0);
		VulkanInterface::Instance->GetGraphicsQueue()->Submit(transitionCommandBuffer, 0, nullptr, 0, 0, nullptr, fence.Get());

		//Wait for the transfer command to finish.
		fence.WaitFor();

		//Release the fence.
		fence.Release();

		//Free the transition command buffer.
		commandPool->FreeCommandBuffer(transitionCommandBuffer);
	}

};
#endif