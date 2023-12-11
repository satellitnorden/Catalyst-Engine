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

	AddPipeline(&_ParticleSystemComputePipeline);

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines1)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline);

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines2)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_InstancedImpostorColorSceneFeaturesGraphicsPipeline);
	AddPipeline(&_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline);

	AddPipeline(&_TerrainSceneFeaturesGraphicsPipeline);

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
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
	_ParticleSystemComputePipeline.Initialize();

	for (GraphicsRenderPipeline& pipeline : _GraphicsRenderPipelines1)
	{
		pipeline.Initialize();
	}

	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines2)
	{
		pipeline.Initialize();
	}
	
	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_ParticleSystemMaskedColorSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_TerrainSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
	_OpaqueModelSceneFeaturesGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), false);
	_OpaqueModelSceneFeaturesGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER), true);
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
	_ParticleSystemComputePipeline.Execute();

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines1)
	{
		pipeline.Execute();
	}

	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Execute();

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines2)
	{
		pipeline.Execute();
	}

	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Execute();

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
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
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines1)
	{
		pipeline.Terminate();
	}

	_ParticleSystemComputePipeline.Terminate();
	_ParticleSystemMaskedDepthSceneFeaturesGraphicsPipeline.Terminate();

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines2)
	{
		pipeline.Terminate();
	}

	_InstancedImpostorColorSceneFeaturesGraphicsPipeline.Terminate();

	for (OpaqueModelSceneFeaturesGraphicsPipeline &pipeline : _OpaqueModelSceneFeaturesGraphicsPipelines)
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