//Header file.
#include <Rendering/Native/RenderPasses/ScreenSpaceAmbientOcclusionRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ScreenSpaceAmbientOcclusionRenderPass);

/*
*	Default constructor.
*/
ScreenSpaceAmbientOcclusionRenderPass::ScreenSpaceAmbientOcclusionRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::ScreenSpaceAmbientOcclusion);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ScreenSpaceAmbientOcclusionRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ScreenSpaceAmbientOcclusionRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void ScreenSpaceAmbientOcclusionRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ScreenSpaceAmbientOcclusionGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void ScreenSpaceAmbientOcclusionRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_ScreenSpaceAmbientOcclusionGraphicsPipeline.Execute();
}