#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/ImGuiRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ImGuiRenderPass::ImGuiRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::IMGUI, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ImGuiRenderPass::Instance->Initialize();
	});

	//Set the pre-record function.
	SetPreRecordFunction([](CommandBuffer *const RESTRICT frame_command_buffer)
	{
		ImGuiRenderPass::Instance->PreRecord(frame_command_buffer);
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ImGuiRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ImGuiRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ImGuiRenderPass::Initialize() NOEXCEPT
{	
	//Reset this render pass.
	ResetRenderPass();

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ImGuiGraphicsPipeline);

	//Initialize all pipelines.
	_ImGuiGraphicsPipeline.Initialize();
}

/*
*	Pre-records this render pass.
*/
void ImGuiRenderPass::PreRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT
{
#if defined(CATALYST_EDITOR)
	frame_command_buffer->BlitImage
	(
		&_ImGuiGraphicsPipeline,
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1),
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::EDITOR_VIEWPORT)
	);
#endif
}

/*
*	Executes this render pass.
*/
void ImGuiRenderPass::Execute() NOEXCEPT
{
	//Execute all pipelines.
	_ImGuiGraphicsPipeline.Execute();
}

/*
*	Terminates this render pass.
*/
void ImGuiRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ImGuiGraphicsPipeline.Terminate();
}
#endif