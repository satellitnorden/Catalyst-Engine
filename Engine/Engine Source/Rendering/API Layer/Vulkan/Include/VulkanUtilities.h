#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanCommandBuffer.h>
#include <VulkanCommandPool.h>
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>

class VulkanUtilities
{

public:

	/*
	*	Copies a Vulkan buffer to another Vulkan buffer.
	*/
	static void CopyBufferToBuffer(const VkDeviceSize &size, const VkBuffer &sourceBuffer, VkBuffer &destinationBuffer) NOEXCEPT
	{
		//Create a command buffer for the copy operation.
		VulkanCommandBuffer copyCommandBuffer;
		VulkanInterface::Instance->GetTransferCommandPool().AllocateVulkanCommandBuffer(copyCommandBuffer);

		//Begin the command buffer.
		copyCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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
		VulkanInterface::Instance->GetTransferQueue().Submit(copyCommandBuffer, 0, nullptr, 0, 0, nullptr, VK_NULL_HANDLE);

		//Wait idle for the transfer queue to finish.
		VulkanInterface::Instance->GetTransferQueue().WaitIdle();

		//Free the copy command buffer.
		VulkanInterface::Instance->GetTransferCommandPool().FreeVulkanCommandBuffer(copyCommandBuffer);
	}

	/*
	*	Copies a Vulkan buffer to a Vulkan image.
	*/
	static void CopyBufferToImage(const VkBuffer &vulkanBuffer, VkImage &vulkanImage, const uint32 mipLevels, const uint32 layerCount, const uint32 width, const uint32 height) NOEXCEPT
	{
		//Create the transfer command buffer.
		VulkanCommandBuffer transferCommandBuffer;
		VulkanInterface::Instance->GetTransferCommandPool().AllocateVulkanCommandBuffer(transferCommandBuffer);

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
			bufferImageCopy.imageExtent = { width >> i, height >> i, 1 };

			bufferImageCopies.EmplaceFast(bufferImageCopy);

			currentOffset += (width >> i) * (height >> i) * 4 * SizeOf(byte);
		}

		//Begin the transfer ommand buffer.
		transferCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the copy command to the transfer command buffer.
		vkCmdCopyBufferToImage(transferCommandBuffer.Get(), vulkanBuffer, vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, StaticCast<uint32>(bufferImageCopies.Size()), bufferImageCopies.Data());

		//End the transfer command buffer.
		transferCommandBuffer.End();

		//Submit the command buffer.
		VulkanInterface::Instance->GetTransferQueue().Submit(transferCommandBuffer, 0, nullptr, 0, 0, nullptr, VK_NULL_HANDLE);

		//Wait for the transfer operation to finish.
		VulkanInterface::Instance->GetTransferQueue().WaitIdle();

		//Free the transfer command buffer,
		VulkanInterface::Instance->GetTransferCommandPool().FreeVulkanCommandBuffer(transferCommandBuffer);
	}

	/*
	*	Copies a Vulkan image to a Vulkan buffer.

	static void CopyImageToBuffer() NOEXCEPT
	{
		//Create the transfer command buffer.
		VulkanCommandBuffer transferCommandBuffer;
		VulkanInterface::Instance->GetTransferCommandPool().AllocateVulkanCommandBuffer(transferCommandBuffer);

		//Begin the transfer ommand buffer.
		transferCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the copy command to the transfer command buffer.
		vkCmdCopyImageToBuffer(transferCommandBuffer.Get(), vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vulkanBuffer, 1, &bufferImageCopy);

		//End the transfer command buffer.
		transferCommandBuffer.End();

		//Submit the command buffer.
		VulkanInterface::Instance->GetTransferQueue().Submit(transferCommandBuffer, 0, nullptr, 0, 0, nullptr, VK_NULL_HANDLE);

		//Wait for the transfer operation to finish.
		VulkanInterface::Instance->GetTransferQueue().WaitIdle();

		//Free the transfer command buffer,
		VulkanInterface::Instance->GetTransferCommandPool().FreeVulkanCommandBuffer(transferCommandBuffer);
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

		//Bind the buffer to the memory.
		VULKAN_ERROR_CHECK(vkBindBufferMemory(VulkanInterface::Instance->GetLogicalDevice().Get(), vulkanBuffer, vulkanDeviceMemory, 0));
	}

	/*
	*	Creates a Vulkan descriptor set layout binding.
	*/
	static constexpr VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(const uint32 binding, const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags) NOEXCEPT
	{
		return VkDescriptorSetLayoutBinding{ binding, descriptorType, 1, stageFlags, nullptr };
	}

	/*
	*	Creates a Vulkan image.
	*/
	static void CreateVulkanImage(const VkImageCreateFlags flags, const VkFormat format, const uint32 width, const uint32 height, const uint32 mipLevels, const uint32 arrayLayers, const VkImageUsageFlags usage, VkImage &vulkanImage, VkDeviceMemory &vulkanDeviceMemory) NOEXCEPT
	{
		//Create the image create info.
		VkImageCreateInfo imageCreateInfo;
		
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = flags;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = arrayLayers;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
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
			if ((memoryRequirements.memoryTypeBits & (1 << i)) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
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
	*	Creates a Vulkan sampler.
	*/
	static void CreateVulkanSampler(VkSampler &vulkanSampler, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode, const float maxLod) NOEXCEPT
	{
		//Create the image view create info.
		VkSamplerCreateInfo samplerCreateInfo;

		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = nullptr;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = magnificationFilter;
		samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.mipmapMode = mipmapMode;
		samplerCreateInfo.addressModeU = addressMode;
		samplerCreateInfo.addressModeV = addressMode;
		samplerCreateInfo.addressModeW = addressMode;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = 16;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = maxLod;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		//Create the sampler!
		VULKAN_ERROR_CHECK(vkCreateSampler(VulkanInterface::Instance->GetLogicalDevice().Get(), &samplerCreateInfo, nullptr, &vulkanSampler));
	}

	/*
	*	Transitions a Vulkan image to a layout.
	*/
	static void TransitionImageToLayout(const VkFormat format, const VkAccessFlags sourceAccessMask, const VkAccessFlags destinationAccessMask, const VkImageAspectFlags aspectMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32 mipLevels, const uint32 layerCount, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask, VkImage &vulkanImage) NOEXCEPT
	{
		//Create the transition command buffer.
		VulkanCommandBuffer transitionCommandBuffer;
		VulkanInterface::Instance->GetGraphicsCommandPool().AllocateVulkanCommandBuffer(transitionCommandBuffer);

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
		transitionCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the pipeline barrier command.
		vkCmdPipelineBarrier(transitionCommandBuffer.Get(), sourceStageMask, destinationStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		//End the transition command buffer.
		transitionCommandBuffer.End();

		//Submit the command buffer.
		VulkanInterface::Instance->GetGraphicsQueue().Submit(transitionCommandBuffer, 0, nullptr, 0, 0, nullptr, VK_NULL_HANDLE);

		//Wait for the transition operation to finish.
		VulkanInterface::Instance->GetGraphicsQueue().WaitIdle();

		//Free the transition command buffer.
		VulkanInterface::Instance->GetGraphicsCommandPool().FreeVulkanCommandBuffer(transitionCommandBuffer);
	}

};