//Header file.
#include <Rendering/Native/RenderPasses/ScreenshotAccumulationRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ScreenshotAccumulationRenderPass::ScreenshotAccumulationRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::SCREENSHOT_ACCUMULATION, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ScreenshotAccumulationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ScreenshotAccumulationRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ScreenshotAccumulationRenderPass::Instance->Terminate();
	});

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Screenshot Accumulation");
#endif
}

/*
*	Initializes this render pass.
*/
void ScreenshotAccumulationRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ScreenshotAccumulationPipeline);

	//Register the input stream.
	{
		struct ScreenshotAccumulationPushConstantData final
		{
			uint32 _NumberOfSamples;
		};

		//Register the "ComputeEighth" input stream.
		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("ScreenshotAccumulation"),
			DynamicArray<VertexInputAttributeDescription>(),
			DynamicArray<VertexInputBindingDescription>(),
			sizeof(ScreenshotAccumulationPushConstantData),
			[](void* const RESTRICT user_data, RenderInputStream* const RESTRICT input_stream)
			{
				input_stream->_Entries.Clear();

				input_stream->_PushConstantDataMemory.Clear();

				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = 0;
				new_entry._DispatchWidth = RenderingSystem::Instance->GetScaledResolution(3)._Width;
				new_entry._DispatchHeight = RenderingSystem::Instance->GetScaledResolution(3)._Height;
				new_entry._DispatchDepth = 1;

				ScreenshotAccumulationPushConstantData push_constant_data;

				push_constant_data._NumberOfSamples = ScreenshotAccumulationRenderPass::Instance->_NumberOfSamples;

				for (uint64 i{ 0 }; i < sizeof(ScreenshotAccumulationPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			},
			RenderInputStream::Mode::DISPATCH,
			nullptr
		);
	}
}

/*
*	Executes this render pass.
*/
void ScreenshotAccumulationRenderPass::Execute() NOEXCEPT
{
	if (RenderingSystem::Instance->IsTakingScreenshot())
	{
		if (!_IsTakingScreenshot)
		{
			RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_AccumulationBuffer);
			_NumberOfSamples = 0;

			{
				ComputeRenderPipelineParameters parameters;

				parameters._ComputeRenderTargets.Emplace(HashString("AccumulationBuffer"), _AccumulationBuffer);

				_ScreenshotAccumulationPipeline.Initialize(parameters);
			}

			_IsTakingScreenshot = true;
		}

		++_NumberOfSamples;

		_ScreenshotAccumulationPipeline.Execute();

		SetEnabled(true);
	}

	else
	{
		if (_IsTakingScreenshot)
		{


			_IsTakingScreenshot = false;
		}

		SetEnabled(false);
	}
}

/*
*	Terminates this render pass.
*/
void ScreenshotAccumulationRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ScreenshotAccumulationPipeline.Terminate();
}