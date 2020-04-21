//Header file.
#include <Rendering/Native/RenderPasses/ToneMappingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ToneMappingRenderPass);

/*
*	Default constructor.
*/
ToneMappingRenderPass::ToneMappingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TONE_MAPPING);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ToneMappingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ToneMappingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void ToneMappingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ToneMappingGraphicsPipeline);

	//Initialize all pipelines.
	_ToneMappingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void ToneMappingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_ToneMappingGraphicsPipeline.Execute();
}