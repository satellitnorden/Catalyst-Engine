#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderingRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugRenderingRenderPass);

/*
*	Default constructor.
*/
DebugRenderingRenderPass::DebugRenderingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::DEBUG_RENDERING);

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
}

/*
*	Initializes this render pass.
*/
void DebugRenderingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines.Size() + 1);

	for (DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline &pipeline : _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_DebugRenderSphereGraphicsPipeline);

	//Initialize all pipelines.
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[0].Initialize(EMPTY_HANDLE, false, false);
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[1].Initialize(EMPTY_HANDLE, false, true);
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[2].Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer(), true, false);
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines[3].Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer(), true, true);
	_DebugRenderSphereGraphicsPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DebugRenderingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	for (DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline &pipeline : _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_DebugRenderSphereGraphicsPipeline.Execute();
}
#endif