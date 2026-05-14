//Header file.
#include <Rendering/Native/RenderPasses/ToneMappingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ToneMappingRenderPass::ToneMappingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::TONE_MAPPING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ToneMappingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ToneMappingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ToneMappingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ToneMappingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_CopyPipeline);
	AddPipeline(&_ToneMappingPipeline);

	//Initialize all pipelines.
	{
		GraphicsRenderPipelineInitializeParameters parameters;

		parameters._InputRenderTargets.Emplace(HashString("InputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE));
		parameters._OutputRenderTargets.Emplace(HashString("OutputRenderTarget"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::PREVIOUS_SCENE));
		parameters._RenderResolution = RenderingSystem::Instance->GetScaledResolution(0);

		_CopyPipeline.Initialize(parameters);
	}

	_ToneMappingPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void ToneMappingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_CopyPipeline.Execute();
	_ToneMappingPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void ToneMappingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_CopyPipeline.Terminate();
	_ToneMappingPipeline.Terminate();
}