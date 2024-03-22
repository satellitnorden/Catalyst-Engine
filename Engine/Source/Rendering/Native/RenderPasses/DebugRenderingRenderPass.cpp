#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderingRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugRenderingRenderPass);

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
	SetNumberOfPipelines(_GraphicsRenderPipelines.Size() + _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines.Size() + 1);

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline &pipeline : _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_DebugRenderSphereGraphicsPipeline);

	//Initialize all pipelines.
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		pipeline.Initialize();
	}

	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[0].Initialize(EMPTY_HANDLE, false, false);
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[1].Initialize(EMPTY_HANDLE, false, true);
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[2].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), true, false);
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[3].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), true, true);
	_DebugRenderSphereGraphicsPipeline.Initialize();
}

/*
*	Executes this render pass.
*/
void DebugRenderingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		pipeline.Execute();
	}

	for (DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline &pipeline : _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_DebugRenderSphereGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void DebugRenderingRenderPass::Terminate() NOEXCEPT
{
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		pipeline.Terminate();
	}

	for (DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline &pipeline : _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_DebugRenderSphereGraphicsPipeline.Terminate();
}
#endif