//Header file.
#include <Rendering/Native/RenderPasses/SpecularIrradianceRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SpecularIrradianceRenderPass);

/*
*	Default constructor.
*/
SpecularIrradianceRenderPass::SpecularIrradianceRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::SpecularIrradiance);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SpecularIrradianceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SpecularIrradianceRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SpecularIrradianceRenderPass::Initialize() NOEXCEPT
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
void SpecularIrradianceRenderPass::Execute() NOEXCEPT
{	
	//Nothing to do here if specular irradiance isn't enabled.
	if (RenderingConfigurationManager::Instance->GetSpecularIrradianceMode() == RenderingConfigurationManager::SpecularIrradianceMode::None)
	{
		SetEnabled(false);

		return;
	}

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