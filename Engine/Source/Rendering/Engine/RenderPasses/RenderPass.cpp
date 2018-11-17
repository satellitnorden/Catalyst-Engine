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
	_InitializationTask._Arguments = nullptr;
	_RenderTask._Arguments = nullptr;
}

/*
*	Initializes this render pass asynchronously.
*/
void RenderPass::InitializeAsynchronous() NOEXCEPT
{
	//Fire off the initialization task.
	TaskSystem::Instance->ExecuteTask(&_InitializationTask);
}

/*
*	Waits for the initialization this render pass to finish.
*/
void RenderPass::WaitForInitialization() const NOEXCEPT
{
	//Wait for the initialization this render pass to finish.
	_InitializationTask.WaitFor();
}

/*
*	Renders this render pass asynchronously.
*/
void RenderPass::RenderAsynchronous() NOEXCEPT
{
	//Fire off the render task.
	TaskSystem::Instance->ExecuteTask(&_RenderTask);
}

/*
*	Waits for the render this render pass to finish.
*/
void RenderPass::WaitForRender() const NOEXCEPT
{
	//Wait for the render this render pass to finish.
	_RenderTask.WaitFor();
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
*	Returns the current command buffer, const.
*/
RESTRICTED const CommandBuffer *const RESTRICT RenderPass::GetCurrentCommandBuffer() const NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFrameBufferIndex()];
}

/*
*	Returns the current command buffer, non-const.
*/
CommandBuffer *const RESTRICT RenderPass::GetCurrentCommandBuffer() NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFrameBufferIndex()];
}