//Header file.
#include <Rendering/Native/RenderPasses/TemporalAccumulationRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAccumulationRenderPass);

/*
*	Default constructor.
*/
TemporalAccumulationRenderPass::TemporalAccumulationRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TemporalAccumulation);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAccumulationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAccumulationRenderPass::Initialize() NOEXCEPT
{
	//Initialize and add the pipelines.
	SetNumberOfPipelines(_TemporalAccumulationGraphicsPipelines.Size());

	_TemporalAccumulationGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer4),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer3),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1));

	_TemporalAccumulationGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer4),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer3),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2));

	_TemporalAccumulationGraphicsPipelines[2].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer4),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer3));

	_TemporalAccumulationGraphicsPipelines[3].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer3),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer4));

	AddPipeline(&_TemporalAccumulationGraphicsPipelines[0]);
	AddPipeline(&_TemporalAccumulationGraphicsPipelines[1]);
	AddPipeline(&_TemporalAccumulationGraphicsPipelines[2]);
	AddPipeline(&_TemporalAccumulationGraphicsPipelines[3]);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void TemporalAccumulationRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._Y == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _TemporalAccumulationGraphicsPipelines.Size() }; i < size; ++i)
	{
		if (i == _CurrentBufferIndex)
		{
			_TemporalAccumulationGraphicsPipelines[i].Execute();
		}

		else
		{
			_TemporalAccumulationGraphicsPipelines[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == _TemporalAccumulationGraphicsPipelines.Size() - 1 ? 0 : _CurrentBufferIndex + 1;
}