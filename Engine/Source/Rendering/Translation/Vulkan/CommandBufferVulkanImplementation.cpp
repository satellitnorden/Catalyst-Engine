#if defined(CATALYST_RENDERING_VULKAN)
//Header file.
#include <Rendering/Native/CommandBuffer.h>

//Rendering.
#include <Rendering/Abstraction/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/Abstraction/Vulkan/VulkanConstantBuffer.h>
#include <Rendering/Native/Pipelines/Pipeline.h>
#include <Rendering/Translation/Vulkan/VulkanPipelineSubStageData.h>
#include <Rendering/Translation/Vulkan/VulkanTranslationUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Begins the command buffer.
*/
void CommandBuffer::Begin(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanPipelineSubStageData *const RESTRICT pipelineSubStageData{ static_cast<const VulkanPipelineSubStageData *const RESTRICT>(pipeline->GetData()) };

	//Begin the command buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->BeginSecondary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, pipelineSubStageData->_RenderPass, VulkanTranslationUtilities::GetSubStageIndex(pipeline->GetMainStage(), pipeline->GetSubStage()), pipeline->GetRenderTargets().Empty() ? pipelineSubStageData->_Framebuffers[0] : pipeline->GetRenderTargets()[0] == RenderTarget::Screen ? pipelineSubStageData->_Framebuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] : pipelineSubStageData->_Framebuffers[0]);
	
	//Bind the pipeline.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindPipeline(pipelineSubStageData->_Pipeline->Get());
}

/*
*	Binds an index buffer.
*/
void CommandBuffer::BindIndexBuffer(const Pipeline *const RESTRICT pipeline, ConstantBufferHandle buffer, const uint64 offset) NOEXCEPT
{
	//Bind the index buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindIndexBuffer(static_cast<VulkanConstantBuffer *const RESTRICT>(buffer)->Get(), offset);
}

/*
*	Binds a render data table.
*/
void CommandBuffer::BindRenderDataTable(const Pipeline *const RESTRICT pipeline, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanPipelineSubStageData *const RESTRICT pipelineSubStageData{ static_cast<const VulkanPipelineSubStageData *const RESTRICT>(pipeline->GetData()) };

	//Bind the render data tables.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindDescriptorSets(pipelineSubStageData->_Pipeline->GetPipelineLayout(), binding, 1, &static_cast<const VulkanDescriptorSet *const RESTRICT>(renderDataTable)->Get());
}

/*
*	Binds a number of vertex buffers.
*/
void CommandBuffer::BindVertexBuffer(const Pipeline *const RESTRICT pipeline, const uint32 binding, ConstantBufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT
{
	//Bind the vertex buffers.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandBindVertexBuffers(binding, 1, &static_cast<const VulkanConstantBuffer *const RESTRICT>(buffer)->Get(), reinterpret_cast<const VkDeviceSize *const RESTRICT>(offset));
}

/*
*	Draws.
*/
void CommandBuffer::Draw(const Pipeline *const RESTRICT pipeline, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDraw(vertexCount, instanceCount);
}

/*
*	Draws indexed.
*/
void CommandBuffer::DrawIndexed(const Pipeline *const RESTRICT pipeline, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	//Draw indexed.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandDrawIndexed(indexCount, instanceCount);
}

/*
*	Pushes constants.
*/
void CommandBuffer::PushConstants(const Pipeline *const RESTRICT pipeline, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	//Cache the Vulkan render pass sub stage data.
	const VulkanPipelineSubStageData *const RESTRICT pipelineSubStageData{ static_cast<const VulkanPipelineSubStageData *const RESTRICT>(pipeline->GetData()) };

	//Push the constants.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->CommandPushConstants(pipelineSubStageData->_Pipeline->GetPipelineLayout(), VulkanTranslationUtilities::GetVulkanShaderStages(shaderStage), offset, size, data);
}

/*
*	Ends the command buffer.
*/
void CommandBuffer::End(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	//End the command buffer.
	reinterpret_cast<VulkanCommandBuffer *const RESTRICT>(_CommandBufferData)->End();
}
#endif