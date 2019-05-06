//Header file.
#include <Rendering/Native/RenderPasses/HorizontalDenoisingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/HorizontalDenoisingGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(HorizontalDenoisingRenderPass);

/*
*	Default constructor.
*/
HorizontalDenoisingRenderPass::HorizontalDenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::HorizontalDenoising);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		HorizontalDenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		HorizontalDenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void HorizontalDenoisingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(HorizontalDenoisingGraphicsPipeline::Instance.Get());

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
void HorizontalDenoisingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}