//Header file.
#include <Rendering/Native/Pipelines/Pipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

/*
*	Default constructor.
*/
Pipeline::Pipeline() NOEXCEPT
{
	//Set up the tasks.
	_InitializationTask._Arguments = nullptr;
	_RenderTask._Arguments = nullptr;
}

/*
*	Initializes this pipeline asynchronously.
*/
void Pipeline::InitializeAsynchronous() NOEXCEPT
{
	//Fire off the initialization task.
	TaskSystem::Instance->ExecuteTask(&_InitializationTask);
}

/*
*	Waits for the initialization this pipeline to finish.
*/
void Pipeline::WaitForInitialization() const NOEXCEPT
{
	//Wait for the initialization this render pass to finish.
	_InitializationTask.WaitFor();
}

/*
*	Renders this pipeline asynchronously.
*/
void Pipeline::RenderAsynchronous() NOEXCEPT
{
	//Fire off the render task.
	TaskSystem::Instance->ExecuteTask(&_RenderTask);
}

/*
*	Waits for the render this pipeline to finish.
*/
void Pipeline::WaitForRender() const NOEXCEPT
{
	//Wait for the render this render pass to finish.
	_RenderTask.WaitFor();
}

/*
*	Finalizes the initialization of a render pass.
*/
void Pipeline::FinalizeInitialization() NOEXCEPT
{
	//Finalize the initialization of this pipeline via the rendering system.
	RenderingSystem::Instance->FinalizePipelineInitialization(this);
}

/*
*	Returns the current command buffer, const.
*/
RESTRICTED const CommandBuffer *const RESTRICT Pipeline::GetCurrentCommandBuffer() const NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Returns the current command buffer, non-const.
*/
CommandBuffer *const RESTRICT Pipeline::GetCurrentCommandBuffer() NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}