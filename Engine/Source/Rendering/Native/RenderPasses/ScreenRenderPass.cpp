//Header file.
#include <Rendering/Native/RenderPasses/ScreenRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ScreenRenderPass);

/*
*	Default constructor.
*/
ScreenRenderPass::ScreenRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::SCREEN, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::SCREEN, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ScreenRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ScreenRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ScreenRenderPass::Instance->Terminate();
	});

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Screen");
#endif
}

/*
*	Initializes this render pass.
*/
void ScreenRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ScreenGraphicsPipeline);

	//Initialize all pipelines.
	_ScreenGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void ScreenRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_ScreenGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void ScreenRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ScreenGraphicsPipeline.Terminate();
}