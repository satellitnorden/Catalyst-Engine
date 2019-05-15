//Header file.
#include <Rendering/Native/RenderPasses/LightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LightingRenderPass);

/*
*	Default constructor.
*/
LightingRenderPass::LightingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Lighting);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		LightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		LightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void LightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_LightingGraphicsPipeline);

	//Initialize all pipelines.
	_LightingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void LightingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_LightingGraphicsPipeline.Execute();
}