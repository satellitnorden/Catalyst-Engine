//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(IndirectLightingRenderPass);

//TEMP
#include <Systems/InputSystem.h>

bool USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING{ true };

void UpdateIndirectLightingOptions()
{
	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::F1) == ButtonState::PRESSED)
	{
		USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING = !USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING;

		PRINT_TO_OUTPUT("Indirect lighting temporal denoising is now " << (USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING ? "on." : "off"));
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

	//Reset the temporal reprojection buffer.
	_TemporalReprojectionBuffer = EMPTY_HANDLE;

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

	//Create the specular bias lookup texture.
	{
		//Retrieve the data.
		ResourcePointer<RawDataResource> data{ ResourceSystem::Instance->GetRawDataResource(HashString("Specular_Bias_Lookup_Texture_Raw_Data")) };

		//Create the texture.
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(static_cast<float32 *const RESTRICT>(static_cast<void *const RESTRICT>(data->_Data.Data())), 512, 512, 1, 2), TextureFormat::RG_FLOAT32, TextureUsage::NONE), &_SpecularBiasLookupTexture);

		//Add the texture to the global render data.
		_SpecularBiasLookupTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_SpecularBiasLookupTexture);
	}

	//Create the appropriate render targets.
	if (_CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		//Create the temporal reprojection buffer.
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RG_FLOAT16, &_TemporalReprojectionBuffer);

		//Create the temporal indirect lighting buffers.
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, &_TemporalIndirectLightingBuffers[i]);
		}
	}

	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + 1 + _IndirectLightingTemporalDenoisingGraphicsPipelines.Size() + 1);

	if (_CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
	{
		if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::SCREEN_SPACE)
		{
			AddPipeline(&_ScreenSpaceIndirectLightingGraphicsPipeline);
			AddPipeline(&_ScreenSpaceIndirectLightingResolveGraphicsPipeline);
		}
		
		else if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
		{
			AddPipeline(&_RayTracedIndirectLightingRayTracingPipeline);
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

			switch (_CurrentIndirectLightingQuality)
			{
				case RenderingConfiguration::IndirectLightingQuality::LOW:
				{
					_ScreenSpaceIndirectLightingResolveGraphicsPipeline.Initialize
					(
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
						RenderingSystem::Instance->GetScaledResolution(1),
						_TemporalReprojectionBuffer
					);

					break;
				}

				case RenderingConfiguration::IndirectLightingQuality::HIGH:
				{
					_ScreenSpaceIndirectLightingResolveGraphicsPipeline.Initialize
					(
						RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
						RenderingSystem::Instance->GetScaledResolution(0),
						_TemporalReprojectionBuffer
					);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		else if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
		{
			_RayTracedIndirectLightingRayTracingPipeline.Initialize(RenderingConfiguration::IndirectLightingQuality::HIGH);
		}

		//Initialize the temporal denoising graphics pipelines.
		_IndirectLightingTemporalDenoisingGraphicsPipelines[0].Initialize(	_TemporalReprojectionBuffer,
																			_TemporalIndirectLightingBuffers[0],
																			_TemporalIndirectLightingBuffers[1]);
		_IndirectLightingTemporalDenoisingGraphicsPipelines[1].Initialize(	_TemporalReprojectionBuffer,
																			_TemporalIndirectLightingBuffers[1],
																			_TemporalIndirectLightingBuffers[0]);
	}

	_IndirectLightingApplicationGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void IndirectLightingRenderPass::Execute() NOEXCEPT
{	
	UpdateIndirectLightingOptions();

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
		_ScreenSpaceIndirectLightingResolveGraphicsPipeline.Execute();
	}

	else if (_CurrentIndirectLightingMode == RenderingConfiguration::IndirectLightingMode::RAY_TRACED)
	{
		_RayTracedIndirectLightingRayTracingPipeline.Execute();
	}

	if (USE_INDIRECT_LIGHTING_TEMPORAL_DENOISING
		&& _CurrentIndirectLightingMode != RenderingConfiguration::IndirectLightingMode::NONE)
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

	else
	{
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			_IndirectLightingTemporalDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
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
	_ScreenSpaceIndirectLightingResolveGraphicsPipeline.Terminate();
	_RayTracedIndirectLightingRayTracingPipeline.Terminate();

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

	//Destroy the specular bias lookup texture.
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_SpecularBiasLookupTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&_SpecularBiasLookupTexture);
}