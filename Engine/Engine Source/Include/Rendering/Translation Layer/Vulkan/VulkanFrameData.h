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
		//Create the command buffers.
		commandBuffers.UpsizeFast(frameDataCount);

		for (auto &commandBuffer : commandBuffers)
		{
			VulkanInterface::Instance->GetGraphicsCommandPool().AllocateVulkanCommandBuffer(commandBuffer);
		}

		//Create the fences.
		fences.UpsizeFast(frameDataCount);

		for (auto &fence : fences)
		{
			fence = VulkanInterface::Instance->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
		}

		//Create the dynamic uniform data buffers.
		dynamicUniformDataBuffers.UpsizeFast(frameDataCount);

		for (auto &dynamicUniformDataBuffer : dynamicUniformDataBuffers)
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

		for (auto &directionalShadowEvent : directionalShadowEvents)
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

	//The command buffers.
	DynamicArray<VulkanCommandBuffer> commandBuffers;

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