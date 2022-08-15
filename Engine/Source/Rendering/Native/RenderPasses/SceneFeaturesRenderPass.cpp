//Header file.
#include <Rendering/Native/RenderPasses/SceneFeaturesRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SceneFeaturesRenderPass);

/*
*	Default constructor.
*/
SceneFeaturesRenderPass::SceneFeaturesRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::SCENE_FEATURES, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		SceneFeaturesRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void SceneFeaturesRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the scene features 1 render target.
	RenderingSystem::Instance->CreateRenderTarget(RenderingSystem::Instance->GetScaledResolution(0), TextureFormat::RGBA_UINT8, &_SceneFeatures1RenderTarget);

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
	AddPipeline(&_UserInterfaceSceneFeaturesGraphicsPipeline);
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
	_UserInterfaceSceneFeaturesGraphicsPipeline.Initialize(_SceneDepthBuffer);
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Initialize(_SceneDepthBuffer);
#endif
	_VelocityGraphicsPipeline.Initialize(_SceneDepthBuffer);
}

/*
*	Executes this render pass.
*/
void SceneFeaturesRenderPass::Execute() NOEXCEPT
{	
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
	_UserInterfaceSceneFeaturesGraphicsPipeline.Execute();
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Execute();
#endif
	_VelocityGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void SceneFeaturesRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ClearGraphicsPipeline.Terminate();
	_ParticleSystemComputePipeline.Terminate();

	for (MaskedModelDepthSceneFeaturesGraphicsPipeline &pipeline : _MaskedModelDepthSceneFeaturesGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Terminate();

	for (InstancedStaticModelDepthSceneFeaturesGraphicsPipeline &pipeline : _InstancedStaticModelDepthSceneFeaturesGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_InstancedImpostorDepthSceneFeaturesGraphicsPipeline.Terminate();

	for (MaskedModelColorSceneFeaturesGraphicsPipeline &pipeline : _MaskedModelColorSceneFeaturesGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Terminate();

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (InstancedStaticModelColorSceneFeaturesGraphicsPipeline &pipeline : _InstancedStaticModelColorSceneFeaturesGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_TerrainSceneFeaturesGraphicsPipeline.Terminate();
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Terminate();
	_AnimatedModelSceneFeaturesGraphicsPipeline.Terminate();
	_UserInterfaceSceneFeaturesGraphicsPipeline.Terminate();
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Terminate();
#endif
	_VelocityGraphicsPipeline.Terminate();

	//Destroy the scene features 1 render target.
	RenderingSystem::Instance->DestroyRenderTarget(&_SceneFeatures1RenderTarget);

	//Destroy the scene depth buffer.
	RenderingSystem::Instance->DestroyDepthBuffer(&_SceneDepthBuffer);
}