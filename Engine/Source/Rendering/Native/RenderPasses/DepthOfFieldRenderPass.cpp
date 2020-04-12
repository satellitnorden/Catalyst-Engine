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
	SetNumberOfPipelines(1 + _DepthOfFieldFloodFillBlurGraphicsPipelines.Size() + 1);

	AddPipeline(&_DepthOfFieldBokehBlurGraphicsPipeline);

	for (DepthOfFieldFloodFillBlurGraphicsPipeline &pipeline : _DepthOfFieldFloodFillBlurGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_DepthOfFieldApplicationGraphicsPipeline);

	//Initialize all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Initialize();
	_DepthOfFieldFloodFillBlurGraphicsPipelines[0].Initialize(	1,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
	_DepthOfFieldFloodFillBlurGraphicsPipelines[1].Initialize(	2,
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
																RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
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
	//No need to execute if depth of field size is zero.
	if (RenderingSystem::Instance->GetPostProcessingSystem()->GetDepthOfFieldSize() == 0.0f)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	_DepthOfFieldBokehBlurGraphicsPipeline.Execute();

	for (DepthOfFieldFloodFillBlurGraphicsPipeline &pipeline : _DepthOfFieldFloodFillBlurGraphicsPipelines)
	{
		//pipeline.Execute();
		pipeline.SetIncludeInRender(false);
	}

	_DepthOfFieldApplicationGraphicsPipeline.Execute();
}