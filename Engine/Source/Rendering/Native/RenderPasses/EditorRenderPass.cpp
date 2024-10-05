#if defined(CATALYST_EDITOR)
//Header file.
#include <Rendering/Native/RenderPasses/EditorRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
EditorRenderPass::EditorRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::EDITOR, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		EditorRenderPass::Instance->Initialize();
	});

	//Set the pre record function.
	SetPreRecordFunction
	(
		[](CommandBuffer *const RESTRICT command_buffer)
		{
			EditorRenderPass::Instance->PreRecord(command_buffer);
		}
	);

	//Set the execution function.
	SetExecutionFunction([]()
	{
		EditorRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		EditorRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void EditorRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines
	(
		_Pipelines.Size()
	);

	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		pipeline.Initialize();
	}
}

/*
*	Pre records this render pass.
*/
void EditorRenderPass::PreRecord(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Clear the depth buffer.
	command_buffer->ClearColorImage(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::EDITOR_METADATA));
}

/*
*	Executes this render pass.
*/
void EditorRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		pipeline.Execute();
	}
}

/*
*	Terminates this render pass.
*/
void EditorRenderPass::Terminate() NOEXCEPT
{
	for (GraphicsRenderPipeline &pipeline : _Pipelines)
	{
		pipeline.Terminate();
	}
}
#endif