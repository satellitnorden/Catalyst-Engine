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
	AddPipeline(&_MotionBlurPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("SceneNearest"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
		parameters._InputRenderTargets.Emplace(HashString("SceneLinear"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));

		_MotionBlurPipeline.Initialize(parameters);
	}
}

/*
*	Executes this render pass.
*/
void MotionBlurRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_MotionBlurPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void MotionBlurRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_MotionBlurPipeline.Terminate();
}