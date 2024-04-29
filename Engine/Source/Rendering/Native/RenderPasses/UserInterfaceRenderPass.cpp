//Header file.
#include <Rendering/Native/RenderPasses/UserInterfaceRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

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
	SetNumberOfPipelines(2);
	AddPipeline(&_UserInterfaceGraphicsPipeline);
	AddPipeline(&_UserInterfaceTextRenderPipeline);

	//Initialize all pipelines.
	_UserInterfaceGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
	_UserInterfaceTextRenderPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void UserInterfaceRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_UserInterfaceGraphicsPipeline.Execute();
	_UserInterfaceTextRenderPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void UserInterfaceRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_UserInterfaceGraphicsPipeline.Terminate();
	_UserInterfaceTextRenderPipeline.Terminate();
}