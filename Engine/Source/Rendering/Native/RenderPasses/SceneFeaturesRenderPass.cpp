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
	_ClearGraphicsPipeline.Initialize();
	_ParticleSystemComputePipeline.Initialize();
	_MaskedModelDepthSceneFeaturesGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), false);
	_MaskedModelDepthSceneFeaturesGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), true);
	_InstancedStaticModelDepthSceneFeaturesGraphicsPipelines[0].Initialize(false, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_InstancedStaticModelDepthSceneFeaturesGraphicsPipelines[1].Initialize(true, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_InstancedImpostorDepthSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_MaskedModelColorSceneFeaturesGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), false);
	_MaskedModelColorSceneFeaturesGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), true);
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_TerrainSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_OpaqueModelSceneFeaturesGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), false);
	_OpaqueModelSceneFeaturesGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), true);
	_InstancedStaticModelColorSceneFeaturesGraphicsPipelines[0].Initialize(false, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_InstancedStaticModelColorSceneFeaturesGraphicsPipelines[1].Initialize(true, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_AnimatedModelSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_UserInterfaceSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
#endif
	_VelocityGraphicsPipeline.Initialize();
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
}