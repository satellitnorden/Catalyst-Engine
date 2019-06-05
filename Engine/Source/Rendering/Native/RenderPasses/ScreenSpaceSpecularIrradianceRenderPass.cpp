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
	SetStage(RenderPassStage::ScreenSpaceSpecularIrradiance);

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
	SetNumberOfPipelines(1);
	AddPipeline(&_ScreenSpaceSpecularIrradianceGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenSpaceSpecularIrradianceGraphicsPipeline.Initialize();

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
}