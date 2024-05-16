//Header file.
#include <Rendering/Native/RenderPasses/IrradianceRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Constants.
#define TEMPORAL_DENOISING (1)
#define SPATIAL_DENOISING (1)

/*
*	Diffuse irradiance spatial denoising push constant data class definition.
*/
class DiffuseIrradianceSpatialDenoisingPushConstantData final
{

public:

	//The stride.
	int32 _Stride;

	//The direction.
	uint32 _Direction;

};

/*
*	Default constructor.
*/
IrradianceRenderPass::IrradianceRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::IRRADIANCE, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		IrradianceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		IrradianceRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		IrradianceRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void IrradianceRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Register the input streams.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("DiffuseIrradianceSpatialDenoising"),
		DynamicArray< VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(DiffuseIrradianceSpatialDenoisingPushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream* const RESTRICT input_stream)
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

	//Create the intermediate diffuse irradiance render target.
	RenderingSystem::Instance->CreateRenderTarget
	(
		RenderingSystem::Instance->GetScaledResolution(1),
		TextureFormat::RGBA_FLOAT32,
		SampleCount::SAMPLE_COUNT_1,
		&_IntermediateDiffuseIrradianceRenderTarget
	);

	//Create the diffuse irradiance temporal buffers.
	for (RenderTargetHandle &temporal_buffer : _DiffuseIrradianceTemporalBuffers)
	{
		RenderingSystem::Instance->CreateRenderTarget
		(
			RenderingSystem::Instance->GetScaledResolution(1),
			TextureFormat::RGBA_FLOAT32,
			SampleCount::SAMPLE_COUNT_1,
			&temporal_buffer
		);
	}

	//Create the screen space specular irradiance data render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(1), TextureFormat::RG_FLOAT16, SampleCount::SAMPLE_COUNT_1, &_ScreenSpaceSpecularIrradianceDataRenderTarget);

	//Add the pipelines.
	SetNumberOfPipelines
	(
		1
		+ _DiffuseIrradianceTemporalDenoisingPipelines.Size()
		+ 1
		+ _DiffuseIrradianceSpatialDenoisingPipelines.Size()
		+ 2
	);

	AddPipeline(&_RayTracedDiffuseIrradiancePipeline);

	for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceTemporalDenoisingPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_PassthroughPipeline);

	for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_ScreenSpaceSpecularIrradiance);
	AddPipeline(&_ScreenSpaceSpecularIrradianceResolve);

	//Initialize all pipelines.
	_RayTracedDiffuseIrradiancePipeline.Initialize();

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), _DiffuseIrradianceTemporalBuffers[0]);
		parameters._InputRenderTargets.Emplace(HashString("InputDiffuseIrradiance"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));
		parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), _DiffuseIrradianceTemporalBuffers[1]);
		parameters._OutputRenderTargets.Emplace(HashString("OutputDiffuseIrradiance"), _IntermediateDiffuseIrradianceRenderTarget);

		_DiffuseIrradianceTemporalDenoisingPipelines[0].Initialize(parameters);
	}

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("PreviousTemporalBuffer"), _DiffuseIrradianceTemporalBuffers[1]);
		parameters._InputRenderTargets.Emplace(HashString("InputDiffuseIrradiance"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));
		parameters._OutputRenderTargets.Emplace(HashString("CurrentTemporalBuffer"), _DiffuseIrradianceTemporalBuffers[0]);
		parameters._OutputRenderTargets.Emplace(HashString("OutputDiffuseIrradiance"), _IntermediateDiffuseIrradianceRenderTarget);

		_DiffuseIrradianceTemporalDenoisingPipelines[1].Initialize(parameters);
	}

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), _IntermediateDiffuseIrradianceRenderTarget);
		parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));

		_PassthroughPipeline.Initialize(parameters);
	}

	for (uint64 i{ 0 }; i < _DiffuseIrradianceSpatialDenoisingPipelines.Size(); i += 2)
	{
		{
			GraphicsRenderPipelineInitializeParameters parameters;

			parameters._InputRenderTargets.Emplace(HashString("InputDiffuseIrradiance"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));
			parameters._OutputRenderTargets.Emplace(HashString("OutputDiffuseIrradiance"), _IntermediateDiffuseIrradianceRenderTarget);

			_DiffuseIrradianceSpatialDenoisingPipelines[i].Initialize(parameters);
		}

		{
			GraphicsRenderPipelineInitializeParameters parameters;
			
			parameters._InputRenderTargets.Emplace(HashString("InputDiffuseIrradiance"), _IntermediateDiffuseIrradianceRenderTarget);
			parameters._OutputRenderTargets.Emplace(HashString("OutputDiffuseIrradiance"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));

			_DiffuseIrradianceSpatialDenoisingPipelines[i + 1].Initialize(parameters);
		}
	}

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._OutputRenderTargets.Emplace(HashString("SpecularIrradianceData"), _ScreenSpaceSpecularIrradianceDataRenderTarget);

		_ScreenSpaceSpecularIrradiance.Initialize(parameters);
	}

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("SpecularIrradianceData"), _ScreenSpaceSpecularIrradianceDataRenderTarget);

		_ScreenSpaceSpecularIrradianceResolve.Initialize(parameters);
	}
}

/*
*	Executes this render pass.
*/
void IrradianceRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetDiffuseIrradianceMode())
	{
		case RenderingConfiguration::DiffuseIrradianceMode::NONE:
		{
			_RayTracedDiffuseIrradiancePipeline.SetIncludeInRender(false);

			for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceTemporalDenoisingPipelines)
			{
				pipeline.SetIncludeInRender(false);
			}

			_PassthroughPipeline.SetIncludeInRender(false);

			for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
			{
				pipeline.SetIncludeInRender(false);
			}

			break;
		}

		case RenderingConfiguration::DiffuseIrradianceMode::RAY_TRACED:
		{
			_RayTracedDiffuseIrradiancePipeline.SetIncludeInRender(true);
			_RayTracedDiffuseIrradiancePipeline.Execute();

			if (TEMPORAL_DENOISING && !RenderingSystem::Instance->IsTakingScreenshot())
			{
				for (uint64 i{ 0 }, size{ _DiffuseIrradianceTemporalDenoisingPipelines.Size() }; i < size; ++i)
				{
					if (i == _CurrentTemporalBufferIndex)
					{
						_DiffuseIrradianceTemporalDenoisingPipelines[i].Execute();
					}

					else
					{
						_DiffuseIrradianceTemporalDenoisingPipelines[i].SetIncludeInRender(false);
					}
				}

				//Update the current buffer index.
				_CurrentTemporalBufferIndex = _CurrentTemporalBufferIndex == _DiffuseIrradianceTemporalDenoisingPipelines.Size() - 1 ? 0 : _CurrentTemporalBufferIndex + 1;

				_PassthroughPipeline.Execute();
			}

			else
			{
				for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceTemporalDenoisingPipelines)
				{
					pipeline.SetIncludeInRender(false);
				}

				_PassthroughPipeline.SetIncludeInRender(false);
			}

			if (SPATIAL_DENOISING && !RenderingSystem::Instance->IsTakingScreenshot())
			{
				for (uint64 i{ 0 }; i < _DiffuseIrradianceSpatialDenoisingPipelines.Size(); ++i)
				{
					_DiffuseIrradianceSpatialDenoisingPipelines[i].SetIncludeInRender(true);

					DiffuseIrradianceSpatialDenoisingPushConstantData push_constant_data;

					push_constant_data._Stride = static_cast<int32>(1 + (i / 2));
					push_constant_data._Direction = BaseMath::IsEven(i) ? 0 : 1;

					GraphicsRenderPipelineExecuteParameters parameters;

					parameters._PushConstantData = &push_constant_data;

					_DiffuseIrradianceSpatialDenoisingPipelines[i].Execute(&parameters);
				}
			}

			else
			{
				for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
				{
					pipeline.SetIncludeInRender(false);
				}
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
	
	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetSpecularIrradianceMode())
	{
		case RenderingConfiguration::SpecularIrradianceMode::NONE:
		{
			_ScreenSpaceSpecularIrradiance.SetIncludeInRender(false);
			_ScreenSpaceSpecularIrradianceResolve.SetIncludeInRender(false);

			break;
		}

		case RenderingConfiguration::SpecularIrradianceMode::SCREEN_SPACE:
		{
			_ScreenSpaceSpecularIrradiance.SetIncludeInRender(true);
			_ScreenSpaceSpecularIrradiance.Execute();

			_ScreenSpaceSpecularIrradianceResolve.SetIncludeInRender(true);
			_ScreenSpaceSpecularIrradianceResolve.Execute();

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
void IrradianceRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_RayTracedDiffuseIrradiancePipeline.Terminate();

	for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceTemporalDenoisingPipelines)
	{
		pipeline.Terminate();
	}

	_PassthroughPipeline.Terminate();

	for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
	{
		pipeline.Terminate();
	}

	_ScreenSpaceSpecularIrradiance.Terminate();
	_ScreenSpaceSpecularIrradianceResolve.Terminate();
}