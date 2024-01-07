//Header file.
#include <Rendering/Native/RenderPasses/AntiAliasingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AntiAliasingRenderPass);

/*
*	Default constructor.
*/
AntiAliasingRenderPass::AntiAliasingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::ANTI_ALIASING, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::ANTI_ALIASING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		AntiAliasingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		AntiAliasingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		AntiAliasingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void AntiAliasingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Set up differently depending on the anti aliasing mode.
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode())
	{
		case RenderingConfiguration::AntiAliasingMode::NONE:
		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
		{
			//Initialize and add the pipelines.
			SetNumberOfPipelines(1);

			_FastApproximateAntiAliasingGraphicsPipeline.Initialize
			(
				RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode() == RenderingConfiguration::AntiAliasingMode::NONE,
				RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1),
				RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2)
			);

			AddPipeline(&_FastApproximateAntiAliasingGraphicsPipeline);

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
		{
			//Create the temporal anti aliasing render targets.
			for (RenderTargetHandle& render_target : _TemporalAntiAliasingRenderTargets)
			{
				RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_UINT8, SampleCount::SAMPLE_COUNT_1, &render_target);
			}

			//Initialize and add the pipelines.
			SetNumberOfPipelines(_TemporalAntiAliasingPipelines.Size());

			for (GraphicsRenderPipeline &pipeline : _TemporalAntiAliasingPipelines)
			{
				AddPipeline(&pipeline);
			}

			for (uint64 i{ 0 }; i < _TemporalAntiAliasingPipelines.Size(); ++i)
			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("SceneNearest"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._InputRenderTargets.Emplace(HashString("SceneLinear"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), CatalystBaseMath::IsEven(i) ? _TemporalAntiAliasingRenderTargets[0] : _TemporalAntiAliasingRenderTargets[1]);
				
				parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), CatalystBaseMath::IsEven(i) ? _TemporalAntiAliasingRenderTargets[1] : _TemporalAntiAliasingRenderTargets[0]);

				_TemporalAntiAliasingPipelines[i].Initialize(parameters);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set the current anti aliasing mode.
	_CurrentAntiAliasingMode = RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode();
}

/*
*	Executes this render pass.
*/
void AntiAliasingRenderPass::Execute() NOEXCEPT
{
	//Check anti aliasing mode.
	if (_CurrentAntiAliasingMode != RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode())
	{
		Terminate();
		RenderingSystem::Instance->TerminateRenderPass(this);

		Initialize();
	}

	//Execute differently depending on anti aliasing mode.
	switch (_CurrentAntiAliasingMode)
	{
		case RenderingConfiguration::AntiAliasingMode::NONE:
		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
		{
			//Exedcute all pipelines.
			_FastApproximateAntiAliasingGraphicsPipeline.Execute();

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
		{
			//Calculate the weight override/weight override weight depending on if the rendering system is taking a screenshot.
			float32 weight_override;
			float32 weight_override_weight;

			if (RenderingSystem::Instance->IsTakingScreenshot())
			{
				++_TemporalAntiAliasingNumberOfAccumulations;

				weight_override = 1.0f - (1.0f / static_cast<float32>(_TemporalAntiAliasingNumberOfAccumulations));
				weight_override_weight = 1.0f;
			}

			else
			{
				_TemporalAntiAliasingNumberOfAccumulations = 0;

				weight_override = RenderingSystem::Instance->GetPostProcessingSystem()->GetTemporalAntiAliasingWeightOverride();
				weight_override_weight = RenderingSystem::Instance->GetPostProcessingSystem()->GetTemporalAntiAliasingWeightOverride() ? 1.0f : 0.0f;
			}

			//Execute the current buffer, don't include the rest.
			for (uint64 i{ 0 }, size{ _TemporalAntiAliasingPipelines.Size() }; i < size; ++i)
			{
				if (i == _TemporalAntiAliasingCurrentBufferIndex)
				{
					_TemporalAntiAliasingPipelines[i].Execute();
				}

				else
				{
					_TemporalAntiAliasingPipelines[i].SetIncludeInRender(false);
				}
			}

			//Update the current buffer index.
			_TemporalAntiAliasingCurrentBufferIndex = _TemporalAntiAliasingCurrentBufferIndex == _TemporalAntiAliasingPipelines.Size() - 1 ? 0 : _TemporalAntiAliasingCurrentBufferIndex + 1;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Terminates this render pass.
*/
void AntiAliasingRenderPass::Terminate() NOEXCEPT
{
	switch (_CurrentAntiAliasingMode)
	{
		case RenderingConfiguration::AntiAliasingMode::NONE:
		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
		{
			//Terminate all pipelines.
			_FastApproximateAntiAliasingGraphicsPipeline.Terminate();

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
		{
			//Terminate all pipelines.
			for (GraphicsRenderPipeline &pipeline : _TemporalAntiAliasingPipelines)
			{
				pipeline.Terminate();
			}

			//Destroy the temporal anti aliasing render targets.
			for (RenderTargetHandle& render_target : _TemporalAntiAliasingRenderTargets)
			{
				RenderingSystem::Instance->DestroyRenderTarget(&render_target);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}