//Header file.
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationCommandBuffer.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderingSystem.h>
#include <Rendering/Translation Layer/Vulkan/VulkanRenderPassData.h>
#include <Rendering/Translation Layer/Vulkan/VulkanTranslationUtilities.h>

/*
*	Begins the command buffer.
*/
void VulkanTranslationCommandBuffer::Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Cache the Vulkan render pass data.
	const VulkanRenderPassData *const RESTRICT renderPassData{ static_cast<const VulkanRenderPassData *const RESTRICT>(renderPass->GetData()) };

	//Begin the command buffer.
	commandBuffer.BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, renderPassData->renderPass, renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? renderPassData->framebuffers[VulkanRenderingSystem::Instance->GetCurrentFrameIndex()] : renderPassData->framebuffers[0]);
	
	//Bind the pipeline.
	commandBuffer.CommandBindPipeline(renderPassData->pipeline);
}

/*
*	Binds an index buffer.
*/
void VulkanTranslationCommandBuffer::BindIndexBuffer(const RenderPass *const RESTRICT renderPass, const ConstantBufferHandle indexBuffer, const uint64 offset) NOEXCEPT
{
	//Bind the index buffer.
	commandBuffer.CommandBindIndexBuffer(reinterpret_cast<const VkBuffer>(indexBuffer), offset);
}

/*
*	Binds a number of render data tables.
*/
void VulkanTranslationCommandBuffer::BindRenderDataTables(const RenderPass *const RESTRICT renderPass, const uint32 firstBinding, const uint32 renderDataTableCount, const RenderDataTableHandle *const RESTRICT renderDataTables) NOEXCEPT
{
	//Cache the Vulkan render pass data.
	const VulkanRenderPassData *const RESTRICT renderPassData{ static_cast<const VulkanRenderPassData *const RESTRICT>(renderPass->GetData()) };

	//Bind the render data tables.
	commandBuffer.CommandBindDescriptorSets(renderPassData->pipelineLayout, firstBinding, renderDataTableCount, reinterpret_cast<const VkDescriptorSet *const RESTRICT>(renderDataTables));
}

/*
*	Binds a number of vertex buffers.
*/
void VulkanTranslationCommandBuffer::BindVertexBuffers(const RenderPass *const RESTRICT renderPass, const uint32 numberOfVertexBuffers, const ConstantBufferHandle *const RESTRICT vertexBuffers, const uint64 *const RESTRICT offsets) NOEXCEPT
{
	//Bind the vertex buffers.
	commandBuffer.CommandBindVertexBuffers(numberOfVertexBuffers, reinterpret_cast<const VkBuffer *const RESTRICT>(vertexBuffers), offsets);
}

/*
*	Draws.
*/
void VulkanTranslationCommandBuffer::Draw(const RenderPass *const RESTRICT renderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw.
	commandBuffer.CommandDraw(vertexCount, instanceCount);
}

/*
*	Draws indexed.
*/
void VulkanTranslationCommandBuffer::DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw indexed.
	commandBuffer.CommandDrawIndexed(indexCount, instanceCount);
}

/*
*	Pushes constants.
*/
void VulkanTranslationCommandBuffer::PushConstants(const RenderPass *const RESTRICT renderPass, PushConstantRange::ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	//Cache the Vulkan render pass data.
	const VulkanRenderPassData *const RESTRICT renderPassData{ static_cast<const VulkanRenderPassData *const RESTRICT>(renderPass->GetData()) };

	//Push the constants.
	commandBuffer.CommandPushConstants(renderPassData->pipelineLayout, VulkanTranslationUtilities::GetVulkanShaderStage(shaderStage), offset, size, data);
}

/*
*	Ends the command buffer.
*/
void VulkanTranslationCommandBuffer::End(const RenderPass *const RESTRICT) NOEXCEPT
{
	//End the command buffer.
	commandBuffer.End();
}