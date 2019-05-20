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
	//Add and initialize the pipelines.
	SetNumberOfPipelines(_DenoisingGraphicsPipelines.Size());

	_DenoisingGraphicsPipelines[0].Initialize(	DenoisingGraphicsPipeline::Direction::Horizontal,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));
	AddPipeline(&_DenoisingGraphicsPipelines[0]);

	_DenoisingGraphicsPipelines[1].Initialize(	DenoisingGraphicsPipeline::Direction::Vertical,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));
	AddPipeline(&_DenoisingGraphicsPipelines[1]);


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