//Header file.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SceneFeaturesRenderPass);

/*
*	Default constructor.
*/
SceneFeaturesRenderPass::SceneFeaturesRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::SceneFeatures);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_SceneFeaturesGraphicsPipeline);

	//Initialize all pipelines.
	_SceneFeaturesGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void SceneFeaturesRenderPass::Execute() NOEXCEPT
{	
	//Actually... Don't. (:
	if (true)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_SceneFeaturesGraphicsPipeline.Execute();
}