//Header file.
#include <Rendering/Native/RenderPasses/DenoisingRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DenoisingGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DenoisingRenderPass);

/*
*	Default constructor.
*/
DenoisingRenderPass::DenoisingRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Denoising);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DenoisingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DenoisingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DenoisingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(DenoisingGraphicsPipeline::Instance.Get());

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
void DenoisingRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}