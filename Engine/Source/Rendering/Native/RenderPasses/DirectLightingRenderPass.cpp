//Header file.
#include <Rendering/Native/RenderPasses/DirectLightingRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingRenderPass);

/*
*	Default constructor.
*/
DirectLightingRenderPass::DirectLightingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DIRECT_LIGHTING);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingRenderPass::Initialize() NOEXCEPT
{	
	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_DirectLightingGraphicsPipeline);
	AddPipeline(&_DirectLightingRayTracingPipeline);

	//Initialize all pipelines.
	_DirectLightingGraphicsPipeline.Initialize();
	_DirectLightingRayTracingPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DirectLightingRenderPass::Execute() NOEXCEPT
{
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	if (RenderingConfigurationManager::Instance->GetShadowsMode() == RenderingConfigurationManager::ShadowsMode::NONE)
	{
		_DirectLightingGraphicsPipeline.Execute();
		_DirectLightingRayTracingPipeline.SetIncludeInRender(false);
	}
	
	else
	{
		_DirectLightingGraphicsPipeline.SetIncludeInRender(false);
		_DirectLightingRayTracingPipeline.Execute();
	}
}