//Header file.
#include <Rendering/Native/RenderPasses/FireflyReductionRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(FireflyReductionRenderPass);

/*
*	Default constructor.
*/
FireflyReductionRenderPass::FireflyReductionRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::FireflyReduction);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		FireflyReductionRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		FireflyReductionRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void FireflyReductionRenderPass::Initialize() NOEXCEPT
{
	//Add and initialize the pipelines.
	SetNumberOfPipelines(_FireflyReductionGraphicsPipelines.Size());

	_FireflyReductionGraphicsPipelines[0].Initialize(	FireflyReductionGraphicsPipeline::Direction::Horizontal,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1));
	AddPipeline(&_FireflyReductionGraphicsPipelines[0]);

	_FireflyReductionGraphicsPipelines[1].Initialize(	FireflyReductionGraphicsPipeline::Direction::Vertical,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));
	AddPipeline(&_FireflyReductionGraphicsPipelines[1]);


	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void FireflyReductionRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadLeft == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute all pipelines.
	if (RenderingConfigurationManager::Instance->GetDiffuseIrradianceMode() == RenderingConfigurationManager::DiffuseIrradianceMode::RayTraced)
	{
		for (FireflyReductionGraphicsPipeline &pipeline : _FireflyReductionGraphicsPipelines)
		{
			pipeline.Execute();
		}
	}

	else
	{
		for (FireflyReductionGraphicsPipeline &pipeline : _FireflyReductionGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}
}