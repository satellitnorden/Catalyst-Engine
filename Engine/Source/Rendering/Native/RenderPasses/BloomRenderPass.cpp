//Header file.
#include <Rendering/Native/RenderPasses/BloomRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(BloomRenderPass);

//Bloom render pass constants.
namespace BloomRenderPassConstants
{
	constexpr float BLOOM_BLUR_SIZE{ 6.0f };
}

/*
*	Default constructor.
*/
BloomRenderPass::BloomRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::Bloom);
#endif

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
}

/*
*	Initializes this render pass.
*/
void BloomRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1 + _BloomDownsampleGraphicsPipelines.Size() + _BloomSeparableBlurGraphicsPipelines.Size() + _BloomUpsampleGraphicsPipelines.Size() + 1);
	AddPipeline(&_BloomIsolationGraphicsPipeline);

	for (ResampleGraphicsPipeline &pipeline : _BloomDownsampleGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (SeparableBlurGraphicsPipeline &pipeline : _BloomSeparableBlurGraphicsPipelines)
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

	_BloomDownsampleGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height)),
													RenderingSystem::Instance->GetScaledResolution() / 2,
													false);

	_BloomDownsampleGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 2), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 2)),
													RenderingSystem::Instance->GetScaledResolution() / 4,
													false);

	_BloomSeparableBlurGraphicsPipelines[0].Initialize(	SeparableBlurGraphicsPipeline::Direction::Horizontal,
														BloomRenderPassConstants::BLOOM_BLUR_SIZE,
														1.0f,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2),
														RenderingSystem::Instance->GetScaledResolution() / 2);

	_BloomSeparableBlurGraphicsPipelines[1].Initialize(	SeparableBlurGraphicsPipeline::Direction::Vertical,
														BloomRenderPassConstants::BLOOM_BLUR_SIZE,
														1.0f,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
														RenderingSystem::Instance->GetScaledResolution() / 2);

	_BloomSeparableBlurGraphicsPipelines[2].Initialize(	SeparableBlurGraphicsPipeline::Direction::Horizontal,
														BloomRenderPassConstants::BLOOM_BLUR_SIZE,
														1.0f,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_2),
														RenderingSystem::Instance->GetScaledResolution() / 4);

	_BloomSeparableBlurGraphicsPipelines[3].Initialize(	SeparableBlurGraphicsPipeline::Direction::Vertical,
														BloomRenderPassConstants::BLOOM_BLUR_SIZE,
														1.0f,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_2),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
														RenderingSystem::Instance->GetScaledResolution() / 4);

	_BloomUpsampleGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 4), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 4)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution() / 2,
													true);

	_BloomUpsampleGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 2), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 2)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution(),
													false);

	_BloomApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void BloomRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_BloomIsolationGraphicsPipeline.Execute();

	_BloomDownsampleGraphicsPipelines[0].Execute();

	_BloomSeparableBlurGraphicsPipelines[0].Execute();
	_BloomSeparableBlurGraphicsPipelines[1].Execute();

	_BloomDownsampleGraphicsPipelines[1].Execute();

	_BloomSeparableBlurGraphicsPipelines[2].Execute();
	_BloomSeparableBlurGraphicsPipelines[3].Execute();

	for (ResampleGraphicsPipeline &pipeline : _BloomUpsampleGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_BloomApplicationGraphicsPipeline.Execute();
}