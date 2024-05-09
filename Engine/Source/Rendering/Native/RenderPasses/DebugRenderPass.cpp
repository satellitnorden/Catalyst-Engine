#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Debug visualization push constant data.
class DebugVisualizationPushConstantData final
{

public:

	//The mode.
	uint32 _Mode;

};

/*
*	Default constructor.
*/
DebugRenderPass::DebugRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::DEBUG, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DebugRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DebugRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		DebugRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void DebugRenderPass::Initialize() NOEXCEPT
{
	//Register the "DebugVisualization" input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("DebugVisualization"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(DebugVisualizationPushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			if (input_stream->_Entries.Empty())
			{
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
			}
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);

	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1 + _RenderPipelines.Size());
	AddPipeline(&_DebugVisualizationPipeline);

	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));

		_DebugVisualizationPipeline.Initialize(parameters);
	}

	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		pipeline.Initialize();
	}
}

/*
*	Executes this render pass.
*/
void DebugRenderPass::Execute() NOEXCEPT
{	
	//Should the visualizationmode be switched?
	if (_CurrentVisualizationMode != _RequestedVisualizationMode)
	{
		switch (_RequestedVisualizationMode)
		{
			case VisualizationMode::NONE:
			{
				_DebugVisualizationPipeline.Terminate();

				break;
			}

			case VisualizationMode::ALBEDO:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::THICKNESS:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::NORMAL:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::DEPTH:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::ROUGHNESS:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::METALLIC:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::AMBIENT_OCCLUSION:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::EMISSIVE:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::DIFFUSE_IRRADIANCE:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::DIFFUSE_IRRADIANCE));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			case VisualizationMode::SPECULAR_IRRADIANCE:
			{
				if (_CurrentVisualizationMode != VisualizationMode::NONE)
				{
					_DebugVisualizationPipeline.Terminate();
				}

				{
					GraphicsRenderPipelineInitializeParameters parameters;

					parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SPECULAR_IRRADIANCE));

					_DebugVisualizationPipeline.Initialize(parameters);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		_CurrentVisualizationMode = _RequestedVisualizationMode;
	}

	//Set execution depending on the current mode.
	if (_CurrentVisualizationMode != VisualizationMode::NONE)
	{
		_DebugVisualizationPipeline.SetIncludeInRender(true);

		{
			DebugVisualizationPushConstantData push_constant_data;

			push_constant_data._Mode = static_cast<uint32>(_CurrentVisualizationMode);

			GraphicsRenderPipelineExecuteParameters parameters;

			parameters._PushConstantData = &push_constant_data;

			_DebugVisualizationPipeline.Execute(&parameters);
		}
	}

	else
	{
		_DebugVisualizationPipeline.SetIncludeInRender(false);
	}

	//Execute all pipelines.
	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		pipeline.Execute();
	}
}

/*
*	Terminates this render pass.
*/
void DebugRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	if (_CurrentVisualizationMode != VisualizationMode::NONE)
	{
		_DebugVisualizationPipeline.Terminate();
	}

	for (GraphicsRenderPipeline &pipeline : _RenderPipelines)
	{
		pipeline.Terminate();
	}
}
#endif