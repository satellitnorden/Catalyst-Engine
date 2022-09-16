//Header file.
#include <Rendering/Native/RenderPasses/TemporalAntiAliasingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAntiAliasingRenderPass);

/*
*	Default constructor.
*/
TemporalAntiAliasingRenderPass::TemporalAntiAliasingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::TEMPORAL_ANTI_ALIASING, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::TEMPORAL_ANTI_ALIASING, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		TemporalAntiAliasingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void TemporalAntiAliasingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the render targets.
	for (RenderTargetHandle &render_target : _RenderTargets)
	{
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &render_target);
	}

	//Initialize and add the pipelines.
	SetNumberOfPipelines(_TemporalAntiAliasingGraphicsPipelines.Size());

	_TemporalAntiAliasingGraphicsPipelines[0].Initialize(	_RenderTargets[0],
															_RenderTargets[1]);

	_TemporalAntiAliasingGraphicsPipelines[1].Initialize(	_RenderTargets[1],
															_RenderTargets[0]);

	for (TemporalAntiAliasingGraphicsPipeline &pipeline : _TemporalAntiAliasingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
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

		weight_override = RenderingSystem::Instance->GetPostProcessingSystem()->GetTemporalAntiAliasingWeightOverride();
		weight_override_weight = RenderingSystem::Instance->GetPostProcessingSystem()->GetTemporalAntiAliasingWeightOverride() ? 1.0f : 0.0f;
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

/*
*	Terminates this render pass.
*/
void TemporalAntiAliasingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	for (TemporalAntiAliasingGraphicsPipeline &pipeline : _TemporalAntiAliasingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	//Destroy the render targets.
	for (RenderTargetHandle &render_target : _RenderTargets)
	{
		RenderingSystem::Instance->DestroyRenderTarget(&render_target);
	}
}