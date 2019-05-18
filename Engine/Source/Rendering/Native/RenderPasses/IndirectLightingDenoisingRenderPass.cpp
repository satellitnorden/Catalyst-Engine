//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingDenoisingRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IndirectLightingDenoisingRenderPass);

/*
*	Default constructor.
*/
IndirectLightingDenoisingRenderPass::IndirectLightingDenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::IndirectLightingDenoising);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		IndirectLightingDenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		IndirectLightingDenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void IndirectLightingDenoisingRenderPass::Initialize() NOEXCEPT
{
	//Add and initialize the pipelines.
	SetNumberOfPipelines(_IndirectLightingDenoisingGraphicsPipelines.Size());

	_IndirectLightingDenoisingGraphicsPipelines[0].Initialize(	IndirectLightingDenoisingGraphicsPipeline::Direction::Horizontal,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::IndirectLighting),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));
	AddPipeline(&_IndirectLightingDenoisingGraphicsPipelines[0]);

	_IndirectLightingDenoisingGraphicsPipelines[1].Initialize(	IndirectLightingDenoisingGraphicsPipeline::Direction::Vertical,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::IndirectLighting));
	AddPipeline(&_IndirectLightingDenoisingGraphicsPipelines[1]);


	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void IndirectLightingDenoisingRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadLeft == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute all pipelines.
	for (IndirectLightingDenoisingGraphicsPipeline &pipeline : _IndirectLightingDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}
}