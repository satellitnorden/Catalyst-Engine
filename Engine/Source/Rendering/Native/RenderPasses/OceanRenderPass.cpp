//Header file.
#include <Rendering/Native/RenderPasses/OceanRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(OceanRenderPass);

/*
*	Default constructor.
*/
OceanRenderPass::OceanRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Ocean);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		OceanRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		OceanRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void OceanRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_OceanGraphicsPipeline);

	//Initialize all pipelines.
	_OceanGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void OceanRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_OceanGraphicsPipeline.Execute();
}