#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderingRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
DebugRenderingRenderPass::DebugRenderingRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::DEBUG_RENDERING, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DebugRenderingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DebugRenderingRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		DebugRenderingRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void DebugRenderingRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines
	(
		_Pipelines.Size()
	);

	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		pipeline.Initialize();
	}
}

/*
*	Executes this render pass.
*/
void DebugRenderingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		pipeline.Execute();
	}
}

/*
*	Terminates this render pass.
*/
void DebugRenderingRenderPass::Terminate() NOEXCEPT
{
	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		pipeline.Terminate();
	}
}
#endif