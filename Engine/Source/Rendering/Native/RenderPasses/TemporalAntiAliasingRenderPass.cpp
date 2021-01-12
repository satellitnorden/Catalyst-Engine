//Header file.
#include <Rendering/Native/RenderPasses/TemporalAntiAliasingRenderPass.h>

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
	SetStage(RenderPassStage::TEMPORAL_ANTI_ALIASING);

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

	_TemporalAntiAliasingGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_2),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_1));

	_TemporalAntiAliasingGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_ANTI_ALIASING_BUFFER_2));

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
	//Calculate the weight override/weight override weight depending on if the rendering system is taking a screenshot.
	float32 weight_override;
	float32 weight_override_weight;

	if (RenderingSystem::Instance->IsTakingScreenshot())
	{
		++_NumberOfAccumulations;

		weight_override = 1.0f - (1.0f / static_cast<float32>(_NumberOfAccumulations));
		weight_override_weight = 1.0f;
	}

	else
	{
		_NumberOfAccumulations = 0;

		weight_override = 0.8f;
		weight_override_weight = 1.0f;
	}

	//Execute the current buffer, don't include the rest.
	for (uint64 i{ 0 }, size{ _TemporalAntiAliasingGraphicsPipelines.Size() }; i < size; ++i)
	{
		if (i == _CurrentBufferIndex)
		{
			_TemporalAntiAliasingGraphicsPipelines[i].Execute(weight_override, weight_override_weight);
		}

		else
		{
			_TemporalAntiAliasingGraphicsPipelines[i].SetIncludeInRender(false);
		}
	}

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == _TemporalAntiAliasingGraphicsPipelines.Size() - 1 ? 0 : _CurrentBufferIndex + 1;
}