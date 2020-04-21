//Header file.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SceneFeaturesRenderPass);

/*
*	Default constructor.
*/
SceneFeaturesRenderPass::SceneFeaturesRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::SCENE_FEATURES);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void SceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Create the scene depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(RenderingSystem::Instance->GetScaledResolution(0), &_SceneDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(14);
	AddPipeline(&_ParticleSystemComputePipeline);
	AddPipeline(&_ModelSceneFeaturesGraphicsPipeline);
	AddPipeline(&_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationDepthSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationImpostorDepthSceneFeaturesGraphicsPipeline);
	AddPipeline(&_TerrainSceneFeaturesGraphicsPipeline);
	AddPipeline(&_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationOpaqueSingleSidedSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationOpaqueDoubleSidedSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VegetationImpostorColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_AnimatedModelSceneFeaturesGraphicsPipeline);
	AddPipeline(&_ModelHighlightSceneFeaturesGraphicsPipeline);
	AddPipeline(&_VelocityGraphicsPipeline);

	//Initialize all pipelines.
	_ParticleSystemComputePipeline.Initialize();
	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationImpostorDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_TerrainSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationOpaqueSingleSidedSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer, false);
	_VegetationOpaqueDoubleSidedSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer, true);
	_VegetationColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VegetationImpostorColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_ModelSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_AnimatedModelSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_ModelHighlightSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_VelocityGraphicsPipeline.Initialize(_SceneDepthBuffer);

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void SceneFeaturesRenderPass::Execute() NOEXCEPT
{	
	//Selectively enable this rendering path.
	if (RenderingConfigurationManager::Instance->GetRenderingPath() != RenderingConfigurationManager::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	//Execute all pipelines.
	_ParticleSystemComputePipeline.Execute();
	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Execute();
	_VegetationDepthSceneFeaturesGraphicsPipeline.Execute();
	_VegetationImpostorDepthSceneFeaturesGraphicsPipeline.Execute();
	_TerrainSceneFeaturesGraphicsPipeline.Execute();
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Execute();
	_VegetationOpaqueSingleSidedSceneFeaturesGraphicsPipeline.Execute();
	_VegetationOpaqueDoubleSidedSceneFeaturesGraphicsPipeline.Execute();
	_VegetationColorSceneFeaturesGraphicsPipeline.Execute();
	_VegetationImpostorColorSceneFeaturesGraphicsPipeline.Execute();
	_ModelSceneFeaturesGraphicsPipeline.Execute();
	_AnimatedModelSceneFeaturesGraphicsPipeline.Execute();
	_ModelHighlightSceneFeaturesGraphicsPipeline.Execute();
	_VelocityGraphicsPipeline.Execute();
}