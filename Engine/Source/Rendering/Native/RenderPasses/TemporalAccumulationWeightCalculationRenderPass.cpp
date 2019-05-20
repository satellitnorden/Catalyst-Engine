//Header file.
#include <Rendering/Native/RenderPasses/TemporalAccumulationWeightCalculationRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAccumulationWeightCalculationRenderPass);

/*
*	Default constructor.
*/
TemporalAccumulationWeightCalculationRenderPass::TemporalAccumulationWeightCalculationRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TemporalAccumulationWeightCalculation);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAccumulationWeightCalculationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAccumulationWeightCalculationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAccumulationWeightCalculationRenderPass::Initialize() NOEXCEPT
{
	//Initialize and add the pipelines.
	SetNumberOfPipelines(_TemporalAccumulationWeightCalculationGraphicsPipelines.Size());

	_TemporalAccumulationWeightCalculationGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer2),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer1));

	_TemporalAccumulationWeightCalculationGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer1),
																			RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer2));

	for (uint64 i{ 0 }, size{ _TemporalAccumulationWeightCalculationGraphicsPipelines.Size() }; i < size; ++i)
	{
		AddPipeline(&_TemporalAccumulationWeightCalculationGraphicsPipelines[i]);
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
void TemporalAccumulationWeightCalculationRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadUp == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _TemporalAccumulationWeightCalculationGraphicsPipelines.Size() }; i < size; ++i)
	{
		if (i == _CurrentBufferIndex)
		{
			_TemporalAccumulationWeightCalculationGraphicsPipelines[i].Execute();
		}

		else
		{
			_TemporalAccumulationWeightCalculationGraphicsPipelines[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == _TemporalAccumulationWeightCalculationGraphicsPipelines.Size() - 1 ? 0 : _CurrentBufferIndex + 1;
}