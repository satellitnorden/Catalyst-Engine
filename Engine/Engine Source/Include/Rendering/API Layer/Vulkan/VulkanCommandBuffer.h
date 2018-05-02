#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Vulkan.
#include <Rendering/API Layer/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanCommandPool;
class VulkanConstantBuffer;
class VulkanDescriptorSet;
class VulkanLogicalDevice;
class VulkanPipeline;
class VulkanRenderPass;

class VulkanCommandBuffer final
{

public:

	/*
	*	Returns the underlying Vulkan command buffer.
	*/
	const VkCommandBuffer& Get() const NOEXCEPT { return vulkanCommandBuffer; }

	/*
	*	Initializes this Vulkan command buffer.
	*/
	void Initialize(const VulkanCommandPool &vulkanCommandPool, const VkCommandBufferLevel level) NOEXCEPT;

	/*
	*	Begins this Vulkan command buffer as a primary command buffer.
	*/
	void BeginPrimary(const VkCommandBufferUsageFlags commandBufferUsageFlags) NOEXCEPT;

	/*
	*	Begins this Vulkan command buffer as a secondary command buffer.
	*/
	void BeginSecondary(const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkRenderPass renderPass, const VkFramebuffer framebuffer) NOEXCEPT;

	/*
	*	Records a begin render pass command.
	*/
	void CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex, const VkExtent2D renderArea, const VkSubpassContents contents) NOEXCEPT;

	/*
	*	Records a begin render pass command and clears.
	*/
	template <uint32 NumberOfClearValues>
	void CommandBeginRenderPassAndClear(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex, const VkExtent2D renderArea, const VkSubpassContents contents) NOEXCEPT;

	/*
	*	Records a bind descriptor sets command.
	*/
	void CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const uint32 descriptorSetCount, const VulkanDescriptorSet *RESTRICT descriptorSets) NOEXCEPT;

	/*
	*	Records a bind index buffer command.
	*/
	void CommandBindIndexBuffer(const VulkanConstantBuffer &vulkanIndexBuffer, const VkDeviceSize offset = 0) NOEXCEPT;

	/*
	*	Records a bind pipeline command.
	*/
	void CommandBindPipeline(const VulkanPipeline &vulkanPipeline) NOEXCEPT;

	/*
	*	Records a bind vertex buffers command.
	*/
	void CommandBindVertexBuffers(const uint32 vertexBufferCount, const VkBuffer *RESTRICT vertexBuffers, const uint64 *RESTRICT offsets) NOEXCEPT;

	/*
	*	Records a draw command.
	*/
	void CommandDraw(const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT;

	/*
	*	Records a draw indexed command.
	*/
	void CommandDrawIndexed(const uint32 indexCount, const uint32 instanceCount) NOEXCEPT;

	/*
	*	Records an end render pass command.
	*/
	void CommandEndRenderPass() NOEXCEPT;

	/*
	*	Records an execute commands command.
	*/
	void CommandExecuteCommands(const VkCommandBuffer commandBuffer) NOEXCEPT;

	/*
	*	Records a push constants command.
	*/
	void CommandPushConstants(const VkPipelineLayout layout, const VkShaderStageFlags stageFlags, const uint32 offset, const uint32 size, const void *RESTRICT data) NOEXCEPT;

	/*
	*	Records a reset event command.
	*/
	void CommandResetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT;

	/*
	*	Records a set event command.
	*/
	void CommandSetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT;

	/*
	*	Records a wait events command.
	*/
	void CommandWaitEvents(const uint32 eventCount, const VkEvent *const RESTRICT events, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask) NOEXCEPT;

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
	void CreateCommandBufferAllocateInfo(VkCommandBufferAllocateInfo &commandBufferAllocateInfo, const VulkanCommandPool &vulkanCommandPool, const VkCommandBufferLevel level) const NOEXCEPT;

	/*
	*	Creates a primary command buffer begin info.
	*/
	void CreatePrimaryCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags) const NOEXCEPT;

	/*
	*	Creates a command buffer inheritance info.
	*/
	void CreateCommandBufferInheritanceInfo(VkCommandBufferInheritanceInfo &commandBufferInheritanceInfo, const VkRenderPass renderPass, const VkFramebuffer framebuffer) const NOEXCEPT;

	/*
	*	Creates a secondary command buffer begin info.
	*/
	void CreateSecondaryCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkCommandBufferInheritanceInfo *const RESTRICT inheritanceInfo) const NOEXCEPT;

};

//Templated function definitions.
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.inl>