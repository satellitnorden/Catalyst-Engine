//Header file.
#include <Rendering/Native/RenderPasses/WaterRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
WaterRenderPass::WaterRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::WATER, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		WaterRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		WaterRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		WaterRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void WaterRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the scene render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_SceneRenderTarget);

	//Add the pipelines.
	SetNumberOfPipelines(2);

	AddPipeline(&_CopyRenderPipeline);
	AddPipeline(&_WaterRenderPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE));
		parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), _SceneRenderTarget);
		parameters._RenderResolution = RenderingSystem::Instance->GetScaledResolution(0);

		_CopyRenderPipeline.Initialize(parameters);
	}

	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("SceneInput"), _SceneRenderTarget);

		_WaterRenderPipeline.Initialize(parameters);
	}
}

/*
*	Executes this render pass.
*/
void WaterRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_CopyRenderPipeline.Execute();
	_WaterRenderPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void WaterRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_CopyRenderPipeline.Terminate();
	_WaterRenderPipeline.Terminate();
}