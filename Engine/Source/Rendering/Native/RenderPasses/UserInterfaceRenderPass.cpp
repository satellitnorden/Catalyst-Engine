//Header file.
#include <Rendering/Native/RenderPasses/UserInterfaceRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(UserInterfaceRenderPass);

/*
*	Default constructor.
*/
UserInterfaceRenderPass::UserInterfaceRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::USER_INTERFACE, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::USER_INTERFACE, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		UserInterfaceRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void UserInterfaceRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_UserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_UserInterfaceGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void UserInterfaceRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_UserInterfaceGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void UserInterfaceRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_UserInterfaceGraphicsPipeline.Terminate();
}