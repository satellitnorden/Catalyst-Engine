//Header file.
#include <Rendering/API Layer/Vulkan/VulkanInterface.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>
#include <Multithreading/Spinlock.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanInterface);

/*
*	Initializes this Vulkan interface.
*/
void VulkanInterface::Initialize(Window &window) NOEXCEPT
{
	//Initialize the Vulkan instance.
	vulkanInstance.Initialize();

	//Initialize the Vulkan surface.
	vulkanSurface.Initialize(window);

	//Initialize the Vulkan physical device.
	vulkanPhysicalDevice.Initialize();

	//Initialize the Vulkan logical device.
	vulkanLogicalDevice.Initialize();

	//Initialize the graphics queue.
	queues[INDEX(Queue::Graphics)].Initialize(vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex());

#if !RENDERDOC_DEBUGGING
	//Initialize the present queue.
	queues[INDEX(Queue::Present)].Initialize(vulkanPhysicalDevice.GetPresentQueueFamilyIndex());
#endif

	//Initialize the Vulkan swap chain.
	vulkanSwapchain.Initialize();

	//Initialize the Vulkan descriptor pool.
	vulkanDescriptorPool.Initialize();
}

/*
*	Pre-updates this Vulkan interface.
*/
void VulkanInterface::PreUpdate(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT
{
	//Update the next image index in the Vulkan swap chain.
	vulkanSwapchain.UpdateNextImageIndex(imageAvailableSemaphore);
}

/*
*	Post-pdates this Vulkan interface.
*/
void VulkanInterface::PostUpdate(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT
{
	//Present the final image!
	vulkanSwapchain.Present(renderFinishedSemaphore);
}

/*
*	Releases this Vulkan interface.
*/
void VulkanInterface::Release() NOEXCEPT
{
	//Wait for all queues to finish.
	queues[INDEX(Queue::Graphics)].WaitIdle();
#if !RENDERDOC_DEBUGGING
	queues[INDEX(Queue::Present)].WaitIdle();
#endif

	//Release all Vulkan 2D textures.
	for (Vulkan2DTexture *const RESTRICT vulkan2DTexture : vulkan2DTextures)
	{
		vulkan2DTexture->Release();
		delete vulkan2DTexture;
	}

	//Release all Vulkan command pools.
	for (VulkanCommandPool *const RESTRICT vulkanCommandPool : vulkanCommandPools)
	{
		vulkanCommandPool->Release();
		delete vulkanCommandPool;
	}

	//Release all Vulkan bufferrs.
	for (VulkanConstantBuffer *const RESTRICT vulkanConstantBuffer : vulkanConstantBuffers)
	{
		vulkanConstantBuffer->Release();
		delete vulkanConstantBuffer;
	}

	//Release all Vulkan cube map textures.
	for (VulkanCubeMapTexture *const RESTRICT vulkanCubeMapTexture : vulkanCubeMapTextures)
	{
		vulkanCubeMapTexture->Release();
		delete vulkanCubeMapTexture;
	}

	//Release all Vulkan depth buffers.
	for (VulkanDepthBuffer *const RESTRICT vulkanDepthBuffer : vulkanDepthBuffers)
	{
		vulkanDepthBuffer->Release();
		delete vulkanDepthBuffer;
	}

	//Release all Vulkan events.
	for (VulkanEvent *const RESTRICT vulkanEvent : vulkanEvents)
	{
		vulkanEvent->Release();
		delete vulkanEvent;
	}

	//Release all Vulkan fences.
	for (VulkanFence *const RESTRICT vulkanFence : vulkanFences)
	{
		vulkanFence->Release();
		delete vulkanFence;
	}

	//Release all Vulkan render targets.
	for (VulkanRenderTarget *const RESTRICT vulkanRenderTarget : vulkanRenderTargets)
	{
		vulkanRenderTarget->Release();
		delete vulkanRenderTarget;
	}

	//Release all Vulkan pipelines.
	for (VulkanPipeline *const RESTRICT vulkanPipeline : vulkanPipelines)
	{
		vulkanPipeline->Release();
		delete vulkanPipeline;
	}

	//Release all Vulkan semaphores.
	for (VulkanSemaphore *const RESTRICT vulkanSemaphore : vulkanSemaphores)
	{
		vulkanSemaphore->Release();
		delete vulkanSemaphore;
	}

	//Release all Vulkan shader modules.
	for (VulkanShaderModule *const RESTRICT vulkanShaderModule : vulkanShaderModules)
	{
		vulkanShaderModule->Release();
		delete vulkanShaderModule;
	}

	//Release all Vulkan storage buffers.
	for (VulkanStorageBuffer *const RESTRICT vulkanStorageBuffer : vulkanStorageBuffers)
	{
		vulkanStorageBuffer->Release();
		delete vulkanStorageBuffer;
	}

	//Release all Vulkan uniform buffers.
	for (VulkanUniformBuffer *const RESTRICT vulkanUniformBuffer : vulkanUniformBuffers)
	{
		vulkanUniformBuffer->Release();
		delete vulkanUniformBuffer;
	}

	//Release the Vulkan descriptor pool.
	vulkanDescriptorPool.Release();

	//Release the Vulkan swap chain.
	vulkanSwapchain.Release();

	//Release the Vulkan logical device.
	vulkanLogicalDevice.Release();

	//Release the Vulkan surface.
	vulkanSurface.Release();

	//Release the Vulkan instance.
	vulkanInstance.Release();
}

/*
*	Creates and returns a 2D texture.
*/
RESTRICTED Vulkan2DTexture* VulkanInterface::Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	Vulkan2DTexture *const RESTRICT new2DTexture = new Vulkan2DTexture;
	new2DTexture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureChannels, textureTexelSize, textureData, format, magnificationFilter, mipmapMode, addressMode);

	vulkan2DTextures.EmplaceSlow(new2DTexture);

	return new2DTexture;
}

/*
*	Destroys a 2D texture.
*/
void VulkanInterface::Destroy2DTexture(Vulkan2DTexture *const RESTRICT texture) NOEXCEPT
{
	texture->Release();
	vulkan2DTextures.Erase(texture);
}

/*
*	Creates and returns a graphics command pool.
*/
RESTRICTED VulkanCommandPool* VulkanInterface::CreateGraphicsCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT newCommandPool = new VulkanCommandPool;
	newCommandPool->Initialize(flags, vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex());

	vulkanCommandPoolsLock.Lock();
	vulkanCommandPools.EmplaceSlow(newCommandPool);
	vulkanCommandPoolsLock.Unlock();

	return newCommandPool;
}

/*
*	Creates and returns a constant buffer.
*/
RESTRICTED VulkanConstantBuffer* VulkanInterface::CreateConstantBuffer(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanConstantBuffer *const RESTRICT newBuffer = new VulkanConstantBuffer;
	newBuffer->Initialize(data, dataSizes, dataChunks);

	vulkanConstantBuffers.EmplaceSlow(newBuffer);

	return newBuffer;
}

/*
*	Creates and returns a cube map texture.
*/
RESTRICTED VulkanCubeMapTexture* VulkanInterface::CreateCubeMapTexture(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanCubeMapTexture *const RESTRICT newCubeMapTexture = new VulkanCubeMapTexture;
	newCubeMapTexture->Initialize(data, width, height);

	vulkanCubeMapTextures.EmplaceSlow(newCubeMapTexture);

	return newCubeMapTexture;
}

/*
*	Creates and returns a depth buffer.
*/
RESTRICTED VulkanDepthBuffer* VulkanInterface::CreateDepthBuffer(const VkExtent2D &depthBufferExtent) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanDepthBuffer *const RESTRICT newDepthBuffer = new VulkanDepthBuffer;
	newDepthBuffer->Initialize(depthBufferExtent);

	vulkanDepthBuffers.EmplaceSlow(newDepthBuffer);

	return newDepthBuffer;
}

/*
*	Destroys a descriptor set.
*/
void VulkanInterface::DestroyDescriptorSet(VkDescriptorSet descriptorSet) const NOEXCEPT
{
	//Destroy the descriptor set.
	vulkanDescriptorPool.FreeDescriptorSet(descriptorSet);
}

/*
*	Creates and returns an event.
*/
RESTRICTED VulkanEvent* VulkanInterface::CreateEvent() NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanEvent *const RESTRICT newEvent = new VulkanEvent;
	newEvent->Initialize();

	vulkanEvents.EmplaceSlow(newEvent);

	return newEvent;
}

/*
*	Creates and returns a fence.
*/
RESTRICTED VulkanFence* VulkanInterface::CreateFence(const VkFenceCreateFlags flags) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanFence *const RESTRICT newFence = new VulkanFence;
	newFence->Initialize(flags);

	vulkanFences.EmplaceSlow(newFence);

	return newFence;
}

/*
*	Creates and returns a pipeline.
*/
RESTRICTED VulkanPipeline* VulkanInterface::CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanPipeline *const RESTRICT newPipeline = new VulkanPipeline;

	newPipeline->Initialize(vulkanPipelineCreationParameters);

	vulkanPipelines.EmplaceSlow(newPipeline);

	return newPipeline;
}

/*
*	Creates and returns a render target.
*/
RESTRICTED VulkanRenderTarget* VulkanInterface::CreateRenderTarget(const VkExtent2D extent, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanRenderTarget *const RESTRICT newRenderTarget = new VulkanRenderTarget;

	newRenderTarget->Initialize(extent, addressMode);

	vulkanRenderTargets.EmplaceSlow(newRenderTarget);

	return newRenderTarget;
}

/*
*	Creates and returns a semaphore.
*/
RESTRICTED VulkanSemaphore* VulkanInterface::CreateSemaphore() NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanSemaphore *const RESTRICT newSemaphore = new VulkanSemaphore;

	newSemaphore->Initialize();

	vulkanSemaphores.EmplaceSlow(newSemaphore);

	return newSemaphore;
}

/*
*	Creates and returns a shader module.
*/
RESTRICTED VulkanShaderModule* VulkanInterface::CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanShaderModule *const RESTRICT newShaderModule = new VulkanShaderModule;
	newShaderModule->Initialize(shaderData, shaderDataSize, stage);

	vulkanShaderModules.EmplaceSlow(newShaderModule);

	return newShaderModule;
}

/*
*	Creates and returns a storage buffer.
*/
RESTRICTED VulkanStorageBuffer* VulkanInterface::CreateStorageBuffer(const VkDeviceSize initialStorageBufferSize) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanStorageBuffer *const RESTRICT newStorageBuffer = new VulkanStorageBuffer;
	newStorageBuffer->Initialize(initialStorageBufferSize);

	vulkanStorageBuffers.EmplaceSlow(newStorageBuffer);

	return newStorageBuffer;
}

/*
*	Creates and returns a uniform buffer.
*/
RESTRICTED VulkanUniformBuffer* VulkanInterface::CreateUniformBuffer(const uint64 newUniformBufferSize) NOEXCEPT
{
	static Spinlock lock;
	ScopedLock<Spinlock>{ lock };

	VulkanUniformBuffer *const RESTRICT newUniformBuffer = new VulkanUniformBuffer;
	newUniformBuffer->Initialize(newUniformBufferSize);

	vulkanUniformBuffers.EmplaceSlow(newUniformBuffer);

	return newUniformBuffer;
}