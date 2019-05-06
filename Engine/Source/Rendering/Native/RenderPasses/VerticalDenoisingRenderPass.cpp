//Header file.
#include <Rendering/Native/RenderPasses/VerticalDenoisingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VerticalDenoisingGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VerticalDenoisingRenderPass);

/*
*	Default constructor.
*/
VerticalDenoisingRenderPass::VerticalDenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::VerticalDenoising);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		VerticalDenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		VerticalDenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void VerticalDenoisingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(VerticalDenoisingGraphicsPipeline::Instance.Get());

	//Initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Initialize();
	}

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void VerticalDenoisingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}