//Header file.
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes this render pass.
*/
void RenderPass::Initialize() NOEXCEPT
{
	//Call the initialization function.
	_InitializationFunction();
}

/*
*	Executes this render pass.
*/
void RenderPass::Execute() NOEXCEPT
{
	//Call the execution function.
	_ExecutionFunction();
}

/*
*	Pre records this render pass.
*/
void RenderPass::PreRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT
{
	//Call the pre record function.
	if (_PreRecordFunction)
	{
		_PreRecordFunction(frame_command_buffer);
	}
}

/*
*	Post records this render pass.
*/
void RenderPass::PostRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT
{
	//Call the post record function.
	if (_PostRecordFunction)
	{
		_PostRecordFunction(frame_command_buffer);
	}
}

/*
*	Terminates this render pass.
*/
void RenderPass::Terminate() NOEXCEPT
{
	//Call the termination function.
	_TerminationFunction();

	//Terminate this render pass.
	RenderingSystem::Instance->TerminateRenderPass(this);

	//Clear the pipelines.
	_Pipelines.Clear();
}

/*
*	Resets this render pass.
*/
void RenderPass::ResetRenderPass() NOEXCEPT
{
	//Clear the pipelines.
	_Pipelines.Clear();
}