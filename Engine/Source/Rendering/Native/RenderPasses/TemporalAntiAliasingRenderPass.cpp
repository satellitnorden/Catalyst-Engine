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
	/*
	//Initialize and add the pipelines.
	SetNumberOfPipelines(_TemporalAntiAliasingGraphicsPipelines.Size());

	_TemporalAccumulationGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationColorBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationColorBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer1));

	_TemporalAccumulationGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationColorBuffer1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationColorBuffer2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer2));


	for (uint64 i{ 0 }, size{ _TemporalAccumulationGraphicsPipelines.Size() }; i < size; ++i)
	{
		AddPipeline(&_TemporalAccumulationGraphicsPipelines[i]);
	}

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
	*/

	//Eh. Not enabled for now.
	SetEnabled(false);
}

/*
*	Executes this render pass.
*/
void TemporalAntiAliasingRenderPass::Execute() NOEXCEPT
{
	/*
	//Toggle enabled.
	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._DpadUp == ButtonState::Pressed)
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
	*/
}