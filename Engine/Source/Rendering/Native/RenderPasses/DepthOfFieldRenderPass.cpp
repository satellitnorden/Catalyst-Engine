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
	SetNumberOfPipelines(3);

	AddPipeline(&_DepthOfFieldBlurGraphicsPipelines[0]);
	AddPipeline(&_DepthOfFieldBlurGraphicsPipelines[1]);
	AddPipeline(&_DepthOfFieldApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_DepthOfFieldBlurGraphicsPipelines[0].Initialize(	DepthOfFieldBlurGraphicsPipeline::Direction::Horizontal,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1));

	_DepthOfFieldBlurGraphicsPipelines[1].Initialize(	DepthOfFieldBlurGraphicsPipeline::Direction::Vertical,
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1),
														RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2));

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
	if (true)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_DepthOfFieldBlurGraphicsPipelines[0].Execute();
	_DepthOfFieldBlurGraphicsPipelines[1].Execute();
	_DepthOfFieldApplicationGraphicsPipeline.Execute();
}