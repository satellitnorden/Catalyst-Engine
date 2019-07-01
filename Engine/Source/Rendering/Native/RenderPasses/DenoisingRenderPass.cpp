//Header file.
#include <Rendering/Native/RenderPasses/DenoisingRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DenoisingRenderPass);

/*
*	Default constructor.
*/
DenoisingRenderPass::DenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::Denoising);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DenoisingRenderPass::Initialize() NOEXCEPT
{
	//Initialize the pipelines.
	SetNumberOfPipelines(_AmbientOcclusionDenoisingGraphicsPipelines.Size() + _DiffuseIrradianceDenoisingGraphicsPipelines.Size());

	_AmbientOcclusionDenoisingGraphicsPipelines[0].Initialize(	AmbientOcclusionDenoisingGraphicsPipeline::Direction::Horizontal,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::AmbientOcclusion),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_1));

	_AmbientOcclusionDenoisingGraphicsPipelines[1].Initialize(	AmbientOcclusionDenoisingGraphicsPipeline::Direction::Vertical,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::AmbientOcclusion));

	_DiffuseIrradianceDenoisingGraphicsPipelines[0].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1));

	_DiffuseIrradianceDenoisingGraphicsPipelines[1].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
																1.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	_DiffuseIrradianceDenoisingGraphicsPipelines[2].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
																2.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1));

	_DiffuseIrradianceDenoisingGraphicsPipelines[3].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
																2.0f,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	//Add all pipelines.
	for (AmbientOcclusionDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (DenoisingGraphicsPipeline &pipeline : _DiffuseIrradianceDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DenoisingRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadLeft == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute all the ambient occlusion denoising pipelines.
	for (AmbientOcclusionDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}

	//Execute all diffuse irradiance denoising pipelines.
	if (RenderingConfigurationManager::Instance->GetDiffuseIrradianceMode() == RenderingConfigurationManager::DiffuseIrradianceMode::RayTraced)
	{
		for (DenoisingGraphicsPipeline &pipeline : _DiffuseIrradianceDenoisingGraphicsPipelines)
		{
			pipeline.Execute();
		}
	}

	else
	{
		for (DenoisingGraphicsPipeline &pipeline : _DiffuseIrradianceDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}
}