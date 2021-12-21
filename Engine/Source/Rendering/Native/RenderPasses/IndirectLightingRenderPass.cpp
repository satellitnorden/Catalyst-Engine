//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IndirectLightingRenderPass);

//TEMP
#include <Systems/InputSystem.h>

bool USE_INDIRECT_LIGHTING_SPATIAL_DENOISING{ true };
bool USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING{ true };

void TempIndirectLighting()
{
	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::F1) == ButtonState::PRESSED)
	{
		USE_INDIRECT_LIGHTING_SPATIAL_DENOISING = !USE_INDIRECT_LIGHTING_SPATIAL_DENOISING;
	}

	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::F2) == ButtonState::PRESSED)
	{
		USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING = !USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING;
	}
}
//TEMP

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
}

/*
*	Initializes this render pass.
*/
void IndirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

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

	{
		const uint64 number_of_spatial_denoising_passes{ _IndirectLightingSpatialDenoisingGraphicsPipelines.Size() };
		const uint64 half_number_of_spatial_denoising_passes{ number_of_spatial_denoising_passes / 2 };

		for (uint8 quality_index{ 0 }; quality_index < 2; ++quality_index)
		{
			for (uint8 pass_index{ 0 }; pass_index < half_number_of_spatial_denoising_passes; pass_index += 2)
			{
				const uint64 index{ (quality_index * half_number_of_spatial_denoising_passes) + pass_index };

				if (quality_index == 0)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[index + 0].Initialize
					(
						1 + (pass_index / 2),
						0,
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3_HALF),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
						RenderingSystem::Instance->GetScaledResolution(1)
					);

					_IndirectLightingSpatialDenoisingGraphicsPipelines[index + 1].Initialize
					(
						1 + (pass_index / 2),
						1,
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3_HALF),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
						RenderingSystem::Instance->GetScaledResolution(1)
					);
				}

				else
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[index + 0].Initialize
					(
						1 + (pass_index / 2),
						0,
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2),
						RenderingSystem::Instance->GetScaledResolution(0)
					);

					_IndirectLightingSpatialDenoisingGraphicsPipelines[index + 1].Initialize
					(
						1 + (pass_index / 2),
						1,
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3),
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
						RenderingSystem::Instance->GetScaledResolution(0)
					);
				}
			}
		}
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
}

/*
*	Executes this render pass.
*/
void IndirectLightingRenderPass::Execute() NOEXCEPT
{	
	TempIndirectLighting();

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
		&& RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() != RenderingConfiguration::IndirectLightingMode::NONE
		&& USE_INDIRECT_LIGHTING_SPATIAL_DENOISING)
	{
		const uint64 number_of_spatial_denoising_passes{ _IndirectLightingSpatialDenoisingGraphicsPipelines.Size() };
		const uint64 half_number_of_spatial_denoising_passes{ number_of_spatial_denoising_passes / 2 };

		switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingQuality())
		{
			case RenderingConfiguration::IndirectLightingQuality::LOW:
			{
				for (uint64 i{ 0 }; i < half_number_of_spatial_denoising_passes; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i].Execute();
				}

				for (uint64 i{ half_number_of_spatial_denoising_passes }; i < number_of_spatial_denoising_passes; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
				}

				break;
			}

			case RenderingConfiguration::IndirectLightingQuality::HIGH:
			{
				for (uint64 i{ 0 }; i < half_number_of_spatial_denoising_passes; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
				}

				for (uint64 i{ half_number_of_spatial_denoising_passes }; i < number_of_spatial_denoising_passes; ++i)
				{
					_IndirectLightingSpatialDenoisingGraphicsPipelines[i].Execute();
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

	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetIndirectLightingMode() != RenderingConfiguration::IndirectLightingMode::NONE
		&& USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING)
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

/*
*	Terminates this render pass.
*/
void IndirectLightingRenderPass::Terminate() NOEXCEPT
{
	for (ScreenSpaceIndirectLightingGraphicsPipeline &pipeline : _ScreenSpaceIndirectLightingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (RayTracedIndirectLightingRayTracingPipeline &pipeline : _RayTracedIndirectLightingRayTracingPipelines)
	{
		pipeline.Terminate();
	}

	for (IndirectLightingSpatialDenoisingGraphicsPipeline &pipeline : _IndirectLightingSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (IndirectLightingTemporalDenoisingGraphicsPipeline &pipeline : _IndirectLightingTemporalDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_IndirectLightingApplicationGraphicsPipeline.Terminate();
}