//Header file.
#include <Rendering/Native/RenderPasses/UserInterfaceRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(UserInterfaceRenderPass);

/*
*	Default constructor.
*/
UserInterfaceRenderPass::UserInterfaceRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::UserInterface);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		UserInterfaceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		UserInterfaceRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void UserInterfaceRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_UserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_UserInterfaceGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void UserInterfaceRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_UserInterfaceGraphicsPipeline.Execute();
}