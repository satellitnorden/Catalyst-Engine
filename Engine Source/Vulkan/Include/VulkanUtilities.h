#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <Vertex.h>

//Vulkan.
#include <VulkanCore.h>
#include <VulkanCommandBuffer.h>
#include <VulkanCommandPool.h>
#include <VulkanInterface.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>

namespace VulkanUtilities
{

	/*
	*	Copies a Vulkan buffer to another Vulkan buffer.
	*/
	static void CopyBufferToBuffer(const VkDeviceSize &size, const VkBuffer &sourceBuffer, VkBuffer &destinationBuffer) CATALYST_NOEXCEPT
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
		VulkanInterface::Instance->GetTransferQueue().Submit(copyCommandBuffer);

		//Wait idle for the transfer queue to finish.
		VulkanInterface::Instance->GetTransferQueue().WaitIdle();

		//Free the copy command buffer.
		VulkanInterface::Instance->GetTransferCommandPool().FreeVulkanCommandBuffer(copyCommandBuffer);
	}

	/*
	*	Copies a Vulkan buffer to a Vulkan image.
	*/
	static void CopyBufferToImage(const VkBuffer &vulkanBuffer, VkImage &vulkanImage, const uint32 width, const uint32 height) CATALYST_NOEXCEPT
	{
		//Create the transfer command buffer.
		VulkanCommandBuffer transferCommandBuffer;
		VulkanInterface::Instance->GetTransferCommandPool().AllocateVulkanCommandBuffer(transferCommandBuffer);

		//Create the buffer image copy.
		VkBufferImageCopy bufferImageCopy;

		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = 0;
		bufferImageCopy.bufferImageHeight = 0;
		bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferImageCopy.imageSubresource.mipLevel = 0;
		bufferImageCopy.imageSubresource.baseArrayLayer = 0;
		bufferImageCopy.imageSubresource.layerCount = 1;
		bufferImageCopy.imageOffset = { 0, 0, 0 };
		bufferImageCopy.imageExtent = { width, height, 1 };

		//Begin the ctransfer ommand buffer.
		transferCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record the copy command to the transfer command buffer.
		vkCmdCopyBufferToImage(transferCommandBuffer.Get(), vulkanBuffer, vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

		//End the transfer command buffer.
		transferCommandBuffer.End();

		//Submit the command buffer.
		VulkanInterface::Instance->GetTransferQueue().Submit(transferCommandBuffer);

		//Wait for the transfer operation to finish.
		VulkanInterface::Instance->GetTransferQueue().WaitIdle();

		//Free the transfer command buffer,
		VulkanInterface::Instance->GetTransferCommandPool().FreeVulkanCommandBuffer(transferCommandBuffer);
	}

	/*
	*	Creates a Vulkan buffer.
	*/
	static void CreateVulkanBuffer(const VkDeviceSize bufferSize, const VkBufferUsageFlags bufferUsageFlags, const VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer &vulkanBuffer, VkDeviceMemory &vulkanDeviceMemory) CATALYST_NOEXCEPT
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
	static VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(const uint32 binding, const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags) CATALYST_NOEXCEPT
	{
		//Create the descriptor set layout binding.
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
		
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = descriptorType;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = stageFlags;
		descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

		return descriptorSetLayoutBinding;
	}

	/*
	*	Creates a Vulkan image.
	*/
	static void CreateVulkanImage(const VkFormat format, const uint32 width, const uint32 height, const VkImageUsageFlags usage, VkImage &vulkanImage, VkDeviceMemory &vulkanDeviceMemory) CATALYST_NOEXCEPT
	{
		//Create the image create info.
		VkImageCreateInfo imageCreateInfo;
		
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
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
	static void CreateVulkanImageView(const VkFormat format, const VkImageAspectFlags aspectMask, const VkImage &vulkanImage, VkImageView &vulkanImageView) CATALYST_NOEXCEPT
	{
		//Create the image view create info.
		VkImageViewCreateInfo imageViewCreateInfo;

		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = vulkanImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		//Create the image view!
		VULKAN_ERROR_CHECK(vkCreateImageView(VulkanInterface::Instance->GetLogicalDevice().Get(), &imageViewCreateInfo, nullptr, &vulkanImageView));
	}

	/*
	*	Creates a Vulkan sampler.
	*/
	static void CreateVulkanSampler(VkSampler &vulkanSampler) CATALYST_NOEXCEPT
	{
		//Create the image view create info.
		VkSamplerCreateInfo samplerCreateInfo;

		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = nullptr;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = 16;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		//Create the sampler!
		VULKAN_ERROR_CHECK(vkCreateSampler(VulkanInterface::Instance->GetLogicalDevice().Get(), &samplerCreateInfo, nullptr, &vulkanSampler));
	}

	/*
	*	Returns the vertex input attribute descriptions for Vertices.
	*/
	static StaticArray<VkVertexInputAttributeDescription, 4> GetVertexInputAttributeDescriptions() CATALYST_NOEXCEPT
	{
		StaticArray<VkVertexInputAttributeDescription, 4> vertexInputAttributeDescriptions;

		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = offsetof(Vertex, position);

		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = offsetof(Vertex, normal);

		vertexInputAttributeDescriptions[2].location = 2;
		vertexInputAttributeDescriptions[2].binding = 0;
		vertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[2].offset = offsetof(Vertex, tangent);

		vertexInputAttributeDescriptions[3].location = 3;
		vertexInputAttributeDescriptions[3].binding = 0;
		vertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[3].offset = offsetof(Vertex, textureCoordinate);

		return vertexInputAttributeDescriptions;
	}

	/*
	*	Returns the vertex input binding description for Vertices. 
	*/
	static VkVertexInputBindingDescription GetVertexInputBindingDescription() CATALYST_NOEXCEPT
	{
		VkVertexInputBindingDescription vertexInputBindingDescription;

		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(Vertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertexInputBindingDescription;
	}

	/*
	*	Transitions a Vulkan image to a layout.
	*/
	static void TransitionImageToLayout(const VkFormat format, const VkAccessFlags sourceAccessMask, const VkAccessFlags destinationAccessMask, const VkImageAspectFlags aspectMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask, VkImage &vulkanImage) CATALYST_NOEXCEPT
	{
		//Create the transfer command buffer.
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
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;

		//Begin the transfer command buffer.
		transitionCommandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//Record thepipeline barrier command.
		vkCmdPipelineBarrier(transitionCommandBuffer.Get(), sourceStageMask, destinationStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		//End the transfer command buffer.
		transitionCommandBuffer.End();

		//Submit the command buffer.
		VulkanInterface::Instance->GetGraphicsQueue().Submit(transitionCommandBuffer);

		//Wait for the transfer operation to finish.
		VulkanInterface::Instance->GetGraphicsQueue().WaitIdle();

		//Free the transition command buffer.
		VulkanInterface::Instance->GetGraphicsCommandPool().FreeVulkanCommandBuffer(transitionCommandBuffer);
	}

}