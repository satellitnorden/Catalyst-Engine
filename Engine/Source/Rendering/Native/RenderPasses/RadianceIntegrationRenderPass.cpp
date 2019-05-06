//Header file.
#include <Rendering/Native/RenderPasses/RadianceIntegrationRenderPass.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/RadianceIntegrationGraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(RadianceIntegrationRenderPass);

/*
*	Default constructor.
*/
RadianceIntegrationRenderPass::RadianceIntegrationRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::RadianceIntegration);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		RadianceIntegrationRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		RadianceIntegrationRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void RadianceIntegrationRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(RadianceIntegrationGraphicsPipeline::Instance.Get());

	//Initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Initialize();
	}

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void RadianceIntegrationRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->Execute();
	}
}