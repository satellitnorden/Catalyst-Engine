#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/Vulkan2DTexture.h>
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanCommandPool.h>
#include <Rendering/API Layer/Vulkan/VulkanConstantBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanCore.h>
#include <Rendering/API Layer/Vulkan/VulkanCubeMapTexture.h>
#include <Rendering/API Layer/Vulkan/VulkanDepthBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorPool.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSet.h>
#include <Rendering/API Layer/Vulkan/VulkanDescriptorSetLayout.h>
#include <Rendering/API Layer/Vulkan/VulkanEvent.h>
#include <Rendering/API Layer/Vulkan/VulkanFence.h>
#include <Rendering/API Layer/Vulkan/VulkanFrameBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanInstance.h>
#include <Rendering/API Layer/Vulkan/VulkanLogicalDevice.h>
#include <Rendering/API Layer/Vulkan/VulkanPhysicalDevice.h>
#include <Rendering/API Layer/Vulkan/VulkanPipeline.h>
#include <Rendering/API Layer/Vulkan/VulkanPipelineCreationParameters.h>
#include <Rendering/API Layer/Vulkan/VulkanQueue.h>
#include <Rendering/API Layer/Vulkan/VulkanRenderPass.h>
#include <Rendering/API Layer/Vulkan/VulkanRenderPassCreationParameters.h>
#include <Rendering/API Layer/Vulkan/VulkanRenderTarget.h>
#include <Rendering/API Layer/Vulkan/VulkanSemaphore.h>
#include <Rendering/API Layer/Vulkan/VulkanShaderModule.h>
#include <Rendering/API Layer/Vulkan/VulkanStorageBuffer.h>
#include <Rendering/API Layer/Vulkan/VulkanSurface.h>
#include <Rendering/API Layer/Vulkan/VulkanSwapChain.h>
#include <Rendering/API Layer/Vulkan/VulkanUniformBuffer.h>

//Forward declarations.
class Window;

class VulkanInterface final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VulkanInterface);

	/*
	*	Default constructor.
	*/
	VulkanInterface() NOEXCEPT = default;

	/*
	*	Initializes this Vulkan interface.
	*/
	void Initialize(Window &window) NOEXCEPT;

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
	*	Returns the graphics queue.
	*/
	const VulkanQueue& GetGraphicsQueue() const NOEXCEPT { return queues[INDEX(Queue::Graphics)]; }

	/*
	*	Returns the present queue.
	*/
#if RENDERDOC_DEBUGGING
	const VulkanQueue& GetPresentQueue() const NOEXCEPT { return queues[INDEX(Queue::Graphics)]; }
#else
	const VulkanQueue& GetPresentQueue() const NOEXCEPT { return queues[INDEX(Queue::Present)]; }
#endif

	/*
	*	Returns the descriptor pool.
	*/
	const VulkanDescriptorPool& GetDescriptorPool() const NOEXCEPT { return vulkanDescriptorPool; }

	/*
	*	Creates and returns a 2D texture with void data.
	*/
	RESTRICTED Vulkan2DTexture* Create2DTexture(const uint32 textureMipmapLevels, const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const uint32 textureTexelSize, const void *RESTRICT const *RESTRICT textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT;

	/*
	*	Creates and returns a 2D texture with byte data.
	*/
	RESTRICTED Vulkan2DTexture* Create2DTexture(const uint32 textureWidth, const uint32 textureHeight, const uint32 textureChannels, const DynamicArray<DynamicArray<byte>> &textureData, const VkFormat format, const VkFilter magnificationFilter, const VkSamplerMipmapMode mipmapMode, const VkSamplerAddressMode addressMode) NOEXCEPT;

	/*
	*	Creates and returns a graphics command pool.
	*/
	RESTRICTED VulkanCommandPool* CreateGraphicsCommandPool(const VkCommandPoolCreateFlags flags) NOEXCEPT;

	/*
	*	Creates and returns a constant buffer.
	*/
	RESTRICTED VulkanConstantBuffer* CreateConstantBuffer(const void *RESTRICT data[], const VkDeviceSize *dataSizes, const uint32 dataChunks) NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	RESTRICTED VulkanCubeMapTexture* CreateCubeMapTexture(const float *const RESTRICT data, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Creates and returns a depth buffer.
	*/
	RESTRICTED VulkanDepthBuffer* CreateDepthBuffer(const VkExtent2D &depthBufferExtent) NOEXCEPT;

	/*
	*	Creates and returns an event.
	*/
	RESTRICTED VulkanEvent* CreateEvent() NOEXCEPT;

	/*
	*	Creates and returns a fence.
	*/
	RESTRICTED VulkanFence* CreateFence(const VkFenceCreateFlags flags = 0) NOEXCEPT;

	/*
	*	Creates and returns a pipeline.
	*/
	RESTRICTED VulkanPipeline* CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) NOEXCEPT;

	/*
	*	Creates and returns a render target.
	*/
	RESTRICTED VulkanRenderTarget* CreateRenderTarget(const VkExtent2D extent, const VkSamplerAddressMode addressMode) NOEXCEPT;

	/*
	*	Creates and returns a semaphore.
	*/
	RESTRICTED VulkanSemaphore* CreateSemaphore() NOEXCEPT;

	/*
	*	Creates and returns a shader module.
	*/
	RESTRICTED VulkanShaderModule* CreateShaderModule(const void* const shaderData, const uint64 shaderDataSize, const VkShaderStageFlagBits stage) NOEXCEPT;

	/*
	*	Creates and returns a storage buffer.
	*/
	RESTRICTED VulkanStorageBuffer* CreateStorageBuffer(const VkDeviceSize initialStorageBufferSize) NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	RESTRICTED VulkanUniformBuffer* CreateUniformBuffer(const uint64 newUniformBufferSize) NOEXCEPT;

	/*
	*	Returns the container for all Vulkan 2D textures.
	*/
	const DynamicArray<Vulkan2DTexture *RESTRICT>& Get2DTextures() const NOEXCEPT { return vulkan2DTextures; }

private:

	//Enumeration covering all queues.
	enum class Queue : uint8
	{
		Graphics,
		Present,
		NumberOfQueues
	};

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

	//Container for all queues.
	VulkanQueue queues[INDEX(Queue::NumberOfQueues)];

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

	//Container for all Vulkan events.
	DynamicArray<VulkanEvent *RESTRICT> vulkanEvents;

	//Container for all Vulkan fences.
	DynamicArray<VulkanFence *RESTRICT> vulkanFences;

	//Container for all Vulkan pipelines.
	DynamicArray<VulkanPipeline *RESTRICT> vulkanPipelines;

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