//Header file.
#include <Rendering/Native/RenderPasses/PostSceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
PostSceneFeaturesRenderPass::PostSceneFeaturesRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::POST_SCENE_FEATURES, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::POST_SCENE_FEATURES, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		PostSceneFeaturesRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void PostSceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the depth mip chain.
	for (uint8 i{ 0 }; i < _DepthMipChain.Size(); ++i)
	{
		RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(i), TextureFormat::R_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_DepthMipChain[i]);
	}

	//Add the pipelines.
	SetNumberOfPipelines(_DepthDownsampleGraphicsPipelines.Size() + 1);

	for (DepthDownsampleGraphicsPipeline &pipeline : _DepthDownsampleGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_SceneFeaturesDownsampleGraphicsPipeline);

	//Initialize all pipelines.
	for (uint8 i{ 0 }; i < _DepthDownsampleGraphicsPipelines.Size(); ++i)
	{
		if (i == 0)
		{
			_DepthDownsampleGraphicsPipelines[i].Initialize(i,
															RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2),
															_DepthMipChain[i]);
		}

		else
		{
			_DepthDownsampleGraphicsPipelines[i].Initialize(i,
															_DepthMipChain[i - 1],
															_DepthMipChain[i]);
		}
	}

	_SceneFeaturesDownsampleGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void PostSceneFeaturesRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (DepthDownsampleGraphicsPipeline &pipeline : _DepthDownsampleGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_SceneFeaturesDownsampleGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void PostSceneFeaturesRenderPass::Terminate() NOEXCEPT
{
	//Destroy the depth mip chain.
	for (uint64 i{ 0 }; i < _DepthMipChain.Size(); ++i)
	{
		RenderingSystem::Instance->DestroyRenderTarget(&_DepthMipChain[i]);
	}

	//Terminate all pipelines.
	for (DepthDownsampleGraphicsPipeline &pipeline : _DepthDownsampleGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_SceneFeaturesDownsampleGraphicsPipeline.Terminate();
}