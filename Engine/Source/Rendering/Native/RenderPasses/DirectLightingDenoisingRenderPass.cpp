//Header file.
#include <Rendering/Native/RenderPasses/DirectLightingDenoisingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/DirectLightingDenoisingComputePipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingDenoisingRenderPass);

/*
*	Default constructor.
*/
DirectLightingDenoisingRenderPass::DirectLightingDenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DirectLightingDenoising);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingDenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingDenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingDenoisingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(DirectLightingDenoisingComputePipeline::Instance.Get());

	//Initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Initialize();
	}

	//Initialize this render pass.
	RenderingSystem::Instance->InitializeRenderPass(this);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DirectLightingDenoisingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}