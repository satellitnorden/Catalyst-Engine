#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/VulkanInterface.h>

class VulkanFrameData final
{

public:

	/*
	*	Initializes the Vulkan frame data.
	*/
	void Initialize(const uint32 frameDataCount, const VulkanDescriptorSetLayout dynamicUniformDataDescriptorSetLayout
#if defined(CATALYST_ENABLE_OCEAN)
		, const VulkanDescriptorSetLayout oceanDescriptorSetLayout
#endif
	) NOEXCEPT
	{
		//Create the primary command pool.
		_PrimaryCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the primary command buffers.
		_PrimaryCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &primaryCommandBuffer : _PrimaryCommandBuffers)
		{
			_PrimaryCommandPool->AllocatePrimaryCommandBuffer(primaryCommandBuffer);
		}

		//Create the fences.
		_Fences.UpsizeFast(frameDataCount);

		for (VulkanFence *RESTRICT &fence : _Fences)
		{
			fence = VulkanInterface::Instance->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
		}

		//Create the dynamic uniform data buffers.
		_DynamicUniformDataBuffers.UpsizeFast(frameDataCount);

		for (VulkanUniformBuffer *RESTRICT &dynamicUniformDataBuffer : _DynamicUniformDataBuffers)
		{
			dynamicUniformDataBuffer = VulkanInterface::Instance->CreateUniformBuffer(sizeof(VulkanDynamicUniformData));
		}

		//Create the dynamic uniform data descriptor sets.
		_DynamicUniformDataDescriptorSets.UpsizeFast(frameDataCount);

		for (uint64 i = 0, size = _DynamicUniformDataDescriptorSets.Size(); i < size; ++i)
		{
			VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(_DynamicUniformDataDescriptorSets[i], dynamicUniformDataDescriptorSetLayout);

			//Update the write descriptor sets.
			StaticArray<VkWriteDescriptorSet, 1> writeDescriptorSets
			{
				_DynamicUniformDataBuffers[i]->GetWriteDescriptorSet(_DynamicUniformDataDescriptorSets[i], 0)
			};

			vkUpdateDescriptorSets(VulkanInterface::Instance->GetLogicalDevice().Get(), static_cast<uint32>(writeDescriptorSets.Size()), writeDescriptorSets.Data(), 0, nullptr);
		}

#if defined(CATALYST_ENABLE_OCEAN)
		//Create the ocean descriptor sets.
		_OceanDescriptorSets.UpsizeFast(frameDataCount);

		for (uint64 i = 0, size = _OceanDescriptorSets.Size(); i < size; ++i)
		{
			VulkanInterface::Instance->GetDescriptorPool().AllocateDescriptorSet(_OceanDescriptorSets[i], oceanDescriptorSetLayout);
		}
#endif
	}

	/*
	*	Returns the current frame.
	*/
	uint32 GetCurrentFrame() const NOEXCEPT
	{
		return _CurrentFrame;
	}

	/*
	*	Sets the current frame.
	*/
	void SetCurrentFrame(const uint32 newCurrentFrame) NOEXCEPT
	{
		_CurrentFrame = newCurrentFrame;
	}

	/*
	*	Returns the current primary command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentPrimaryCommandBuffer() NOEXCEPT { return &_PrimaryCommandBuffers[_CurrentFrame]; }

	/*
	*	Returns the current fence.
	*/
	VulkanFence *RESTRICT GetCurrentFence() NOEXCEPT
	{
		//Return the current fence.
		return _Fences[_CurrentFrame];
	}

	/*
	*	Returns the current dynamic uniform data buffer.
	*/
	VulkanUniformBuffer *RESTRICT GetCurrentDynamicUniformDataBuffer() NOEXCEPT
	{
		//Return the current dynamic uniform data buffer.
		return _DynamicUniformDataBuffers[_CurrentFrame];
	}

	/*
	*	Returns the current dynamic uniform data descriptor set.
	*/
	VulkanDescriptorSet *RESTRICT GetCurrentDynamicUniformDataRenderDataTable() NOEXCEPT
	{
		//Return the current dynamic uniform data buffer.
		return &_DynamicUniformDataDescriptorSets[_CurrentFrame];
	}

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Returns the current ocean descriptor set.
	*/
	VulkanDescriptorSet *RESTRICT GetCurrentOceanRenderDataTable() NOEXCEPT { return &_OceanDescriptorSets[_CurrentFrame]; }
#endif

private:

	//Keeps track of the current frame.
	uint32 _CurrentFrame;

	//The primary command pool.
	VulkanCommandPool *RESTRICT _PrimaryCommandPool;

	//The primary command buffers.
	DynamicArray<VulkanCommandBuffer> _PrimaryCommandBuffers;

	//The fences.
	DynamicArray<VulkanFence *RESTRICT> _Fences;

	//The dynamic uniform data buffers.
	DynamicArray<VulkanUniformBuffer *RESTRICT> _DynamicUniformDataBuffers;

	//The dynamic uniform data descriptor sets.
	DynamicArray<VulkanDescriptorSet> _DynamicUniformDataDescriptorSets;

#if defined(CATALYST_ENABLE_OCEAN)
	//The ocean descriptor sets.
	DynamicArray<VulkanDescriptorSet> _OceanDescriptorSets;
#endif

};