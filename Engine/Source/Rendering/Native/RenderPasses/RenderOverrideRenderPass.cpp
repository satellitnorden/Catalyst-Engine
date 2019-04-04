#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
//Header file.
#include <Rendering/Native/RenderPasses/RenderOverrideRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/RenderOverrideGraphicsPipeline.h>

//Singleton definition.
DEFINE_SINGLETON(RenderOverrideRenderPass);

/*
*	Default constructor.
*/
RenderOverrideRenderPass::RenderOverrideRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::RenderOverride);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		RenderOverrideRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		RenderOverrideRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void RenderOverrideRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(RenderOverrideGraphicsPipeline::Instance);

	//Initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Initialize();
	}
}

/*
*	Executes this render pass.
*/
void RenderOverrideRenderPass::Execute() NOEXCEPT
{

}
#endif