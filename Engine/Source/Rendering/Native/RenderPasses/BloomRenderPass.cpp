//Header file.
#include <Rendering/Native/RenderPasses/BloomRenderPass.h>

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
	SetNumberOfPipelines(2 + _BloomResampleGraphicsPipelines.Size());
	AddPipeline(&_BloomIsolationGraphicsPipeline);

	for (ResampleGraphicsPipeline &pipeline : _BloomResampleGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_BloomApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_BloomIsolationGraphicsPipeline.Initialize();

	_BloomResampleGraphicsPipelines[0].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height)),
													RenderingSystem::Instance->GetScaledResolution() / 2);

	_BloomResampleGraphicsPipelines[1].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 2), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 2)),
													RenderingSystem::Instance->GetScaledResolution() / 4);

	_BloomResampleGraphicsPipelines[2].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Quarter_R32G32B32A32_Float),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 4), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 4)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution() / 2);

	_BloomResampleGraphicsPipelines[3].Initialize(	RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float),
													RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene),
													1.0f / Vector2<float>(static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width / 2), static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height / 2)) * 0.5f,
													RenderingSystem::Instance->GetScaledResolution());

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

	for (ResampleGraphicsPipeline &pipeline : _BloomResampleGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_BloomApplicationGraphicsPipeline.Execute();
}