#if defined(CATALYST_EDITOR)
//Header file.
#include <Rendering/Native/RenderPasses/EditorUserInterfaceRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EditorUserInterfaceRenderPass);

/*
*	Default constructor.
*/
EditorUserInterfaceRenderPass::EditorUserInterfaceRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::EDITOR_USER_INTERFACE, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::EDITOR_USER_INTERFACE, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		EditorUserInterfaceRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void EditorUserInterfaceRenderPass::Initialize() NOEXCEPT
{	
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_EditorUserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_EditorUserInterfaceGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void EditorUserInterfaceRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_EditorUserInterfaceGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void EditorUserInterfaceRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_EditorUserInterfaceGraphicsPipeline.Terminate();
}
#endif