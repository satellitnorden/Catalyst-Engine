#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/DebugRenderingRenderPass.h>

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
	SetStage(RenderPassStage::DEBUG_RENDERING);

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
	SetNumberOfPipelines(2);
	AddPipeline(&_DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline);
	AddPipeline(&_DebugRenderSphereGraphicsPipeline);

	//Initialize all pipelines.
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline.Initialize();
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
	_DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline.Execute();
	_DebugRenderSphereGraphicsPipeline.Execute();
}
#endif