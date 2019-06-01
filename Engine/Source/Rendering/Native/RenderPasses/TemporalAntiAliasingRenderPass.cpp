//Header file.
#include <Rendering/Native/RenderPasses/TemporalAntiAliasingRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAntiAliasingRenderPass);

/*
*	Default constructor.
*/
TemporalAntiAliasingRenderPass::TemporalAntiAliasingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::TemporalAntiAliasing);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAntiAliasingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAntiAliasingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAntiAliasingRenderPass::Initialize() NOEXCEPT
{
	//Initialize and add the pipelines.
	SetNumberOfPipelines(_TemporalAntiAliasingGraphicsPipelines.Size());

	_TemporalAntiAliasingGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAntiAliasingBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAntiAliasingBuffer1));

	_TemporalAntiAliasingGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAntiAliasingBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAntiAliasingBuffer2));


	for (TemporalAntiAliasingGraphicsPipeline &pipeline : _TemporalAntiAliasingGraphicsPipelines)
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
void TemporalAntiAliasingRenderPass::Execute() NOEXCEPT
{
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadUp == ButtonState::Pressed)
	{
		SetEnabled(!IsEnabled());
	}

	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _TemporalAntiAliasingGraphicsPipelines.Size() }; i < size; ++i)
	{
		if (i == _CurrentBufferIndex)
		{
			_TemporalAntiAliasingGraphicsPipelines[i].Execute();
		}

		else
		{
			_TemporalAntiAliasingGraphicsPipelines[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == _TemporalAntiAliasingGraphicsPipelines.Size() - 1 ? 0 : _CurrentBufferIndex + 1;
}