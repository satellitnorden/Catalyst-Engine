#pragma once

//Engine core.
#include <EngineCore.h>

//Vulkan.
#include <VulkanCore.h>

//Forward declarations.
class VulkanCommandPool;
class VulkanDescriptorSet;
class VulkanIndexBuffer;
class VulkanLogicalDevice;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanVertexBuffer;

class VulkanCommandBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	VulkanCommandBuffer() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanCommandBuffer() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying Vulkan command buffer.
	*/
	const VkCommandBuffer& Get() const CATALYST_NOEXCEPT { return vulkanCommandBuffer; }

	/*
	*	Initializes this Vulkan command buffer.
	*/
	void Initialize(const VulkanCommandPool &vulkanCommandPool) CATALYST_NOEXCEPT;

	/*
	*	Begins this Vulkan command buffer.
	*/
	void Begin(const VkCommandBufferUsageFlags commandBufferUsageFlags) CATALYST_NOEXCEPT;

	/*
	*	Records a begin render pass command.
	*/
	void CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const size_t framebufferIndex, const bool clearDepth, const uint32 numberOfClearValues) CATALYST_NOEXCEPT;

	/*
	*	Records a bind descriptor sets command.
	*/
	void CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const VulkanDescriptorSet &vulkanDescriptorSet) CATALYST_NOEXCEPT;

	/*
	*	Records a bind index buffer command.
	*/
	void CommandBindIndexBuffer(const VulkanIndexBuffer &vulkanIndexBuffer) CATALYST_NOEXCEPT;

	/*
	*	Records a bind pipeline command.
	*/
	void CommandBindPipeline(const VulkanPipeline &vulkanPipeline) CATALYST_NOEXCEPT;

	/*
	*	Records a bind vertex buffers command.
	*/
	void CommandBindVertexBuffers(const VulkanVertexBuffer &vulkanVertexBuffer) CATALYST_NOEXCEPT;

	/*
	*	Records a draw command.
	*/
	void CommandDraw(const uint32 vertexCount) CATALYST_NOEXCEPT;

	/*
	*	Records a draw indexed command.
	*/
	void CommandDrawIndexed(const uint32 indexCount) CATALYST_NOEXCEPT;

	/*
	*	Records an end render pass command.
	*/
	void CommandEndRenderPass() CATALYST_NOEXCEPT;

	/*
	*	Ends this Vulkan command buffer.
	*/
	void End() CATALYST_NOEXCEPT;

	/*
	*	Resets this Vulkan command buffer.
	*/
	void Reset() CATALYST_NOEXCEPT;

private:

	//The underlying Vulkan command buffer.
	VkCommandBuffer vulkanCommandBuffer;

	/*
	*	Creates a command buffer allocate info.
	*/
	void CreateCommandBufferAllocateInfo(VkCommandBufferAllocateInfo &commandBufferAllocateInfo, const VulkanCommandPool &vulkanCommandPool) const CATALYST_NOEXCEPT;

	/*
	*	Creates a command buffer begin info.
	*/
	void CreateCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags) const CATALYST_NOEXCEPT;

};