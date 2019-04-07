//Header file.
#include <Rendering/Native/RenderPasses/AntiAliasingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AntiAliasingGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AntiAliasingRenderPass);

/*
*	Default constructor.
*/
AntiAliasingRenderPass::AntiAliasingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::AntiAliasing);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		AntiAliasingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		AntiAliasingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void AntiAliasingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(AntiAliasingGraphicsPipeline::Instance.Get());

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
void AntiAliasingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}