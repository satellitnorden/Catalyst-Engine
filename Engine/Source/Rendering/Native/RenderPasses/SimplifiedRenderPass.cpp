//Header file.
#include <Rendering/Native/RenderPasses/SimplifiedRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SimplifiedRenderPass);

/*
*	Default constructor.
*/
SimplifiedRenderPass::SimplifiedRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterSimplifiedRenderPass(SimplifiedNativeRenderPassStage::SIMPLIFIED, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SimplifiedRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SimplifiedRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		SimplifiedRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void SimplifiedRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetScaledResolution(0), &_SceneDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(3);
	AddPipeline(&_SimplifiedModelGraphicsPipeline);
	AddPipeline(&_SimplifiedSkyGraphicsPipeline);
	AddPipeline(&_UserInterfaceGraphicsPipeline);

	//Initialize all pipelines.
	_SimplifiedModelGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_SimplifiedSkyGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_UserInterfaceGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void SimplifiedRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_SimplifiedModelGraphicsPipeline.Execute();
	_SimplifiedSkyGraphicsPipeline.Execute();
	_UserInterfaceGraphicsPipeline.Execute();

	//Enable this render pass.
	SetEnabled(true);
}

/*
*	Terminates this render pass.
*/
void SimplifiedRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_SimplifiedModelGraphicsPipeline.Terminate();
	_SimplifiedSkyGraphicsPipeline.Terminate();
	_UserInterfaceGraphicsPipeline.Terminate();
}