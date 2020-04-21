//Header file.
#include <Rendering/Native/RenderPasses/LuminanceLightingRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LuminanceLightingRenderPass);

/*
*	Default constructor.
*/
LuminanceLightingRenderPass::LuminanceLightingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::LUMINANCE_LIGHTING);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		LuminanceLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		LuminanceLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void LuminanceLightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_LuminanceLightingGraphicsPipeline);

	//Initialize all pipelines.
	_LuminanceLightingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void LuminanceLightingRenderPass::Execute() NOEXCEPT
{	
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_LuminanceLightingGraphicsPipeline.Execute();
}