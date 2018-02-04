#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanBuffer;
class VulkanCommandPool;
class VulkanDescriptorSet;
class VulkanLogicalDevice;
class VulkanPipeline;
class VulkanRenderPass;

class VulkanCommandBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanCommandBuffer() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanCommandBuffer() NOEXCEPT;

	/*
	*	Returns the underlying Vulkan command buffer.
	*/
	const VkCommandBuffer& Get() const NOEXCEPT { return vulkanCommandBuffer; }

	/*
	*	Initializes this Vulkan command buffer.
	*/
	void Initialize(const VulkanCommandPool &vulkanCommandPool) NOEXCEPT;

	/*
	*	Begins this Vulkan command buffer.
	*/
	void Begin(const VkCommandBufferUsageFlags commandBufferUsageFlags) NOEXCEPT;

	/*
	*	Records a begin render pass command.
	*/
	void CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const size_t framebufferIndex, const bool clearDepth, const uint32 numberOfClearValues) NOEXCEPT;

	/*
	*	Records a bind descriptor sets command.
	*/
	void CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const VulkanDescriptorSet &vulkanDescriptorSet) NOEXCEPT;

	/*
	*	Records a bind index buffer command.
	*/
	void CommandBindIndexBuffer(const VulkanBuffer &vulkanIndexBuffer, const VkDeviceSize offset = 0) NOEXCEPT;

	/*
	*	Records a bind pipeline command.
	*/
	void CommandBindPipeline(const VulkanPipeline &vulkanPipeline) NOEXCEPT;

	/*
	*	Records a bind vertex buffers command.
	*/
	void CommandBindVertexBuffers(const VulkanBuffer &vulkanVertexBuffer, const VkDeviceSize offset = 0) NOEXCEPT;

	/*
	*	Records a draw command.
	*/
	void CommandDraw(const uint32 vertexCount) NOEXCEPT;

	/*
	*	Records a draw indexed command.
	*/
	void CommandDrawIndexed(const uint32 indexCount) NOEXCEPT;

	/*
	*	Records an end render pass command.
	*/
	void CommandEndRenderPass() NOEXCEPT;

	/*
	*	Ends this Vulkan command buffer.
	*/
	void End() NOEXCEPT;

	/*
	*	Resets this Vulkan command buffer.
	*/
	void Reset() NOEXCEPT;

private:

	//The underlying Vulkan command buffer.
	VkCommandBuffer vulkanCommandBuffer;

	/*
	*	Creates a command buffer allocate info.
	*/
	void CreateCommandBufferAllocateInfo(VkCommandBufferAllocateInfo &commandBufferAllocateInfo, const VulkanCommandPool &vulkanCommandPool) const NOEXCEPT;

	/*
	*	Creates a command buffer begin info.
	*/
	void CreateCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags) const NOEXCEPT;

};