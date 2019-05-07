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
	SetStage(RenderPassStage::Compositing);

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
	AddPipeline(&_CompositingComputePipeline);

	//Initialize all pipelines.
	_CompositingComputePipeline.Initialize();

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
	_CompositingComputePipeline.Execute();
}