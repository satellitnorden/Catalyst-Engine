//Header file.
#include <VulkanInterface.h>

//Singleton definition.
DEFINE_SINGLETON(VulkanInterface);

/*
*	Default constructor.
*/
VulkanInterface::VulkanInterface() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
VulkanInterface::~VulkanInterface() CATALYST_NOEXCEPT
{

}

/*
*	Initializes this Vulkan interface.
*/
void VulkanInterface::Initialize(Window &window) CATALYST_NOEXCEPT
{
	//Initialize the Vulkan instance.
	vulkanInstance.Initialize();

	//Initialize the Vulkan surface.
	vulkanSurface.Initialize(vulkanInstance, window);

	//Initialize the Vulkan physical device.
	vulkanPhysicalDevice.Initialize();

	//Initialize the Vulkan logical device.
	vulkanLogicalDevice.Initialize(vulkanPhysicalDevice);

	//Initialize the graphics Vulkan command pool.
	graphicsVulkanCommandPool.Initialize(vulkanPhysicalDevice.GetGraphicsQueueFamilyIndex());

	//Initialize the transfer Vulkan command pool.
	transferVulkanCommandPool.Initialize(vulkanPhysicalDevice.GetTransferQueueFamilyIndex());

	//Initialize the Vulkan swap chain.
	vulkanSwapchain.Initialize();

	//Initialize the Vulkan descriptor pool.
	vulkanDescriptorPool.Initialize();

	//Initialize all Vulkan semaphores.
	for (size_t i = 0; i < Semaphore::NumberOfSemaphores; ++i)
	{
		vulkanSemaphores[i].Initialize();
	}
}

/*
*	Pre-updates this Vulkan interface.
*/
void VulkanInterface::PreUpdate() CATALYST_NOEXCEPT
{
	//Update the next image index in the Vulkan swap chain.
	vulkanSwapchain.UpdateNextImageIndex(vulkanSemaphores[Semaphore::ImageAvailableSemaphore]);
}

/*
*	Post-pdates this Vulkan interface.
*/
void VulkanInterface::PostUpdate() CATALYST_NOEXCEPT
{
	//Present the final image!
	vulkanSwapchain.Present(vulkanSemaphores[Semaphore::RenderFinishedSemaphore], vulkanLogicalDevice.GetPresentQueue());
}

/*
*	Releases this Vulkan interface.
*/
void VulkanInterface::Release() CATALYST_NOEXCEPT
{
	//Wait for all queues to finish.
	vulkanLogicalDevice.GetGraphicsQueue().WaitIdle();
	vulkanLogicalDevice.GetPresentQueue().WaitIdle();
	vulkanLogicalDevice.GetTransferQueue().WaitIdle();

	//Release all Vulkan depth buffers.
	for (VulkanDepthBuffer * CATALYST_RESTRICT vulkanDepthBuffer : vulkanDepthBuffers)
	{
		vulkanDepthBuffer->Release();
		delete vulkanDepthBuffer;
	}

	//Release all Vulkan fences.
	for (VulkanFence * CATALYST_RESTRICT vulkanFence : vulkanFences)
	{
		vulkanFence->Release();
		delete vulkanFence;
	}

	//Release all Vulkan index buffers.
	for (VulkanIndexBuffer * CATALYST_RESTRICT vulkanIndexBuffer : vulkanIndexBuffers)
	{
		vulkanIndexBuffer->Release();
		delete vulkanIndexBuffer;
	}

	//Release all Vulkan pipelines.
	for (VulkanPipeline * CATALYST_RESTRICT vulkanPipeline : vulkanPipelines)
	{
		vulkanPipeline->Release();
		delete vulkanPipeline;
	}

	//Release all Vulkan shader modules.
	for (VulkanShaderModule * CATALYST_RESTRICT vulkanShaderModule : vulkanShaderModules)
	{
		vulkanShaderModule->Release();
		delete vulkanShaderModule;
	}

	//Release all Vulkan textures.
	for (VulkanTexture * CATALYST_RESTRICT vulkanTexture : vulkanTextures)
	{
		vulkanTexture->Release();
		delete vulkanTexture;
	}

	//Release all Vulkan uniform buffers.
	for (VulkanUniformBuffer * CATALYST_RESTRICT vulkanUniformBuffer : vulkanUniformBuffers)
	{
		vulkanUniformBuffer->Release();
		delete vulkanUniformBuffer;
	}

	//Release all Vulkan vertex buffers.
	for (VulkanVertexBuffer * CATALYST_RESTRICT vulkanVertexBuffer : vulkanVertexBuffers)
	{
		vulkanVertexBuffer->Release();
		delete vulkanVertexBuffer;
	}

	//Release all Vulkan semaphores.
	for (size_t i = 0; i < Semaphore::NumberOfSemaphores; ++i)
	{
		vulkanSemaphores[i].Release();
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
	vulkanSurface.Release(vulkanInstance);

	//Release the Vulkan instance.
	vulkanInstance.Release();
}

/*
*	Creates and returns a depth buffer.
*/
CATALYST_RESTRICTED VulkanDepthBuffer* VulkanInterface::CreateDepthBuffer(const VkExtent2D &depthBufferExtent) CATALYST_NOEXCEPT
{
	VulkanDepthBuffer *CATALYST_RESTRICT newDepthBuffer = new VulkanDepthBuffer;
	newDepthBuffer->Initialize(depthBufferExtent);

	vulkanDepthBuffers.Emplace(newDepthBuffer);

	return newDepthBuffer;
}

/*
*	Creates and returns a fence.
*/
CATALYST_RESTRICTED VulkanFence* VulkanInterface::CreateFence(const VkFenceCreateFlags flags) CATALYST_NOEXCEPT
{
	VulkanFence *CATALYST_RESTRICT newFence = new VulkanFence;
	newFence->Initialize(flags);

	vulkanFences.Emplace(newFence);

	return newFence;
}

/*
*	Creates and returns an index buffer.
*/
CATALYST_RESTRICTED VulkanIndexBuffer* VulkanInterface::CreateIndexBuffer(const DynamicArray<uint32> &indices) CATALYST_NOEXCEPT
{
	VulkanIndexBuffer *CATALYST_RESTRICT newIndexBuffer = new VulkanIndexBuffer;
	newIndexBuffer->Initialize(indices);

	vulkanIndexBuffers.Emplace(newIndexBuffer);

	return newIndexBuffer;
}

/*
*	Creates and returns a pipeline.
*/
CATALYST_RESTRICTED VulkanPipeline* VulkanInterface::CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) CATALYST_NOEXCEPT
{
	VulkanPipeline *CATALYST_RESTRICT newPipeline = new VulkanPipeline;

	newPipeline->Initialize(vulkanPipelineCreationParameters);

	vulkanPipelines.Emplace(newPipeline);

	return newPipeline;
}

/*
*	Creates and returns a shader module.
*/
CATALYST_RESTRICTED VulkanShaderModule* VulkanInterface::CreateShaderModule(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits stage) CATALYST_NOEXCEPT
{
	VulkanShaderModule *CATALYST_RESTRICT newShaderModule = new VulkanShaderModule;
	newShaderModule->Initialize(shaderByteCode, stage);

	vulkanShaderModules.Emplace(newShaderModule);

	return newShaderModule;
}

/*
*	Creates and returns a texture.
*/
CATALYST_RESTRICTED VulkanTexture* VulkanInterface::CreateTexture(const uint32 width, const uint32 height, const byte *textureData) CATALYST_NOEXCEPT
{
	VulkanTexture *CATALYST_RESTRICT newTexture = new VulkanTexture;
	newTexture->Initialize(width, height, textureData);

	vulkanTextures.Emplace(newTexture);

	return newTexture;
}

/*
*	Creates and returns a uniform buffer.
*/
CATALYST_RESTRICTED VulkanUniformBuffer* VulkanInterface::CreateUniformBuffer(const size_t newUniformBufferSize) CATALYST_NOEXCEPT
{
	VulkanUniformBuffer *CATALYST_RESTRICT newUniformBuffer = new VulkanUniformBuffer;
	newUniformBuffer->Initialize(newUniformBufferSize);

	vulkanUniformBuffers.Emplace(newUniformBuffer);

	return newUniformBuffer;
}

/*
*	Creates and returns a vertex buffer.
*/
CATALYST_RESTRICTED VulkanVertexBuffer* VulkanInterface::CreateVertexBuffer(const DynamicArray<Vertex> &vertices) CATALYST_NOEXCEPT
{
	VulkanVertexBuffer *CATALYST_RESTRICT newVertexBuffer = new VulkanVertexBuffer;
	newVertexBuffer->Initialize(vertices);

	vulkanVertexBuffers.Emplace(newVertexBuffer);

	return newVertexBuffer;
}