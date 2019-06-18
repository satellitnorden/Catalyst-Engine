//Header file.
#include <Rendering/Native/RenderPasses/WorldRayTracingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldRayTracingRenderPass);

/*
*	Default constructor.
*/
WorldRayTracingRenderPass::WorldRayTracingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::WorldRayTracing);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		WorldRayTracingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		WorldRayTracingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void WorldRayTracingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_WorldRayTracingPipeline);

	//Initialize all pipelines.
	_WorldRayTracingPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void WorldRayTracingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_WorldRayTracingPipeline.Execute();	
}