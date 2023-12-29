//Header file.
#include <Rendering/Native/RenderPasses/AmbientOcclusionRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AmbientOcclusionRenderPass);

/*
*	Default constructor.
*/
AmbientOcclusionRenderPass::AmbientOcclusionRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::AMBIENT_OCCLUSION, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		AmbientOcclusionRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		
	});
}

/*
*	Initializes this render pass.
*/
void AmbientOcclusionRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Register the input stream(s).
	{
		struct ScreenSpaceAmbientOcclusionPushConstantData final
		{
			uint32 _NumberOfSamples;
		};

		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("ScreenSpaceAmbientOcclusion"),
			DynamicArray< VertexInputAttributeDescription>(),
			DynamicArray<VertexInputBindingDescription>(),
			sizeof(ScreenSpaceAmbientOcclusionPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				//Clear the entries.
				input_stream->_Entries.Clear();

				//Clear the push constant data memory.
				input_stream->_PushConstantDataMemory.Clear();

				//Add an entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._VertexBuffer = EMPTY_HANDLE;
				new_entry._IndexBuffer = EMPTY_HANDLE;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 3;
				new_entry._IndexCount = 0;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				ScreenSpaceAmbientOcclusionPushConstantData push_constant_data;

				switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionQuality())
				{
					case RenderingConfiguration::AmbientOcclusionQuality::LOW:
					{
						push_constant_data._NumberOfSamples = 4;

						break;
					}

					case RenderingConfiguration::AmbientOcclusionQuality::MEDIUM:
					{
						push_constant_data._NumberOfSamples = 8;

						break;
					}

					case RenderingConfiguration::AmbientOcclusionQuality::HIGH:
					{
						push_constant_data._NumberOfSamples = 16;

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				for (uint64 i{ 0 }; i < sizeof(ScreenSpaceAmbientOcclusionPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			},
			RenderInputStream::Mode::DRAW,
			nullptr
		);
	}

	//Create the ambient occlusion render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::R_UINT8, SampleCount::SAMPLE_COUNT_1, &_AmbientOcclusionRenderTarget);

	//Create the intermediate ambient occlusion render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::R_UINT8, SampleCount::SAMPLE_COUNT_1, &_IntermediateAmbientOcclusionRenderTarget);

	//Create the ambient occlusion temporal buffer render targets.
	for (RenderTargetHandle &render_target : _AmbientOcclusionTemporalBufferRenderTargets)
	{
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::R_UINT8, SampleCount::SAMPLE_COUNT_1, &render_target);
	}

	//Add the pipelines.
	SetNumberOfPipelines(1 + 1 + _AmbientOcclusionSpatialDenoisingGraphicsPipelines.Size() + _AmbientOcclusionTemporalDenoisingGraphicsPipelines.Size() + 1);
	AddPipeline(&_ScreenSpaceAmbientOcclusionGraphicsPipeline);
	AddPipeline(&_AmbientOcclusionRayTracingPipeline);

	for (GraphicsRenderPipeline &pipeline : _AmbientOcclusionTemporalDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (AmbientOcclusionSpatialDenoisingGraphicsPipeline& pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_AmbientOcclusionApplicationGraphicsPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._OutputRenderTargets.Emplace(HashString("AmbientOcclusion"), _AmbientOcclusionRenderTarget);

		_ScreenSpaceAmbientOcclusionGraphicsPipeline.Initialize(parameters);
	}

	_AmbientOcclusionRayTracingPipeline.Initialize(_AmbientOcclusionRenderTarget);

	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), _AmbientOcclusionTemporalBufferRenderTargets[0]);
		parameters._InputRenderTargets.Emplace(HashString("InputAmbientOcclusion"), _AmbientOcclusionRenderTarget);

		parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), _AmbientOcclusionTemporalBufferRenderTargets[1]);
		parameters._OutputRenderTargets.Emplace(HashString("OutputAmbientOcclusion"), _IntermediateAmbientOcclusionRenderTarget);
	
		_AmbientOcclusionTemporalDenoisingGraphicsPipelines[0].Initialize(parameters);
	}

	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), _AmbientOcclusionTemporalBufferRenderTargets[1]);
		parameters._InputRenderTargets.Emplace(HashString("InputAmbientOcclusion"), _AmbientOcclusionRenderTarget);

		parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), _AmbientOcclusionTemporalBufferRenderTargets[0]);
		parameters._OutputRenderTargets.Emplace(HashString("OutputAmbientOcclusion"), _IntermediateAmbientOcclusionRenderTarget);

		_AmbientOcclusionTemporalDenoisingGraphicsPipelines[1].Initialize(parameters);
	}

	for (uint64 i{ 0 }; i < _AmbientOcclusionSpatialDenoisingGraphicsPipelines.Size(); i += 2)
	{
		_AmbientOcclusionSpatialDenoisingGraphicsPipelines[i + 0].Initialize(	_IntermediateAmbientOcclusionRenderTarget,
																				static_cast<int32>(i / 2 + 1),
																				0,
																				_AmbientOcclusionRenderTarget);
		_AmbientOcclusionSpatialDenoisingGraphicsPipelines[i + 1].Initialize(	_AmbientOcclusionRenderTarget,
																				static_cast<int32>(i / 2 + 1),
																				1,
																				_IntermediateAmbientOcclusionRenderTarget);
	}

	_AmbientOcclusionApplicationGraphicsPipeline.Initialize(_IntermediateAmbientOcclusionRenderTarget);
}

/*
*	Executes this render pass.
*/
void AmbientOcclusionRenderPass::Execute() NOEXCEPT
{	
	//Check the ambient occlusion mode.
	{
		const RenderingConfiguration::AmbientOcclusionMode previous_ambient_occlusion_mode{ _AmbientOcclusionMode };
	
		_AmbientOcclusionMode = RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode();
	
		//Should this render pass be initialized?
		if (previous_ambient_occlusion_mode == RenderingConfiguration::AmbientOcclusionMode::NONE
			&& _AmbientOcclusionMode != RenderingConfiguration::AmbientOcclusionMode::NONE)
		{
			Initialize();
			RenderingSystem::Instance->InitializeRenderPass(this);
		}

		else if (previous_ambient_occlusion_mode != RenderingConfiguration::AmbientOcclusionMode::NONE
			&& _AmbientOcclusionMode == RenderingConfiguration::AmbientOcclusionMode::NONE)
		{
			Terminate();
			RenderingSystem::Instance->TerminateRenderPass(this);
		}
	}

	//Nothing to do here if ambient occlusion isn't enabled.
	if (_AmbientOcclusionMode == RenderingConfiguration::AmbientOcclusionMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::SCREEN_SPACE)
	{
		_ScreenSpaceAmbientOcclusionGraphicsPipeline.Execute();
		_AmbientOcclusionRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionMode() == RenderingConfiguration::AmbientOcclusionMode::RAY_TRACED)
	{
		_ScreenSpaceAmbientOcclusionGraphicsPipeline.SetIncludeInRender(false);
		_AmbientOcclusionRayTracingPipeline.Execute();
	}

	if (!RenderingSystem::Instance->IsTakingScreenshot())
	{
		//Execute the current buffer, don't include the rest.
		for (uint64 i{ 0 }, size{ _AmbientOcclusionTemporalDenoisingGraphicsPipelines.Size() }; i < size; ++i)
		{
			if (i == _CurrentTemporalBufferIndex)
			{
				_AmbientOcclusionTemporalDenoisingGraphicsPipelines[i].Execute();
			}

			else
			{
				_AmbientOcclusionTemporalDenoisingGraphicsPipelines[i].SetIncludeInRender(false);
			}
		}

		//Update the current buffer index.
		_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _AmbientOcclusionTemporalDenoisingGraphicsPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

		for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
		{
			pipeline.Execute();
		}
	}
	
	else
	{
		for (GraphicsRenderPipeline &pipeline : _AmbientOcclusionTemporalDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}

		for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
		{
			pipeline.SetIncludeInRender(false);
		}
	}

	_AmbientOcclusionApplicationGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void AmbientOcclusionRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Terminate();
	_AmbientOcclusionRayTracingPipeline.Terminate();

	for (AmbientOcclusionSpatialDenoisingGraphicsPipeline &pipeline : _AmbientOcclusionSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (GraphicsRenderPipeline&pipeline : _AmbientOcclusionTemporalDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_AmbientOcclusionApplicationGraphicsPipeline.Terminate();

	//Destroy the ambient occlusion render target.
	RenderingSystem::Instance->DestroyRenderTarget(&_AmbientOcclusionRenderTarget);

	//Destroy the ambient occlusion temporal buffer render targets.
	for (RenderTargetHandle &render_target : _AmbientOcclusionTemporalBufferRenderTargets)
	{
		RenderingSystem::Instance->DestroyRenderTarget(&render_target);
	}
}