#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <Vertex.h>

//Vulkan.
#include <VulkanCommandBuffer.h>
#include <VulkanCommandPool.h>
#include <VulkanCore.h>
#include <VulkanDepthBuffer.h>
#include <VulkanDescriptorPool.h>
#include <VulkanDescriptorSet.h>
#include <VulkanDescriptorSetLayout.h>
#include <VulkanFence.h>
#include <VulkanFrameBuffer.h>
#include <VulkanIndexBuffer.h>
#include <VulkanInstance.h>
#include <VulkanLogicalDevice.h>
#include <VulkanPhysicalDevice.h>
#include <VulkanPipeline.h>
#include <VulkanPipelineCreationParameters.h>
#include <VulkanQueue.h>
#include <VulkanRenderPass.h>
#include <VulkanSemaphore.h>
#include <VulkanShaderModule.h>
#include <VulkanSurface.h>
#include <VulkanSwapChain.h>
#include <VulkanTexture.h>
#include <VulkanUniformBuffer.h>
#include <VulkanVertexBuffer.h>

//Forward declarations.
class Window;

class VulkanInterface
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VulkanInterface);

	/*
	*	Default constructor.
	*/
	VulkanInterface() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanInterface() CATALYST_NOEXCEPT;

	/*
	*	Initializes this Vulkan interface.
	*/
	void Initialize(Window &window) CATALYST_NOEXCEPT;

	/*
	*	Pre-updates this Vulkan interface.
	*/
	void PreUpdate(const VulkanSemaphore *const CATALYST_RESTRICT imageAvailableSemaphore) CATALYST_NOEXCEPT;

	/*
	*	Post-updates this Vulkan interface.
	*/
	void PostUpdate(const VulkanSemaphore *const CATALYST_RESTRICT renderFinishedSemaphore) CATALYST_NOEXCEPT;

	/*
	*	Releases this Vulkan interface.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns the Vulkan instance.
	*/
	CATALYST_FORCE_INLINE const VulkanInstance& GetVulkanInstance() const CATALYST_NOEXCEPT { return vulkanInstance; }

	/*
	*	Returns the Vulkan surface.
	*/
	CATALYST_FORCE_INLINE const VulkanSurface& GetVulkanSurface() const CATALYST_NOEXCEPT { return vulkanSurface; }

	/*
	*	Returns the Vulkan physical device.
	*/
	CATALYST_FORCE_INLINE const VulkanPhysicalDevice& GetVulkanPhysicalDevice() const CATALYST_NOEXCEPT { return vulkanPhysicalDevice; }

	/*
	*	Returns the Vulkan logical device.
	*/
	CATALYST_FORCE_INLINE const VulkanLogicalDevice& GetVulkanLogicalDevice() const CATALYST_NOEXCEPT { return vulkanLogicalDevice; }

	/*
	*	Returns the Vulkan swapchain.
	*/
	CATALYST_FORCE_INLINE const VulkanSwapchain& GetVulkanSwapchain() const CATALYST_NOEXCEPT { return vulkanSwapchain; }

	/*
	*	Returns the graphics Vulkan queue.
	*/
	CATALYST_FORCE_INLINE const VulkanQueue& GetGraphicsVulkanQueue() const CATALYST_NOEXCEPT { return vulkanLogicalDevice.GetGraphicsQueue(); }

	/*
	*	Returns the present Vulkan queue.
	*/
	CATALYST_FORCE_INLINE const VulkanQueue& GetPresentVulkanQueue() const CATALYST_NOEXCEPT { return vulkanLogicalDevice.GetPresentQueue(); }

	/*
	*	Returns the transfer Vulkan queue.
	*/
	CATALYST_FORCE_INLINE const VulkanQueue& GetTransferVulkanQueue() const CATALYST_NOEXCEPT { return vulkanLogicalDevice.GetTransferQueue(); }

	/*
	*	Returns the graphics Vulkan command pool.
	*/
	CATALYST_FORCE_INLINE const VulkanCommandPool& GetGraphicsVulkanCommandPool() const CATALYST_NOEXCEPT { return graphicsVulkanCommandPool; }

	/*
	*	Returns the transfer Vulkan command pool.
	*/
	CATALYST_FORCE_INLINE const VulkanCommandPool& GetTransferVulkanCommandPool() const CATALYST_NOEXCEPT { return transferVulkanCommandPool; }

	/*
	*	Returns the Vulkan descriptor pool.
	*/
	CATALYST_FORCE_INLINE const VulkanDescriptorPool& GetVulkanDescriptorPool() const CATALYST_NOEXCEPT { return vulkanDescriptorPool; }

	/*
	*	Creates and returns a depth buffer.
	*/
	CATALYST_RESTRICTED VulkanDepthBuffer* CreateDepthBuffer(const VkExtent2D &depthBufferExtent) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a fence.
	*/
	CATALYST_RESTRICTED VulkanFence* CreateFence(const VkFenceCreateFlags flags = 0) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns an index buffer.
	*/
	CATALYST_RESTRICTED VulkanIndexBuffer* CreateIndexBuffer(const DynamicArray<uint32> &indices) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a pipeline.
	*/
	CATALYST_RESTRICTED VulkanPipeline* CreatePipeline(const VulkanPipelineCreationParameters &vulkanPipelineCreationParameters) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a semaphore.
	*/
	CATALYST_RESTRICTED VulkanSemaphore* CreateSemaphore() CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a shader module.
	*/
	CATALYST_RESTRICTED VulkanShaderModule* CreateShaderModule(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits stage) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a texture.
	*/
	CATALYST_RESTRICTED VulkanTexture* CreateTexture(const uint32 width, const uint32 height, const byte *textureData) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	CATALYST_RESTRICTED VulkanUniformBuffer* CreateUniformBuffer(const size_t newUniformBufferSize) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a vertex buffer.
	*/
	CATALYST_RESTRICTED VulkanVertexBuffer* CreateVertexBuffer(const DynamicArray<Vertex> &vertices) CATALYST_NOEXCEPT;

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

	//The graphics Vulkan command pool.
	VulkanCommandPool graphicsVulkanCommandPool;

	//The transfer Vulkan command pool.
	VulkanCommandPool transferVulkanCommandPool;

	//The Vulkan descriptor pool.
	VulkanDescriptorPool vulkanDescriptorPool;

	//Container for all Vulkan depth buffers.
	DynamicArray<VulkanDepthBuffer *CATALYST_RESTRICT> vulkanDepthBuffers;

	//Container for all Vulkan fences.
	DynamicArray<VulkanFence *CATALYST_RESTRICT> vulkanFences;

	//Container for all Vulkan index buffers.
	DynamicArray<VulkanIndexBuffer *CATALYST_RESTRICT> vulkanIndexBuffers;

	//Container for all Vulkan pipelines.
	DynamicArray<VulkanPipeline *CATALYST_RESTRICT> vulkanPipelines;

	//Container for all Vulkan semaphores.
	DynamicArray<VulkanSemaphore *CATALYST_RESTRICT> vulkanSemaphores;

	//Container for all Vulkan shader modules.
	DynamicArray<VulkanShaderModule *CATALYST_RESTRICT> vulkanShaderModules;

	//Container for all Vulkan textures.
	DynamicArray<VulkanTexture *CATALYST_RESTRICT> vulkanTextures;

	//Container for all Vulkan uniform buffers.
	DynamicArray<VulkanUniformBuffer *CATALYST_RESTRICT> vulkanUniformBuffers;

	//Container for all Vulkan vertex buffers.
	DynamicArray<VulkanVertexBuffer *CATALYST_RESTRICT> vulkanVertexBuffers;

};