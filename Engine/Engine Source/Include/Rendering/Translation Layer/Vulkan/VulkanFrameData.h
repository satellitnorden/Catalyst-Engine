#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

class VulkanFrameData final
{

public:

	/*
	*	Initializes the Vulkan frame data.
	*/
	void Initialize(const uint32 frameDataCount, const VulkanDescriptorSetLayout dynamicUniformDataDescriptorSetLayout, const VulkanDescriptorSetLayout environmentDescriptorSetLayout, const VulkanDescriptorSetLayout oceanDescriptorSetLayout) NOEXCEPT
	{
		//Create the primary command pool.
		primaryCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the primary command buffers.
		primaryCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &primaryCommandBuffer : primaryCommandBuffers)
		{
			primaryCommandPool->AllocatePrimaryCommandBuffer(primaryCommandBuffer);
		}

		//Create the directional shadow command pool.
		directionalShadowCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the directional shadow command buffers.
		directionalShadowCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &directionalShadowCommandBuffer : directionalShadowCommandBuffers)
		{
			directionalShadowCommandPool->AllocatePrimaryCommandBuffer(directionalShadowCommandBuffer);
		}

		//Create the fences.
		fences.UpsizeFast(frameDataCount);

		for (VulkanFence *RESTRICT &fence : fences)
		{
			fence = VulkanInterface::Instance->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
		}

		//Create the dynamic uniform data buffers.
		dynamicUniformDataBuffers.UpsizeFast(frameDataCount);

		for (VulkanUniformBuffer *RESTRICT &dynamicUniformDataBuffer : dynamicUniformDataBuffers)
		{
			dynamicUniformDataBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(VulkanDynamicUniformData));
		}

		//Create the dynamic uniform data descriptor sets.
		dynamicUniformDataDescriptorSets.UpsizeFast(frameDataCount);

		for (uint64 i = 0, size = dynamicUniformDataDescriptorSets.Size(); i < size; ++i)
		{
			VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(dynamicUniformDataDescriptorSets[i], dynamicUniformDataDescriptorSetLayout);

			//Update the write descriptor sets.
			StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
			{
				dynamicUniformDataBuffers[i]->GetWriteDescriptorSet(dynamicUniformDataDescriptorSets[i], 0)
			};

			vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
		}

		//Create the environment descriptor sets.
		environmentDescriptorSets.UpsizeFast(frameDataCount);

		for (uint64 i = 0, size = environmentDescriptorSets.Size(); i < size; ++i)
		{
			VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(environmentDescriptorSets[i], environmentDescriptorSetLayout);
		}

		//Create the ocean descriptor sets.
		oceanDescriptorSets.UpsizeFast(frameDataCount);

		for (uint64 i = 0, size = oceanDescriptorSets.Size(); i < size; ++i)
		{
			VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(oceanDescriptorSets[i], oceanDescriptorSetLayout);
		}

		//Create the directional shadow events.
		directionalShadowEvents.UpsizeFast(frameDataCount);

		for (VulkanEvent *RESTRICT &directionalShadowEvent : directionalShadowEvents)
		{
			directionalShadowEvent = VulkanInterface::Instance->CreateEvent();
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
	*	Returns the current primary command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentPrimaryCommandBuffer() NOEXCEPT
	{
		//Return the current primary command buffer.
		return &primaryCommandBuffers[currentFrame];
	}

	/*
	*	Returns the current directional shadow command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentDirectionalShadowCommandBuffer() NOEXCEPT { return &directionalShadowCommandBuffers[currentFrame]; }

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

	/*
	*	Returns the current environment descriptor set.
	*/
	VulkanDescriptorSet *RESTRICT GetCurrentEnvironmentDescriptorSet() NOEXCEPT
	{
		//Return the current environment descriptor set.
		return &environmentDescriptorSets[currentFrame];
	}

	/*
	*	Returns the current ocean descriptor set.
	*/
	VulkanDescriptorSet *RESTRICT GetCurrentOceanDescriptorSet() NOEXCEPT { return &oceanDescriptorSets[currentFrame]; }

	/*
	*	Returns the current directional shadow event.
	*/
	VulkanEvent *RESTRICT GetCurrentDirectionalShadowEvent() NOEXCEPT { return directionalShadowEvents[currentFrame]; }

private:

	//Keeps track of the current frame.
	uint32 currentFrame;

	//The primary command pool.
	VulkanCommandPool *RESTRICT primaryCommandPool;

	//The primary command buffers.
	DynamicArray<VulkanCommandBuffer> primaryCommandBuffers;

	//The directional shadow command pool.
	VulkanCommandPool *RESTRICT directionalShadowCommandPool;

	//The directional shadow command buffers.
	DynamicArray<VulkanCommandBuffer> directionalShadowCommandBuffers;

	//The fences.
	DynamicArray<VulkanFence *RESTRICT> fences;

	//The dynamic uniform data buffers.
	DynamicArray<VulkanUniformBuffer *RESTRICT> dynamicUniformDataBuffers;

	//The dynamic uniform data descriptor sets.
	DynamicArray<VulkanDescriptorSet> dynamicUniformDataDescriptorSets;

	//The environment descriptor sets.
	DynamicArray<VulkanDescriptorSet> environmentDescriptorSets;

	//The ocean descriptor sets.
	DynamicArray<VulkanDescriptorSet> oceanDescriptorSets;

	//The directional shadow events.
	DynamicArray<VulkanEvent *RESTRICT> directionalShadowEvents;

};