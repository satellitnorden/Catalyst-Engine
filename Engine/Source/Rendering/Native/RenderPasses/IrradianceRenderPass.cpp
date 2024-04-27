//Header file.
#include <Rendering/Native/RenderPasses/IrradianceRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Constants.
#define SPATIAL_DENOISING (1)

//Singleton definition.
DEFINE_SINGLETON(IrradianceRenderPass);

/*
*	Diffuse irradiance spatial denoising push constant data class definition.
*/
class DiffuseIrradianceSpatialDenoisingPushConstantData final
{

public:

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
		HashString("DiffuseIrradianceSpatialDenoisingHorizontal"),
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

				DiffuseIrradianceSpatialDenoisingPushConstantData push_constant_data;

				push_constant_data._Direction = 0;

				input_stream->_PushConstantDataMemory.Upsize<false>(sizeof(DiffuseIrradianceSpatialDenoisingPushConstantData));

				Memory::Copy(input_stream->_PushConstantDataMemory.Data(), &push_constant_data, sizeof(DiffuseIrradianceSpatialDenoisingPushConstantData));
			}
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);

	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("DiffuseIrradianceSpatialDenoisingVertical"),
		DynamicArray< VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(DiffuseIrradianceSpatialDenoisingPushConstantData),
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

				DiffuseIrradianceSpatialDenoisingPushConstantData push_constant_data;

				push_constant_data._Direction = 1;

				input_stream->_PushConstantDataMemory.Upsize<false>(sizeof(DiffuseIrradianceSpatialDenoisingPushConstantData));

				Memory::Copy(input_stream->_PushConstantDataMemory.Data(), &push_constant_data, sizeof(DiffuseIrradianceSpatialDenoisingPushConstantData));
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

	//Add the pipelines.
	SetNumberOfPipelines
	(
		1
		+ _DiffuseIrradianceSpatialDenoisingPipelines.Size()
	);

	AddPipeline(&_RayTracedDiffuseIrradiancePipeline);

	for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	_RayTracedDiffuseIrradiancePipeline.Initialize();

	for (uint64 i{ 0 }; i < _DiffuseIrradianceSpatialDenoisingPipelines.Size(); i += 2)
	{
		{
			GraphicsRenderPipelineParameters parameters;

			parameters._InputRenderTargets.Emplace(HashString("InputDiffuseIrradiance"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));
			parameters._OutputRenderTargets.Emplace(HashString("OutputDiffuseIrradiance"), _IntermediateDiffuseIrradianceRenderTarget);
			parameters._InputStreamSubscriptions.Emplace(HashString("DiffuseIrradianceSpatialDenoisingHorizontal"));

			_DiffuseIrradianceSpatialDenoisingPipelines[i].Initialize(parameters);
		}

		{
			GraphicsRenderPipelineParameters parameters;
			
			parameters._InputRenderTargets.Emplace(HashString("InputDiffuseIrradiance"), _IntermediateDiffuseIrradianceRenderTarget);
			parameters._OutputRenderTargets.Emplace(HashString("OutputDiffuseIrradiance"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(HashString("DiffuseIrradiance")));
			parameters._InputStreamSubscriptions.Emplace(HashString("DiffuseIrradianceSpatialDenoisingVertical"));

			_DiffuseIrradianceSpatialDenoisingPipelines[i + 1].Initialize(parameters);
		}
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

#if SPATIAL_DENOISING
			for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
			{
				pipeline.SetIncludeInRender(true);
				pipeline.Execute();
			}
#else
			for (GraphicsRenderPipeline& pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
			{
				pipeline.SetIncludeInRender(false);
			}
#endif

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

	for (GraphicsRenderPipeline &pipeline : _DiffuseIrradianceSpatialDenoisingPipelines)
	{
		pipeline.Terminate();
	}
}