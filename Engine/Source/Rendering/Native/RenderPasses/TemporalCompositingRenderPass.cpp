//Header file.
#include <Rendering/Native/RenderPasses/TemporalCompositingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalCompositingGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalCompositingRenderPass);

/*
*	Default constructor.
*/
TemporalCompositingRenderPass::TemporalCompositingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TemporalCompositing);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalCompositingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalCompositingRenderPass::Instance->Execute();
	});
}
/*
*	Initializes this render pass.
*/
void TemporalCompositingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(TemporalCompositingGraphicsPipeline::Instance.Get());

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
void TemporalCompositingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}