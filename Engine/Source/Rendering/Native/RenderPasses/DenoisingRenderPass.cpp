//Header file.
#include <Rendering/Native/RenderPasses/DenoisingRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

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
	SetStage(RenderPassStage::Denoising);

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
	SetNumberOfPipelines(_DenoisingGraphicsPipelines.Size());

	_DenoisingGraphicsPipelines[0].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
												1.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));

	_DenoisingGraphicsPipelines[1].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
												1.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	_DenoisingGraphicsPipelines[2].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
												2.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));

	_DenoisingGraphicsPipelines[3].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
												2.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	_DenoisingGraphicsPipelines[4].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
												4.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));

	_DenoisingGraphicsPipelines[5].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
												4.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	_DenoisingGraphicsPipelines[6].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
												8.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));

	_DenoisingGraphicsPipelines[7].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
												8.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	_DenoisingGraphicsPipelines[8].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
												16.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));

	_DenoisingGraphicsPipelines[9].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
												16.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));

	//Add all pipelines.
	for (DenoisingGraphicsPipeline &pipeline : _DenoisingGraphicsPipelines)
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

	//Execute all pipelines.
	for (DenoisingGraphicsPipeline &pipeline : _DenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}
}