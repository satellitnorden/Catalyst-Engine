#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <Vertex.h>

//Vulkan.
#include <Vulkan2DTexture.h>
#include <VulkanCommandBuffer.h>
#include <VulkanCommandPool.h>
#include <VulkanCore.h>
#include <VulkanCubeMapTexture.h>
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
#include <VulkanRenderTarget.h>
#include <VulkanSemaphore.h>
#include <VulkanShaderModule.h>
#include <VulkanSurface.h>
#include <VulkanSwapChain.h>
#include <VulkanUniformBuffer.h>
#include <VulkanVertexBuffer.h>

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
	*	Returns the instance.
	*/
	const VulkanInstance& GetInstance() const CATALYST_NOEXCEPT { return vulkanInstance; }

	/*
	*	Returns the surface.
	*/
	const VulkanSurface& GetSurface() const CATALYST_NOEXCEPT { return vulkanSurface; }

	/*
	*	Returns the physical device.
	*/
	const VulkanPhysicalDevice& GetPhysicalDevice() const CATALYST_NOEXCEPT { return vulkanPhysicalDevice; }

	/*
	*	Returns the logical device.
	*/
	const VulkanLogicalDevice& GetLogicalDevice() const CATALYST_NOEXCEPT { return vulkanLogicalDevice; }

	/*
	*	Returns the swapchain.
	*/
	const VulkanSwapchain& GetSwapchain() const CATALYST_NOEXCEPT { return vulkanSwapchain; }

	/*
	*	Returns the graphics queue.
	*/
	const VulkanQueue& GetGraphicsQueue() const CATALYST_NOEXCEPT { return queues[static_cast<uint8>(Queue::Graphics)]; }

	/*
	*	Returns the present queue.
	*/
	const VulkanQueue& GetPresentQueue() const CATALYST_NOEXCEPT { return queues[static_cast<uint8>(Queue::Present)]; }

	/*
	*	Returns the transfer queue.
	*/
	const VulkanQueue& GetTransferQueue() const CATALYST_NOEXCEPT { return queues[static_cast<uint8>(Queue::Transfer)]; }

	/*
	*	Returns the graphics command pool.
	*/
	const VulkanCommandPool& GetGraphicsCommandPool() const CATALYST_NOEXCEPT { return graphicsVulkanCommandPool; }

	/*
	*	Returns the transfer command pool.
	*/
	const VulkanCommandPool& GetTransferCommandPool() const CATALYST_NOEXCEPT { return transferVulkanCommandPool; }

	/*
	*	Returns the descriptor pool.
	*/
	const VulkanDescriptorPool& GetDescriptorPool() const CATALYST_NOEXCEPT { return vulkanDescriptorPool; }

	/*
	*	Creates and returns a 2D texture.
	*/
	CATALYST_RESTRICTED Vulkan2DTexture* Create2DTexture(const uint32 width, const uint32 height, const byte *CATALYST_RESTRICT textureData) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a cube map texture.
	*/
	CATALYST_RESTRICTED VulkanCubeMapTexture* CreateCubeMapTexture(const uint32 width, const uint32 height, const byte *CATALYST_RESTRICT *CATALYST_RESTRICT textureData) CATALYST_NOEXCEPT;

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
	*	Creates and returns a render target.
	*/
	CATALYST_RESTRICTED VulkanRenderTarget* CreateRenderTarget(const VkExtent2D extent) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a semaphore.
	*/
	CATALYST_RESTRICTED VulkanSemaphore* CreateSemaphore() CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a shader module.
	*/
	CATALYST_RESTRICTED VulkanShaderModule* CreateShaderModule(const DynamicArray<char> &shaderByteCode, const VkShaderStageFlagBits stage) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a uniform buffer.
	*/
	CATALYST_RESTRICTED VulkanUniformBuffer* CreateUniformBuffer(const size_t newUniformBufferSize) CATALYST_NOEXCEPT;

	/*
	*	Creates and returns a vertex buffer.
	*/
	CATALYST_RESTRICTED VulkanVertexBuffer* CreateVertexBuffer(const DynamicArray<Vertex> &vertices) CATALYST_NOEXCEPT;

private:

	//Enumeration covering all queues.
	enum class Queue : uint8
	{
		Graphics,
		Present,
		Transfer,
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

	//The graphics Vulkan command pool.
	VulkanCommandPool graphicsVulkanCommandPool;

	//The transfer Vulkan command pool.
	VulkanCommandPool transferVulkanCommandPool;

	//The Vulkan descriptor pool.
	VulkanDescriptorPool vulkanDescriptorPool;

	//Container for all queues.
	VulkanQueue queues[static_cast<uint8>(Queue::NumberOfQueues)];

	//Container for all Vulkan 2D textures.
	DynamicArray<Vulkan2DTexture *CATALYST_RESTRICT> vulkan2DTextures;

	//Container for all Vulkan cube map textures.
	DynamicArray<VulkanCubeMapTexture *CATALYST_RESTRICT> vulkanCubeMapTextures;

	//Container for all Vulkan depth buffers.
	DynamicArray<VulkanDepthBuffer *CATALYST_RESTRICT> vulkanDepthBuffers;

	//Container for all Vulkan fences.
	DynamicArray<VulkanFence *CATALYST_RESTRICT> vulkanFences;

	//Container for all Vulkan index buffers.
	DynamicArray<VulkanIndexBuffer *CATALYST_RESTRICT> vulkanIndexBuffers;

	//Container for all Vulkan pipelines.
	DynamicArray<VulkanPipeline *CATALYST_RESTRICT> vulkanPipelines;

	//Container for all Vulkan render targets.
	DynamicArray<VulkanRenderTarget *CATALYST_RESTRICT> vulkanRenderTargets;

	//Container for all Vulkan semaphores.
	DynamicArray<VulkanSemaphore *CATALYST_RESTRICT> vulkanSemaphores;

	//Container for all Vulkan shader modules.
	DynamicArray<VulkanShaderModule *CATALYST_RESTRICT> vulkanShaderModules;

	//Container for all Vulkan uniform buffers.
	DynamicArray<VulkanUniformBuffer *CATALYST_RESTRICT> vulkanUniformBuffers;

	//Container for all Vulkan vertex buffers.
	DynamicArray<VulkanVertexBuffer *CATALYST_RESTRICT> vulkanVertexBuffers;

};