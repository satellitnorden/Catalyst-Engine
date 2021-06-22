//Header file.
#include <Rendering/Native/RenderPasses/OceanRenderPass.h>

//Rendering.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(OceanRenderPass);

/*
*	Default constructor.
*/
OceanRenderPass::OceanRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::OCEAN, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		OceanRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void OceanRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(3);
	AddPipeline(&_SceneFeatures1CopyGraphicsPipeline);
	AddPipeline(&_SceneFeatures2CopyGraphicsPipeline);
	AddPipeline(&_OceanSceneFeaturesGraphicsPipeline);

	//Initialize all pipelines.
	_SceneFeatures1CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1), RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_UINT8));
	_SceneFeatures2CopyGraphicsPipeline.Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2), RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1));
	_OceanSceneFeaturesGraphicsPipeline.Initialize(SceneFeaturesRenderPass::Instance->GetSceneDepthBuffer());
}

/*
*	Executes this render pass.
*/
void OceanRenderPass::Execute() NOEXCEPT
{
	//Selectively enable this rendering path.
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::DEFAULT)
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

/*
*	Terminates this render pass.
*/
void OceanRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_SceneFeatures1CopyGraphicsPipeline.Terminate();
	_SceneFeatures2CopyGraphicsPipeline.Terminate();
	_OceanSceneFeaturesGraphicsPipeline.Terminate();
}