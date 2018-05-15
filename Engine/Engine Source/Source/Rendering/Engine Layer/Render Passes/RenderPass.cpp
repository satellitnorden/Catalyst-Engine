//Header file.
#include <Rendering\Engine Layer\Render Passes\RenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Finalizes the initialization of a render pass.
*/
void RenderPass::FinalizeInitialization() NOEXCEPT
{
	//Finalize the initialization of this render pass via the rendering system.
	RenderingSystem::Instance->FinalizeRenderPassInitialization(this);
}

/*
*	Returns the current command buffer.
*/
CommandBuffer *const RESTRICT RenderPass::GetCurrentCommandBuffer() NOEXCEPT
{
	return commandBuffers[RenderingSystem::Instance->GetCurrentFrameIndex()];
}