#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Engine/CommandBuffer.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>
#include <Rendering/Translation/Vulkan/VulkanRenderingSystem.h>
#include <Rendering/Translation/Vulkan/VulkanRenderPassSubStageData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

/*
*	Begins the command buffer.
*/
void CommandBuffer::Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanRenderPassSubStageData *const RESTRICT renderPassSubStageData{ static_cast<const VulkanRenderPassSubStageData *const RESTRICT>(renderPass->GetData()) };

	//Begin the command buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, renderPassSubStageData->_RenderPass, VulkanTranslationUtilities::GetSubStageIndex(renderPass->GetMainStage(), renderPass->GetSubStage()), renderPass->GetRenderTargets().Empty() ? renderPassSubStageData->_Framebuffers[0] : renderPass->GetRenderTargets()[0] == RenderTarget::Screen ? renderPassSubStageData->_Framebuffers[VulkanRenderingSystem::Instance->GetCurrentFrameBufferIndex()] : renderPassSubStageData->_Framebuffers[0]);
	
	//Bind the pipeline.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(renderPassSubStageData->_Pipeline->Get());
}

/*
*	Binds an index buffer.
*/
void CommandBuffer::BindIndexBuffer(const RenderPass *const RESTRICT renderPass, ConstantBufferHandle buffer, const uint64 offset) NOEXCEPT
{
	//Bind the index buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindIndexBuffer(static_cast<VulkanConstantBuffer *const RESTRICT>(buffer)->Get(), offset);
}

/*
*	Binds a render data table.
*/
void CommandBuffer::BindRenderDataTable(const RenderPass *const RESTRICT renderPass, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanRenderPassSubStageData *const RESTRICT renderPassSubStageData{ static_cast<const VulkanRenderPassSubStageData *const RESTRICT>(renderPass->GetData()) };

	//Bind the render data tables.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(renderPassSubStageData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
}

/*
*	Binds a number of vertex buffers.
*/
void CommandBuffer::BindVertexBuffer(const RenderPass *const RESTRICT renderPass, const uint32 binding, ConstantBufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT
{
	//Bind the vertex buffers.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindVertexBuffers(binding, 1, &static_cast<const VulkanConstantBuffer *const RESTRICT>(buffer)->Get(), reinterpret_cast<const VkDeviceSize *const RESTRICT>(offset));
}

/*
*	Draws.
*/
void CommandBuffer::Draw(const RenderPass *const RESTRICT renderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDraw(vertexCount, instanceCount);
}

/*
*	Draws indexed.
*/
void CommandBuffer::DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw indexed.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDrawIndexed(indexCount, instanceCount);
}

/*
*	Pushes constants.
*/
void CommandBuffer::PushConstants(const RenderPass *const RESTRICT renderPass, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanRenderPassSubStageData *const RESTRICT renderPassSubStageData{ static_cast<const VulkanRenderPassSubStageData *const RESTRICT>(renderPass->GetData()) };

	//Push the constants.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(renderPassSubStageData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
}

/*
*	Ends the command buffer.
*/
void CommandBuffer::End(const RenderPass *const RESTRICT) NOEXCEPT
{
	//End the command buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->End();
}
#endif