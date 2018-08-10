//Header file.
#include <Rendering\Engine\RenderPasses\RenderPass.h>

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
	renderTask.arguments = nullptr;
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
void RenderPass::WaitForInitialization() const NOEXCEPT
{
	//Wait for the initialization this render pass to finish.
	initializationTask.WaitFor();
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
void RenderPass::WaitForRender() const NOEXCEPT
{
	//Wait for the render this render pass to finish.
	renderTask.WaitFor();
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
*	Returns the current command buffer, non-const.
*/
RESTRICTED const CommandBuffer *const RESTRICT RenderPass::GetCurrentCommandBuffer() const NOEXCEPT
{
	return commandBuffers[RenderingSystem::Instance->GetCurrentFrameIndex()];
}

/*
*	Returns the current command buffer, non-const.
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
}