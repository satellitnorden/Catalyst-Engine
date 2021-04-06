//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IndirectLightingRenderPass);

/*
*	Default constructor.
*/
IndirectLightingRenderPass::IndirectLightingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::INDIRECT_LIGHTING);

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
}

/*
*	Initializes this render pass.
*/
void IndirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(_ScreenSpaceIndirectLightingGraphicsPipelines.Size() + _RayTracedIndirectLightingRayTracingPipelines.Size() + _IndirectLightingSpatialDenoisingGraphicsPipelines.Size() + _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() + 1);

	for (ScreenSpaceIndirectLightingGraphicsPipeline &pipeline : _ScreenSpaceIndirectLightingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (RayTracedIndirectLightingRayTracingPipeline &pipeline : _RayTracedIndirectLightingRayTracingPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (IndirectLightingSpatialDenoisingGraphicsPipeline& pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_IndirectLightingApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenSpaceIndirectLightingGraphicsPipelines[0].Initialize(RenderingConfiguration::IndirectLightingQuality::LOW);
	_ScreenSpaceIndirectLightingGraphicsPipelines[1].Initialize(RenderingConfiguration::IndirectLightingQuality::HIGH);
	_RayTracedIndirectLightingRayTracingPipelines[0].Initialize(RenderingConfiguration::IndirectLightingQuality::LOW);
	_RayTracedIndirectLightingRayTracingPipelines[1].Initialize(RenderingConfiguration::IndirectLightingQuality::HIGH);

	for (uint8 i{ 0 }; i < NUMBER_OF_SPATIAL_DENOISING_PASSES; ++i)
	{
		_IndirectLightingSpatialDenoisingGraphicsPipelines[i * 2 + 0].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_3_HALF_RENDER_TARGET_INDEX,
																					i * 2 + 1,
																					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
																					RenderingSystem::Instance->GetScaledResolution(1));
		_IndirectLightingSpatialDenoisingGraphicsPipelines[i * 2 + 1].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_2_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_2_HALF_RENDER_TARGET_INDEX,
																					CatalystShaderConstants::SCENE_FEATURES_3_HALF_RENDER_TARGET_INDEX,
																					i * 2 + 2,
																					RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																					RenderingSystem::Instance->GetScaledResolution(1));
	}

	for (uint8 i{ 0 }; i < NUMBER_OF_SPATIAL_DENOISING_PASSES; ++i)
	{
		_IndirectLightingSpatialDenoisingGraphicsPipelines[NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 + i * 2 + 0].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX,
																															CatalystShaderConstants::SCENE_FEATURES_2_RENDER_TARGET_INDEX,
																															CatalystShaderConstants::SCENE_FEATURES_3_RENDER_TARGET_INDEX,
																															i * 2 + 1,
																															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2),
																															RenderingSystem::Instance->GetScaledResolution(0));
		_IndirectLightingSpatialDenoisingGraphicsPipelines[NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 + i * 2 + 1].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_2_RENDER_TARGET_INDEX,
																															CatalystShaderConstants::SCENE_FEATURES_2_RENDER_TARGET_INDEX,
																															CatalystShaderConstants::SCENE_FEATURES_3_RENDER_TARGET_INDEX,
																															i * 2 + 2,
																															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
																															RenderingSystem::Instance->GetScaledResolution(0));
	}

	_IndirectLightingTemporalDenoisingGraphicsPipelines[0].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::TEMPORAL_INDIRECT_LIGHTING_BUFFER_HALF_2_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_HALF_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																		RenderingSystem::Instance->GetScaledResolution(1));
	_IndirectLightingTemporalDenoisingGraphicsPipelines[1].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::TEMPORAL_INDIRECT_LIGHTING_BUFFER_HALF_1_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::SCENE_FEATURES_4_HALF_RENDER_TARGET_INDEX,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_HALF_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																		RenderingSystem::Instance->GetScaledResolution(1));
	_IndirectLightingTemporalDenoisingGraphicsPipelines[2].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::TEMPORAL_INDIRECT_LIGHTING_BUFFER_FULL_2_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::SCENE_FEATURES_4_RENDER_TARGET_INDEX,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_FULL_1),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
																		RenderingSystem::Instance->GetScaledResolution(0));
	_IndirectLightingTemporalDenoisingGraphicsPipelines[3].Initialize(	CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_1_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::TEMPORAL_INDIRECT_LIGHTING_BUFFER_FULL_1_RENDER_TARGET_INDEX,
																		CatalystShaderConstants::SCENE_FEATURES_4_RENDER_TARGET_INDEX,
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::TEMPORAL_INDIRECT_LIGHTING_BUFFER_FULL_2),
																		RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
																		RenderingSystem::Instance->GetScaledResolution(0));

	_IndirectLightingApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void IndirectLightingRenderPass::Execute() NOEXCEPT
{	
	//Selectively enable this rendering path.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetRenderingPath() != RenderingConfiguration::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() == RenderingConfiguration::IndirectLightingMode::NONE)
	{
		for (ScreenSpaceIndirectLightingGraphicsPipeline &pipeline : _ScreenSpaceIndirectLightingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}

		for (RayTracedIndirectLightingRayTracingPipeline &pipeline : _RayTracedIndirectLightingRayTracingPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() == RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE)
	{
		switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality())
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				_ScreenSpaceIndirectLightingGraphicsPipelines[0].Execute();
				_ScreenSpaceIndirectLightingGraphicsPipelines[1].SetIncludeInRender(false);

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				_ScreenSpaceIndirectLightingGraphicsPipelines[0].SetIncludeInRender(false);
				_ScreenSpaceIndirectLightingGraphicsPipelines[1].Execute();

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		for (RayTracedIndirectLightingRayTracingPipeline &pipeline : _RayTracedIndirectLightingRayTracingPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
	{
		for (ScreenSpaceIndirectLightingGraphicsPipeline &pipeline : _ScreenSpaceIndirectLightingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}

		switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality())
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				_RayTracedIndirectLightingRayTracingPipelines[0].Execute();
				_RayTracedIndirectLightingRayTracingPipelines[1].SetIncludeInRender(false);

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				_RayTracedIndirectLightingRayTracingPipelines[0].SetIncludeInRender(false);
				_RayTracedIndirectLightingRayTracingPipelines[1].Execute();

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	if (!RenderingSystem::Instance->IsTakingScreenshot()
		&& RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality())
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				for (uint8 i{ 0 }; i < NUMBER_OF_SPATIAL_DENOISING_PASSES; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i * 2 + 0].Execute();
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i * 2 + 1].Execute();
				}

				for (uint8 i{ 0 }; i < NUMBER_OF_SPATIAL_DENOISING_PASSES; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 + i * 2 + 0].SetIncludeInRender(false);
					_IndirectLightingSpatialDenoisingGraphicsPipelines[NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 + i * 2 + 1].SetIncludeInRender(false);
				}

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				for (uint8 i{ 0 }; i < NUMBER_OF_SPATIAL_DENOISING_PASSES; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i * 2 + 0].SetIncludeInRender(false);
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i * 2 + 1].SetIncludeInRender(false);
				}

				for (uint8 i{ 0 }; i < NUMBER_OF_SPATIAL_DENOISING_PASSES; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 + i * 2 + 0].Execute();
					_IndirectLightingSpatialDenoisingGraphicsPipelines[NUMBER_OF_SPATIAL_DENOISING_PASSES * 2 + i * 2 + 1].Execute();
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

	else
	{
		for (IndirectLightingSpatialDenoisingGraphicsPipeline& pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	//Execute the current buffer, don't include the rest.
	if (/*!RenderingSystem::Instance->IsTakingScreenshot()
		&& */RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality())
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				for (uint64 i{ 0 }; i < 2; ++i)
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

				_IndirectLightingTemporalDenoisingGraphicsPipelines[2].SetIncludeInRender(false);
				_IndirectLightingTemporalDenoisingGraphicsPipelines[3].SetIncludeInRender(false);

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				for (uint64 i{ 0 }; i < 2; ++i)
				{
					if (i == _CurrentTemporalBufferIndex)
					{
						_IndirectLightingTemporalDenoisingGraphicsPipelines[i + 2].Execute();
					}

					else
					{
						_IndirectLightingTemporalDenoisingGraphicsPipelines[i + 2].SetIncludeInRender(false);
					}
				}

				_IndirectLightingTemporalDenoisingGraphicsPipelines[0].SetIncludeInRender(false);
				_IndirectLightingTemporalDenoisingGraphicsPipelines[1].SetIncludeInRender(false);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
		
	}

	else
	{
		for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	_IndirectLightingApplicationGraphicsPipeline.Execute();

	//Update the current buffer index.
	_CurrentTemporalBufferIndex ^= static_cast<uint8>(1);
}