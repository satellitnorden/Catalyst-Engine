//Header file.
#include <Rendering/Native/RenderPasses/DirectLightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingRenderPass);

/*
*	Default constructor.
*/
DirectLightingRenderPass::DirectLightingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::DirectLighting);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_DirectLightingRayTracingPipeline);

	//Initialize all pipelines.
	_DirectLightingRayTracingPipeline.Initialize();

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DirectLightingRenderPass::Execute() NOEXCEPT
{
	_DirectLightingRayTracingPipeline.Execute();
}