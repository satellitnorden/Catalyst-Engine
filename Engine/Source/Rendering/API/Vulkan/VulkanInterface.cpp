//Header file.
#include <Rendering/API/Vulkan/VulkanInterface.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>
#include <Multithreading/Spinlock.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanInterface);

/*
*	Initializes this Vulkan interface.
*/
void VulkanInterface::Initialize() NOEXCEPT
{
	//Load vulkan.
	PlatformVulkan::LoadVulkan();

	//Initialize the Vulkan instance.
	vulkanInstance.Initialize();

	//Initialize the Vulkan surface.
	vulkanSurface.Initialize();

	//Initialize the Vulkan physical device.
	vulkanPhysicalDevice.Initialize();

	//Initialize the Vulkan logical device.
	vulkanLogicalDevice.Initialize();

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
	vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Compute)->WaitIdle();
	vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Graphics)->WaitIdle();
	vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Present)->WaitIdle();
	vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Transfer)->WaitIdle();

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

	//Release all Vulkan descriptor set layouts.
	for (VulkanDescriptorSetLayout *const RESTRICT vulkanDescriptorSetLayout : vulkanDescriptorSetLayouts)
	{
		vulkanDescriptorSetLayout->Release();
		delete vulkanDescriptorSetLayout;
	}

	//Release all Vulkan descriptor sets.
	for (VulkanDescriptorSet *const RESTRICT vulkanDescriptorSet : vulkanDescriptorSets)
	{
		delete vulkanDescriptorSet;
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

	//Release all Vulkan render passes.
	for (VulkanRenderPass *const RESTRICT vulkanRenderPass : vulkanRenderPasses)
	{
		vulkanRenderPass->Release();
		delete vulkanRenderPass;
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
RESTRICTED Vulkan2DTexture *const RESTRICT VulkanInterface::Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	Vulkan2DTexture *const RESTRICT new2DTexture = new Vulkan2DTexture;
	new2DTexture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureChannels, textureTexelSize, textureData, format, magnificationFilter, mipmapMode, addressMode);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };


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
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateGraphicsCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT newCommandPool = new VulkanCommandPool;
	newCommandPool->Initialize(flags, vulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Graphics));

	vulkanCommandPoolsLock.Lock();
	vulkanCommandPools.EmplaceSlow(newCommandPool);
	vulkanCommandPoolsLock.Unlock();

	return newCommandPool;
}

/*
*	Creates and returns a transfer command pool.
*/
RESTRICTED VulkanCommandPool *const RESTRICT VulkanInterface::CreateTransferCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT
{
	VulkanCommandPool *const RESTRICT newCommandPool = new VulkanCommandPool;
	newCommandPool->Initialize(flags, vulkanLogicalDevice.GetQueueFamilyIndex(VulkanLogicalDevice::QueueType::Transfer));

	vulkanCommandPoolsLock.Lock();
	vulkanCommandPools.EmplaceSlow(newCommandPool);
	vulkanCommandPoolsLock.Unlock();

	return newCommandPool;
}

/*
*	Creates and returns a constant buffer.
*/
RESTRICTED VulkanConstantBuffer *const RESTRICT VulkanInterface::CreateConstantBuffer(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT
{
	VulkanConstantBuffer *const RESTRICT newBuffer = new VulkanConstantBuffer;
	newBuffer->Initialize(data, dataSizes, dataChunks);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanConstantBuffers.EmplaceSlow(newBuffer);

	return newBuffer;
}

/*
*	Creates and returns a cube map texture.
*/
RESTRICTED VulkanCubeMapTexture *const RESTRICT VulkanInterface::CreateCubeMapTexture(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT
{
	VulkanCubeMapTexture *const RESTRICT newCubeMapTexture = new VulkanCubeMapTexture;
	newCubeMapTexture->Initialize(data, width, height);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanCubeMapTextures.EmplaceSlow(newCubeMapTexture);

	return newCubeMapTexture;
}

/*
*	Creates and returns a depth buffer.
*/
RESTRICTED VulkanDepthBuffer *const RESTRICT VulkanInterface::CreateDepthBuffer(const VkExtent2D &depthBufferExtent) NOEXCEPT
{
	VulkanDepthBuffer *const RESTRICT newDepthBuffer = new VulkanDepthBuffer;
	newDepthBuffer->Initialize(depthBufferExtent);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanDepthBuffers.EmplaceSlow(newDepthBuffer);

	return newDepthBuffer;
}

/*
*	Creates and returns a descriptor set layout.
*/
RESTRICTED VulkanDescriptorSetLayout *const RESTRICT VulkanInterface::CreateDescriptorSetLayout(const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings, const uint32 numberOfDescriptorSetLayoutBindings) NOEXCEPT
{
	VulkanDescriptorSetLayout *const RESTRICT newDescriptorSetLayout = new VulkanDescriptorSetLayout;
	newDescriptorSetLayout->Initialize(numberOfDescriptorSetLayoutBindings, descriptorSetLayoutBindings);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanDescriptorSetLayouts.EmplaceSlow(newDescriptorSetLayout);

	return newDescriptorSetLayout;
}

/*
*	Creates and returns a descriptor set.
*/
RESTRICTED VulkanDescriptorSet *const RESTRICT VulkanInterface::CreateDescriptorSet(const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT
{
	VulkanDescriptorSet *const RESTRICT newDescriptorSet = new VulkanDescriptorSet;
	newDescriptorSet->Initialize(vulkanDescriptorPool, vulkanDescriptorSetLayout);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanDescriptorSets.EmplaceSlow(newDescriptorSet);

	return newDescriptorSet;
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
RESTRICTED VulkanEvent *const RESTRICT VulkanInterface::CreateEvent() NOEXCEPT
{
	VulkanEvent *const RESTRICT newEvent = new VulkanEvent;
	newEvent->Initialize();

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanEvents.EmplaceSlow(newEvent);

	return newEvent;
}

/*
*	Creates and returns a fence.
*/
RESTRICTED VulkanFence *const RESTRICT VulkanInterface::CreateFence(const VkFenceCreateFlags flags) NOEXCEPT
{
	VulkanFence *const RESTRICT newFence = new VulkanFence;
	newFence->Initialize(flags);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanFences.EmplaceSlow(newFence);

	return newFence;
}

/*
*	Creates and returns a pipeline.
*/
RESTRICTED VulkanPipeline *const RESTRICT VulkanInterface::CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT
{
	VulkanPipeline *const RESTRICT newPipeline = new VulkanPipeline;
	newPipeline->Initialize(vulkanPipelineCreationParameters);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanPipelines.EmplaceSlow(newPipeline);

	return newPipeline;
}

Spinlock renderPassLock;

/*
*	Creates and returns a render pass.
*/
RESTRICTED VulkanRenderPass *const RESTRICT VulkanInterface::CreateRenderPass(const VulkanPipelineCreationParameters &parameters) NOEXCEPT
{
	VulkanRenderPass *const RESTRICT newRenderPass = new VulkanRenderPass;
	newRenderPass->Initialize(parameters);

	ScopedLock<Spinlock> scopedLock{ renderPassLock };

	vulkanRenderPasses.EmplaceSlow(newRenderPass);

	return newRenderPass;
}

/*
*	Creates and returns a render pass.
*/
RESTRICTED VulkanRenderPass *const RESTRICT VulkanInterface::CreateRenderPass(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT
{
	VulkanRenderPass *const RESTRICT newRenderPass = new VulkanRenderPass;
	newRenderPass->Initialize(parameters);

	ScopedLock<Spinlock> scopedLock{ renderPassLock };

	vulkanRenderPasses.EmplaceSlow(newRenderPass);

	return newRenderPass;
}

/*
*	Creates and returns a render target.
*/
RESTRICTED VulkanRenderTarget *const RESTRICT VulkanInterface::CreateRenderTarget(const VkExtent2D extent, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	VulkanRenderTarget *const RESTRICT newRenderTarget = new VulkanRenderTarget;
	newRenderTarget->Initialize(extent, addressMode);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanRenderTargets.EmplaceSlow(newRenderTarget);

	return newRenderTarget;
}

/*
*	Creates and returns a semaphore.
*/
RESTRICTED VulkanSemaphore *const RESTRICT VulkanInterface::CreateSemaphore() NOEXCEPT
{
	VulkanSemaphore *const RESTRICT newSemaphore = new VulkanSemaphore;
	newSemaphore->Initialize();

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanSemaphores.EmplaceSlow(newSemaphore);

	return newSemaphore;
}

/*
*	Creates and returns a shader module.
*/
RESTRICTED VulkanShaderModule *const RESTRICT VulkanInterface::CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT
{
	VulkanShaderModule *const RESTRICT newShaderModule = new VulkanShaderModule;
	newShaderModule->Initialize(shaderData, shaderDataSize, stage);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanShaderModules.EmplaceSlow(newShaderModule);

	return newShaderModule;
}

/*
*	Creates and returns a storage buffer.
*/
RESTRICTED VulkanStorageBuffer *const RESTRICT VulkanInterface::CreateStorageBuffer(const VkDeviceSize initialStorageBufferSize) NOEXCEPT
{
	VulkanStorageBuffer *const RESTRICT newStorageBuffer = new VulkanStorageBuffer;
	newStorageBuffer->Initialize(initialStorageBufferSize);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanStorageBuffers.EmplaceSlow(newStorageBuffer);

	return newStorageBuffer;
}

/*
*	Creates and returns a uniform buffer.
*/
RESTRICTED VulkanUniformBuffer *const RESTRICT VulkanInterface::CreateUniformBuffer(const uint64 newUniformBufferSize) NOEXCEPT
{
	VulkanUniformBuffer *const RESTRICT newUniformBuffer = new VulkanUniformBuffer;
	newUniformBuffer->Initialize(newUniformBufferSize);

	static Spinlock lock;
	ScopedLock<Spinlock> scopedLock{ lock };

	vulkanUniformBuffers.EmplaceSlow(newUniformBuffer);

	return newUniformBuffer;
}