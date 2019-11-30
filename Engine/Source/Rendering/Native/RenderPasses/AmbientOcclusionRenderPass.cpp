//Header file.
#include <Rendering/Native/RenderPasses/AmbientOcclusionRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AmbientOcclusionRenderPass);

/*
*	Default constructor.
*/
AmbientOcclusionRenderPass::AmbientOcclusionRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::AmbientOcclusion);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		AmbientOcclusionRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		AmbientOcclusionRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void AmbientOcclusionRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1 + _AmbientOcclusionDenoisingGraphicsPipelines.Size());
	AddPipeline(&_ScreenSpaceAmbientOcclusionGraphicsPipeline);

	for (AmbientOcclusionDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Initialize();
	_AmbientOcclusionDenoisingGraphicsPipelines[0].Initialize(	AmbientOcclusionDenoisingGraphicsPipeline::Direction::Horizontal,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::AmbientOcclusion),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R8_Byte));

	_AmbientOcclusionDenoisingGraphicsPipelines[1].Initialize(	AmbientOcclusionDenoisingGraphicsPipeline::Direction::Vertical,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R8_Byte),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::AmbientOcclusion));

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void AmbientOcclusionRenderPass::Execute() NOEXCEPT
{	
	//Nothing to do here if specular irradiance isn't enabled.
	if (RenderingConfigurationManager::Instance->GetAmbientOcclusionMode() == RenderingConfigurationManager::AmbientOcclusionMode::None)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Execute();

	for (AmbientOcclusionDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
		//pipeline.SetIncludeInRender(false);
	}
}