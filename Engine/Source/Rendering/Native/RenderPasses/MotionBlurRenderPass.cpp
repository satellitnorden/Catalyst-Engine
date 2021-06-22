//Header file.
#include <Rendering/Native/RenderPasses/MotionBlurRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MotionBlurRenderPass);

/*
*	Default constructor.
*/
MotionBlurRenderPass::MotionBlurRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::MOTION_BLUR, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::MOTION_BLUR, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		MotionBlurRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		MotionBlurRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		MotionBlurRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void MotionBlurRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_MotionBlurGraphicsPipeline);

	//Initialize all pipelines.
	_MotionBlurGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void MotionBlurRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_MotionBlurGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void MotionBlurRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_MotionBlurGraphicsPipeline.Terminate();
}