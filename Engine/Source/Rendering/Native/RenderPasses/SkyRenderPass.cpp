//Header file.
#include <Rendering/Native/RenderPasses/SkyRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SkyRenderPass);

/*
*	Default constructor.
*/
SkyRenderPass::SkyRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Sky);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SkyRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SkyRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SkyRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_SkyGraphicsPipeline);

	//Initialize all pipelines.
	_SkyGraphicsPipeline.Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer());

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void SkyRenderPass::Execute() NOEXCEPT
{
	_SkyGraphicsPipeline.Execute();
}