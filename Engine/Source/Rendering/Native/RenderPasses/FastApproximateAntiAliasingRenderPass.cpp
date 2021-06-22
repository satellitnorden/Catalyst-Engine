//Header file.
#include <Rendering/Native/RenderPasses/FastApproximateAntiAliasingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(FastApproximateAntiAliasingRenderPass);

/*
*	Default constructor.
*/
FastApproximateAntiAliasingRenderPass::FastApproximateAntiAliasingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::FAST_APPROXIMATE_ANTI_ALIASING, this);
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::FAST_APPROXIMATE_ANTI_ALIASING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		FastApproximateAntiAliasingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		FastApproximateAntiAliasingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		FastApproximateAntiAliasingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void FastApproximateAntiAliasingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_FastApproximateAntiAliasingGraphicsPipeline);

	//Initialize all pipelines.
	_FastApproximateAntiAliasingGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void FastApproximateAntiAliasingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_FastApproximateAntiAliasingGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void FastApproximateAntiAliasingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_FastApproximateAntiAliasingGraphicsPipeline.Terminate();
}