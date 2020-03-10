//Header file.
#include <Rendering/Native/RenderPasses/PathTracingRayTracingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PathTracingRayTracingRenderPass);

/*
*	Default constructor.
*/
PathTracingRayTracingRenderPass::PathTracingRayTracingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::PathTracing);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PathTracingRayTracingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PathTracingRayTracingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void PathTracingRayTracingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_PathTracingRayTracingPipeline);

	//Initialize all pipelines.
	_PathTracingRayTracingPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void PathTracingRayTracingRenderPass::Execute() NOEXCEPT
{
	if (true)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_PathTracingRayTracingPipeline.Execute();
}