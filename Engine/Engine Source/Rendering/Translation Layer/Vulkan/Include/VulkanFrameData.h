#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanInterface.h>

class VulkanFrameData final
{

public:

	/*
	*	Initializes the Vulkan frame data.
	*/
	void Initialize(const uint32 frameDataCount, const VulkanDescriptorSetLayout dynamicUniformDataDescriptorSetLayout) NOEXCEPT
	{
		//Create the command buffers.
		commandBuffers.Resize(frameDataCount);

		for (auto &commandBuffer : commandBuffers)
		{
			VulkanInterface::Instance->GetGraphicsCommandPool().AllocateVulkanCommandBuffer(commandBuffer);
		}

		//Create the fences.
		fences.Resize(frameDataCount);

		for (auto &fence : fences)
		{
			fence = VulkanInterface::Instance->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
		}

		//Create the dynamic uniform data buffers.
		dynamicUniformDataBuffers.Resize(frameDataCount);

		for (auto &dynamicUniformDataBuffer : dynamicUniformDataBuffers)
		{
			dynamicUniformDataBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(VulkanDynamicUniformData));
		}

		//Create the dynamic uniform data descriptor sets.
		dynamicUniformDataDescriptorSets.Resize(frameDataCount);

		for (uint32 i = 0, size = dynamicUniformDataDescriptorSets.Size(); i < size; ++i)
		{
			VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(dynamicUniformDataDescriptorSets[i], dynamicUniformDataDescriptorSetLayout);

			//Update the write descriptor sets.
			StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
			{
				dynamicUniformDataBuffers[i]->GetWriteDescriptorSet(dynamicUniformDataDescriptorSets[i], 0)
			};

			vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), writeDescriptorSets.Size(), writeDescriptorSets.Data(), 0, nullptr);
		}
	}

	/*
	*	Returns the current frame.
	*/
	uint32 GetCurrentFrame() const NOEXCEPT
	{
		return currentFrame;
	}

	/*
	*	Sets the current frame.
	*/
	void SetCurrentFrame(const uint32 newCurrentFrame) NOEXCEPT
	{
		currentFrame = newCurrentFrame;
	}

	/*
	*	Returns the current command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentCommandBuffer() NOEXCEPT
	{
		//Return the current command buffer.
		return &commandBuffers[currentFrame];
	}

	/*
	*	Returns the current fence.
	*/
	VulkanFence *RESTRICT GetCurrentFence() NOEXCEPT
	{
		//Return the current fence.
		return fences[currentFrame];
	}

	/*
	*	Returns the current dynamic uniform data buffer.
	*/
	VulkanUniformBuffer *RESTRICT GetCurrentDynamicUniformDataBuffer() NOEXCEPT
	{
		//Return the current dynamic uniform data buffer.
		return dynamicUniformDataBuffers[currentFrame];
	}

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	VulkanDescriptorSet *RESTRICT GetCurrentDynamicUniformDataDescriptorSet() NOEXCEPT
	{
		//Return the current dynamic uniform data buffer.
		return &dynamicUniformDataDescriptorSets[currentFrame];
	}

private:

	//Keeps track of the current frame.
	uint32 currentFrame;

	//The command buffers.
	DynamicArray<VulkanCommandBuffer> commandBuffers;

	//The fences.
	DynamicArray<VulkanFence *RESTRICT> fences;

	//The dynamic uniform data buffers.
	DynamicArray<VulkanUniformBuffer *RESTRICT> dynamicUniformDataBuffers;

	//The dynamic uniform data descriptor sets.
	DynamicArray<VulkanDescriptorSet> dynamicUniformDataDescriptorSets;

};