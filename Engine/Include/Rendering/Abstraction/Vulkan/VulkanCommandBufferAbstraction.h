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
	*	Records a blit image command.
	*/
	void CommandBlitImage(VkImage source, VkImage destination, const VkExtent2D exten) NOEXCEPT;

	/*
	*	Records a build acceleration structure command.
	*/
	void CommandBuildAccelerationStructure
	(
		const VkAccelerationStructureTypeNV type,
		const uint32 instance_count,
		const VkBuffer instance_buffer,
		const uint32 geometry_count,
		const VkGeometryNV *const RESTRICT geometries,
		const VkAccelerationStructureNV acceleration_structure,
		const VkBuffer scratch_buffer
	) NOEXCEPT;

	/*
	*	Records a copy buffer command.
	*/
	void CommandCopyBuffer(const VkBuffer source, VkBuffer destination, const VkDeviceSize size) NOEXCEPT;

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
	*	Records an image memory barrier command.
	*/
	void CommandImageMemoryBarrier(	const VkAccessFlags source_access_mask,
									const VkAccessFlags destination_access_mask,
									const VkImageLayout old_layout,
									const VkImageLayout new_layout,
									const VkImage image,
									const VkPipelineStageFlags source_stage_mask,
									const VkPipelineStageFlags destination_stage_mask) NOEXCEPT;

	/*
	*	Records a next subpass command.
	*/
	void CommandNextSubpass() NOEXCEPT;

	/*
	*	Records a push constants command.
	*/
	void CommandPushConstants(const VkPipelineLayout layout, const VkShaderStageFlags stageFlags, const uint32 offset, const uint32 size, const void *RESTRICT data) NOEXCEPT;

	/*
	*	Records a reset event command.
	*/
	void CommandResetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT;

	/*
	*	Records a reset query pool command.
	*/
	void CommandResetQueryPool(const VkQueryPool query_pool, const uint32 query_count) NOEXCEPT;

	/*
	*	Records a set event command.
	*/
	void CommandSetEvent(const VkEvent event, const VkPipelineStageFlags stageMask) NOEXCEPT;

	/*
	*	Records a trace rays command.
	*/
	void CommandTraceRays(	const VkBuffer shader_binding_table_buffer,
							const uint32 miss_shader_binding_offset,
							const uint32 miss_shader_binding_stride,
							const uint32 hit_shader_binding_offset,
							const uint32 hit_shader_binding_stride,
							const uint32 width,
							const uint32 height) NOEXCEPT;

	/*
	*	Records a wait events command.
	*/
	void CommandWaitEvents(const uint32 eventCount, const VkEvent *const RESTRICT events, const VkPipelineStageFlags sourceStageMask, const VkPipelineStageFlags destinationStageMask) NOEXCEPT;

	/*
	*	Records a write timestamp command.
	*/
	void CommandWriteTimestamp(const VkPipelineStageFlagBits pipeline_stage, const VkQueryPool query_pool, const uint32 query_index) NOEXCEPT;

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