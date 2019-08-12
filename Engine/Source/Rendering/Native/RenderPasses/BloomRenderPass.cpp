//Header file.
#include <Rendering/Native/RenderPasses/BloomRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(BloomRenderPass);

/*
*	Default constructor.
*/
BloomRenderPass::BloomRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Bloom);

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
	//No need to perform bloom if it's turned off.
	if (RenderingConfigurationManager::Instance->GetBloomIntensity() == 0.0f)
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