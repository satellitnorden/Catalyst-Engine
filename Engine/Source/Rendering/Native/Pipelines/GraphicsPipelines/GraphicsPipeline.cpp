//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

/*
*	Default constructor.
*/
GraphicsPipeline::GraphicsPipeline() NOEXCEPT
{
	//Set the type.
	_Type = Type::Graphics;

	//Set up the tasks.
	_InitializationTask._Arguments = nullptr;
	_RenderTask._Arguments = nullptr;
}

/*
*	Initializes this pipeline asynchronously.
*/
void GraphicsPipeline::InitializeAsynchronous() NOEXCEPT
{
	//Fire off the initialization task.
	TaskSystem::Instance->ExecuteTask(&_InitializationTask);
}

/*
*	Waits for the initialization this pipeline to finish.
*/
void GraphicsPipeline::WaitForInitialization() const NOEXCEPT
{
	//Wait for the initialization this render pass to finish.
	_InitializationTask.WaitFor();
}

/*
*	Renders this pipeline asynchronously.
*/
void GraphicsPipeline::RenderAsynchronous() NOEXCEPT
{
	//Fire off the render task.
	TaskSystem::Instance->ExecuteTask(&_RenderTask);
}

/*
*	Waits for the render this pipeline to finish.
*/
void GraphicsPipeline::WaitForRender() const NOEXCEPT
{
	//Wait for the render this render pass to finish.
	_RenderTask.WaitFor();
}

/*
*	Returns the current command buffer, const.
*/
RESTRICTED const CommandBuffer *const RESTRICT GraphicsPipeline::GetCurrentCommandBuffer() const NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Returns the current command buffer, non-const.
*/
CommandBuffer *const RESTRICT GraphicsPipeline::GetCurrentCommandBuffer() NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}