//Header file.
#include <Rendering/Native/RenderPasses/ScreenRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ScreenRenderPass);

/*
*	Default constructor.
*/
ScreenRenderPass::ScreenRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Screen);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ScreenRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ScreenRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void ScreenRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ScreenGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void ScreenRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_ScreenGraphicsPipeline.Execute();
}