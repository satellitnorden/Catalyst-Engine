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

		//Create the instanced physical entities command pool.
		instancedPhysicalEntitiesCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the instanced physical entities command buffers.
		instancedPhysicalEntitiesCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &instancedPhysicalEntitiesCommandBuffer : instancedPhysicalEntitiesCommandBuffers)
		{
			instancedPhysicalEntitiesCommandPool->AllocateSecondaryCommandBuffer(instancedPhysicalEntitiesCommandBuffer);
		}

		//Create the vegetation entities command pool.
		vegetationEntitiesCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the vegetation entities command buffers.
		vegetationEntitiesCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &vegetationEntitiesCommandBuffer : vegetationEntitiesCommandBuffers)
		{
			vegetationEntitiesCommandPool->AllocateSecondaryCommandBuffer(vegetationEntitiesCommandBuffer);
		}

		//Create the lighting command pool.
		lightingCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the lighting command buffers.
		lightingCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &lightingCommandBuffer : lightingCommandBuffers)
		{
			lightingCommandPool->AllocateSecondaryCommandBuffer(lightingCommandBuffer);
		}

		//Create the skybox command pool.
		skyboxCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the skybox command buffers.
		skyboxCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &skyboxCommandBuffer : skyboxCommandBuffers)
		{
			skyboxCommandPool->AllocateSecondaryCommandBuffer(skyboxCommandBuffer);
		}

		//Create the particle system entities command pool.
		particleSystemEntitiesCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the particle system entities command buffers.
		particleSystemEntitiesCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &particleSystemEntitiesCommandBuffer : particleSystemEntitiesCommandBuffers)
		{
			particleSystemEntitiesCommandPool->AllocateSecondaryCommandBuffer(particleSystemEntitiesCommandBuffer);
		}

		//Create the ocean command pool.
		oceanCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the ocean command buffers.
		oceanCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &oceanCommandBuffer : oceanCommandBuffers)
		{
			oceanCommandPool->AllocateSecondaryCommandBuffer(oceanCommandBuffer);
		}

		//Create the post processing command pool.
		postProcessingCommandPool = VulkanInterface::Instance->CreateGraphicsCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		//Create the post processing command buffers.
		postProcessingCommandBuffers.UpsizeFast(frameDataCount);

		for (VulkanCommandBuffer &postProcessingCommandBuffer : postProcessingCommandBuffers)
		{
			postProcessingCommandPool->AllocateSecondaryCommandBuffer(postProcessingCommandBuffer);
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
	VulkanCommandBuffer *RESTRICT GetCurrentPrimaryCommandBuffer() NOEXCEPT { return &primaryCommandBuffers[currentFrame]; }

	/*
	*	Returns the current directional shadow command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentDirectionalShadowCommandBuffer() NOEXCEPT { return &directionalShadowCommandBuffers[currentFrame]; }

	/*
	*	Returns the current instanced physical entities command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentInstancedPhysicalEntitiesCommandBuffer() NOEXCEPT { return &instancedPhysicalEntitiesCommandBuffers[currentFrame]; }

	/*
	*	Returns the current vegetation entities command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentVegetationEntitiesCommandBuffer() NOEXCEPT { return &vegetationEntitiesCommandBuffers[currentFrame]; }

	/*
	*	Returns the current lighting command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentLightingCommandBuffer() NOEXCEPT { return &lightingCommandBuffers[currentFrame]; }

	/*
	*	Returns the current skybox command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentSkyboxCommandBuffer() NOEXCEPT { return &skyboxCommandBuffers[currentFrame]; }

	/*
	*	Returns the current particle system entities command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentParticleSystemEntitiesCommandBuffer() NOEXCEPT { return &particleSystemEntitiesCommandBuffers[currentFrame]; }

	/*
	*	Returns the current ocean command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentOceanCommandBuffer() NOEXCEPT { return &oceanCommandBuffers[currentFrame]; }

	/*
	*	Returns the current post processing command buffer.
	*/
	VulkanCommandBuffer *RESTRICT GetCurrentPostProcessingCommandBuffer() NOEXCEPT { return &postProcessingCommandBuffers[currentFrame]; }

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

	//The instanced physical entities command pool.
	VulkanCommandPool *RESTRICT instancedPhysicalEntitiesCommandPool;

	//The instanced physical entitiees command buffers.
	DynamicArray<VulkanCommandBuffer> instancedPhysicalEntitiesCommandBuffers;

	//The vegetation entities command pool.
	VulkanCommandPool *RESTRICT vegetationEntitiesCommandPool;

	//The vegetation entitiees command buffers.
	DynamicArray<VulkanCommandBuffer> vegetationEntitiesCommandBuffers;

	//The lighting command pool.
	VulkanCommandPool *RESTRICT lightingCommandPool;

	//The lighting command buffers.
	DynamicArray<VulkanCommandBuffer> lightingCommandBuffers;

	//The skybox command pool.
	VulkanCommandPool *RESTRICT skyboxCommandPool;

	//The skybox command buffers.
	DynamicArray<VulkanCommandBuffer> skyboxCommandBuffers;

	//The particle system entities command pool.
	VulkanCommandPool *RESTRICT particleSystemEntitiesCommandPool;

	//The particle system entities command buffers.
	DynamicArray<VulkanCommandBuffer> particleSystemEntitiesCommandBuffers;

	//The ocean command pool.
	VulkanCommandPool *RESTRICT oceanCommandPool;

	//The ocean command buffers.
	DynamicArray<VulkanCommandBuffer> oceanCommandBuffers;

	//The post processing command pool.
	VulkanCommandPool *RESTRICT postProcessingCommandPool;

	//The post processing command buffers.
	DynamicArray<VulkanCommandBuffer> postProcessingCommandBuffers;

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