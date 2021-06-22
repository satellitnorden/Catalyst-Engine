//Header file.
#include <Rendering/Native/RenderPasses/PathTracingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PathTracingRenderPass);

/*
*	Default constructor.
*/
PathTracingRenderPass::PathTracingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::PATH_TRACING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PathTracingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PathTracingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		PathTracingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void PathTracingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_PathTracingRayTracingPipeline);

	//Initialize all pipelines.
	_PathTracingRayTracingPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void PathTracingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_PathTracingRayTracingPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void PathTracingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_PathTracingRayTracingPipeline.Terminate();
}