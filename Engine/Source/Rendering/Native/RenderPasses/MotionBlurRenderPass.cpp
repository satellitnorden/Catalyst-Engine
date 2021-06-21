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
}

/*
*	Initializes this render pass.
*/
void MotionBlurRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_MotionBlurGraphicsPipeline);

	//Initialize all pipelines.
	_MotionBlurGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void MotionBlurRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_MotionBlurGraphicsPipeline.Execute();
}