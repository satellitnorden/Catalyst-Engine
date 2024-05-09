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
	NativeRenderPassManager::RegisterPathTracingRenderPass(PathTracingNativeRenderPassStage::DEBUG_RENDERING, this);

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
		1
#if defined(CATALYST_EDITOR)
		+ _EditorGraphicsRenderPipelines.Size()
#endif
	);

#if defined(CATALYST_EDITOR)
	for (GraphicsRenderPipeline &pipeline : _EditorGraphicsRenderPipelines)
	{
		AddPipeline(&pipeline);
	}
#endif

	//Initialize all pipelines.
#if defined(CATALYST_EDITOR)
	for (GraphicsRenderPipeline &pipeline : _EditorGraphicsRenderPipelines)
	{
		pipeline.Initialize();
	}
#endif
}

/*
*	Executes this render pass.
*/
void DebugRenderingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
#if defined(CATALYST_EDITOR)
	for (GraphicsRenderPipeline &pipeline : _EditorGraphicsRenderPipelines)
	{
		pipeline.Execute();
	}
#endif
}

/*
*	Terminates this render pass.
*/
void DebugRenderingRenderPass::Terminate() NOEXCEPT
{
#if defined(CATALYST_EDITOR)
	for (GraphicsRenderPipeline &pipeline : _EditorGraphicsRenderPipelines)
	{
		pipeline.Terminate();
	}
#endif
}
#endif