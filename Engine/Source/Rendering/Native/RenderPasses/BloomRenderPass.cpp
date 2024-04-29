//Header file.
#include <Rendering/Native/RenderPasses/BloomRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
BloomRenderPass::BloomRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::BLOOM, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::BLOOM, this);

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
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1 + _BloomDownsampleGraphicsPipelines.Size() + _BloomUpsampleGraphicsPipelines.Size() + 1);
	AddPipeline(&_BloomIsolationGraphicsPipeline);

	for (ResampleGraphicsPipeline &pipeline : _BloomDownsampleGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (ResampleGraphicsPipeline &pipeline : _BloomUpsampleGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_BloomApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_BloomIsolationGraphicsPipeline.Initialize();

	_BloomDownsampleGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE),
													RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(0)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(0)._Height)),
													RenderingSystem::Instance->GetScaledResolution(1),
													false);

	_BloomDownsampleGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
													RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(1)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(1)._Height)),
													RenderingSystem::Instance->GetScaledResolution(2),
													false);

	_BloomDownsampleGraphicsPipelines[2].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(2)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(2)._Height)),
													RenderingSystem::Instance->GetScaledResolution(3),
													false);

	_BloomDownsampleGraphicsPipelines[3].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(3)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(3)._Height)),
													RenderingSystem::Instance->GetScaledResolution(4),
													false);

	_BloomDownsampleGraphicsPipelines[4].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(4)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(4)._Height)),
													RenderingSystem::Instance->GetScaledResolution(5),
													false);

	_BloomDownsampleGraphicsPipelines[5].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(5)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(5)._Height)),
													RenderingSystem::Instance->GetScaledResolution(6),
													false);

	_BloomDownsampleGraphicsPipelines[6].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HUNDREDTWENTYEIGHTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(6)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(6)._Height)),
													RenderingSystem::Instance->GetScaledResolution(7),
													false);

	_BloomUpsampleGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HUNDREDTWENTYEIGHTH),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(7)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(7)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(6),
													true);

	_BloomUpsampleGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTYFOURTH),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(6)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(6)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(5),
													true);

	_BloomUpsampleGraphicsPipelines[2].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_THIRTYSECOND),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(5)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(5)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(4),
													true);

	_BloomUpsampleGraphicsPipelines[3].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_SIXTEENTH),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(4)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(4)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(3),
													true);

	_BloomUpsampleGraphicsPipelines[4].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_EIGHTH),
													RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(3)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(3)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(2),
													true);

	_BloomUpsampleGraphicsPipelines[5].Initialize(	RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1),
													RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(2)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(2)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(1),
													true);

	_BloomUpsampleGraphicsPipelines[6].Initialize(	RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
													RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution(1)._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution(1)._Height)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(0),
													true);

	_BloomApplicationGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void BloomRenderPass::Execute() NOEXCEPT
{
	//No need to perform bloom if it's turned off.
	if (RenderingSystem::Instance->GetPostProcessingSystem()->GetBloomIntensity() == 0.0f)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_BloomIsolationGraphicsPipeline.Execute();

	for (ResampleGraphicsPipeline &pipeline : _BloomDownsampleGraphicsPipelines)
	{
		pipeline.Execute();
	}

	for (ResampleGraphicsPipeline &pipeline : _BloomUpsampleGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_BloomApplicationGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void BloomRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_BloomIsolationGraphicsPipeline.Terminate();

	for (ResampleGraphicsPipeline &pipeline : _BloomDownsampleGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (ResampleGraphicsPipeline &pipeline : _BloomUpsampleGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_BloomApplicationGraphicsPipeline.Terminate();
}