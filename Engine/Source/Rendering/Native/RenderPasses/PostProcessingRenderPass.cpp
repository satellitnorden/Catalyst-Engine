//Header file.
#include <Rendering/Native/RenderPasses/PostProcessingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PostProcessingRenderPass);

/*
*	Default constructor.
*/
PostProcessingRenderPass::PostProcessingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::PostProcessing);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PostProcessingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PostProcessingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void PostProcessingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_PostProcessingGraphicsPipeline);

	//Initialize all pipelines.
	_PostProcessingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void PostProcessingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_PostProcessingGraphicsPipeline.Execute();
}