#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurreny.
#include <Concurrency/Spinlock.h>

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

class ALIGN(8) VulkanInterface final
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
	VulkanSurface& GetSurface() NOEXCEPT { return _VulkanSurface; }

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
	VulkanSwapchain& GetSwapchain() NOEXCEPT { return _VulkanSwapchain; }

	/*
	*	Returns the "main" queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetMainQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::MAIN);
	}

	/*
	*	Returns the "async compute" queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetAsyncComputeQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::ASYNC_COMPUTE);
	}

	/*
	*	Returns the "async transfer" queue.
	*/
	RESTRICTED VulkanQueue *const RESTRICT GetAsyncTransferQueue() NOEXCEPT
	{
		return _VulkanLogicalDevice.GetQueue(VulkanLogicalDevice::QueueType::ASYNC_TRANSFER);
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
	RESTRICTED Vulkan2DTexture *const RESTRICT Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkImageUsageFlags image_usage_flags, const bool updatable) NOEXCEPT;

	/*
	*	Updates a 2D texture.
	*/
	void Update2DTexture(const void *RESTRICT const *RESTRICT texture_data, Vulkan2DTexture *const RESTRICT texture) NOEXCEPT;

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
	RESTRICTED VulkanAccelerationStructure *const RESTRICT CreateAccelerationStructure
	(
		const VkAccelerationStructureTypeNV type,
		const ArrayProxy<VulkanGeometryInstance> &instances,
		const ArrayProxy<VkGeometryNV> &geometry,
		VulkanCommandBuffer *const RESTRICT command_buffer
	) NOEXCEPT;

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
	*	Creates and returns a command pool from the "main" queue.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateMainCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a command pool from the "async compute" queue.
	*/
	RESTRICTED VulkanCommandPool *const RESTRICT CreateAsyncComputeCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a command pool from the "async transfer" queue.
	*/
	RESTRICTED VulkanCommandPool* const RESTRICT CreateAsyncTransferCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	RESTRICTED VulkanCubeMapTexture *const RESTRICT CreateCubeMapTexture(const DynamicArray<DynamicArray<float32>> &data, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Creates and returns a depth buffer.
	*/
	RESTRICTED VulkanDepthBuffer *const RESTRICT CreateDepthBuffer(const VkExtent2D &extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT;

	/*
	*	Destroys a depth buffer.
	*/
	void DestroyDepthBuffer(VulkanDepthBuffer *const RESTRICT depth_buffer) NOEXCEPT;

	/*
	*	Creates and returns a descriptor set layout.
	*/
	RESTRICTED VulkanDescriptorSetLayout *const RESTRICT CreateDescriptorSetLayout(const VkDescriptorSetLayoutBinding *RESTRICT descriptorSetLayoutBindings, const uint32 numberOfDescriptorSetLayoutBindings) NOEXCEPT;

	/*
	*	Destroys a descriptor set layout.
	*/
	void DestroyDescriptorSetLayout(VulkanDescriptorSetLayout *const RESTRICT descriptor_set_layout) NOEXCEPT;

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
	*	Destroys a framebuffer.
	*/
	void DestroyFramebuffer(VulkanFramebuffer *const RESTRICT framebuffer) NOEXCEPT;

	/*
	*	Creates and returns a compute pipeline.
	*/
	RESTRICTED VulkanComputePipeline *const RESTRICT CreateComputePipeline(const VulkanComputePipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Destroys a compute pipeline.
	*/
	void DestroyComputePipeline(VulkanComputePipeline *const RESTRICT pipeline) NOEXCEPT;

	/*
	*	Creates and returns a graphics pipeline.
	*/
	RESTRICTED VulkanGraphicsPipeline *const RESTRICT CreateGraphicsPipeline(const VulkanGraphicsPipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Destroys a graphics pipeline.
	*/
	void DestroyGraphicsPipeline(VulkanGraphicsPipeline *const RESTRICT pipeline) NOEXCEPT;

	/*
	*	Creates and returns a ray tracing pipeline.
	*/
	RESTRICTED VulkanRayTracingPipeline *const RESTRICT CreateRayTracingPipeline(const VulkanRayTracingPipelineCreationParameters &parameters) NOEXCEPT;

	/*
	*	Destroys a ray tracing pipeline.
	*/
	void DestroyRayTracingPipeline(VulkanRayTracingPipeline *const RESTRICT pipeline) NOEXCEPT;

	/*
	*	Creates and returns a query pool.
	*/
	RESTRICTED VulkanQueryPool *const RESTRICT CreateQueryPool(const VkQueryType query_type, const uint32 query_count) NOEXCEPT;

	/*
	*	Creates and returns a render pass.
	*/
	RESTRICTED VulkanRenderPass *const RESTRICT CreateRenderPass(const VulkanRenderPassCreationParameters &parameters) NOEXCEPT;

	/*
	*	Destroys a render pass.
	*/
	void DestroyRenderPass(VulkanRenderPass *const RESTRICT render_pass) NOEXCEPT;

	/*
	*	Creates and returns a render target.
	*/
	RESTRICTED VulkanRenderTarget *const RESTRICT CreateRenderTarget(const VkExtent2D extent, const VkFormat format, const VkSampleCountFlagBits sample_count) NOEXCEPT;

	/*
	*	Destroys a render target.
	*/
	void DestroyRenderTarget(VulkanRenderTarget *const RESTRICT render_target) NOEXCEPT;

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

	//The Vulkan 2D textures lock.
	Spinlock _Vulkan2DTexturesLock;

	//Container for all Vulkan 3D textures.
	DynamicArray<Vulkan3DTexture * RESTRICT> _Vulkan3DTextures;

	//The Vulkan 3D textures lock.
	Spinlock _Vulkan3DTexturesLock;

	//Container for all Vulkan acceleration structures.
	DynamicArray<VulkanAccelerationStructure *RESTRICT> _VulkanAccelerationStructures;

	//The Vulkan acceleration structures lock.
	Spinlock _VulkanAccelerationStructuresLock;

	//Container for all Vulkan buffers.
	DynamicArray<VulkanBuffer *RESTRICT> _VulkanBuffers;

	//The Vulkan buffers lock.
	Spinlock _VulkanBuffersLock;

	//Container for all Vulkan command pools.
	DynamicArray<VulkanCommandPool *RESTRICT> _VulkanCommandPools;

	//The Vulkan command pools lock.
	Spinlock _VulkanCommandPoolsLock;

	//Container for all Vulkan cube map textures.
	DynamicArray<VulkanCubeMapTexture *RESTRICT> _VulkanCubeMapTextures;

	//The Vulkan cube map textures lock.
	Spinlock _VulkanCubeMapTexturesLock;

	//Container for all Vulkan depth buffers.
	DynamicArray<VulkanDepthBuffer *RESTRICT> _VulkanDepthBuffers;

	//The Vulkan depth buffers lock.
	Spinlock _VulkanDepthBuffersLock;

	//Container for all Vulkan descriptor set layouts.
	DynamicArray<VulkanDescriptorSetLayout *RESTRICT> _VulkanDescriptorSetLayouts;

	//The Vulkan descriptor set layouts lock.
	Spinlock _VulkanDescriptorSetLayoutsLock;

	//Container for all Vulkan descriptor sets.
	DynamicArray<VulkanDescriptorSet *RESTRICT> _VulkanDescriptorSets;

	//The Vulkan descriptor sets lock.
	Spinlock _VulkanDescriptorSetsLock;

	//Container for all Vulkan events.
	DynamicArray<VulkanEvent *RESTRICT> _VulkanEvents;

	//The Vulkan events lock.
	Spinlock _VulkanEventsLock;

	//Container for all Vulkan fences.
	DynamicArray<VulkanFence *RESTRICT> _VulkanFences;

	//The Vulkan fences lock.
	Spinlock _VulkanFencesLock;

	//Container for all Vulkan framebuffers.
	DynamicArray<VulkanFramebuffer *RESTRICT> _VulkanFramebuffers;

	//The Vulkan framebuffers lock.
	Spinlock _VulkanFramebuffersLock;

	//Container for all Vulkan compute pipelines.
	DynamicArray<VulkanComputePipeline *RESTRICT> _VulkanComputePipelines;

	//The Vulkan compute pipelines lock.
	Spinlock _VulkanComputePipelinesLock;

	//Container for all Vulkan graphics pipelines.
	DynamicArray<VulkanGraphicsPipeline *RESTRICT> _VulkanGraphicsPipelines;

	//The Vulkan graphics pipelines lock.
	Spinlock _VulkanGraphicsPipelinesLock;

	//Container for all Vulkan ray tracing pipelines.
	DynamicArray<VulkanRayTracingPipeline *RESTRICT> _VulkanRayTracingPipelines;

	//The Vulkan ray tracing pipelines lock.
	Spinlock _VulkanRayTracingPipelinesLock;

	//Container for all Vulkan query pools.
	DynamicArray<VulkanQueryPool *RESTRICT> _VulkanQueryPools;

	//The Vulkan query pools lock.
	Spinlock _VulkanQueryPoolsLock;

	//Container for all Vulkan render passes.
	DynamicArray<VulkanRenderPass *RESTRICT> _VulkanRenderPasses;

	//The Vulkan render passes lock.
	Spinlock _VulkanRenderPassesLock;

	//Container for all Vulkan render targets.
	DynamicArray<VulkanRenderTarget *RESTRICT> _VulkanRenderTargets;

	//The Vulkan render targets lock.
	Spinlock _VulkanRenderTargetsLock;

	//Container for all Vulkan semaphores.
	DynamicArray<VulkanSemaphore *RESTRICT> _VulkanSemaphores;

	//The Vulkan semaphores lock.
	Spinlock _VulkanSemaphoresLock;

	//Container for all Vulkan samplers.
	DynamicArray<VulkanSampler *RESTRICT> _VulkanSamplers;

	//The Vulkan samplers lock.
	Spinlock _VulkanSamplersLock;

	//Container for all Vulkan shader modules.
	DynamicArray<VulkanShaderModule *RESTRICT> _VulkanShaderModules;

	//The Vulkan shader modules lock.
	Spinlock _VulkanShaderModulesLock;

	/*
	*	Create the Vulkan memory allocator.
	*/
	void CreateVulkanMemoryAllocator() NOEXCEPT;

	/*
	*	Destroys the Vulkan memory allocator.
	*/
	void DestroyVulkanMemoryAllocator() NOEXCEPT;


};
#endif