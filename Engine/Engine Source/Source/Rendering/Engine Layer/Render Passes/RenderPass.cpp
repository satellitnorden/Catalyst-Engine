//Header file.
#include <Rendering\Engine Layer\Render Passes\RenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

/*
*	Default constructor.
*/
RenderPass::RenderPass() NOEXCEPT
{
	//Set up the tasks.
	initializationTask.arguments = nullptr;
	initializationTask.semaphore = &initializationSemaphore;
	renderTask.arguments = nullptr;
	renderTask.semaphore = &renderSemaphore;
}

/*
*	Initializes this render pass asynchronously.
*/
void RenderPass::InitializeAsynchronous() NOEXCEPT
{
	//Fire off the initialization task.
	TaskSystem::Instance->ExecuteTask(&initializationTask);
}

/*
*	Waits for the initialization this render pass to finish.
*/
void RenderPass::WaitForInitialization() NOEXCEPT
{
	//Wait for the initialization this render pass to finish.
	initializationSemaphore.WaitFor();
}

/*
*	Renders this render pass asynchronously.
*/
void RenderPass::RenderAsynchronous() NOEXCEPT
{
	//Fire off the render task.
	TaskSystem::Instance->ExecuteTask(&renderTask);
}

/*
*	Waits for the render this render pass to finish.
*/
void RenderPass::WaitForRender() NOEXCEPT
{
	//Wait for the render this render pass to finish.
	renderSemaphore.WaitFor();
}

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

/*
*	Sets the stage.
*/
void RenderPass::SetStage(const RenderPassStage newStage) NOEXCEPT
{
	//Set the stage.
	stage = newStage;

	//Register this render pass.
	RenderingSystem::Instance->RegisterRenderPass(this);
}