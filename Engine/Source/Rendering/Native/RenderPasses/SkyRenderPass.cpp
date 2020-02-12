//Header file.
#include <Rendering/Native/RenderPasses/SkyRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SkyRenderPass);

/*
*	Default constructor.
*/
SkyRenderPass::SkyRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Sky);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SkyRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SkyRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SkyRenderPass::Initialize() NOEXCEPT
{
	//Define constants.
	constexpr float CLOUDS_BLUR_SIZE{ 8.0f };

	//Add the pipelines.
	SetNumberOfPipelines(7);
	AddPipeline(&_SkyComputePipeline);
	AddPipeline(&_SkyDownsampleComputePipeline);
	AddPipeline(&_SkyGraphicsPipeline);
	AddPipeline(&_CloudsGraphicsPipeline);
	AddPipeline(&_CloudsBlurGraphicsPipelines[0]);
	AddPipeline(&_CloudsBlurGraphicsPipelines[1]);
	AddPipeline(&_CloudsApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_SkyComputePipeline.Initialize();
	_SkyDownsampleComputePipeline.Initialize();
	_SkyGraphicsPipeline.Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer());
	_CloudsGraphicsPipeline.Initialize();
	_CloudsBlurGraphicsPipelines[0].Initialize(	SeparableBlurGraphicsPipeline::Direction::Horizontal,
												CLOUDS_BLUR_SIZE,
												1.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2),
												RenderingSystem::Instance->GetScaledResolution() / 2);
	_CloudsBlurGraphicsPipelines[1].Initialize(	SeparableBlurGraphicsPipeline::Direction::Vertical,
												CLOUDS_BLUR_SIZE,
												1.0f,
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_2),
												RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float_1),
												RenderingSystem::Instance->GetScaledResolution() / 2);
	_CloudsApplicationGraphicsPipeline.Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer());

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void SkyRenderPass::Execute() NOEXCEPT
{
	_SkyComputePipeline.Execute();
	_SkyDownsampleComputePipeline.Execute();
	_SkyGraphicsPipeline.Execute();

	_CloudsGraphicsPipeline.SetIncludeInRender(false);
	_CloudsBlurGraphicsPipelines[0].SetIncludeInRender(false);
	_CloudsBlurGraphicsPipelines[1].SetIncludeInRender(false);
	_CloudsApplicationGraphicsPipeline.SetIncludeInRender(false);

	/*
	_CloudsGraphicsPipeline.Execute();
	_CloudsBlurGraphicsPipelines[0].Execute();
	_CloudsBlurGraphicsPipelines[1].Execute();
	_CloudsApplicationGraphicsPipeline.Execute();
	*/
}