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

	//Initialize this render pass.
	RenderingSystem::Instance->InitializeRenderPass(this);
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