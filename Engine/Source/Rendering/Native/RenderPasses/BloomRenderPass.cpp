//Header file.
#include <Rendering/Native/RenderPasses/BloomRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Bloom downsample push constant data class definition.
*/
class BloomDownsamplePushConstantData final
{

public:

	//The inverse source resolution.
	Vector2<float32> _InverseSourceResolution;

};

/*
*	Default constructor.
*/
BloomRenderPass::BloomRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::BLOOM, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		BloomRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		BloomRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		BloomRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void BloomRenderPass::Initialize() NOEXCEPT
{
	//Register the "BloomDownsample" input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("BloomDownsample"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(BloomDownsamplePushConstantData),
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
	SetNumberOfPipelines(_BloomDownsamplePipelines.Size() + _BloomUpsamplePipelines.Size() + 1);

	for (GraphicsRenderPipeline &pipeline : _BloomDownsamplePipelines)
	{
		AddPipeline(&pipeline);
	}

	for (GraphicsRenderPipeline &pipeline : _BloomUpsamplePipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_BloomApplicationPipeline);

	//Initialize all pipelines.
	for (uint64 i{ 0 }; i < _BloomDownsamplePipelines.Size(); ++i)
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		switch (i)
		{
			case 0:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_HALF));

				break;
			}

			case 1:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_HALF));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_QUARTER));

				break;
			}

			case 2:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_QUARTER));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_EIGHTH));

				break;
			}

			case 3:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_EIGHTH));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_SIXTEENTH));

				break;
			}

			case 4:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_SIXTEENTH));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_THIRTYSECOND));

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		parameters._RenderResolution = RenderingSystem::Instance->GetScaledResolution(static_cast<uint8>(i + 1));

		_BloomDownsamplePipelines[i].Initialize(parameters);
	}

	for (uint64 i{ 0 }; i < _BloomUpsamplePipelines.Size(); ++i)
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		switch (i)
		{
			case 0:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_THIRTYSECOND));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_SIXTEENTH));

				break;
			}

			case 1:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_SIXTEENTH));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_EIGHTH));

				break;
			}

			case 2:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_EIGHTH));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_QUARTER));

				break;
			}

			case 3:
			{
				parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_QUARTER));
				parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::BLOOM_HALF));

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		parameters._RenderResolution = RenderingSystem::Instance->GetScaledResolution(static_cast<uint8>(4 - i));

		_BloomUpsamplePipelines[i].Initialize(parameters);
	}

	_BloomApplicationPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void BloomRenderPass::Execute() NOEXCEPT
{
	//No need to perform bloom if it's turned off.
	if (!RenderingSystem::Instance->GetPostProcessingSystem()->GetBloomEnabled() || RenderingSystem::Instance->GetPostProcessingSystem()->GetBloomIntensity() <= 0.0f)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	for (uint64 i{ 0 }; i < _BloomDownsamplePipelines.Size(); ++i)
	{
		BloomDownsamplePushConstantData push_constant_data;

		push_constant_data._InverseSourceResolution._X = 1.0f / static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(static_cast<uint8>(i))._Width);
		push_constant_data._InverseSourceResolution._Y = 1.0f / static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(static_cast<uint8>(i))._Height);

		GraphicsRenderPipelineExecuteParameters parameters;

		parameters._PushConstantData = &push_constant_data;

		_BloomDownsamplePipelines[i].Execute(&parameters);
	}

	for (GraphicsRenderPipeline &pipeline : _BloomUpsamplePipelines)
	{
		pipeline.Execute();
	}

	_BloomApplicationPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void BloomRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	for (GraphicsRenderPipeline &pipeline : _BloomDownsamplePipelines)
	{
		pipeline.Terminate();
	}

	for (GraphicsRenderPipeline &pipeline : _BloomUpsamplePipelines)
	{
		pipeline.Terminate();
	}

	_BloomApplicationPipeline.Terminate();
}