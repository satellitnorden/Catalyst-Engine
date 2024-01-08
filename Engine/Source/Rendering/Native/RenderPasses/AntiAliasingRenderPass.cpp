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

	//Register the input stream(s).
	{
		struct AntiAliasingSharpenPushConstantData final
		{
			float32 _SharpenStrength;
			float32 _SharpenClamp;
		};

		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("AntiAliasingSharpen"),
			DynamicArray<VertexInputAttributeDescription>(),
			DynamicArray<VertexInputBindingDescription>(),
			sizeof(AntiAliasingSharpenPushConstantData),
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
				AntiAliasingSharpenPushConstantData push_constant_data;

				switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode())
				{
					case RenderingConfiguration::AntiAliasingMode::NONE:
					{
						push_constant_data._SharpenStrength = 0.0f;
						push_constant_data._SharpenClamp = 1.0f;

						break;
					}

					case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
					{
						push_constant_data._SharpenStrength = 1.0f;
						push_constant_data._SharpenClamp = 1.0f;

						break;
					}

					case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
					{
						push_constant_data._SharpenStrength = 2.0f;
						push_constant_data._SharpenClamp = 1.0f;

						break;
					}

					case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE_PLUS_TEMPORAL:
					{
						push_constant_data._SharpenStrength = 4.0f;
						push_constant_data._SharpenClamp = 1.0f;

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				for (uint64 i{ 0 }; i < sizeof(AntiAliasingSharpenPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			},
			RenderInputStream::Mode::DRAW,
			nullptr
		);
	}

	//Set up differently depending on the anti aliasing mode.
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetAntiAliasingMode())
	{
		case RenderingConfiguration::AntiAliasingMode::NONE:
		{
			//Do nothing. (:

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
		{
			//Add and initialize the pipelines.
			SetNumberOfPipelines(2);

			AddPipeline(&_FastApproximateAntiAliasingPipeline);
			AddPipeline(&_PassthroughPipeline);

			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));

				_FastApproximateAntiAliasingPipeline.Initialize(parameters);
			}

			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));

				_PassthroughPipeline.Initialize(parameters);
			}

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
		{
			//Create the temporal anti aliasing render targets.
			for (RenderTargetHandle &render_target : _TemporalAntiAliasingRenderTargets)
			{
				RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_UINT8, SampleCount::SAMPLE_COUNT_1, &render_target);
			}

			//Add and initialize the pipelines.
			SetNumberOfPipelines(_TemporalAntiAliasingPipelines.Size() + 1);

			for (GraphicsRenderPipeline &pipeline : _TemporalAntiAliasingPipelines)
			{
				AddPipeline(&pipeline);
			}

			AddPipeline(&_SharpenPipeline);

			for (uint64 i{ 0 }; i < _TemporalAntiAliasingPipelines.Size(); ++i)
			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("SceneNearest"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._InputRenderTargets.Emplace(HashString("SceneLinear"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), CatalystBaseMath::IsEven(i) ? _TemporalAntiAliasingRenderTargets[0] : _TemporalAntiAliasingRenderTargets[1]);
				
				parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), CatalystBaseMath::IsEven(i) ? _TemporalAntiAliasingRenderTargets[1] : _TemporalAntiAliasingRenderTargets[0]);
				parameters._OutputRenderTargets.Emplace(HashString("CurrentScene"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));

				_TemporalAntiAliasingPipelines[i].Initialize(parameters);
			}

			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._InputStreamSubscriptions.Emplace(HashString("AntiAliasingSharpen"));

				_SharpenPipeline.Initialize(parameters);
			}

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE_PLUS_TEMPORAL:
		{
			//Create the temporal anti aliasing render targets.
			for (RenderTargetHandle &render_target : _TemporalAntiAliasingRenderTargets)
			{
				RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_UINT8, SampleCount::SAMPLE_COUNT_1, &render_target);
			}

			//Add and initialize the pipelines.
			SetNumberOfPipelines(1 + _TemporalAntiAliasingPipelines.Size() + 1 + 1);

			AddPipeline(&_FastApproximateAntiAliasingPipeline);

			for (GraphicsRenderPipeline& pipeline : _TemporalAntiAliasingPipelines)
			{
				AddPipeline(&pipeline);
			}

			AddPipeline(&_SharpenPipeline);
			AddPipeline(&_PassthroughPipeline);

			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));

				_FastApproximateAntiAliasingPipeline.Initialize(parameters);
			}

			for (uint64 i{ 0 }; i < _TemporalAntiAliasingPipelines.Size(); ++i)
			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("SceneNearest"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
				parameters._InputRenderTargets.Emplace(HashString("SceneLinear"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
				parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), CatalystBaseMath::IsEven(i) ? _TemporalAntiAliasingRenderTargets[0] : _TemporalAntiAliasingRenderTargets[1]);

				parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), CatalystBaseMath::IsEven(i) ? _TemporalAntiAliasingRenderTargets[1] : _TemporalAntiAliasingRenderTargets[0]);
				parameters._OutputRenderTargets.Emplace(HashString("CurrentScene"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));

				_TemporalAntiAliasingPipelines[i].Initialize(parameters);
			}

			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
				parameters._InputStreamSubscriptions.Emplace(HashString("AntiAliasingSharpen"));

				_SharpenPipeline.Initialize(parameters);
			}

			{
				GraphicsRenderPipelineParameters parameters;

				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));

				_PassthroughPipeline.Initialize(parameters);
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
		{
			//Do nothing. (:

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
		{
			//Exedcute all pipelines.
			_FastApproximateAntiAliasingPipeline.Execute();
			_PassthroughPipeline.Execute();

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
		{
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

			//Execute the sharpen pipeline.
			_SharpenPipeline.Execute();

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE_PLUS_TEMPORAL:
		{
			//Execute the fast approximate pipeline.
			_FastApproximateAntiAliasingPipeline.Execute();

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

			//Execute the sharpen pipeline.
			_SharpenPipeline.Execute();

			//Execute the passthrough pipeline.
			_PassthroughPipeline.Execute();

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
		{
			//Do nothing. (:

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE:
		{
			//Terminate all pipelines.
			_FastApproximateAntiAliasingPipeline.Terminate();
			_PassthroughPipeline.Terminate();

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::TEMPORAL:
		{
			//Terminate all pipelines.
			for (GraphicsRenderPipeline &pipeline : _TemporalAntiAliasingPipelines)
			{
				pipeline.Terminate();
			}

			_SharpenPipeline.Terminate();

			//Destroy the temporal anti aliasing render targets.
			for (RenderTargetHandle &render_target : _TemporalAntiAliasingRenderTargets)
			{
				RenderingSystem::Instance->DestroyRenderTarget(&render_target);
			}

			break;
		}

		case RenderingConfiguration::AntiAliasingMode::FAST_APPROXIMATE_PLUS_TEMPORAL:
		{
			//Terminate all pipelines.
			_FastApproximateAntiAliasingPipeline.Terminate();

			for (GraphicsRenderPipeline &pipeline : _TemporalAntiAliasingPipelines)
			{
				pipeline.Terminate();
			}

			_SharpenPipeline.Terminate();
			_PassthroughPipeline.Terminate();

			//Destroy the temporal anti aliasing render targets.
			for (RenderTargetHandle &render_target : _TemporalAntiAliasingRenderTargets)
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