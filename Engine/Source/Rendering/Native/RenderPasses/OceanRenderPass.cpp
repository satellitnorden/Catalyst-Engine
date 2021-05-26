#if !defined(CATALYST_SIMPLIFIED_RENDERING)
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
	SetStage(NativeRenderPassStage::OCEAN);

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
	_SceneFeatures1CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1), RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_UINT8));
	_SceneFeatures2CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2), RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1));
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
	//Selectively enable this rendering path.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetRenderingPath() != RenderingConfiguration::RenderingPath::MAIN/* || true*/)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
	}

	//Execute all pipelines.
	_SceneFeatures1CopyGraphicsPipeline.Execute();
	_SceneFeatures2CopyGraphicsPipeline.Execute();
	_OceanSceneFeaturesGraphicsPipeline.Execute();
}
#endif