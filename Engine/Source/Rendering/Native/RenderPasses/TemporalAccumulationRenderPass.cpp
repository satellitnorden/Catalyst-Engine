//Header file.
#include <Rendering/Native/RenderPasses/TemporalAccumulationRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAccumulationGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAccumulationRenderPass);

/*
*	Default constructor.
*/
TemporalAccumulationRenderPass::TemporalAccumulationRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TemporalAccumulation);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAccumulationRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(TemporalAccumulationGraphicsPipeline::Instance.Get());

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
void TemporalAccumulationRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}