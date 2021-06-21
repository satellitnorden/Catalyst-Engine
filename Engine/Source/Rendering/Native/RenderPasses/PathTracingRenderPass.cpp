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
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::PATH_TRACING)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	_PathTracingRayTracingPipeline.Execute();
}