//Header file.
#include <Rendering/Native/RenderPasses/DepthOfFieldRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DepthOfFieldRenderPass);

/*
*	Default constructor.
*/
DepthOfFieldRenderPass::DepthOfFieldRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DepthOfField);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DepthOfFieldRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DepthOfFieldRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(_DepthOfFieldDownsampleGraphicsPipelines.Size() + 1 + 1 + 1);

	for (ResampleGraphicsPipeline &pipeline : _DepthOfFieldDownsampleGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_DepthOfFieldBokehGraphicsPipeline);
	AddPipeline(&_DepthOfFieldUpsampleGraphicsPipeline);
	AddPipeline(&_DepthOfFieldApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_DepthOfFieldDownsampleGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
															1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height)),
															RenderingSystem::Instance->GetScaledResolution() / 2,
															false);

	_DepthOfFieldDownsampleGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_1),
															1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 2), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 2)),
															RenderingSystem::Instance->GetScaledResolution() / 4,
															false);

	_DepthOfFieldBokehGraphicsPipeline.Initialize();

	_DepthOfFieldUpsampleGraphicsPipeline.Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float_2),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
														1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 4), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 4)) * 0.5f,
														RenderingSystem::Instance->GetScaledResolution() / 2,
														false);

	_DepthOfFieldApplicationGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DepthOfFieldRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	for (ResampleGraphicsPipeline &pipeline : _DepthOfFieldDownsampleGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_DepthOfFieldBokehGraphicsPipeline.Execute();
	_DepthOfFieldUpsampleGraphicsPipeline.Execute();
	_DepthOfFieldApplicationGraphicsPipeline.Execute();
}