//Header file.
#include <Rendering/Native/RenderPasses/IndirectLightingRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
IndirectLightingRenderPass::IndirectLightingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::INDIRECT_LIGHTING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		IndirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		IndirectLightingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		IndirectLightingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void IndirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);

	AddPipeline(&_IndirectLightingPipeline);
	_IndirectLightingPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void IndirectLightingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	_IndirectLightingPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void IndirectLightingRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines
	_IndirectLightingPipeline.Terminate();
}