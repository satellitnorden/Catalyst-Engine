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
	SetStage(NativeRenderPassStage::SCENE_FEATURES);

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
	SetNumberOfPipelines(200
#if defined(CATALYST_EDITOR)
	+ 1
#endif
	);
	AddPipeline(&_ClearGraphicsPipeline);
	AddPipeline(&_ParticleSystemComputePipeline);

	for (MaskedModelDepthSceneFeaturesGraphicsPipeline &pipeline : _MaskedModelDepthSceneFeaturesGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline);

	for (InstancedStaticModelDepthSceneFeaturesGraphicsPipeline &pipeline : _InstancedStaticModelDepthSceneFeaturesGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_InstancedImpostorDepthSceneFeaturesGraphicsPipeline);

	for (MaskedModelColorSceneFeaturesGraphicsPipeline &pipeline : _MaskedModelColorSceneFeaturesGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_InstancedImpostorColorSceneFeaturesGraphicsPipeline);

	AddPipeline(&_TerrainSceneFeaturesGraphicsPipeline);

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	for (InstancedStaticModelColorSceneFeaturesGraphicsPipeline &pipeline : _InstancedStaticModelColorSceneFeaturesGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_AnimatedModelSceneFeaturesGraphicsPipeline);
#if defined(CATALYST_EDITOR)
	AddPipeline(&_EditorSelectedModelGraphicsPipeline);
#endif
	AddPipeline(&_VelocityGraphicsPipeline);

	//Initialize all pipelines.
	_ClearGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_ParticleSystemComputePipeline.Initialize();
	_MaskedModelDepthSceneFeaturesGraphicsPipelines[0].Initialize(_SceneDepthBuffer, false);
	_MaskedModelDepthSceneFeaturesGraphicsPipelines[1].Initialize(_SceneDepthBuffer, true);
	_InstancedStaticModelDepthSceneFeaturesGraphicsPipelines[0].Initialize(false, _SceneDepthBuffer);
	_InstancedStaticModelDepthSceneFeaturesGraphicsPipelines[1].Initialize(true, _SceneDepthBuffer);
	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_InstancedImpostorDepthSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_MaskedModelColorSceneFeaturesGraphicsPipelines[0].Initialize(_SceneDepthBuffer, false);
	_MaskedModelColorSceneFeaturesGraphicsPipelines[1].Initialize(_SceneDepthBuffer, true);
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_TerrainSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
	_OpaqueModelSceneFeaturesGraphicsPipelines[0].Initialize(_SceneDepthBuffer, false);
	_OpaqueModelSceneFeaturesGraphicsPipelines[1].Initialize(_SceneDepthBuffer, true);
	_InstancedStaticModelColorSceneFeaturesGraphicsPipelines[0].Initialize(false, _SceneDepthBuffer);
	_InstancedStaticModelColorSceneFeaturesGraphicsPipelines[1].Initialize(true, _SceneDepthBuffer);

	_AnimatedModelSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Initialize(_SceneDepthBuffer);
#endif
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
	_ClearGraphicsPipeline.Execute();
	_ParticleSystemComputePipeline.Execute();

	for (MaskedModelDepthSceneFeaturesGraphicsPipeline &pipeline : _MaskedModelDepthSceneFeaturesGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Execute();

	for (InstancedStaticModelDepthSceneFeaturesGraphicsPipeline &pipeline : _InstancedStaticModelDepthSceneFeaturesGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_InstancedImpostorDepthSceneFeaturesGraphicsPipeline.Execute();

	for (MaskedModelColorSceneFeaturesGraphicsPipeline &pipeline : _MaskedModelColorSceneFeaturesGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Execute();

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
	{
		pipeline.Execute();
	}

	for (InstancedStaticModelColorSceneFeaturesGraphicsPipeline &pipeline : _InstancedStaticModelColorSceneFeaturesGraphicsPipelines)
	{
		pipeline.Execute();
	}

	_TerrainSceneFeaturesGraphicsPipeline.Execute();
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Execute();
	
	_AnimatedModelSceneFeaturesGraphicsPipeline.Execute();
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Execute();
#endif
	_VelocityGraphicsPipeline.Execute();
}