#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Vulkan.
#include <Rendering/API/Vulkan/Vulkan2DTexture.h>
#include <Rendering/API/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/API/Vulkan/VulkanCommandPool.h>
#include <Rendering/API/Vulkan/VulkanConstantBuffer.h>
#include <Rendering/API/Vulkan/VulkanCore.h>
#include <Rendering/API/Vulkan/VulkanCubeMapTexture.h>
#include <Rendering/API/Vulkan/VulkanDepthBuffer.h>
#include <Rendering/API/Vulkan/VulkanDescriptorPool.h>
#include <Rendering/API/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/API/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/API/Vulkan/VulkanEvent.h>
#include <Rendering/API/Vulkan/VulkanFence.h>
#include <Rendering/API/Vulkan/VulkanFramebuffer.h>
#include <Rendering/API/Vulkan/VulkanInstance.h>
#include <Rendering/API/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/API/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/API/Vulkan/VulkanPipeline.h>
#include <Rendering/API/Vulkan/VulkanPipelineCreationParameters.h>
#include <Rendering/API/Vulkan/VulkanQueue.h>
#include <Rendering/API/Vulkan/VulkanRenderPass.h>
#include <Rendering/API/Vulkan/VulkanRenderPassCreationParameters.h>
#include <Rendering/API/Vulkan/VulkanRenderTarget.h>
#include <Rendering/API/Vulkan/VulkanSemaphore.h>
#include <Rendering/API/Vulkan/VulkanShaderModule.h>
#include <Rendering/API/Vulkan/VulkanStorageBuffer.h>
#include <Rendering/API/Vulkan/VulkanSurface.h>
#include <Rendering/API/Vulkan/VulkanSwapChain.h>
#include <Rendering/API/Vulkan/VulkanUniformBuffer.h>

class VulkanInterface final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VulkanInterface);

	/*
	*	Default constructor.
	*/
	VulkanInterface() NOEXCEPT { }

	/*
	*	Initializes this Vulkan interface.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Pre-updates this Vulkan interface.
	*/
	void PreUpdate(const VulkanSemaphore *const RESTRICT imageAvailableSemaphore) NOEXCEPT;

	/*
	*	Post-updates this Vulkan interface.
	*/
	void PostUpdate(const VulkanSemaphore *const RESTRICT renderFinishedSemaphore) NOEXCEPT;

	/*
	*	Releases this Vulkan interface.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns the instance.
	*/
	const VulkanInstance& GetInstance() const NOEXCEPT { return vulkanInstance; }

	/*
	*	Returns the surface.
	*/
	const VulkanSurface& GetSurface() const NOEXCEPT { return vulkanSurface; }

	/*
	*	Returns the physical device.
	*/
	const VulkanPhysicalDevice& GetPhysicalDevice() const NOEXCEPT { return vulkanPhysicalDevice; }

	/*
	*	Returns the logical device.
	*/
	const VulkanLogicalDevice& GetLogicalDevice() const NOEXCEPT { return vulkanLogicalDevice; }

	/*
	*	Returns the swapchain.
	*/
	const VulkanSwapchain& GetSwapchain() const NOEXCEPT { return vulkanSwapchain; }

	/*
	*	Returns the compute queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetQueue() NOEXCEPT { return vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Compute); }

	/*
	*	Returns the graphics queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetGraphicsQueue() NOEXCEPT { return vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Graphics); }

	/*
	*	Returns the present queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetPresentQueue() NOEXCEPT { return vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Present); }

	/*
	*	Returns the transfer queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetTransferQueue() NOEXCEPT { return vulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Transfer); }

	/*
	*	Returns the descriptor pool.
	*/
	const VulkanDescriptorPool& GetDescriptorPool() const NOEXCEPT { return vulkanDescriptorPool; }

	/*
	*	Creates and returns a 2D texture.
	*/
	RESTRICTED Vulkan2DTexture *const RESTRICT Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT;

	/*
	*	Destroys a 2D texture.
	*/
	void Destroy2DTexture(Vulkan2DTexture *const RESTRICT texture) NOEXCEPT;

	/*
	*	Creates and returns a graphics command pool.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateGraphicsCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a transfer command pool.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateTransferCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a constant buffer.
	*/
	RESTRICTED VulkanConstantBuffer *const RESTRICT CreateConstantBuffer(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	RESTRICTED VulkanCubeMapTexture *const RESTRICT CreateCubeMapTexture(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Creates and returns a depth buffer.
	*/
	RESTRICTED VulkanDepthBuffer *const RESTRICT CreateDepthBuffer(const VkExtent2D &depthBufferExtent) NOEXCEPT;

	/*
	*	Creates and returns a descriptor set layout.
	*/
	RESTRICTED VulkanDescriptorSetLayout *const RESTRICT CreateDescriptorSetLayout(const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings, const uint32 numberOfDescriptorSetLayoutBindings) NOEXCEPT;

	/*
	*	Creates and returns a descriptor set.
	*/
	RESTRICTED VulkanDescriptorSet *const RESTRICT CreateDescriptorSet(const VulkanDescriptorSetLayout &vulkanDescriptorSetLayout) NOEXCEPT;

	/*
	*	Destroys a descriptor set.
	*/
	void DestroyDescriptorSet(VkDescriptorSet descriptorSet) const NOEXCEPT;

	/*
	*	Creates and returns an event.
	*/
	RESTRICTED VulkanEvent *const RESTRICT CreateEvent() NOEXCEPT;

	/*
	*	Creates and returns a fence.
	*/
	RESTRICTED VulkanFence *const RESTRICT CreateFence(const VkFenceCreateFlags flags = 0) NOEXCEPT;

	/*
	*	Creates and returns a framebuffer.
	*/
	RESTRICTED VulkanFramebuffer *const RESTRICT CreateFramebuffer(const VulkanFramebufferCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a pipeline.
	*/
	RESTRICTED VulkanPipeline *const RESTRICT CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT;

	/*
	*	Creates and returns a render pass.
	*/
	RESTRICTED VulkanRenderPass *const RESTRICT CreateRenderPass(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a render target.
	*/
	RESTRICTED VulkanRenderTarget *const RESTRICT CreateRenderTarget(const VkExtent2D extent, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerAddressMode addressMode) NOEXCEPT;

	/*
	*	Creates and returns a semaphore.
	*/
	RESTRICTED VulkanSemaphore *const RESTRICT CreateSemaphore() NOEXCEPT;

	/*
	*	Creates and returns a shader module.
	*/
	RESTRICTED VulkanShaderModule *const RESTRICT CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT;

	/*
	*	Creates and returns a storage buffer.
	*/
	RESTRICTED VulkanStorageBuffer *const RESTRICT CreateStorageBuffer(const VkDeviceSize initialStorageBufferSize) NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	RESTRICTED VulkanUniformBuffer *const RESTRICT CreateUniformBuffer(const uint64 newUniformBufferSize) NOEXCEPT;

private:

	//The Vulkan instance.
	VulkanInstance vulkanInstance;

	//The Vulkan surface.
	VulkanSurface vulkanSurface;

	//The Vulkan physical device.
	VulkanPhysicalDevice vulkanPhysicalDevice;

	//The Vulkan logical device.
	VulkanLogicalDevice vulkanLogicalDevice;

	//The Vulkan swap chain.
	VulkanSwapchain vulkanSwapchain;

	//The Vulkan descriptor pool.
	VulkanDescriptorPool vulkanDescriptorPool;

	//Container for all Vulkan 2D textures.
	DynamicArray<Vulkan2DTexture *RESTRICT> vulkan2DTextures;

	//The lock for all Vulkan command pools.
	Spinlock vulkanCommandPoolsLock;

	//Container for all Vulkan command pools.
	DynamicArray<VulkanCommandPool *RESTRICT> vulkanCommandPools;

	//Container for all Vulkan constant buffers.
	DynamicArray<VulkanConstantBuffer *RESTRICT> vulkanConstantBuffers;

	//Container for all Vulkan cube map textures.
	DynamicArray<VulkanCubeMapTexture *RESTRICT> vulkanCubeMapTextures;

	//Container for all Vulkan depth buffers.
	DynamicArray<VulkanDepthBuffer *RESTRICT> vulkanDepthBuffers;

	//Container for all Vulkan descriptor set layouts.
	DynamicArray<VulkanDescriptorSetLayout *RESTRICT> vulkanDescriptorSetLayouts;

	//Container for all Vulkan descriptor sets.
	DynamicArray<VulkanDescriptorSet *RESTRICT> vulkanDescriptorSets;

	//Container for all Vulkan events.
	DynamicArray<VulkanEvent *RESTRICT> vulkanEvents;

	//Container for all Vulkan fences.
	DynamicArray<VulkanFence *RESTRICT> vulkanFences;

	//Container for all Vulkan framebuffers.
	DynamicArray<VulkanFramebuffer *RESTRICT> vulkanFramebuffers;

	//Container for all Vulkan pipelines.
	DynamicArray<VulkanPipeline *RESTRICT> vulkanPipelines;

	//Container for all Vulkan render passes.
	DynamicArray<VulkanRenderPass *RESTRICT> vulkanRenderPasses;

	//Container for all Vulkan render targets.
	DynamicArray<VulkanRenderTarget *RESTRICT> vulkanRenderTargets;

	//Container for all Vulkan semaphores.
	DynamicArray<VulkanSemaphore *RESTRICT> vulkanSemaphores;

	//Container for all Vulkan shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> vulkanShaderModules;

	//Container for all Vulkan storage buffers.
	DynamicArray<VulkanStorageBuffer *RESTRICT> vulkanStorageBuffers;

	//Container for all Vulkan uniform buffers.
	DynamicArray<VulkanUniformBuffer *RESTRICT> vulkanUniformBuffers;

	/*
	*	Returns a new command pool.
	*/
	VulkanCommandPool GetNewCommandPool(const uint32 queueFamilyIndex) NOEXCEPT;

};