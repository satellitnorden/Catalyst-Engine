#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Abstraction/Vulkan/VulkanInterface.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>
#include <Multithreading/Spinlock.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanErrorReporting.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanInterface);

/*
*	Initializes this Vulkan interface.
*/
void VulkanInterface::Initialize() NOEXCEPT
{
	//Initialize the Vulkan instance.
	_VulkanInstance.Initialize();

#if VULKAN_DEBUGGING
	//Initialize the Vulkan error reporting.
	VulkanErrorReporting::Initialize();
#endif

	//Initialize the Vulkan surface.
	_VulkanSurface.Initialize();

	//Initialize the Vulkan physical device.
	_VulkanPhysicalDevice.Initialize();

	//Initialize the Vulkan logical device.
	_VulkanLogicalDevice.Initialize();

	//Initialize the Vulkan swap chain.
	_VulkanSwapchain.Initialize();

	//Initialize the Vulkan descriptor pool.
	_VulkanDescriptorPool.Initialize();
}

/*
*	Pre-updates this Vulkan interface.
*/
void VulkanInterface::PreUpdate(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT
{
	//Update the next image index in the Vulkan swap chain.
	_VulkanSwapchain.UpdateNextImageIndex(imageAvailableSemaphore);
}

/*
*	Post-pdates this Vulkan interface.
*/
void VulkanInterface::PostUpdate(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT
{
	//Present the final image!
	_VulkanSwapchain.Present(renderFinishedSemaphore);
}

/*
*	Releases this Vulkan interface.
*/
void VulkanInterface::Release() NOEXCEPT
{
	//Wait for all queues to finish.
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Compute)->WaitIdle();
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Graphics)->WaitIdle();
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Present)->WaitIdle();
	_VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Transfer)->WaitIdle();

	//Release all Vulkan 2D textures.
	for (Vulkan2DTexture *const RESTRICT vulkan2DTexture : _Vulkan2DTextures)
	{
		vulkan2DTexture->Release();
		Memory::GlobalPoolDeAllocate<sizeof(Vulkan2DTexture)>(vulkan2DTexture);
	}

	//Release all Vulkan 3D textures.
	for (Vulkan3DTexture* const RESTRICT vulkan3DTexture : _Vulkan3DTextures)
	{
		vulkan3DTexture->Release();
		Memory::GlobalPoolDeAllocate<sizeof(Vulkan3DTexture)>(vulkan3DTexture);
	}

	//Release all Vulkan acceleration structures.
	for (VulkanAccelerationStructure *const RESTRICT vulkanAccelerationStructure : _VulkanAccelerationStructures)
	{
		vulkanAccelerationStructure->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanAccelerationStructure)>(vulkanAccelerationStructure);
	}

	//Release all Vulkan bufferrs.
	for (VulkanBuffer *const RESTRICT vulkanBuffer : _VulkanBuffers)
	{
		vulkanBuffer->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanBuffer)>(vulkanBuffer);
	}

	//Release all Vulkan command pools.
	for (VulkanCommandPool *const RESTRICT vulkanCommandPool : _VulkanCommandPools)
	{
		vulkanCommandPool->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanCommandPool)>(vulkanCommandPool);
	}

	//Release all Vulkan cube map textures.
	for (VulkanCubeMapTexture *const RESTRICT vulkanCubeMapTexture : _VulkanCubeMapTextures)
	{
		vulkanCubeMapTexture->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanCubeMapTexture)>(vulkanCubeMapTexture);
	}

	//Release all Vulkan depth buffers.
	for (VulkanDepthBuffer *const RESTRICT vulkanDepthBuffer : _VulkanDepthBuffers)
	{
		vulkanDepthBuffer->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanDepthBuffer)>(vulkanDepthBuffer);
	}

	//Release all Vulkan descriptor set layouts.
	for (VulkanDescriptorSetLayout *const RESTRICT vulkanDescriptorSetLayout : _VulkanDescriptorSetLayouts)
	{
		vulkanDescriptorSetLayout->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanDescriptorSetLayout)>(vulkanDescriptorSetLayout);
	}

	//Release all Vulkan descriptor sets.
	for (VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet : _VulkanDescriptorSets)
	{
		Memory::GlobalPoolDeAllocate<sizeof(VulkanDescriptorSet)>(vulkanDescriptorSet);
	}

	//Release all Vulkan events.
	for (VulkanEvent *const RESTRICT vulkanEvent : _VulkanEvents)
	{
		vulkanEvent->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanEvent)>(vulkanEvent);
	}

	//Release all Vulkan fences.
	for (VulkanFence *const RESTRICT vulkanFence : _VulkanFences)
	{
		vulkanFence->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanFence)>(vulkanFence);
	}

	//Release all Vulkan framebuffers.
	for (VulkanFramebuffer *const RESTRICT vulkanFramebuffer : _VulkanFramebuffers)
	{
		vulkanFramebuffer->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanFramebuffer)>(vulkanFramebuffer);
	}

	//Release all Vulkan render targets.
	for (VulkanRenderTarget *const RESTRICT vulkanRenderTarget : _VulkanRenderTargets)
	{
		vulkanRenderTarget->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanRenderTarget)>(vulkanRenderTarget);
	}

	//Release all Vulkan compute pipelines.
	for (VulkanComputePipeline *const RESTRICT vulkanComputePipeline : _VulkanComputePipelines)
	{
		vulkanComputePipeline->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanComputePipeline)>(vulkanComputePipeline);
	}

	//Release all Vulkan graphics pipelines.
	for (VulkanGraphicsPipeline *const RESTRICT vulkanGraphicsPipeline : _VulkanGraphicsPipelines)
	{
		vulkanGraphicsPipeline->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanGraphicsPipeline)>(vulkanGraphicsPipeline);
	}

	//Release all Vulkan ray tracing pipelines.
	for (VulkanRayTracingPipeline *const RESTRICT vulkanRayTracingPipeline : _VulkanRayTracingPipelines)
	{
		vulkanRayTracingPipeline->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanRayTracingPipeline)>(vulkanRayTracingPipeline);
	}

	//Release all Vulkan query pools.
	for (VulkanQueryPool *const RESTRICT vulkan_query_pool : _VulkanQueryPools)
	{
		vulkan_query_pool->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanQueryPool)>(vulkan_query_pool);
	}

	//Release all Vulkan render passes.
	for (VulkanRenderPass *const RESTRICT vulkanRenderPass : _VulkanRenderPasses)
	{
		vulkanRenderPass->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanRenderPass)>(vulkanRenderPass);
	}

	//Release all Vulkan semaphores.
	for (VulkanSemaphore *const RESTRICT vulkanSemaphore : _VulkanSemaphores)
	{
		vulkanSemaphore->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanSemaphore)>(vulkanSemaphore);
	}

	//Release all Vulkan samplers.
	for (VulkanSampler *const RESTRICT vulkanSampler : _VulkanSamplers)
	{
		vulkanSampler->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanSampler)>(vulkanSampler);
	}

	//Release all Vulkan shader modules.
	for (VulkanShaderModule *const RESTRICT vulkanShaderModule : _VulkanShaderModules)
	{
		vulkanShaderModule->Release();
		Memory::GlobalPoolDeAllocate<sizeof(VulkanShaderModule)>(vulkanShaderModule);
	}

	//Release the Vulkan descriptor pool.
	_VulkanDescriptorPool.Release();

	//Release the Vulkan swap chain.
	_VulkanSwapchain.Release();

	//Release the Vulkan logical device.
	_VulkanLogicalDevice.Release();

	//Release the Vulkan surface.
	_VulkanSurface.Release();

#if VULKAN_DEBUGGING
	//Release the Vulkan error reporting.
	VulkanErrorReporting::Release();
#endif

	//Release the Vulkan instance.
	_VulkanInstance.Release();
}

/*
*	Creates and returns a 2D texture.
*/
RESTRICTED Vulkan2DTexture *const RESTRICT VulkanInterface::Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format) NOEXCEPT
{
	Vulkan2DTexture *const RESTRICT new2DTexture{ new (Memory::GlobalPoolAllocate<sizeof(Vulkan2DTexture)>()) Vulkan2DTexture() };
	new2DTexture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureChannels, textureTexelSize, textureData, format);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_Vulkan2DTextures.Emplace(new2DTexture);

	return new2DTexture;
}

/*
*	Destroys a 2D texture.
*/
void VulkanInterface::Destroy2DTexture(Vulkan2DTexture *const RESTRICT texture) NOEXCEPT
{
	texture->Release();
	_Vulkan2DTextures.Erase(texture);
	Memory::GlobalPoolDeAllocate<sizeof(Vulkan2DTexture)>(texture);
}

/*
*	Creates and returns a 3D texture.
*/
RESTRICTED Vulkan3DTexture* const RESTRICT VulkanInterface::Create3DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureDepth, const uint32 textureChannels, const uint32 textureTexelSize, const void* RESTRICT const* RESTRICT textureData, const VkFormat format) NOEXCEPT
{
	Vulkan3DTexture* const RESTRICT new3DTexture{ new (Memory::GlobalPoolAllocate<sizeof(Vulkan3DTexture)>()) Vulkan3DTexture() };
	new3DTexture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureDepth, textureChannels, textureTexelSize, textureData, format);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_Vulkan3DTextures.Emplace(new3DTexture);

	return new3DTexture;
}

/*
*	Creates an acceleration structure.
*/
RESTRICTED VulkanAccelerationStructure *const RESTRICT VulkanInterface::CreateAccelerationStructure(const VkAccelerationStructureTypeNV type, const ArrayProxy<VulkanGeometryInstance> &instances, const ArrayProxy<VkGeometryNV> &geometry) NOEXCEPT
{
	VulkanAccelerationStructure *const RESTRICT newAccelerationStructure{ new (Memory::GlobalPoolAllocate<sizeof(VulkanAccelerationStructure)>()) VulkanAccelerationStructure() };
	newAccelerationStructure->Initialize(type, instances, geometry);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanAccelerationStructures.Emplace(newAccelerationStructure);

	return newAccelerationStructure;
}

/*
*	Destroys an acceleration structure
*/
void VulkanInterface::DestroyAccelerationStructure(VulkanAccelerationStructure *const RESTRICT accelerationStructure) NOEXCEPT
{
	accelerationStructure->Release();
	_VulkanAccelerationStructures.Erase(accelerationStructure);
	Memory::GlobalPoolDeAllocate<sizeof(VulkanAccelerationStructure)>(accelerationStructure);
}

/*
*	Creates and returns a  buffer.
*/
RESTRICTED VulkanBuffer *const RESTRICT VulkanInterface::CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperties) NOEXCEPT
{
	VulkanBuffer *const RESTRICT newBuffer{ new (Memory::GlobalPoolAllocate<sizeof(VulkanBuffer)>()) VulkanBuffer() };
	newBuffer->Initialize(size, usage, memoryProperties);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanBuffers.Emplace(newBuffer);

	return newBuffer;
}

/*
*	Destroys a buffer.
*/
void VulkanInterface::DestroyBuffer(VulkanBuffer *const RESTRICT buffer) NOEXCEPT
{
	buffer->Release();
	_VulkanBuffers.Erase(buffer);
	Memory::GlobalPoolDeAllocate<sizeof(VulkanBuffer)>(buffer);
}

/*
*	Creates and returns a compute command pool.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateComputeCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT newCommandPool{ new (Memory::GlobalPoolAllocate<sizeof(VulkanCommandPool)>()) VulkanCommandPool() };
	newCommandPool->Initialize(flags, _VulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Compute));

	SCOPED_LOCK(_VulkanCommandPoolsLock);
	_VulkanCommandPools.Emplace(newCommandPool);

	return newCommandPool;
}

/*
*	Creates and returns a graphics command pool.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateGraphicsCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT newCommandPool{ new (Memory::GlobalPoolAllocate<sizeof(VulkanCommandPool)>()) VulkanCommandPool() };
	newCommandPool->Initialize(flags, _VulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Graphics));

	SCOPED_LOCK(_VulkanCommandPoolsLock);
	_VulkanCommandPools.Emplace(newCommandPool);

	return newCommandPool;
}

/*
*	Creates and returns a transfer command pool.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateTransferCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT newCommandPool{ new (Memory::GlobalPoolAllocate<sizeof(VulkanCommandPool)>()) VulkanCommandPool() };
	newCommandPool->Initialize(flags, _VulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Transfer));

	SCOPED_LOCK(_VulkanCommandPoolsLock);

	_VulkanCommandPools.Emplace(newCommandPool);

	return newCommandPool;
}

/*
*	Creates and returns a cube map texture.
*/
RESTRICTED VulkanCubeMapTexture *const RESTRICT VulkanInterface::CreateCubeMapTexture(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT
{
	VulkanCubeMapTexture *const RESTRICT newCubeMapTexture{ new (Memory::GlobalPoolAllocate<sizeof(VulkanCubeMapTexture)>()) VulkanCubeMapTexture() };
	newCubeMapTexture->Initialize(data, width, height);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanCubeMapTextures.Emplace(newCubeMapTexture);

	return newCubeMapTexture;
}

/*
*	Creates and returns a depth buffer.
*/
RESTRICTED VulkanDepthBuffer *const RESTRICT VulkanInterface::CreateDepthBuffer(const VkExtent2D &depthBufferExtent) NOEXCEPT
{
	VulkanDepthBuffer *const RESTRICT newDepthBuffer{ new (Memory::GlobalPoolAllocate<sizeof(VulkanDepthBuffer)>()) VulkanDepthBuffer() };
	newDepthBuffer->Initialize(depthBufferExtent);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanDepthBuffers.Emplace(newDepthBuffer);

	return newDepthBuffer;
}

/*
*	Creates and returns a descriptor set layout.
*/
RESTRICTED VulkanDescriptorSetLayout *const RESTRICT VulkanInterface::CreateDescriptorSetLayout(const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings, const uint32 numberOfDescriptorSetLayoutBindings) NOEXCEPT
{
	VulkanDescriptorSetLayout *const RESTRICT newDescriptorSetLayout{ new (Memory::GlobalPoolAllocate<sizeof(VulkanDescriptorSetLayout)>()) VulkanDescriptorSetLayout() };
	newDescriptorSetLayout->Initialize(numberOfDescriptorSetLayoutBindings, descriptorSetLayoutBindings);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanDescriptorSetLayouts.Emplace(newDescriptorSetLayout);

	return newDescriptorSetLayout;
}

/*
*	Creates and returns a descriptor set.
*/
RESTRICTED VulkanDescriptorSet *const RESTRICT VulkanInterface::CreateDescriptorSet(const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT
{
	VulkanDescriptorSet *const RESTRICT newDescriptorSet{ new (Memory::GlobalPoolAllocate<sizeof(VulkanDescriptorSet)>()) VulkanDescriptorSet() };
	_VulkanDescriptorPool.AllocateDescriptorSet(*newDescriptorSet, vulkanDescriptorSetLayout);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanDescriptorSets.Emplace(newDescriptorSet);

	return newDescriptorSet;
}

/*
*	Destroys a descriptor set.
*/
void VulkanInterface::DestroyDescriptorSet(VulkanDescriptorSet *const RESTRICT descriptorSet) NOEXCEPT
{
	_VulkanDescriptorPool.FreeDescriptorSet(descriptorSet->Get());
	_VulkanDescriptorSets.Erase(descriptorSet);
	Memory::GlobalPoolDeAllocate<sizeof(VulkanDescriptorSet)>(descriptorSet);
}

/*
*	Creates and returns an event.
*/
RESTRICTED VulkanEvent *const RESTRICT VulkanInterface::CreateEvent() NOEXCEPT
{
	VulkanEvent *const RESTRICT newEvent{ new (Memory::GlobalPoolAllocate<sizeof(VulkanEvent)>()) VulkanEvent() };
	newEvent->Initialize();

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanEvents.Emplace(newEvent);

	return newEvent;
}

/*
*	Creates and returns a fence.
*/
RESTRICTED VulkanFence *const RESTRICT VulkanInterface::CreateFence(const VkFenceCreateFlags flags) NOEXCEPT
{
	VulkanFence *const RESTRICT newFence{ new (Memory::GlobalPoolAllocate<sizeof(VulkanFence)>()) VulkanFence() };
	newFence->Initialize(flags);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanFences.Emplace(newFence);

	return newFence;
}

/*
*	Creates and returns a framebuffer.
*/
RESTRICTED VulkanFramebuffer *const RESTRICT VulkanInterface::CreateFramebuffer(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT
{
	VulkanFramebuffer *const RESTRICT newFramebuffer{ new (Memory::GlobalPoolAllocate<sizeof(VulkanFramebuffer)>()) VulkanFramebuffer() };
	newFramebuffer->Initialize(parameters);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanFramebuffers.Emplace(newFramebuffer);

	return newFramebuffer;
}

/*
*	Creates and returns a compute pipeline.
*/
RESTRICTED VulkanComputePipeline *const RESTRICT VulkanInterface::CreateComputePipeline(const VulkanComputePipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanComputePipeline *const RESTRICT newPipeline{ new (Memory::GlobalPoolAllocate<sizeof(VulkanComputePipeline)>()) VulkanComputePipeline() };
	newPipeline->Initialize(parameters);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanComputePipelines.Emplace(newPipeline);

	return newPipeline;
}

/*
*	Creates and returns a graphics pipeline.
*/
RESTRICTED VulkanGraphicsPipeline *const RESTRICT VulkanInterface::CreateGraphicsPipeline(const VulkanGraphicsPipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanGraphicsPipeline *const RESTRICT newPipeline{ new (Memory::GlobalPoolAllocate<sizeof(VulkanGraphicsPipeline)>()) VulkanGraphicsPipeline() };
	newPipeline->Initialize(parameters);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanGraphicsPipelines.Emplace(newPipeline);

	return newPipeline;
}

/*
*	Creates and returns a ray tracing pipeline.
*/
RESTRICTED VulkanRayTracingPipeline *const RESTRICT VulkanInterface::CreateRayTracingPipeline(const VulkanRayTracingPipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanRayTracingPipeline *const RESTRICT newPipeline{ new (Memory::GlobalPoolAllocate<sizeof(VulkanRayTracingPipeline)>()) VulkanRayTracingPipeline() };
	newPipeline->Initialize(parameters);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanRayTracingPipelines.Emplace(newPipeline);

	return newPipeline;
}

/*
*	Creates and returns a query pool.
*/
RESTRICTED VulkanQueryPool *const RESTRICT VulkanInterface::CreateQueryPool(const VkQueryType query_type, const uint32 query_count) NOEXCEPT
{
	VulkanQueryPool *const RESTRICT new_query_pool{ new (Memory::GlobalPoolAllocate<sizeof(VulkanQueryPool)>()) VulkanQueryPool() };
	new_query_pool->Initialize(query_type, query_count);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanQueryPools.Emplace(new_query_pool);

	return new_query_pool;
}

/*
*	Creates and returns a render pass.
*/
RESTRICTED VulkanRenderPass *const RESTRICT VulkanInterface::CreateRenderPass(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT
{
	VulkanRenderPass *const RESTRICT newRenderPass{ new (Memory::GlobalPoolAllocate<sizeof(VulkanRenderPass)>()) VulkanRenderPass() };
	newRenderPass->Initialize(parameters);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanRenderPasses.Emplace(newRenderPass);

	return newRenderPass;
}

/*
*	Creates and returns a render target.
*/
RESTRICTED VulkanRenderTarget *const RESTRICT VulkanInterface::CreateRenderTarget(const VkExtent2D extent, const VkFormat format) NOEXCEPT
{
	VulkanRenderTarget *const RESTRICT newRenderTarget{ new (Memory::GlobalPoolAllocate<sizeof(VulkanRenderTarget)>()) VulkanRenderTarget() };
	newRenderTarget->Initialize(extent, format);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanRenderTargets.Emplace(newRenderTarget);

	return newRenderTarget;
}

/*
*	Creates and returns a semaphore.
*/
RESTRICTED VulkanSemaphore *const RESTRICT VulkanInterface::CreateSemaphore() NOEXCEPT
{
	VulkanSemaphore *const RESTRICT newSemaphore{ new (Memory::GlobalPoolAllocate<sizeof(VulkanSemaphore)>()) VulkanSemaphore() };
	newSemaphore->Initialize();

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanSemaphores.Emplace(newSemaphore);

	return newSemaphore;
}

/*
*	Creates and returns a sampler.
*/
RESTRICTED VulkanSampler *const RESTRICT VulkanInterface::CreateSampler(const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	VulkanSampler *const RESTRICT newSampler{ new (Memory::GlobalPoolAllocate<sizeof(VulkanSampler)>()) VulkanSampler() };
	newSampler->Initialize(magnificationFilter, mipmapMode, addressMode);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanSamplers.Emplace(newSampler);

	return newSampler;
}

/*
*	Creates and returns a shader module.
*/
RESTRICTED VulkanShaderModule *const RESTRICT VulkanInterface::CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT
{
	VulkanShaderModule *const RESTRICT newShaderModule{ new (Memory::GlobalPoolAllocate<sizeof(VulkanShaderModule)>()) VulkanShaderModule() };
	newShaderModule->Initialize(shaderData, shaderDataSize, stage);

	static Spinlock lock;
	SCOPED_LOCK(lock);

	_VulkanShaderModules.Emplace(newShaderModule);

	return newShaderModule;
}
#endif