//Header file.
#include <Rendering/Native/RenderPasses/ShadowsRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ShadowsRenderPass);

/*
*	Default constructor.
*/
ShadowsRenderPass::ShadowsRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::SHADOWS);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ShadowsRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ShadowsRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void ShadowsRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ShadowsRayTracingPipeline);

	//Initialize all pipelines.
	_ShadowsRayTracingPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void ShadowsRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	if (RenderingConfigurationManager::Instance->GetSurfaceShadowsMode() == RenderingConfigurationManager::SurfaceShadowsMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	_ShadowsRayTracingPipeline.Execute();

	SetEnabled(true);
}