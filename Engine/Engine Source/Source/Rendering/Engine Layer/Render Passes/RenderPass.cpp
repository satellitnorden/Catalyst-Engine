//Header file.
#include <Rendering\Engine Layer\Render Passes\RenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

/*
*	Renders this render pass asynchronously.
*/
void RenderPass::RenderAsynchronous() NOEXCEPT
{
	//Fire off the render task.
	TaskSystem::Instance->ExecuteTask(&renderTask);
}

/*
*	Waits for this render pass to finish.
*/
void RenderPass::WaitForRender() NOEXCEPT
{
	//Wait for this render pass to finish.
	renderSemaphore.WaitFor();
}

/*
*	Finalizes the initialization of a render pass.
*/
void RenderPass::FinalizeInitialization() NOEXCEPT
{
	//Set up the render task.
	renderTask.arguments = nullptr;
	renderTask.semaphore = &renderSemaphore;

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