//Header file.
#include <Rendering/Native/RenderPasses/OceanRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

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
	SetNumberOfPipelines(3);
	AddPipeline(&_SceneFeatures1CopyGraphicsPipeline);
	AddPipeline(&_SceneFeatures2CopyGraphicsPipeline);
	AddPipeline(&_OceanSceneFeaturesGraphicsPipeline);

	//Initialize all pipelines.
	_SceneFeatures1CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1), RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R8G8B8A8_Byte));
	_SceneFeatures2CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2), RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1));
	_OceanSceneFeaturesGraphicsPipeline.Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer());

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
	_SceneFeatures1CopyGraphicsPipeline.Execute();
	_SceneFeatures2CopyGraphicsPipeline.Execute();
	_OceanSceneFeaturesGraphicsPipeline.Execute();
}