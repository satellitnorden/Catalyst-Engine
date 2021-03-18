#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/Vulkan2DTexture.h>
#include <Rendering/Abstraction/Vulkan/Vulkan3DTexture.h>
#include <Rendering/Abstraction/Vulkan/VulkanAccelerationStructure.h>
#include <Rendering/Abstraction/Vulkan/VulkanBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandBufferAbstraction.h>
#include <Rendering/Abstraction/Vulkan/VulkanCommandPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanComputePipeline.h>
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>
#include <Rendering/Abstraction/Vulkan/VulkanCubeMapTexture.h>
#include <Rendering/Abstraction/Vulkan/VulkanDepthBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/Abstraction/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/Abstraction/Vulkan/VulkanEvent.h>
#include <Rendering/Abstraction/Vulkan/VulkanFence.h>
#include <Rendering/Abstraction/Vulkan/VulkanFramebuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanGeometryInstance.h>
#include <Rendering/Abstraction/Vulkan/VulkanGraphicsPipeline.h>
#include <Rendering/Abstraction/Vulkan/VulkanInstance.h>
#include <Rendering/Abstraction/Vulkan/VulkanRayTracingPipeline.h>
#include <Rendering/Abstraction/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/Abstraction/Vulkan/VulkanQueryPool.h>
#include <Rendering/Abstraction/Vulkan/VulkanQueue.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderPass.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderPassCreationParameters.h>
#include <Rendering/Abstraction/Vulkan/VulkanRenderTarget.h>
#include <Rendering/Abstraction/Vulkan/VulkanSampler.h>
#include <Rendering/Abstraction/Vulkan/VulkanSemaphore.h>
#include <Rendering/Abstraction/Vulkan/VulkanShaderModule.h>
#include <Rendering/Abstraction/Vulkan/VulkanSurface.h>
#include <Rendering/Abstraction/Vulkan/VulkanSwapChain.h>

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
	RESTRICTED VulkanQueue *const RESTRICT GetComputeQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::COMPUTE);
	}

	/*
	*	Returns the graphics queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetGraphicsQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::GRAPHICS);
	}

	/*
	*	Returns the present queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetPresentQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::PRESENT);
	}

	/*
	*	Returns the transfer queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetTransferQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::TRANSFER);
	}

	/*
	*	Returns the descriptor pool.
	*/
	const VulkanDescriptorPool& GetDescriptorPool() const NOEXCEPT
	{
		return _VulkanDescriptorPool;
	}

	/*
	*	Creates and returns a 2D texture.
	*/
	RESTRICTED Vulkan2DTexture *const RESTRICT Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkImageUsageFlags image_usage_flags) NOEXCEPT;

	/*
	*	Destroys a 2D texture.
	*/
	void Destroy2DTexture(Vulkan2DTexture *const RESTRICT texture) NOEXCEPT;

	/*
	*	Creates and returns a 3D texture.
	*/
	RESTRICTED Vulkan3DTexture* const RESTRICT Create3DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureDepth, const uint32 textureChannels, const uint32 textureTexelSize, const void* RESTRICT const* RESTRICT textureData, const VkFormat format) NOEXCEPT;

	/*
	*	Creates an acceleration structure.
	*/
	RESTRICTED VulkanAccelerationStructure *const RESTRICT CreateAccelerationStructure(const VkAccelerationStructureTypeNV type, const ArrayProxy<VulkanGeometryInstance> &instances, const ArrayProxy<VkGeometryNV> &geometry) NOEXCEPT;

	/*
	*	Destroys an acceleration structure
	*/
	void DestroyAccelerationStructure(VulkanAccelerationStructure *const RESTRICT acceleration_structure) NOEXCEPT;

	/*
	*	Creates and returns a buffer.
	*/
	RESTRICTED VulkanBuffer *const RESTRICT CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperties) NOEXCEPT;

	/*
	*	Destroys a buffer.
	*/
	void DestroyBuffer(VulkanBuffer *const RESTRICT buffer) NOEXCEPT;

	/*
	*	Creates and returns a compute command pool.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateComputeCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a graphics command pool.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateGraphicsCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a transfer command pool.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateTransferCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	RESTRICTED VulkanCubeMapTexture *const RESTRICT CreateCubeMapTexture(const DynamicArray<DynamicArray<float32>> &data, const uint32 width, const uint32 height) NOEXCEPT;

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
	void DestroyDescriptorSet(VulkanDescriptorSet *const RESTRICT descriptor_set) NOEXCEPT;

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
	*	Creates and returns a compute pipeline.
	*/
	RESTRICTED VulkanComputePipeline *const RESTRICT CreateComputePipeline(const VulkanComputePipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a graphics pipeline.
	*/
	RESTRICTED VulkanGraphicsPipeline *const RESTRICT CreateGraphicsPipeline(const VulkanGraphicsPipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a ray tracing pipeline.
	*/
	RESTRICTED VulkanRayTracingPipeline *const RESTRICT CreateRayTracingPipeline(const VulkanRayTracingPipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Creates and returns a query pool.
	*/
	RESTRICTED VulkanQueryPool *const RESTRICT CreateQueryPool(const VkQueryType query_type, const uint32 query_count) NOEXCEPT;

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
	RESTRICTED VulkanSampler *const RESTRICT CreateSampler(const VkFilter magnification_filter, const VkSamplerMipmapMode mipmap_mode, const VkSamplerAddressMode address_mode, const uint8 anisotropic_samples) NOEXCEPT;

	/*
	*	Creates and returns a shader module.
	*/
	RESTRICTED VulkanShaderModule *const RESTRICT CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT;

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

	//Container for all Vulkan 3D textures.
	DynamicArray<Vulkan3DTexture * RESTRICT> _Vulkan3DTextures;

	//Container for all Vulkan acceleration structures.
	DynamicArray<VulkanAccelerationStructure *RESTRICT> _VulkanAccelerationStructures;

	//Container for all Vulkan buffers.
	DynamicArray<VulkanBuffer *RESTRICT> _VulkanBuffers;

	//The lock for all Vulkan command pools.
	Spinlock _VulkanCommandPoolsLock;

	//Container for all Vulkan command pools.
	DynamicArray<VulkanCommandPool *RESTRICT> _VulkanCommandPools;

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

	//Container for all Vulkan compute pipelines.
	DynamicArray<VulkanComputePipeline *RESTRICT> _VulkanComputePipelines;

	//Container for all Vulkan graphics pipelines.
	DynamicArray<VulkanGraphicsPipeline *RESTRICT> _VulkanGraphicsPipelines;

	//Container for all Vulkan ray tracing pipelines.
	DynamicArray<VulkanRayTracingPipeline *RESTRICT> _VulkanRayTracingPipelines;

	//Container for all Vulkan query pools.
	DynamicArray<VulkanQueryPool *RESTRICT> _VulkanQueryPools;

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

};
#endif