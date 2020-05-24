#if defined(CATALYST_EDITOR)
//Header file.
#include <Rendering/Native/RenderPasses/EditorUserInterfaceRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EditorUserInterfaceRenderPass);

/*
*	Default constructor.
*/
EditorUserInterfaceRenderPass::EditorUserInterfaceRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::EDITOR_USER_INTERFACE);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		EditorUserInterfaceRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		EditorUserInterfaceRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void EditorUserInterfaceRenderPass::Initialize() NOEXCEPT
{	
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_EditorUserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_EditorUserInterfaceGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void EditorUserInterfaceRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_EditorUserInterfaceGraphicsPipeline.Execute();
}
#endif