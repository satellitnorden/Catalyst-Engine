#if defined(CATALYST_RENDERING_VULKAN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Vulkan.
#include <Rendering/Abstraction/Vulkan/VulkanCore.h>

//Forward declarations.
class VulkanCommandPool;

class VulkanCommandBuffer final
{

public:

	/*
	*	Returns the underlying Vulkan command buffer.
	*/
	const VkCommandBuffer& Get() const NOEXCEPT { return _VulkanCommandBuffer; }

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
	void BeginSecondary(const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkRenderPass renderPass, const uint32 subpass, const VkFramebuffer framebuffer) NOEXCEPT;

	/*
	*	Records a begin render pass command.
	*/
	void CommandBeginRenderPass(const VkRenderPass renderPass, const VkFramebuffer framebuffer, const VkExtent2D renderArea, const VkSubpassContents contents) NOEXCEPT;

	/*
	*	Records a begin render pass command and clears.
	*/
	void CommandBeginRenderPassAndClear(const Vector4<float> &color, const float depth, const VkRenderPass renderPass, const VkFramebuffer framebuffer, const VkExtent2D renderArea, const VkSubpassContents contents, const uint32 numberOfClearValues) NOEXCEPT;

	/*
	*	Records a bind descriptor sets command.
	*/
	void CommandBindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const uint32 firstBinding, const uint32 descriptorSetCount, const VkDescriptorSet *RESTRICT descriptorSets) NOEXCEPT;

	/*
	*	Records a bind index buffer command.
	*/
	void CommandBindIndexBuffer(const VkBuffer vulkanIndexBuffer, const VkDeviceSize offset = 0) NOEXCEPT;

	/*
	*	Records a bind pipeline command.
	*/
	void CommandBindPipeline(const VkPipelineBindPoint pipelineBindPoint, const VkPipeline vulkanPipeline) NOEXCEPT;

	/*
	*	Records a bind vertex buffers command.
	*/
	void CommandBindVertexBuffers(const uint32 firstBinding, const uint32 vertexBufferCount, const VkBuffer *RESTRICT vertexBuffers, const VkDeviceSize *RESTRICT offsets) NOEXCEPT;

	/*
	*	Records a copy image command.
	*/
	void CommandCopyImage(VkImage source, VkImage destination, const VkExtent3D extent) NOEXCEPT;

	/*
	*	Records a dispatch command.
	*/
	void CommandDispatch(const uint32 width, const uint32 height, const uint32 depth) NOEXCEPT;

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
	*	Records a next subpass command.
	*/
	void CommandNextSubpass() NOEXCEPT;

	/*
	*	Records a pipeline barrier command.
	*/
	void CommandPipelineBarrier(const VkAccessFlags sourceAccessMask, const VkAccessFlags destinationAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, VkImage image, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask, const VkDependencyFlags dependencyFlags) NOEXCEPT;

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
	*	Records a trace rays command.
	*/
	void CommandTraceRays(const VkBuffer shaderBindingTableBuffer, const uint32 missShaderBindingOffset, const uint32 hitShaderBindingOffset, const uint32 width, const uint32 height) NOEXCEPT;

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
	VkCommandBuffer _VulkanCommandBuffer;

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
	void CreateCommandBufferInheritanceInfo(VkCommandBufferInheritanceInfo &commandBufferInheritanceInfo, const VkRenderPass renderPass, const uint32 subpass, const VkFramebuffer framebuffer) const NOEXCEPT;

	/*
	*	Creates a secondary command buffer begin info.
	*/
	void CreateSecondaryCommandBufferBeginInfo(VkCommandBufferBeginInfo &commandBufferBeginInfo, const VkCommandBufferUsageFlags commandBufferUsageFlags, const VkCommandBufferInheritanceInfo *const RESTRICT inheritanceInfo) const NOEXCEPT;

};
#endif