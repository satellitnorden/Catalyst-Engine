//Header file.
#include <Rendering/Native/RenderPasses/WorldRayTracingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

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
	SetStage(RenderPassStage::WorldRayTracing);

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
	AddPipeline(WorldRayTracingPipeline::Instance.Get());

	//Initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Initialize();
	}

	//Initialize this render pass.
	RenderingSystem::Instance->InitializeRenderPass(this);

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
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}