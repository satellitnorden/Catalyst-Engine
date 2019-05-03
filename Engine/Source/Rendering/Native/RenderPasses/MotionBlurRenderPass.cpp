//Header file.
#include <Rendering/Native/RenderPasses/MotionBlurRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/MotionBlurGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MotionBlurRenderPass);

/*
*	Default constructor.
*/
MotionBlurRenderPass::MotionBlurRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::MotionBlur);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		MotionBlurRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		MotionBlurRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void MotionBlurRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(MotionBlurGraphicsPipeline::Instance.Get());

	//Initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Initialize();
	}

	//Initialize this render pass.
	RenderingSystem::Instance->InitializeRenderPass(this);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void MotionBlurRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}