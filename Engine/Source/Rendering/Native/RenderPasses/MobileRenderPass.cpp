//Header file.
#include <Rendering/Native/RenderPasses/MobileRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MobileRenderPass);

/*
*	Default constructor.
*/
MobileRenderPass::MobileRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterMobileRenderPass(MobileNativeRenderPassStage::MOBILE, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		MobileRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		MobileRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		MobileRenderPass::Instance->Terminate();
	});

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Mobile");
#endif
}

/*
*	Initializes this render pass.
*/
void MobileRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetScaledResolution(0), &_SceneDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_MobileGraphicsPipeline);
	AddPipeline(&_UserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_MobileGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_UserInterfaceGraphicsPipeline.Initialize(true);
}

/*
*	Executes this render pass.
*/
void MobileRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_MobileGraphicsPipeline.Execute();
	_UserInterfaceGraphicsPipeline.Execute();

	//Enable this render pass.
	SetEnabled(true);
}

/*
*	Terminates this render pass.
*/
void MobileRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_MobileGraphicsPipeline.Terminate();
	_UserInterfaceGraphicsPipeline.Terminate();
}