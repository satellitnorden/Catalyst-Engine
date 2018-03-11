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
	void CommandBeginRenderPass(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex) NOEXCEPT;

	/*
	*	Records a begin render pass command and clears.
	*/
	template <uint32 NumberOfClearValues>
	void CommandBeginRenderPassAndClear(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex) NOEXCEPT;

	/*
	*	Records a bind descriptor sets command.
	*/
	void CommandBindDescriptorSets(const VulkanPipeline &vulkanPipeline, const uint32 descriptorSetCount, const VulkanDescriptorSet *RESTRICT descriptorSets) NOEXCEPT;

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
	void CommandBindVertexBuffers(const uint32 vertexBufferCount, const VkBuffer *RESTRICT vertexBuffers, const uint64 *RESTRICT offsets) NOEXCEPT;

	/*
	*	Records a draw command.
	*/
	void CommandDraw(const uint32 vertexCount) NOEXCEPT;

	/*
	*	Records a draw indexed command.
	*/
	void CommandDrawIndexed(const uint32 indexCount, const uint32 instanceCount) NOEXCEPT;

	/*
	*	Records an end render pass command.
	*/
	void CommandEndRenderPass() NOEXCEPT;

	/*
	*	Records a push constants command.
	*/
	void CommandPushConstants(const VkPipelineLayout layout, const VkShaderStageFlags stageFlags, const uint32 offset, const uint32 size, const void *RESTRICT data) NOEXCEPT;

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

//Templated function definitions.
#include <VulkanCommandBuffer.inl>