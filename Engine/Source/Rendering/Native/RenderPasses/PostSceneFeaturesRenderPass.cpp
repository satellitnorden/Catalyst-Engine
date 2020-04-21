//Header file.
#include <Rendering/Native/RenderPasses/PostSceneFeaturesRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PostSceneFeaturesRenderPass);

/*
*	Default constructor.
*/
PostSceneFeaturesRenderPass::PostSceneFeaturesRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::POST_SCENE_FEATURES);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void PostSceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_SceneFeaturesDownsampleGraphicsPipeline);

	//Initialize all pipelines.
	_SceneFeaturesDownsampleGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void PostSceneFeaturesRenderPass::Execute() NOEXCEPT
{
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_SceneFeaturesDownsampleGraphicsPipeline.Execute();
}