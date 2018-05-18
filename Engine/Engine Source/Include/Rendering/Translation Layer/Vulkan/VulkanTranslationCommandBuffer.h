#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/Engine Layer/CommandBuffer.h>

class VulkanTranslationCommandBuffer final : public CommandBuffer
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	VulkanTranslationCommandBuffer() NOEXCEPT = delete;

	/*
	*	Constructor taking the Vulkan command buffer.
	*/
	VulkanTranslationCommandBuffer(VulkanCommandBuffer initialCommandBuffer) NOEXCEPT
		:
		commandBuffer(initialCommandBuffer)
	{

	}

	/*
	*	Returns the underlying Vulkan command buffer.
	*/
	VulkanCommandBuffer GetVulkanCommandBuffer() const NOEXCEPT { return commandBuffer; }

	/*
	*	Begins the command buffer.
	*/
	void Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT override;

	/*
	*	Binds a number of descriptor sets.
	*/
	void BindDescriptorSets(const RenderPass *const RESTRICT renderPass, const uint32 firstBinding, const uint32 descriptorSetCount, const DescriptorSetHandle *const RESTRICT descriptorSets) NOEXCEPT override;

	/*
	*	Binds an index buffer.
	*/
	void BindIndexBuffer(const RenderPass *const RESTRICT renderPass, const GraphicsBufferHandle indexBuffer, const uint64 offset) NOEXCEPT override;

	/*
	*	Binds a number of vertex buffers.
	*/
	void BindVertexBuffers(const RenderPass *const RESTRICT renderPass, const uint32 numberOfVertexBuffers, const GraphicsBufferHandle *const RESTRICT vertexBuffers, const uint64 *const RESTRICT offsets) NOEXCEPT override;

	/*
	*	Draws.
	*/
	void Draw(const RenderPass *const RESTRICT renderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT override;

	/*
	*	Draws indexed.
	*/
	void DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT override;

	/*
	*	Pushes constants.
	*/
	void PushConstants(const RenderPass *const RESTRICT renderPass, PushConstantRange::ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT override;

	/*
	*	Waits for events.
	*/
	void WaitForEvents(const RenderPass *const RESTRICT renderPass, const uint32 eventCount, const EventHandle *const RESTRICT events) NOEXCEPT override;

	/*
	*	Ends the command buffer.
	*/
	void End(const RenderPass *const RESTRICT) NOEXCEPT override;

private:

	//The underlying Vulkan command buffer.
	VulkanCommandBuffer commandBuffer;

};