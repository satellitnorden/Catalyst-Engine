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
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::DirectLighting);
#endif

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
	if (false)
	{
		SetEnabled(false);

		return;
	}

	if (RenderingConfigurationManager::Instance->GetShadowsMode() == RenderingConfigurationManager::ShadowsMode::None)
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