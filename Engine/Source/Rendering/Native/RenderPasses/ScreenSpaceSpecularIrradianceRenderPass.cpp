//Header file.
#include <Rendering/Native/RenderPasses/ScreenSpaceSpecularIrradianceRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ScreenSpaceSpecularIrradianceRenderPass);

/*
*	Default constructor.
*/
ScreenSpaceSpecularIrradianceRenderPass::ScreenSpaceSpecularIrradianceRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::ScreenSpaceSpecularIrradiance);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ScreenSpaceSpecularIrradianceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ScreenSpaceSpecularIrradianceRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void ScreenSpaceSpecularIrradianceRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_ScreenSpaceSpecularIrradianceGraphicsPipeline);
	AddPipeline(&_SpecularIrradianceApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenSpaceSpecularIrradianceGraphicsPipeline.Initialize();
	_SpecularIrradianceApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void ScreenSpaceSpecularIrradianceRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	if (RenderingConfigurationManager::Instance->GetSpecularIrradianceMode() == RenderingConfigurationManager::SpecularIrradianceMode::ScreenSpace)
	{
		_ScreenSpaceSpecularIrradianceGraphicsPipeline.Execute();
	}
	
	else
	{
		_ScreenSpaceSpecularIrradianceGraphicsPipeline.SetIncludeInRender(false);
	}

	_SpecularIrradianceApplicationGraphicsPipeline.Execute();
}