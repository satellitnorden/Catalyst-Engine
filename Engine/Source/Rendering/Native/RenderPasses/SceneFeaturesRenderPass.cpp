//Header file.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

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
	SetNumberOfPipelines(13);
	AddPipeline(&_ParticleSystemComputePipeline);

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

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
	_OpaqueModelSceneFeaturesGraphicsPipelines[0].Initialize(_SceneDepthBuffer, false);
	_OpaqueModelSceneFeaturesGraphicsPipelines[1].Initialize(_SceneDepthBuffer, true);
	_AnimatedModelSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
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
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetRenderingPath() != RenderingConfiguration::RenderingPath::MAIN)
	{
		SetEnabled(false);

		return;
	}

	else
	{
		SetEnabled(true);
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
	
	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_AnimatedModelSceneFeaturesGraphicsPipeline.Execute();
	_VelocityGraphicsPipeline.Execute();
}