#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/Vulkan2DTexture.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanConstantBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanCubeMapTexture.h>
#include <Rendering/Abstraction/Vulkan/VulkanDepthBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/Abstraction/Vulkan/VulkanEvent.h>
#include <Rendering/Abstraction/Vulkan/VulkanFence.h>
#include <Rendering/Abstraction/Vulkan/VulkanFramebuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanGraphicsPipeline.h>
#include <Rendering/Abstraction/Vulkan/VulkanGraphicsPipelineCreationParameters.h>
#include <Rendering/Abstraction/Vulkan/VulkanInstance.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanQueue.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderPass.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderPassCreationParameters.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderTarget.h>
#include <Rendering/Abstraction/Vulkan/VulkanSampler.h>
#include <Rendering/Abstraction/Vulkan/VulkanSemaphore.h>
#include <Rendering/Abstraction/Vulkan/VulkanShaderModule.h>
#include <Rendering/Abstraction/Vulkan/VulkanStorageBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanSurface.h>
#include <Rendering/Abstraction/Vulkan/VulkanSwapChain.h>
#include <Rendering/Abstraction/Vulkan/VulkanUniformBuffer.h>

class VulkanInterface final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VulkanInterface);

	/*
	*	Default constructor.
	*/
	VulkanInterface() NOEXCEPT
	{
	
	}

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
	const VulkanInstance& GetInstance() const NOEXCEPT { return _VulkanInstance; }

	/*
	*	Returns the surface.
	*/
	const VulkanSurface& GetSurface() const NOEXCEPT { return _VulkanSurface; }

	/*
	*	Returns the physical device.
	*/
	const VulkanPhysicalDevice& GetPhysicalDevice() const NOEXCEPT { return _VulkanPhysicalDevice; }

	/*
	*	Returns the logical device.
	*/
	const VulkanLogicalDevice& GetLogicalDevice() const NOEXCEPT { return _VulkanLogicalDevice; }

	/*
	*	Returns the swapchain.
	*/
	const VulkanSwapchain& GetSwapchain() const NOEXCEPT { return _VulkanSwapchain; }

	/*
	*	Returns the compute queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetQueue() NOEXCEPT { return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Compute); }

	/*
	*	Returns the graphics queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetGraphicsQueue() NOEXCEPT { return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Graphics); }

	/*
	*	Returns the present queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetPresentQueue() NOEXCEPT { return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Present); }

	/*
	*	Returns the transfer queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetTransferQueue() NOEXCEPT { return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::Transfer); }

	/*
	*	Returns the descriptor pool.
	*/
	const VulkanDescriptorPool& GetDescriptorPool() const NOEXCEPT { return _VulkanDescriptorPool; }

	/*
	*	Creates and returns a 2D texture.
	*/
	RESTRICTED Vulkan2DTexture *const RESTRICT Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format) NOEXCEPT;

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
	*	Creates and returns a buffer.
	*/
	RESTRICTED VulkanConstantBuffer *const RESTRICT CreateBuffer(const VkDeviceSize size) NOEXCEPT;

	/*
	*	Destroys a constant buffer.
	*/
	void DestroyConstantBuffer(VulkanConstantBuffer *const RESTRICT buffer) NOEXCEPT;

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
	void DestroyDescriptorSet(VulkanDescriptorSet *const RESTRICT descriptorSet) NOEXCEPT;

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
	*	Creates and returns a graphics pipeline.
	*/
	RESTRICTED VulkanGraphicsPipeline *const RESTRICT CreateGraphicsPipeline(const VulkanGraphicsPipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a render pass.
	*/
	RESTRICTED VulkanRenderPass *const RESTRICT CreateRenderPass(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a render target.
	*/
	RESTRICTED VulkanRenderTarget *const RESTRICT CreateRenderTarget(const VkExtent2D extent, const VkFormat format) NOEXCEPT;

	/*
	*	Creates and returns a semaphore.
	*/
	RESTRICTED VulkanSemaphore *const RESTRICT CreateSemaphore() NOEXCEPT;

	/*
	*	Creates and returns a sampler.
	*/
	RESTRICTED VulkanSampler *const RESTRICT CreateSampler(const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT;

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
	RESTRICTED VulkanUniformBuffer *const RESTRICT CreateUniformBuffer(const uint64 newUniformBufferSize, const VkBufferUsageFlags usage) NOEXCEPT;

	/*
	*	Destroys a uniform buffer.
	*/
	void DestroyUniformBuffer(VulkanUniformBuffer *const RESTRICT uniformBuffer) NOEXCEPT;

private:

	//The Vulkan instance.
	VulkanInstance _VulkanInstance;

	//The Vulkan surface.
	VulkanSurface _VulkanSurface;

	//The Vulkan physical device.
	VulkanPhysicalDevice _VulkanPhysicalDevice;

	//The Vulkan logical device.
	VulkanLogicalDevice _VulkanLogicalDevice;

	//The Vulkan swap chain.
	VulkanSwapchain _VulkanSwapchain;

	//The Vulkan descriptor pool.
	VulkanDescriptorPool _VulkanDescriptorPool;

	//Container for all Vulkan 2D textures.
	DynamicArray<Vulkan2DTexture *RESTRICT> _Vulkan2DTextures;

	//The lock for all Vulkan command pools.
	Spinlock _VulkanCommandPoolsLock;

	//Container for all Vulkan command pools.
	DynamicArray<VulkanCommandPool *RESTRICT> _VulkanCommandPools;

	//Container for all Vulkan constant buffers.
	DynamicArray<VulkanConstantBuffer *RESTRICT> _VulkanConstantBuffers;

	//Container for all Vulkan cube map textures.
	DynamicArray<VulkanCubeMapTexture *RESTRICT> _VulkanCubeMapTextures;

	//Container for all Vulkan depth buffers.
	DynamicArray<VulkanDepthBuffer *RESTRICT> _VulkanDepthBuffers;

	//Container for all Vulkan descriptor set layouts.
	DynamicArray<VulkanDescriptorSetLayout *RESTRICT> _VulkanDescriptorSetLayouts;

	//Container for all Vulkan descriptor sets.
	DynamicArray<VulkanDescriptorSet *RESTRICT> _VulkanDescriptorSets;

	//Container for all Vulkan events.
	DynamicArray<VulkanEvent *RESTRICT> _VulkanEvents;

	//Container for all Vulkan fences.
	DynamicArray<VulkanFence *RESTRICT> _VulkanFences;

	//Container for all Vulkan framebuffers.
	DynamicArray<VulkanFramebuffer *RESTRICT> _VulkanFramebuffers;

	//Container for all Vulkan pipelines.
	DynamicArray<VulkanGraphicsPipeline *RESTRICT> _VulkanPipelines;

	//Container for all Vulkan render passes.
	DynamicArray<VulkanRenderPass *RESTRICT> _VulkanRenderPasses;

	//Container for all Vulkan render targets.
	DynamicArray<VulkanRenderTarget *RESTRICT> _VulkanRenderTargets;

	//Container for all Vulkan semaphores.
	DynamicArray<VulkanSemaphore *RESTRICT> _VulkanSemaphores;

	//Container for all Vulkan samplers.
	DynamicArray<VulkanSampler *RESTRICT> _VulkanSamplers;

	//Container for all Vulkan shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> _VulkanShaderModules;

	//Container for all Vulkan storage buffers.
	DynamicArray<VulkanStorageBuffer *RESTRICT> _VulkanStorageBuffers;

	//Container for all Vulkan uniform buffers.
	DynamicArray<VulkanUniformBuffer *RESTRICT> _VulkanUniformBuffers;

};
#endif