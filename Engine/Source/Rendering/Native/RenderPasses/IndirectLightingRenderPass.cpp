//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IndirectLightingRenderPass);

/*
*	Default constructor.
*/
IndirectLightingRenderPass::IndirectLightingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::INDIRECT_LIGHTING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		IndirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		IndirectLightingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		IndirectLightingRenderPass::Instance->Terminate();
	});

	//Reset the temporal indirect lighting buffers.
	for (uint8 i{ 0 }; i < 2; ++i)
	{
		_TemporalIndirectLightingBuffers[i] = EMPTY_HANDLE;
	}
}

/*
*	Initializes this render pass.
*/
void IndirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Determine the resolution.
	Resolution resolution;

	switch (_CurrentIndirectLightingQuality)
	{
		case RenderingConfiguration::IndirectLightingQuality::LOW:
		{
			resolution = RenderingSystem::Instance->GetScaledResolution(1);

			break;
		}

		case RenderingConfiguration::IndirectLightingQuality::HIGH:
		{
			resolution = RenderingSystem::Instance->GetScaledResolution(0);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Create the temporal indirect lighting buffers.
	if (_CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		//Create the temporal indirect lighting buffers.
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			RenderingSystem::Instance->CreateRenderTarget(resolution, TextureFormat::RGBA_FLOAT32, &_TemporalIndirectLightingBuffers[i]);
		}
	}

	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + _IndirectLightingSpatialDenoisingGraphicsPipelines.Size() + _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() + 1);

	if (_CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE)
		{
			AddPipeline(&_ScreenSpaceIndirectLightingGraphicsPipeline);
		}
		
		else if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
		{
			AddPipeline(&_RayTracedIndirectLightingRayTracingPipeline);
		}

		for (IndirectLightingSpatialDenoisingGraphicsPipeline& pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
		{
			AddPipeline(&pipeline);
		}

		for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
		{
			AddPipeline(&pipeline);
		}
	}

	AddPipeline(&_IndirectLightingApplicationGraphicsPipeline);

	//Initialize all pipelines.
	if (_CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE)
		{
			_ScreenSpaceIndirectLightingGraphicsPipeline.Initialize(_CurrentIndirectLightingQuality);
		}

		else if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
		{
			_RayTracedIndirectLightingRayTracingPipeline.Initialize(_CurrentIndirectLightingQuality);
		}

		//Initialize the spatial denoising graphics pipelines.
		switch (_CurrentIndirectLightingQuality)
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				_IndirectLightingSpatialDenoisingGraphicsPipelines[0].Initialize
				(
					1,
					0,
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3_HALF),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
					resolution
				);

				_IndirectLightingSpatialDenoisingGraphicsPipelines[1].Initialize
				(
					1,
					1,
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3_HALF),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
					resolution
				);

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				_IndirectLightingSpatialDenoisingGraphicsPipelines[0].Initialize
				(
					1,
					0,
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
					SceneFeaturesRenderPass::Instance->GetSceneFeatures2RenderTarget(),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2),
					resolution
				);

				_IndirectLightingSpatialDenoisingGraphicsPipelines[1].Initialize
				(
					1,
					1,
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2),
					SceneFeaturesRenderPass::Instance->GetSceneFeatures2RenderTarget(),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3),
					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
					resolution
				);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Initialize the temporal denoising graphics pipelines.
		switch (_CurrentIndirectLightingQuality)
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				_IndirectLightingTemporalDenoisingGraphicsPipelines[0].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX,
																					_TemporalIndirectLightingBuffers[1],
																					_TemporalIndirectLightingBuffers[0],
																					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																					resolution);
				_IndirectLightingTemporalDenoisingGraphicsPipelines[1].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX,
																					_TemporalIndirectLightingBuffers[0],
																					_TemporalIndirectLightingBuffers[1],
																					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																					resolution);

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				_IndirectLightingTemporalDenoisingGraphicsPipelines[0].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_4_RENDER_TARGET_INDEX,
																					_TemporalIndirectLightingBuffers[1],
																					_TemporalIndirectLightingBuffers[0],
																					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
																					resolution);
				_IndirectLightingTemporalDenoisingGraphicsPipelines[1].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_4_RENDER_TARGET_INDEX,
																					_TemporalIndirectLightingBuffers[0],
																					_TemporalIndirectLightingBuffers[1],
																					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
																					resolution);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	_IndirectLightingApplicationGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void IndirectLightingRenderPass::Execute() NOEXCEPT
{	
	//Check the indirect lighting mode/quality.
	{
		_PreviousIndirectLightingMode = _CurrentIndirectLightingMode;
		_CurrentIndirectLightingMode = RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode();

		_PreviousIndirectLightingQuality = _CurrentIndirectLightingQuality;
		_CurrentIndirectLightingQuality = RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality();
	
		//Was indirect lighting settings changed?
		if (_PreviousIndirectLightingMode != _CurrentIndirectLightingMode
			|| _PreviousIndirectLightingQuality != _CurrentIndirectLightingQuality)
		{
			Terminate();
			RenderingSystem::Instance->TerminateRenderPass(this);

			Initialize();
			RenderingSystem::Instance->InitializeRenderPass(this);
		}
	}

	//Execute all pipelines.
	if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE)
	{
		_ScreenSpaceIndirectLightingGraphicsPipeline.Execute();
	}

	else if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
	{
		_RayTracedIndirectLightingRayTracingPipeline.Execute();
	}

	if (!RenderingSystem::Instance->IsTakingScreenshot()
		&& _CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			_IndirectLightingSpatialDenoisingGraphicsPipelines[i].Execute();
		}
	}

	if (_CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			if (i == _CurrentTemporalBufferIndex)
			{
				_IndirectLightingTemporalDenoisingGraphicsPipelines[i].Execute();
			}

			else
			{
				_IndirectLightingTemporalDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
			}
		}
	}

	_IndirectLightingApplicationGraphicsPipeline.Execute();

	//Update the current buffer index.
	_CurrentTemporalBufferIndex ^= static_cast<uint8>(1);
}

/*
*	Terminates this render pass.
*/
void IndirectLightingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines
	_ScreenSpaceIndirectLightingGraphicsPipeline.Terminate();
	_RayTracedIndirectLightingRayTracingPipeline.Terminate();

	for (IndirectLightingSpatialDenoisingGraphicsPipeline &pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_IndirectLightingApplicationGraphicsPipeline.Terminate();

	//Destroy the temporal indirect lighting buffers.
	for (uint8 i{ 0 }; i < 2; ++i)
	{
		if (_TemporalIndirectLightingBuffers[i])
		{
			RenderingSystem::Instance->DestroyRenderTarget(&_TemporalIndirectLightingBuffers[i]);
			_TemporalIndirectLightingBuffers[i] = EMPTY_HANDLE;
		}
	}
}