//Header file.
#include <Rendering/Native/RenderPasses/PathTracingRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PathTracingRenderPass);

/*
*	Default constructor.
*/
PathTracingRenderPass::PathTracingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::PathTracing);

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
}

/*
*	Initializes this render pass.
*/
void PathTracingRenderPass::Initialize() NOEXCEPT
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
void PathTracingRenderPass::Execute() NOEXCEPT
{
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::PATH_TRACING)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_PathTracingRayTracingPipeline.Execute();
}