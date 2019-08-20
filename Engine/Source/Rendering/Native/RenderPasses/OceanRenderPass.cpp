//Header file.
#include <Rendering/Native/RenderPasses/OceanRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(OceanRenderPass);

/*
*	Default constructor.
*/
OceanRenderPass::OceanRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Ocean);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		OceanRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		OceanRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void OceanRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(2);
	AddPipeline(&_OceanGraphicsPipeline);
	AddPipeline(&_CopyGraphicsPipeline);

	//Initialize all pipelines.
	_OceanGraphicsPipeline.Initialize();
	_CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1), RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void OceanRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_OceanGraphicsPipeline.Execute();
	_CopyGraphicsPipeline.Execute();
}