//Header file.
#include <Rendering/Native/RenderPasses/PostProcessingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PostProcessingRenderPass);

/*
*	Default constructor.
*/
PostProcessingRenderPass::PostProcessingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::POST_PROCESSING, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::POST_PROCESSING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		PostProcessingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		PostProcessingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		PostProcessingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void PostProcessingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_MotionBlurPipeline);
	AddPipeline(&_PostProcessPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));
		parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
	
		_MotionBlurPipeline.Initialize(parameters);
	}

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_2));
		parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1));

		_PostProcessPipeline.Initialize(parameters);
	}
}

/*
*	Executes this render pass.
*/
void PostProcessingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_MotionBlurPipeline.Execute();
	_PostProcessPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void PostProcessingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_MotionBlurPipeline.Terminate();
	_PostProcessPipeline.Terminate();
}