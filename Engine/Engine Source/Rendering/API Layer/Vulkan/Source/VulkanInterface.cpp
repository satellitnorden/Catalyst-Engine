//Header file.
#include <VulkanInterface.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanInterface);

/*
*	Default constructor.
*/
VulkanInterface::VulkanInterface() NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanInterface::~VulkanInterface() NOEXCEPT
{

}

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
	queues[static_cast<uint8>(Queue::Graphics)].Initialize(vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex());

#if !RENDERDOC_DEBUGGING
	//Initialize the present queue.
	queues[static_cast<uint8>(Queue::Present)].Initialize(vulkanPhysicalDevice.GetPresentQueueFamilyIndex());

	//Initialize the transfer queue.
	queues[static_cast<uint8>(Queue::Transfer)].Initialize(vulkanPhysicalDevice.GetTransferQueueFamilyIndex());
#endif

	//Initialize the graphics Vulkan command pool.
	graphicsVulkanCommandPool.Initialize(vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex());

	//Initialize the transfer Vulkan command pool.
	transferVulkanCommandPool.Initialize(vulkanPhysicalDevice.GetTransferQueueFamilyIndex());

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
	queues[static_cast<uint8>(Queue::Graphics)].WaitIdle();
#if !RENDERDOC_DEBUGGING
	queues[static_cast<uint8>(Queue::Present)].WaitIdle();
	queues[static_cast<uint8>(Queue::Transfer)].WaitIdle();
#endif

	//Release all Vulkan 2D textures.
	for (Vulkan2DTexture * RESTRICT vulkan2DTexture : vulkan2DTextures)
	{
		vulkan2DTexture->Release();
		delete vulkan2DTexture;
	}

	//Release all Vulkan bufferrs.
	for (VulkanBuffer * RESTRICT vulkanBuffer : vulkanBuffers)
	{
		vulkanBuffer->Release();
		delete vulkanBuffer;
	}

	//Release all Vulkan cube map textures.
	for (VulkanCubeMapTexture * RESTRICT vulkanCubeMapTexture : vulkanCubeMapTextures)
	{
		vulkanCubeMapTexture->Release();
		delete vulkanCubeMapTexture;
	}

	//Release all Vulkan depth buffers.
	for (VulkanDepthBuffer * RESTRICT vulkanDepthBuffer : vulkanDepthBuffers)
	{
		vulkanDepthBuffer->Release();
		delete vulkanDepthBuffer;
	}

	//Release all Vulkan fences.
	for (VulkanFence * RESTRICT vulkanFence : vulkanFences)
	{
		vulkanFence->Release();
		delete vulkanFence;
	}

	//Release all Vulkan render targets.
	for (VulkanRenderTarget * RESTRICT vulkanRenderTarget : vulkanRenderTargets)
	{
		vulkanRenderTarget->Release();
		delete vulkanRenderTarget;
	}

	//Release all Vulkan pipelines.
	for (VulkanPipeline * RESTRICT vulkanPipeline : vulkanPipelines)
	{
		vulkanPipeline->Release();
		delete vulkanPipeline;
	}

	//Release all Vulkan semaphores.
	for (VulkanSemaphore * RESTRICT vulkanSemaphore : vulkanSemaphores)
	{
		vulkanSemaphore->Release();
		delete vulkanSemaphore;
	}

	//Release all Vulkan shader modules.
	for (VulkanShaderModule * RESTRICT vulkanShaderModule : vulkanShaderModules)
	{
		vulkanShaderModule->Release();
		delete vulkanShaderModule;
	}

	//Release all Vulkan uniform buffers.
	for (VulkanUniformBuffer * RESTRICT vulkanUniformBuffer : vulkanUniformBuffers)
	{
		vulkanUniformBuffer->Release();
		delete vulkanUniformBuffer;
	}

	//Release the Vulkan descriptor pool.
	vulkanDescriptorPool.Release();

	//Release the graphics Vulkan command pool.
	graphicsVulkanCommandPool.Release();

	//Release the transfer Vulkan command pool.
	transferVulkanCommandPool.Release();

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
*	Creates and returns a 2D texture with void data.
*/
RESTRICTED Vulkan2DTexture* VulkanInterface::Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	Vulkan2DTexture *RESTRICT new2DTexture = new Vulkan2DTexture;
	new2DTexture->Initialize(textureMipmapLevels, textureWidth, textureHeight, textureChannels, textureTexelSize, textureData, format, magnificationFilter, mipmapMode, addressMode);

	vulkan2DTextures.EmplaceSlow(new2DTexture);

	return new2DTexture;
}

/*
*	Creates and returns a 2D texture with byte data.
*/
RESTRICTED Vulkan2DTexture* VulkanInterface::Create2DTexture(const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const DynamicArray<DynamicArray<byte>> &textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT
{
	Vulkan2DTexture *RESTRICT new2DTexture = new Vulkan2DTexture;
	new2DTexture->Initialize(textureWidth, textureHeight, textureChannels, textureData, format, magnificationFilter, mipmapMode, addressMode);

	vulkan2DTextures.EmplaceSlow(new2DTexture);

	return new2DTexture;
}

/*
*	Creates and returns a buffer.
*/
RESTRICTED VulkanBuffer* VulkanInterface::CreateBuffer(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT
{
	VulkanBuffer *RESTRICT newBuffer = new VulkanBuffer;
	newBuffer->Initialize(data, dataSizes, dataChunks);

	vulkanBuffers.EmplaceSlow(newBuffer);

	return newBuffer;
}

/*
*	Creates and returns a cube map texture.
*/
RESTRICTED VulkanCubeMapTexture* VulkanInterface::CreateCubeMapTexture(const uint32 width, const uint32 height, const byte *RESTRICT *RESTRICT textureData) NOEXCEPT
{
	VulkanCubeMapTexture *RESTRICT newCubeMapTexture = new VulkanCubeMapTexture;
	newCubeMapTexture->Initialize(width, height, textureData);

	vulkanCubeMapTextures.EmplaceSlow(newCubeMapTexture);

	return newCubeMapTexture;
}

/*
*	Creates and returns a depth buffer.
*/
RESTRICTED VulkanDepthBuffer* VulkanInterface::CreateDepthBuffer(const VkExtent2D &depthBufferExtent) NOEXCEPT
{
	VulkanDepthBuffer *RESTRICT newDepthBuffer = new VulkanDepthBuffer;
	newDepthBuffer->Initialize(depthBufferExtent);

	vulkanDepthBuffers.EmplaceSlow(newDepthBuffer);

	return newDepthBuffer;
}

/*
*	Creates and returns a fence.
*/
RESTRICTED VulkanFence* VulkanInterface::CreateFence(const VkFenceCreateFlags flags) NOEXCEPT
{
	VulkanFence *RESTRICT newFence = new VulkanFence;
	newFence->Initialize(flags);

	vulkanFences.EmplaceSlow(newFence);

	return newFence;
}

/*
*	Creates and returns a pipeline.
*/
RESTRICTED VulkanPipeline* VulkanInterface::CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT
{
	VulkanPipeline *RESTRICT newPipeline = new VulkanPipeline;

	newPipeline->Initialize(vulkanPipelineCreationParameters);

	vulkanPipelines.EmplaceSlow(newPipeline);

	return newPipeline;
}

/*
*	Creates and returns a render target.
*/
RESTRICTED VulkanRenderTarget* VulkanInterface::CreateRenderTarget(const VkExtent2D extent) NOEXCEPT
{
	VulkanRenderTarget *RESTRICT newRenderTarget = new VulkanRenderTarget;

	newRenderTarget->Initialize(extent);

	vulkanRenderTargets.EmplaceSlow(newRenderTarget);

	return newRenderTarget;
}

/*
*	Creates and returns a semaphore.
*/
RESTRICTED VulkanSemaphore* VulkanInterface::CreateSemaphore() NOEXCEPT
{
	VulkanSemaphore *RESTRICT newSemaphore = new VulkanSemaphore;

	newSemaphore->Initialize();

	vulkanSemaphores.EmplaceSlow(newSemaphore);

	return newSemaphore;
}

/*
*	Creates and returns a shader module.
*/
RESTRICTED VulkanShaderModule* VulkanInterface::CreateShaderModule(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits stage) NOEXCEPT
{
	VulkanShaderModule *RESTRICT newShaderModule = new VulkanShaderModule;
	newShaderModule->Initialize(shaderByteCode, stage);

	vulkanShaderModules.EmplaceSlow(newShaderModule);

	return newShaderModule;
}

/*
*	Creates and returns a uniform buffer.
*/
RESTRICTED VulkanUniformBuffer* VulkanInterface::CreateUniformBuffer(const uint64 newUniformBufferSize) NOEXCEPT
{
	VulkanUniformBuffer *RESTRICT newUniformBuffer = new VulkanUniformBuffer;
	newUniformBuffer->Initialize(newUniformBufferSize);

	vulkanUniformBuffers.EmplaceSlow(newUniformBuffer);

	return newUniformBuffer;
}