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

	//Set the pre record function.
	SetPreRecordFunction
	(
		[](CommandBuffer *const RESTRICT command_buffer)
		{
			SceneFeaturesRenderPass::Instance->PreRecord(command_buffer);
		}
	);

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

	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		AddPipeline(&pipeline);
	}

	AddPipeline(&_AnimatedModelSceneFeaturesGraphicsPipeline);
#if defined(CATALYST_EDITOR)
	AddPipeline(&_EditorSelectedModelGraphicsPipeline);
#endif
	AddPipeline(&_VelocityGraphicsPipeline);

	//Initialize all pipelines.
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		pipeline.Initialize();
	}

	_AnimatedModelSceneFeaturesGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
#endif
	_VelocityGraphicsPipeline.Initialize();
}

/*
*	Pre records this render pass.
*/
void SceneFeaturesRenderPass::PreRecord(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Clear the depth buffer.
	command_buffer->ClearDepthStencilImage(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_DEPTH_BUFFER));
}

/*
*	Executes this render pass.
*/
void SceneFeaturesRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		pipeline.Execute();
	}

	_AnimatedModelSceneFeaturesGraphicsPipeline.Execute();
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
	for (GraphicsRenderPipeline &pipeline : _GraphicsRenderPipelines)
	{
		pipeline.Terminate();
	}

	_AnimatedModelSceneFeaturesGraphicsPipeline.Terminate();
#if defined(CATALYST_EDITOR)
	_EditorSelectedModelGraphicsPipeline.Terminate();
#endif
	_VelocityGraphicsPipeline.Terminate();
}