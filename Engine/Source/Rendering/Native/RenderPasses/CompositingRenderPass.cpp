//Header file.
#include <Rendering/Native/RenderPasses/CompositingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CompositingRenderPass);

/*
*	Default constructor.
*/
CompositingRenderPass::CompositingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::Compositing);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		CompositingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		CompositingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void CompositingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_CompositingGraphicsPipeline);

	//Initialize all pipelines.
	_CompositingGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void CompositingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_CompositingGraphicsPipeline.Execute();
}