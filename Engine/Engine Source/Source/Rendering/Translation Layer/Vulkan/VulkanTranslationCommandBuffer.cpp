//Header file.
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationCommandBuffer.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderPassData.h>

/*
*	Begins the command buffer.
*/
void VulkanTranslationCommandBuffer::Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Cache the Vulkan render pass data.
	const VulkanRenderPassData *const RESTRICT data{ static_cast<const VulkanRenderPassData *const RESTRICT>(renderPass->GetData()) };

	//Begin the command buffer.
	commandBuffer.BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, data->renderPass, data->framebuffer);

	//Bind the pipeline.
	commandBuffer.CommandBindPipeline(data->pipeline);
}

/*
*	Binds a number of descriptor sets.
*/
void VulkanTranslationCommandBuffer::BindDescriptorSets(const RenderPass *const RESTRICT renderPass, const uint32 firstBinding, const uint32 descriptorSetCount, const DescriptorSetHandle *const RESTRICT descriptorSets) NOEXCEPT
{
	//Cache the Vulkan render pass data.
	const VulkanRenderPassData *const RESTRICT data{ static_cast<const VulkanRenderPassData *const RESTRICT>(renderPass->GetData()) };

	//Bind the descriptor sets.
	commandBuffer.CommandBindDescriptorSets(data->pipelineLayout, firstBinding, descriptorSetCount, reinterpret_cast<const VkDescriptorSet *const RESTRICT>(descriptorSets));
}

/*
*	Binds an index buffer.
*/
void VulkanTranslationCommandBuffer::BindIndexBuffer(const RenderPass *const RESTRICT renderPass, const GraphicsBufferHandle indexBuffer, const uint64 offset) NOEXCEPT
{
	//Bind the index buffer.
	//commandBuffer.CommandBindIndexBuffer()
}

/*
*	Binds a number of vertex buffers.
*/
void VulkanTranslationCommandBuffer::BindVertexBuffers(const RenderPass *const RESTRICT renderPass, const uint32 numberOfVertexBuffers, const GraphicsBufferHandle *const RESTRICT vertexBuffers, const uint64 *const RESTRICT offsets) NOEXCEPT
{
	//Bind the vertex buffers.
	commandBuffer.CommandBindVertexBuffers(numberOfVertexBuffers, reinterpret_cast<const VkBuffer *const RESTRICT>(vertexBuffers), offsets);
}

/*
*	Ends the command buffer.
*/
void VulkanTranslationCommandBuffer::End(const RenderPass *const RESTRICT) NOEXCEPT
{
	//End the command buffer.
	commandBuffer.End();
}