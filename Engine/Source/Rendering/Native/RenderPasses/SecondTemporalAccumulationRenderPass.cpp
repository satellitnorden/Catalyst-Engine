//Header file.
#include <Rendering/Native/RenderPasses/SecondTemporalAccumulationRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/SecondTemporalAccumulationGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SecondTemporalAccumulationRenderPass);

/*
*	Default constructor.
*/
SecondTemporalAccumulationRenderPass::SecondTemporalAccumulationRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::SecondTemporalAccumulation);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SecondTemporalAccumulationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SecondTemporalAccumulationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SecondTemporalAccumulationRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(SecondTemporalAccumulationGraphicsPipeline::Instance.Get());

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
void SecondTemporalAccumulationRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}