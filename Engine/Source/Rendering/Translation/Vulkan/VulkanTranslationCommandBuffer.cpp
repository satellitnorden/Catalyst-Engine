//Header file.
#include <Rendering/Translation/Vulkan/VulkanTranslationCommandBuffer.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>
#include <Rendering/Translation/Vulkan/VulkanRenderPassSubStageData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

/*
*	Begins the command buffer.
*/
void VulkanTranslationCommandBuffer::Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanRenderPassSubStageData *const RESTRICT renderPassSubStageData{ static_cast<const VulkanRenderPassSubStageData *const RESTRICT>(renderPass->GetData()) };

	//Begin the command buffer.
	_CommandBuffer.BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, renderPassSubStageData->_RenderPass, renderPass->GetSubStageIndex(), renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? renderPassSubStageData->_Framebuffers[VulkanRenderingSystem::Instance->GetCurrentFrameIndex()] : renderPassSubStageData->_Framebuffers[0]);
	
	//Bind the pipeline.
	_CommandBuffer.CommandBindPipeline(renderPassSubStageData->_Pipeline->Get());
}

/*
*	Binds an index buffer.
*/
void VulkanTranslationCommandBuffer::BindIndexBuffer(const RenderPass *const RESTRICT renderPass, const ConstantBufferHandle indexBuffer, const uint64 offset) NOEXCEPT
{
	//Bind the index buffer.
	_CommandBuffer.CommandBindIndexBuffer(reinterpret_cast<const VkBuffer>(indexBuffer), offset);
}

/*
*	Binds a render data table.
*/
void VulkanTranslationCommandBuffer::BindRenderDataTable(const RenderPass *const RESTRICT renderPass, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanRenderPassSubStageData *const RESTRICT renderPassSubStageData{ static_cast<const VulkanRenderPassSubStageData *const RESTRICT>(renderPass->GetData()) };

	//Bind the render data tables.
	_CommandBuffer.CommandBindDescriptorSets(renderPassSubStageData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
}

/*
*	Binds a number of vertex buffers.
*/
void VulkanTranslationCommandBuffer::BindVertexBuffers(const RenderPass *const RESTRICT renderPass, const uint32 numberOfVertexBuffers, const ConstantBufferHandle *const RESTRICT vertexBuffers, const uint64 *const RESTRICT offsets) NOEXCEPT
{
	//Bind the vertex buffers.
	_CommandBuffer.CommandBindVertexBuffers(numberOfVertexBuffers, (const VkBuffer *const RESTRICT) vertexBuffers, reinterpret_cast<const VkDeviceSize *const RESTRICT>(offsets));
}

/*
*	Draws.
*/
void VulkanTranslationCommandBuffer::Draw(const RenderPass *const RESTRICT renderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw.
	_CommandBuffer.CommandDraw(vertexCount, instanceCount);
}

/*
*	Draws indexed.
*/
void VulkanTranslationCommandBuffer::DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw indexed.
	_CommandBuffer.CommandDrawIndexed(indexCount, instanceCount);
}

/*
*	Pushes constants.
*/
void VulkanTranslationCommandBuffer::PushConstants(const RenderPass *const RESTRICT renderPass, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanRenderPassSubStageData *const RESTRICT renderPassSubStageData{ static_cast<const VulkanRenderPassSubStageData *const RESTRICT>(renderPass->GetData()) };

	//Push the constants.
	_CommandBuffer.CommandPushConstants(renderPassSubStageData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
}

/*
*	Ends the command buffer.
*/
void VulkanTranslationCommandBuffer::End(const RenderPass *const RESTRICT) NOEXCEPT
{
	//End the command buffer.
	_CommandBuffer.End();
}